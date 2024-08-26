#include <xSignature.h>
#include <xAssert.h>
#include <stdio.h>
#include <elf.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define X_SIGNATURE_SECTION ".signature"

////////////////////////////////////////////////////////////////
// Function: getDataFromFile
// Description: get the signature and the content of the file
////////////////////////////////////////////////////////////////
int getDataFromFile(const char *p_ptucFile, 
                    unsigned char *p_ptucSignature, 
                    unsigned char *p_ptucContent, 
                    long *p_lContentSize,
                    long *p_lSignatureSize)
{
    int l_iFile = open(p_ptucFile, O_RDONLY);
    if (l_iFile == -1)
    {
        perror("Erreur lors de l'ouverture du fichier");
        return -1;
    }

    struct stat l_tStat = {};
    if (fstat(l_iFile, &l_tStat) == -1)
    {
        perror("Erreur lors de la récupération des informations du fichier");
        close(l_iFile);
        return -1;
    }

    void *l_pucFile = mmap(NULL, l_tStat.st_size, PROT_READ, MAP_PRIVATE, l_iFile, 0);
    if (l_pucFile == MAP_FAILED)
    {
        perror("Erreur lors de la lecture du fichier");
        close(l_iFile);
        return -1;
    }

    Elf64_Ehdr *l_ptElfHeader = (Elf64_Ehdr *)l_pucFile;
    //verifier si le fichier est un fichier ELF
    if (memcmp(l_ptElfHeader->e_ident, ELFMAG, SELFMAG) != 0)
    {
        fprintf(stderr, "Erreur: le fichier n'est pas un fichier ELF\n");
        munmap(l_pucFile, l_tStat.st_size);
        close(l_iFile);
        return -1;
    }

    Elf64_Shdr *l_ptSectionHeader = (Elf64_Shdr *)(l_pucFile + l_ptElfHeader->e_shoff);
    Elf64_Shdr *l_ptTabHeader = &l_ptSectionHeader[l_ptElfHeader->e_shstrndx];
    char *l_pucStringTable = (char *)(l_pucFile + l_ptSectionHeader[l_ptElfHeader->e_shstrndx].sh_offset);


    Elf64_Shdr *l_ptSignatureSection = NULL;
    long l_lContentSize = 0;

    for (int i = 0; i < l_ptElfHeader->e_shnum; i++)
    {
        if (strcmp(&l_pucStringTable[l_ptSectionHeader[i].sh_name], X_SIGNATURE_SECTION) == 0)
        {
            l_ptSignatureSection = &l_ptSectionHeader[i];
            break;
        }

        else 
        {
            l_lContentSize += l_ptSectionHeader[i].sh_size;
        }
    }

    if (l_ptSignatureSection == NULL)
    {
        fprintf(stderr, "Erreur: la section de signature n'a pas été trouvée\n");
        munmap(l_pucFile, l_tStat.st_size);
        close(l_iFile);
        return -1;
    }

    //get the signature
    *p_lSignatureSize = l_ptSignatureSection->sh_size;
    p_ptucSignature = (unsigned char*) realloc(p_ptucSignature, *p_lSignatureSize);
    memcpy(p_ptucSignature, l_pucFile + l_ptSignatureSection->sh_offset, l_ptSignatureSection->sh_size);

    //close the file
    munmap(l_pucFile, l_tStat.st_size);
    close(l_iFile);

    //open the file os binary
    FILE *l_pFile = fopen(p_ptucFile, "rb");
    if (l_pFile == NULL)
    {
        perror("Erreur lors de l'ouverture du fichier");
        return -1;
    }

    //get the content 
    fseek(l_pFile, 0, SEEK_END);
    *p_lContentSize = ftell(l_pFile);
    fseek(l_pFile, 0, SEEK_SET);
    
    //locate allocation for the content
    unsigned char *l_ptucContent = (unsigned char*) malloc(*p_lContentSize);
    if (l_ptucContent == NULL)
    {
        perror("Erreur lors de l'allocation de la mémoire pour le contenu");
        fclose(l_pFile);
        return -1;
    }

    //read the content
    fread(l_ptucContent, 1, *p_lContentSize, l_pFile);

    //delete the signature from the content 
    for (int i = 0; i < *p_lContentSize; i++)
    {
        if (memcmp(&l_ptucContent[i], p_ptucSignature, *p_lSignatureSize) == 0)
        {
            memmove(&l_ptucContent[i], &l_ptucContent[i + *p_lSignatureSize], *p_lContentSize - i - *p_lSignatureSize);
            *p_lContentSize -= *p_lSignatureSize;
            break;
        }
    }

    //realloc the output 
    p_ptucContent = (unsigned char*) realloc(p_ptucContent, *p_lContentSize);

    //copy the content
    memcpy(p_ptucContent, l_ptucContent, *p_lContentSize);
    free(l_ptucContent);

    fclose(l_pFile);

    return 0;
}

////////////////////////////////////////////////////////////////
// Function: getPublicKeyFromFile
// Description: get the public key from a file
////////////////////////////////////////////////////////////////
int getPublicKeyFromFile(const char *p_ptucFile, EVP_PKEY **p_ptucPublicKey)
{
    X_ASSERT(p_ptucFile != NULL);
    X_ASSERT(p_ptucPublicKey != NULL);

    FILE *l_pFile = fopen(p_ptucFile, "rb");
    if (l_pFile == NULL)
    {
        perror("Erreur lors de l'ouverture du fichier");
        return -1;
    }

    // Lire la clé publique à partir du fichier PEM
    *p_ptucPublicKey = PEM_read_PUBKEY(l_pFile, NULL, NULL, NULL);
    if (*p_ptucPublicKey == NULL)
    {
        fprintf(stderr, "Erreur lors de la lecture de la clé publique : %s\n", ERR_error_string(ERR_get_error(), NULL));
        fclose(l_pFile);
        return -1;
    }

    fclose(l_pFile);

    return 0;  // Succès
}

////////////////////////////////////////////////////////////////
// Function: xSignatureCheck
// Description: check the signature
////////////////////////////////////////////////////////////////
int xSignatureCheck(unsigned char *p_ptucSignature, 
                    unsigned char *p_ptucContent, 
                    long p_lContentSize,
                    long p_lSignatureSize,
                    EVP_PKEY *p_ptucPublicKey)
{
    X_ASSERT(p_ptucSignature != NULL);
    X_ASSERT(p_ptucContent != NULL);
    X_ASSERT(p_ptucPublicKey != NULL);

    // Créer un contexte de vérification de signature
    EVP_MD_CTX *l_pCtx = EVP_MD_CTX_new();
    if (l_pCtx == NULL)
    {
        fprintf(stderr, "Erreur lors de la création du contexte de vérification de signature\n");
        return -1;
    }

    // Initialiser le contexte de vérification de signature
    if (EVP_DigestVerifyInit(l_pCtx, NULL, EVP_shake256(), NULL, p_ptucPublicKey) != 1)
    {
        fprintf(stderr, "Erreur lors de l'initialisation du contexte de vérification de signature\n");
        EVP_MD_CTX_free(l_pCtx);
        return -1;
    }

    // Mettre à jour le contexte de vérification de signature
    if (EVP_DigestVerifyUpdate(l_pCtx, p_ptucContent, p_lContentSize) != 1)
    {
        fprintf(stderr, "Erreur lors de la mise à jour du contexte de vérification de signature\n");
        EVP_MD_CTX_free(l_pCtx);
        return -1;
    }

    // Vérifier la signature
    int l_iResult = EVP_DigestVerifyFinal(l_pCtx, p_ptucSignature, p_lSignatureSize);

    if (l_iResult != 1)
    {
        fprintf(stderr, "Erreur lors de la vérification de la signature\n");
        EVP_MD_CTX_free(l_pCtx);
        return -1;
    }

    EVP_MD_CTX_free(l_pCtx);
    return 0;
}
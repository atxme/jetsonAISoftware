#include <xSignature.h>
#include <xAssert.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////
// Function: getDataFromFile
// Description: get the signature and the content of the file
////////////////////////////////////////////////////////////////
int getDataFromFile(const char *p_ptucFile, 
                    unsigned char *p_ptucSignature, 
                    unsigned char *p_ptucContent, 
                    long *p_lContentSize)
{
    X_ASSERT(p_ptucFile != NULL);
    X_ASSERT(p_ptucSignature != NULL);
    X_ASSERT(p_ptucContent != NULL);

    FILE *l_pFile = fopen(p_ptucFile, "rb");  // Ouvrir en mode binaire
    if (l_pFile == NULL)
    {
        return -1;
    }

    fseek(l_pFile, 0, SEEK_END);
    long l_lSize = ftell(l_pFile);
    if (l_lSize < X_SIGNATURE_SIZE)  // Vérifier que la taille du fichier est suffisante
    {
        fclose(l_pFile);
        return -1;
    }
    rewind(l_pFile);

    // Allouer de la mémoire pour le contenu
    *p_lContentSize = l_lSize - X_SIGNATURE_SIZE;
    unsigned char *l_ptucBuffer = (unsigned char *)malloc(*p_lContentSize);
    if (l_ptucBuffer == NULL)
    {
        fclose(l_pFile);
        return -1;
    }

    p_ptucContent = (unsigned char *)realloc(p_ptucContent, *p_lContentSize);

    // Lire le contenu du fichier
    size_t l_uiRead = fread(l_ptucBuffer, 1, *p_lContentSize, l_pFile);
    if (l_uiRead != (unsigned long)*p_lContentSize)
    {
        fclose(l_pFile);
        free(l_ptucBuffer);
        return -1;
    }

    // Lire la signature à la fin du fichier
    fseek(l_pFile, -X_SIGNATURE_SIZE, SEEK_END);
    size_t l_uiSigRead = fread(p_ptucSignature, 1, X_SIGNATURE_SIZE, l_pFile);
    if (l_uiSigRead != X_SIGNATURE_SIZE)
    {
        fclose(l_pFile);
        free(l_ptucBuffer);
        return -1;
    }

    fclose(l_pFile);

    // Copier le contenu dans p_ptucContent
    memcpy(p_ptucContent, l_ptucBuffer, *p_lContentSize);

    free(l_ptucBuffer);

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
    int l_iResult = EVP_DigestVerifyFinal(l_pCtx, p_ptucSignature, X_SIGNATURE_SIZE);

    if (l_iResult != 1)
    {
        fprintf(stderr, "Erreur lors de la vérification de la signature\n");
        EVP_MD_CTX_free(l_pCtx);
        return -1;
    }

    EVP_MD_CTX_free(l_pCtx);
    return 0;
}
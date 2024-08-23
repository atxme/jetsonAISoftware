#ifndef XSIGNATURE_H
#define XSIGNATURE_H

#define X_SIGNATURE_SIZE 132
#define X_CURVE_NAME "prime521r1"

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/ecdsa.h>
#include <openssl/err.h>
#include <openssl/bio.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** 
* @brief get the signature and the content of the file 
* @param p_ptucFile: file name
* @param p_ptucSignature: pointer to store the signature 
* @param p_ptucContent: pointer to store the content
* @param p_lContentSize: pointer to store the size of the content
* @return 0 if success, -1 if error
*/
int getDataFromFile(const char *p_ptucFile, 
                    unsigned char *p_ptucSignature, 
                    unsigned char *p_ptucContent, 
                    long *p_lContentSize);

/**
 * @brief get the public key from openssl PEM file
 * @param p_ptucFile: file name
 * @param p_ptucPublicKey: openSSL public key prime 521
 * @return 0 if success, -1 if error
 */
int getPublicKeyFromFile(const char *p_ptucFile, EVP_PKEY **p_ptucPublicKey);

/**
 * @brief check the signature 
 * @param p_ptucSignature: signature
 * @param p_ptucContent: content
 * @param p_ptucPublicKey: public key
 * @return 0 if success, -1 if error
 */
int xSignatureCheck(unsigned char *p_ptucSignature, 
                    unsigned char *p_ptucContent, 
                    long p_lContentSize,
                    EVP_PKEY *p_ptucPublicKey);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif
//
// Created by christophe on 8/13/24.
//

#ifndef I2C_CORE_H
#define I2C_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_MAX_BUFFER_SIZE 1024

    typedef struct tI2CConfig
    {
        int m_iBus;        // Numéro du bus I2C
        int m_iAddress;    // Adresse du périphérique I2C
    } tI2CConfig;

    /**
     * @brief Ouvre une connexion I2C avec le périphérique spécifié.
     *
     * @param p_ptConfig Configuration du bus I2C et de l'adresse du périphérique.
     * @return int Descripteur de fichier I2C ou -1 en cas d'erreur.
     */
    int i2c_open(const tI2CConfig *p_ptConfig);

    /**
     * @brief Ferme la connexion I2C.
     *
     * @param p_iFile Descripteur de fichier I2C.
     * @return int 0 en cas de succès, -1 en cas d'erreur.
     */
    int i2c_close(int p_iFile);

    /**
     * @brief Lit des données depuis le périphérique I2C.
     *
     * @param p_iFile Descripteur de fichier I2C.
     * @param p_ptucBuffer Buffer pour stocker les données lues.
     * @param p_ulLength Longueur des données à lire.
     * @return int Nombre de bytes lus ou -1 en cas d'erreur.
     */
    int i2c_read(int p_iFile, unsigned char *p_ptucBuffer, unsigned long p_ulLength);

    /**
     * @brief Écrit des données vers le périphérique I2C.
     *
     * @param p_iFile Descripteur de fichier I2C.
     * @param p_kptucBuffer Buffer contenant les données à écrire.
     * @param p_ulLength Longueur des données à écrire.
     * @return int Nombre de bytes écrits ou -1 en cas d'erreur.
     */
    int i2c_write(int p_iFile, const unsigned char *p_kptucBuffer, unsigned long p_ulLength);

#ifdef __cplusplus
}
#endif

#endif // I2C_CORE_H
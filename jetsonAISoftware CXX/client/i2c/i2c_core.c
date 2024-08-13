//
// Created by christophe on 8/13/24.
//

#include "i2c_core.h"
#include <linux/i2c-dev.h>
#include <xAssert.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>


/////////////////////////////////
/// i2c_open
/////////////////////////////////
int i2c_open(const tI2CConfig *p_ptConfig)
{
    X_ASSERT(p_ptConfig != NULL);
    X_ASSERT(p_ptConfig->m_iBus >= 0);
    X_ASSERT(p_ptConfig->m_iAddress >= 0);

    char l_cFilename[20];
    snprintf(l_cFilename, sizeof(l_cFilename), "/dev/i2c-%d", p_ptConfig->m_iBus);

    int l_iFile = open(l_cFilename, O_RDWR);

    if (l_iFile < 0)
    {
        perror("Failed to open the I2C bus");
        return -1;
    }

    if (ioctl(l_iFile, I2C_SLAVE, p_ptConfig->m_iAddress) < 0)
    {
        perror("Failed to acquire bus access and/or talk to slave");
        close(l_iFile);
        return -1;
    }

    return l_iFile;
}


/////////////////////////////////
/// i2c_close
/////////////////////////////////
int i2c_close(int p_iFile)
{
    //check if the file descriptor is valid
    if (p_iFile < 0)
    {
        return -1;
    }
    return close(p_iFile);
}

/////////////////////////////////
/// i2c_read
/////////////////////////////////
int i2c_read(int p_iFile, unsigned char *p_ptucBuffer, unsigned long p_ulLength)
{
    X_ASSERT(p_iFile >= 0);
    X_ASSERT(p_ptucBuffer != NULL);
    X_ASSERT(p_ulLength != 0 && p_ulLength > 0 && p_ulLength < 1024);
    int l_iReturn = 0;

    l_iReturn = read(p_iFile, p_ptucBuffer, p_ulLength);
    if (l_iReturn != p_ulLength)
    {
        perror("Failed to read from the i2c bus");
        return -1;
    }
    return 0;
}

/////////////////////////////////
/// i2c_write
/////////////////////////////////
int i2c_write(int p_iFile, const unsigned char *p_kptucBuffer, unsigned long p_ulLength)
{
    unsigned char l_cBuffer[I2C_MAX_BUFFER_SIZE];
    unsigned short l_usLength = 0;
    int l_iReturn = 0;

    X_ASSERT(p_iFile >= 0);
    X_ASSERT(p_kptucBuffer != NULL);
    X_ASSERT(p_ulLength != 0 && p_ulLength > 0 && p_ulLength < 1024);

    if (l_usLength > I2C_MAX_BUFFER_SIZE)
    {
        perror("Size give to the buffer is too big");
        X_ASSERT(0);
    }
    memcpy(l_cBuffer, p_kptucBuffer, p_ulLength);

    l_iReturn = write(p_iFile, l_cBuffer, p_ulLength);

    if (l_iReturn != p_ulLength)
    {
        perror("Failed to write to the i2c bus");
        return -1;
    }

    return 0;
}






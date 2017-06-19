#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>
#include <linux/spi/spidev.h>


static const char       *spiDev0  = "/dev/spidev0.0" ;
static const char       *spiDev1  = "/dev/spidev0.1" ;
static const uint8_t     spiBPW   = 8 ;
static const uint16_t    spiDelay = 0 ;

static uint32_t    spiSpeeds [2] ;
static int         spiFds [2] ;


int SPI_GetFd     (int channel) {
   
}
int SPI_DataRW    (int channel, unsigned char *data, int len) {
 
}
int SPI_SetupMode (int channel, int speed, int mode) {
  	
}
int SPI_Setup     (int channel, int speed) {

}



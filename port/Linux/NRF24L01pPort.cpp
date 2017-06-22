/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NRF24L01pPort.cpp
 * Author: emon1
 * 
 * Created on January 29, 2017, 8:03 AM
 */

#include "../NRF24L01pPortConfig.h"
#include "../NRF24L01pPort.h"

#if (NRF24L01pPort_Linux == 1)


#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include <linux/spi/spidev.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>

#define __NRF24L01p_CSN_pin 7
#define __NRF24L01p_CE_pin 12

#define __NRF24L01p_SPI 1
#define __NRF24L01p_SPI_SPEED 500000



#ifdef __cplusplus
extern "C" {
#endif

static void GPIO_export (int pin){
    int fd;
    char buf[255];
    fd = open("/sys/class/gpio/export", O_WRONLY);
    sprintf(buf, "%d", pin); 
    write(fd, buf, strlen(buf));
    close(fd);
}
static void GPIO_unexport (int pin){
    int fd;
    char buf[255];
    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    sprintf(buf, "%d", pin); 
    write(fd, buf, strlen(buf));
    close(fd);
}

static void GPIO_direction(int pin, int dir){
    int fd;
    char buf[255];
    sprintf(buf, "/sys/class/gpio/gpio%d/direction", pin);
    fd = open(buf, O_WRONLY);

    if (dir)
    {
        write(fd, "out", 3);
    }
    else
    {
        write(fd, "in", 2);
    }
    close(fd);
}

static void GPIO_write(int pin, int val){
    int fd;
    char buf[255];
    sprintf(buf, "/sys/class/gpio/gpio%d/value", pin);
    fd = open(buf, O_WRONLY);
    sprintf(buf, "%d", val);
    write(fd, buf, 1);
    
    close(fd);
}
static int GPIO_read(int pin){
    int fd;
    char buf[255];
    sprintf(buf, "/sys/class/gpio/gpio%d/value", pin);
    fd = open(buf, O_RDONLY);
    int n = read (fd,buf,1);
    return strtol(buf, NULL, 16);
}

static const char       *spiDev0  = "/dev/spidev0.0" ;
static const char       *spiDev1  = "/dev/spidev0.1" ;
static const uint8_t     spiBPW   = 8 ;
static const uint16_t    spiDelay = 0 ;

static uint32_t    spiSpeeds [2] ;
static int         spiFds [2] ;

static int SPI_GetFd     (int channel) {
    return spiFds [channel & 1] ;
}
static int SPI_DataRW    (int channel, unsigned char *data, int len) {
 struct spi_ioc_transfer spi ;

  channel &= 1 ;

// Mentioned in spidev.h but not used in the original kernel documentation
//      test program )-:

  memset (&spi, 0, sizeof (spi)) ;

  spi.tx_buf        = (unsigned long)data ;
  spi.rx_buf        = (unsigned long)data ;
  spi.len           = len ;
  spi.delay_usecs   = spiDelay ;
  spi.speed_hz      = spiSpeeds [channel] ;
  spi.bits_per_word = spiBPW ;

  return ioctl (spiFds [channel], SPI_IOC_MESSAGE(1), &spi) ;
}


static int SPI_SetupMode (int channel, int speed, int mode) {
     int fd ;

  mode    &= 3 ;        // Mode is 0, 1, 2 or 3
  channel &= 1 ;        // Channel is 0 or 1

  if ((fd = open (channel == 0 ? spiDev0 : spiDev1, O_RDWR)) < 0)
    //return wiringPiFailure (WPI_ALMOST, "Unable to open SPI device: %s\n", strerror (errno)) ;
    printf("failed\r\n");

  spiSpeeds [channel] = speed ;
  spiFds    [channel] = fd ;

// Set SPI parameters.

  if (ioctl (fd, SPI_IOC_WR_MODE, &mode)            < 0)
    //return wiringPiFailure (WPI_ALMOST, "SPI Mode Change failure: %s\n", strerror (errno)) ;
    printf("failed\r\n");
  
  if (ioctl (fd, SPI_IOC_WR_BITS_PER_WORD, &spiBPW) < 0)
    //return wiringPiFailure (WPI_ALMOST, "SPI BPW Change failure: %s\n", strerror (errno)) ;
    printf("failed\r\n");

  if (ioctl (fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed)   < 0)
    //return wiringPiFailure (WPI_ALMOST, "SPI Speed Change failure: %s\n", strerror (errno)) ;
    printf("failed\r\n");

  return fd ;
}
static int SPI_Setup     (int channel, int speed) {
    return SPI_SetupMode (channel, speed, 0) ;
}

#ifdef __cplusplus
}
#endif






NRF24L01pPort::NRF24L01pPort() {
}

NRF24L01pPort::NRF24L01pPort(const NRF24L01pPort& orig) {
}

NRF24L01pPort::~NRF24L01pPort() {
}

void NRF24L01pPort::port_Initialize(){
    __NRF24L01p_port_initialize();
}
void NRF24L01pPort::port_DeInitialize(){
    
}
bool NRF24L01pPort::port_Pin_CE(){
    return __NRF24L01p_port_GPIO_read(__NRF24L01p_GPIO_CE);
}
bool NRF24L01pPort::port_Pin_CSN(){
    return __NRF24L01p_port_GPIO_read(__NRF24L01p_GPIO_CSN);
}
void NRF24L01pPort::port_Pin_CE(bool val){
    __NRF24L01p_port_GPIO_write(__NRF24L01p_GPIO_CE, val);
}
void NRF24L01pPort::port_Pin_CSN(bool val){
    __NRF24L01p_port_GPIO_write(__NRF24L01p_GPIO_CSN, val);
}
int NRF24L01pPort::port_SPI_Transcieve(uint8_t *dataInOut, unsigned int size){
    __NRF24L01p_port_SPI_Transceive(dataInOut, size);
}
void NRF24L01pPort::port_DelayMs(unsigned int ms){

}
void NRF24L01pPort::port_DelayUs(unsigned int us){

}
unsigned int NRF24L01pPort::port_ClockMs(){

}



    
void NRF24L01pPort::__NRF24L01p_port_initialize(void){
    __NRF24L01p_port_GPIO_initialize(__NRF24L01p_GPIO_CSN);
    __NRF24L01p_port_GPIO_initialize(__NRF24L01p_GPIO_CE);
    
    __NRF24L01p_port_SPI_initialize();
}
void NRF24L01pPort::__NRF24L01p_port_deinitialize(void){
}

void NRF24L01pPort::__NRF24L01p_port_GPIO_initialize(__NRF24L01pGPIO_t pin){
    switch(pin){
        case __NRF24L01p_GPIO_CSN:
            //pinMode(__NRF24L01p_CSN_pin ,OUTPUT);//CSN pin as OUTPUT
            GPIO_export(__NRF24L01p_CSN_pin);
            GPIO_direction(__NRF24L01p_CSN_pin, 1);
            break;
        case __NRF24L01p_GPIO_CE:
            //pinMode(__NRF24L01p_CE_pin ,OUTPUT);//CE pin as OUTPUT
            GPIO_export(__NRF24L01p_CE_pin);
            GPIO_direction(__NRF24L01p_CE_pin, 1);
            break;
        case __NRF24L01p_GPIO_IRQ:
            break;
        default:
            break;
    }
}
void NRF24L01pPort::__NRF24L01p_port_GPIO_deinitialize(__NRF24L01pGPIO_t pin){
    switch(pin){
        case __NRF24L01p_GPIO_CSN:
            GPIO_unexport(__NRF24L01p_CSN_pin);
            break;
        case __NRF24L01p_GPIO_CE:
            GPIO_unexport(__NRF24L01p_CE_pin);
            break;
        case __NRF24L01p_GPIO_IRQ:
            break;
        default:
            break;
    }
}
bool NRF24L01pPort::__NRF24L01p_port_GPIO_read(__NRF24L01pGPIO_t pin){
    switch(pin){
        case __NRF24L01p_GPIO_CSN:
            //return digitalRead(__NRF24L01p_CSN_pin);
            return GPIO_read(__NRF24L01p_CSN_pin);
            break;
        case __NRF24L01p_GPIO_CE:
            //return digitalRead(__NRF24L01p_CE_pin);
            return GPIO_read(__NRF24L01p_CE_pin);
            break;
        default:
            break;
    }
}
void NRF24L01pPort::__NRF24L01p_port_GPIO_write(__NRF24L01pGPIO_t pin, bool val){
    switch(pin){
        case __NRF24L01p_GPIO_CSN:
            //digitalWrite(__NRF24L01p_CSN_pin,val);
            GPIO_write(__NRF24L01p_CSN_pin,val);
            break;
        case __NRF24L01p_GPIO_CE:
            //digitalWrite(__NRF24L01p_CE_pin,val);
            GPIO_write(__NRF24L01p_CE_pin,val);
            break;
        default:
            break;
    }
}

void NRF24L01pPort::__NRF24L01p_port_SPI_initialize(void){
    SPI_Setup(__NRF24L01p_SPI,__NRF24L01p_SPI_SPEED);
}
void NRF24L01pPort::__NRF24L01p_port_SPI_deinitialize(void){
}
void NRF24L01pPort::__NRF24L01p_port_SPI_Transceive(uint8_t *data, unsigned int size){
    SPI_DataRW(__NRF24L01p_SPI, (unsigned char*)data,size);
}

void NRF24L01pPort::__NRF24L01p_port_DelayMs(unsigned int ms){
}
void NRF24L01pPort::__NRF24L01p_port_DelayUs(unsigned int us){
}
unsigned int NRF24L01pPort::__NRF24L01p_port_ClockMS(void){
}





#endif
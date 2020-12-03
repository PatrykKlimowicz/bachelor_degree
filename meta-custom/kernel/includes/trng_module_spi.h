#ifndef __TRNG_MODULE_SPI_H_
#define __TRNG_MODULE_SPI_H_

#include <linux/spi/spi.h>

#include <trng_module_nl.h>

#define MY_BUS_NUM 0
#define ALIVE_DEV 0x81
#define BUF_LEN (256 * 4)

/**
 * @brief send message to device via SPI
 * 
 * @param msg pointer to message to be send
 */
void trng_module_spi_send(Msg *msg);

/**
 * @brief recive data from device via SPI
 *  
 * if couldn't read from device or message shouldn't be forwarded to the user,
 * sets message.length to 0
 * @param msg pointer to Msg struct which contains all message elements
 * @param buf buffer for message's payload. It's size should be 256 * 4 bytes
 */
void trng_module_spi_receive(Msg *msg, void *buf);


void register_spi_dev(struct spi_device * dev);

#endif /* __TRNG_MODULE_SPI_H_ */

// SPDX-License-Identifier: GPL

#include <linux/random.h>
#include <linux/kernel.h>

#include <trng_module_spi.h>
#include <trng_module_nl.h>

#define pr_error(fmt, ...) \
	printk(KERN_ERR pr_fmt(fmt), ##__VA_ARGS__)

#define ACK_FLAG 0x01

static struct spi_device *slave_dev;

void trng_module_spi_send(Msg *msg)
{
	int i;
	char buf[BUF_LEN] = {0};

	if (!slave_dev) {
		pr_error("SPI device was not initialized\n");
		return;
	}
	if (!msg) {
		pr_error("Message is null\n");
		return;
	}
	if (msg->seq < 0) {
		pr_error("Could not assign seq number\n");
		return;
	}

	buf[0] = msg->id;
	buf[1] = msg->seq;
	buf[2] = msg->flag;
	buf[3] = msg->length;

	for (i = 4; i < msg->length * 4; i++)
		buf[i] = ((char *)msg->data)[i-4];

	if (spi_write(slave_dev, buf, msg->length * 4))
		pr_error("Sending data to SPI device failed\n");

	// POMYSŁ GRZESIA 
	// if (spi_write(slave_dev, msg, sizeof(*msg)))
	// 	pr_error("Sending data to SPI device failed\n");
}

void trng_module_spi_receive(Msg *msg, void *buf)
{
	char buf_ch[BUF_LEN] = {0};

	if (!slave_dev) {
		pr_error("SPI device was not initialized\n");
		msg->length = 0;
		return;
	}
	if (!msg) {
		pr_error("Message is null\n");
		msg->length = 0;
		return;
	}

	if (spi_read(slave_dev, buf_ch, BUF_LEN)) {
		pr_error("Could not read from spi device\n");
		msg->length = 0;
		return;
	}

	if (buf_ch[3] <= 0 || buf_ch[3] > 256) {
		pr_error("SPI sent invalid data\n");
		msg->length = 0;
		return;
	}

	/* reading control word */
	msg->id = buf_ch[0];
	msg->seq = buf_ch[1];
	msg->flag = buf_ch[2];
	msg->length = buf_ch[3];


	/* ACK messages are not being send to userspace */
	if (msg->flag & ACK_FLAG) {
		msg->length = 0;
		return;
	}

	/* sending response to the device */
	msg->flag = msg->flag | ACK_FLAG;
	msg->length = 1;
	trng_module_spi_send(msg);

	msg->flag = buf_ch[2];
	msg->length = buf_ch[3];

	/* reading message payload */
	memcpy(buf, buf_ch + 4, (msg->length-1)*4);
	msg->data = buf;

	/* ALIVE_DEV messages are not being send to userspace */
	if (msg->id == ALIVE_DEV)
		msg->length = 0;
}

void register_spi_dev(struct spi_device *dev)
{
	slave_dev = dev;
}

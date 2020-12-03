#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int fd;

int spiTxRx(unsigned char txDat) {
	unsigned char rxDat;
	struct spi_ioc_transfer spi;
	memset (&spi, 0, sizeof (spi));

	spi.tx_buf = (unsigned long)&txDat;
	spi.rx_buf = (unsigned long)&rxDat;
	spi.len = 1;

	ioctl (fd, SPI_IOC_MESSAGE(1), &spi);
	return rxDat;
}

int sendCommand(char command, int num1, int num2) {
	unsigned char resultByte;
	bool ack;

	union p1Buffer_T {
		int p1Int;
		unsigned char p1Char [2];
	} p1Buffer;

	union p2Buffer_T {
		int p2Int;
		unsigned char p2Char [2];
	} p2Buffer;

	union resultBuffer_T {
		int resultInt;
		unsigned char resultChar [2];
	} resultBuffer;

	p1Buffer.p1Int = num1;
	p2Buffer.p2Int = num2;
	resultBuffer.resultInt = 0;

	ack = false;
	do {
		// initialize communication
		spiTxRx('c');
		usleep (10);

		// send command
		resultByte = spiTxRx(command);
		if (resultByte == 'a') {
			printf("ACK: 1");
			ack = true;
		}
		usleep (10);  
	} while (!ack);

	// Send the parameters one byte at a time.
	spiTxRx(p1Buffer.p1Char[0]);
	usleep (10);

	spiTxRx(p1Buffer.p1Char[1]);
	usleep (10);

	spiTxRx(p2Buffer.p2Char[0]);
	usleep (10);

	spiTxRx(p2Buffer.p2Char[1]);
	usleep (10);

	// read results
	resultByte = spiTxRx(0);
	resultBuffer.resultChar[0] = resultByte;
	usleep (10);
	resultByte = spiTxRx(0);
	resultBuffer.resultChar[1] = resultByte;
	
	return resultBuffer.resultInt;
}

int main (void) {
	int result, num1, num2;
	unsigned int speed;

	fd = open("/dev/spidev0.0", O_RDWR);
	speed = 1000000;
	ioctl (fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

	printf("Enter first num: ");
	scanf("%d", &num1);
	printf("Enter second num: ");
	scanf("%d", &num2);

	result = sendCommand('a', num1, num2);
	printf("%d + %d = %d\n", num1, num2, result);
	result = sendCommand('s', num1, num2);
	printf("%d - %d = %d\n", num1, num2, result);
}

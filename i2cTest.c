#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int openI2C(char *filename, int address)
{
	int file;
	if ((file = open(filename, O_RDWR)) < 0)
	{
		perror("Failed to open the I2C bus!");
		exit(1);	
	}
	
	if (ioctl(file, I2C_SLAVE, address) < 0)
	{
		printf("Failed to acquire bus access and/or talk to slave. \n");
		exit(1);
	}
	return file;
}

void main(int argc, char *argv[])
{
	int file = openI2C("/dev/i2c-1", 0x45);
	char buf[10] = {0};
	buf[0] = 0b11110000;
	if (write(file,buf,1) != 1)
	{
		printf("Failed to write to the i2c bus. \n");
	}	
}

/*
 * writeeeprom.c
 * this function use for writing the rotation paraments into eeprom
 * the app path must be /root/app/
 *  Created on: Mar 13, 2025
 *      Author: john chang
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define EEPROM_PATH "/sys/class/i2c-dev/i2c-0/device/0-0052/eeprom"

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		printf("Usage: %s <offset> <data>\n", argv[0]);
		return 1;
	}

	int fd = open(EEPROM_PATH,O_WRONLY);
	if(fd < 0)
	{
		perror("open failed");
		return 1;
	}

	/*format change: string to int*/
    char *endptr;
    int num = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0') {
        printf("failed Invalid part detected: %s\n", endptr);
    } else {
        printf("successful Converted number: %d\n", num);
    }

    /*set the eeprom writing postion*/
	off_t offset = lseek(fd, num, SEEK_SET);
	if(offset < 0)
	{
		perror("Failed to lseek file");
		return 1;
	}

	ssize_t ret = write(fd,argv[2],strlen(argv[1]));
    if (ret < 0) {
        perror("write failed");
        close(fd);
        return 1;
    }
    close(fd);

	return 0;
}

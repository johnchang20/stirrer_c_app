/*
 * set3pwm.c
 *
 *  Created on: Mar 14, 2025
 *      Author: phyvm
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define FileCheck "/sys/class/pwm/pwmchip1/pwm0/enable"
#define ExportPath "/sys/class/pwm/pwmchip1/export"
#define PeriodPath "/sys/class/pwm/pwmchip1/pwm0/period"   //min: 1ns
#define DutycyclePath "/sys/class/pwm/pwmchip1/pwm0/duty_cycle"
#define EnablePath "/sys/class/pwm/pwmchip1/pwm0/enable"
#define step_operation (short)32
#define step_angle (float)0.9

int main(int argc, char* argv[])
{
	if (argc < 4)
	{
		printf("Usage: %s <rmp1> <rpm2> <rpm3>\n", argv[0]);
		return 1;
	}

	 /* relationship :
	     * rpm
	     * CLK frequency HZ (motor driver IC)
	     * step_operation(motor driver IC full 1/2 1/4 1/8 1/16 1/32)
	     * step_angle(step motor 0.9° 1.8° )
	     */

	/*base on this formula
	 *
	 *                 Pulse Frequency×60
	 * Speed(rmp) = ------------------------
	 *                 Steps per Revolution
	 *
	 *                             360
	 * Steps per Revolution = -------------
	 *                          step_angle
	 *
	 *         Pulse Frequency*step_angle
	 * rmp = -----------------------------
	 *             6*step_operation
	 */

	/********************************set the pwm1**********************************/
	/*write export*/
    if (0 == access(FileCheck, F_OK) )
    {
    	printf("Successful pwmchip1 registered. File exists.\n");
    }
    else
    {
        printf("register pwmchip1 now...\n");
    	int fd = open(ExportPath,O_WRONLY);
		if(fd < 0)
		{
			perror("ExportPath open failed");
			return 1;
		}
		ssize_t ret = write(fd,"0",1);
		if (ret < 0) {
			perror("ExportPath write failed");
			close(fd);
			return 1;
		}
		close(fd);
		printf("Successful register pwmchip1.\n");
		sleep(1);
    }

    /*must to set period1 dutycycle to 0 first otherwise writing value will failed*/
	int fd = open(DutycyclePath,O_WRONLY);
	if(fd < 0)
	{
		perror("dutycycle open failed");
		return 1;
	}
	ssize_t ret = write(fd,"0",1);
	if (ret < 0) {
		perror("dutycycle write failed");
		close(fd);
		return 1;
	}
	close(fd);

    fd = open(PeriodPath,O_WRONLY);
	if(fd < 0)
	{
		perror("period1 open failed");
		return 1;
	}
	ret = write(fd,"1",1);
	if (ret < 0)
	{
		perror("period1 write failed");
		close(fd);
		return 1;
	}
	close(fd);

	int CLK1 = 0;
	/*format change: string to int*/
	//int num = atoi(argv[1]);  // Convert string to integer
    char *endptr;
    int num = (int)strtol(argv[1], &endptr, 10); // Convert to base 10 integer
    if (*endptr == '\0')
        printf("Integer value: %d\n", num);
    else
        printf("Invalid number\n");

	CLK1 = (int)((6*num*step_operation)/step_angle);
	int period1 = (int)1000000000/CLK1;
	int dutycycle1 = period1/2;

	/*write period1*/
    char period1string[15];  // Buffer to store the string
    sprintf(period1string, "%d", period1);  // Convert int to string
	fd = open(PeriodPath,O_WRONLY);
	if(fd < 0)
	{
		perror("period1 open failed");
		return 1;
	}
	ret = write(fd,period1string,strlen(period1string));
    if (ret < 0) {
        perror("period1 write failed");
        close(fd);
        return 1;
    }
    close(fd);

	/*write dutycycle*/
    char dutycycle1string[15];  // Buffer to store the string
    sprintf(dutycycle1string, "%d", dutycycle1);  // Convert int to string
	fd = open(DutycyclePath,O_WRONLY);
	if(fd < 0)
	{
		perror("dutycycle open failed");
		return 1;
	}
	ret = write(fd,dutycycle1string,strlen(dutycycle1string));
    if (ret < 0) {
        perror("dutycycle write failed");
        close(fd);
        return 1;
    }
    close(fd);

	/*write enable*/
    fd = open(EnablePath,O_WRONLY);
	if(fd < 0)
	{
		perror("enable open failed");
		return 1;
	}
	ret = write(fd,"1",1);
	if (ret < 0) {
		perror("enable write failed");
		close(fd);
		return 1;
	}
	close(fd);

	printf("Successful, pwm1 all arguments setup well!\n");
	/********************************set the pwm1**********************************/


	/********************************set the pwm2**********************************/
	return 0;
}

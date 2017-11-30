/*
 * myLog.cpp
 *
 *  Created on: 2017-1-16
 *      Author: root
 */

/*
 * mdvrLog.cpp
 *
 *  Created on: 2013-11-25
 *      Author: root
 */
#include "includes.h"
#include <stdarg.h>

void writeLog(const char * format, ...)
{
	va_list args;
	char output[2048] = {0}, filename[1024] = {0}, tmp[2048] = {0};
	va_start(args, format);
	vsnprintf(tmp, 2048, format, args);
	int rlen = 0;

	time_t tt = time(NULL);
	struct tm * st = localtime(&tt);
	snprintf(filename + rlen, sizeof(filename) - rlen, "%d-%02d-%02d.log", st->tm_year + 1900, st->tm_mon + 1, st->tm_mday);
	FILE *flog = fopen(filename, "a");
	if(flog != NULL)
	{
		snprintf(output, 2048, "%d-%02d-%02d %02d:%02d:%02d   %s\r\n", st->tm_year + 1900, st->tm_mon + 1, st->tm_mday, st->tm_hour, st->tm_min, st->tm_sec, tmp);
		fwrite(output, 1, strlen(output), flog);
		fclose(flog);
	}
	va_end(args);
}

void traceLog(const char * format, ...)
{
	va_list args;
	char output[2048] = {0};
	va_start(args, format);
	vsnprintf(output, 2048, format, args);
	//printf("tarcelog: %s\n", output);
	va_end(args);
}



#ifndef LOGGING_H
#define LOGGING_H

#include <stdarg.h>

#define TEMP_LOG_NAME   "/home/fenteale/Projects/fenmesh-server/fenmesh.log"

void openLog(int logOnly);
void logWrite(const char * msg, ...);
void closeLog();

#endif
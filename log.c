#include "log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static FILE* logFile;
static int onlyLog = 0;

void openLog(int logOnly) {
    onlyLog = logOnly;
    if (logFile)
        fclose(logFile);
    
    logFile = fopen(TEMP_LOG_NAME, "w");
    if (logFile)
        fclose(logFile);
}
void logWrite(const char * msg, ...) {
    logFile = fopen(TEMP_LOG_NAME, "a");
    if(logFile) {
        time_t tm;
        time(&tm);
        char * curtime = ctime(&tm);
        curtime[strlen(curtime)-1] = '\0'; //remove trailing newline
        fprintf(logFile, "[%s] ", curtime);
        va_list args;
        va_start(args, msg);
        vfprintf(logFile, msg, args);
        va_end(args);
        fprintf(logFile, "\n");
        fclose(logFile);
        if(!onlyLog) {
            printf("[%s] ", curtime);
            va_start(args, msg);
            vprintf(msg, args);
            va_end(args);
            printf("\n");
        }
    }
}
void closeLog() {
    if(logFile)
        fclose(logFile);
}
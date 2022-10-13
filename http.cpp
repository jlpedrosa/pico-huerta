#include "http.h"

#include <stdio.h>
#include <string.h>

#include "pico/time.h"


#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/opt.h"
#include "lwip/apps/httpd.h"
#include "lwip/apps/httpd_opts.h"
#include "lwip/apps/fs.h"

#include "log.h"

static HttpHandler * requestHandler;


int RunWebServer(HttpHandler *handler)
{
    log("Initializing web server\n");
    httpd_init();

    log("Starting web server\n");

    requestHandler = handler;   
    return 0;
}


char * append(char *buffer, const char *content, int length) {
    memcpy(buffer, content, length);
    return buffer + length;
}

char * writeResponse(char *buffer, int statuCode) {
    buffer += sprintf(buffer, "HTTP/1.1 %d OK\r\n", statuCode);
    buffer += sprintf(buffer, "Connection: Keep-Alive\r\n");
    return buffer;    
}

char * writeContentLength(char *buffer, int contentLength) {
    buffer += sprintf(buffer, "Content-type: text/html; charset=UTF-8\r\n");
    buffer += sprintf(buffer, "Content-Length: %d\r\n", contentLength);
    return buffer;    
}

char * closeHeaders(char *buffer) {
    buffer += sprintf(buffer, "\r\n");
    return buffer;
}

char * writeHeaders(char *buffer, int httpStatus, int bodySize) {
    buffer = writeResponse(buffer, httpStatus);
    buffer = writeContentLength(buffer, bodySize);
    buffer = closeHeaders(buffer);
    return buffer;
}

int generateResponse(char *buffer, const char *path) {
    char * initialBuffer = buffer;
    char bodyBuffer[1024];

    int bodyLength = requestHandler(bodyBuffer);
    buffer = writeHeaders(buffer, 200, bodyLength);
    buffer = append(buffer, bodyBuffer, bodyLength);
    return buffer - initialBuffer;
}


#if LWIP_HTTPD_CUSTOM_FILES
int fs_open_custom(struct fs_file *file, const char *name) {

    printf("reading file %s\n", name);

    char *buffer = (char *)malloc(1024);
    if (buffer == NULL ) {
        log("Unable to allow memory for http request");
        return 0;
    }
 
    int len = generateResponse(buffer, name);
    file->data = buffer;
    file->len = len;
    file->index = len;
    file->flags = 0;
    return len;
}

void fs_close_custom(struct fs_file *file) {
     char *buffer = (char *) file->data;
     free(buffer);     
}

#endif

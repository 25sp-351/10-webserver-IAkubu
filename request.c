#include "request.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define BUFFER_SIZE 4096

HttpRequest *read_request_from_fd(int socket_fd) {
    char buffer[BUFFER_SIZE];
    int total_bytes_read = 0;
    int bytes_read = 0;

    // Read until \r\n\r\n is found
    while (1) {
        bytes_read = read(socket_fd, buffer + total_bytes_read, sizeof(buffer) - total_bytes_read - 1);
        if (bytes_read <= 0) {
            printf("[!] Read error or client closed connection.\n");
            return NULL;
        }
        total_bytes_read += bytes_read;
        buffer[total_bytes_read] = '\0'; // Null-terminate

        if (strstr(buffer, "\r\n\r\n")) {
            break; // End of headers found
        }

        if (total_bytes_read >= BUFFER_SIZE - 1) {
            printf("[!] Request too large.\n");
            return NULL;
        }
    }

    // Allocate request
    HttpRequest *request = malloc(sizeof(HttpRequest));
    if (!request) {
        printf("[!] Failed to allocate HttpRequest.\n");
        return NULL;
    }
    memset(request, 0, sizeof(HttpRequest)); // Set everything to 0

    // Parse method, path, version
    char *line_end = strstr(buffer, "\r\n");
    if (!line_end) {
        printf("[!] Invalid request (no line end).\n");
        free(request);
        return NULL;
    }
    *line_end = '\0';

    char *method = strtok(buffer, " ");
    char *path = strtok(NULL, " ");
    char *http_version = strtok(NULL, " ");

    if (!method || !path || !http_version) {
        printf("[!] Malformed request line.\n");
        free(request);
        return NULL;
    }

    request->method = strdup(method);
    request->path = strdup(path);
    request->http_version = strdup(http_version);

    // Headers
    char *headers_start = line_end + 2;
    char *headers_end = strstr(headers_start, "\r\n\r\n");

    if (headers_end) {
        *headers_end = '\0'; // terminate headers
        request->headers = strdup(headers_start);

        // Body for POST requests mainly
        char *body_start = headers_end + 4;
        if (*body_start != '\0') {
            request->body = strdup(body_start);
            request->body_length = strlen(request->body);
        } else {
            request->body = NULL;
            request->body_length = 0;
        }
    } else {
        request->headers = NULL;
        request->body = NULL;
        request->body_length = 0;
    }

    return request;
}

void free_request(HttpRequest *request) {
    if (!request) return;
    free(request->method);
    free(request->path);
    free(request->http_version);
    free(request->headers);
    free(request->body);
    free(request);
}

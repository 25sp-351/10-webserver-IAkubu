#include "response.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

HttpResponse *create_response(const char *status_code) {
    HttpResponse *response = malloc(sizeof(HttpResponse));
    if (!response) return NULL;
    response->status_code = strdup(status_code);
    response->headers = strdup("");
    response->body = NULL;
    response->body_length = 0;
    return response;
}

void set_response_header(HttpResponse *response, const char *header_key, const char *header_value) {
    char header_buffer[1024];
    snprintf(header_buffer, sizeof(header_buffer), "%s: %s\r\n", header_key, header_value);

    char *new_headers = malloc(strlen(response->headers) + strlen(header_buffer) + 1);
    if (!new_headers) return;
    strcpy(new_headers, response->headers);
    strcat(new_headers, header_buffer);
    free(response->headers);
    response->headers = new_headers;
}

void set_response_body(HttpResponse *response, const char *body_content, int body_length) {
    response->body = malloc(body_length);
    if (response->body) {
        memcpy(response->body, body_content, body_length);
        response->body_length = body_length;
    }
}

void send_response(int client_socket, HttpResponse *response) {
    char header_buffer[4096];
    int header_length = snprintf(header_buffer, sizeof(header_buffer),
        "HTTP/1.1 %s\r\n"
        "%s"
        "Content-Length: %d\r\n"
        "\r\n",
        response->status_code,
        response->headers,
        response->body_length
    );
    write(client_socket, header_buffer, header_length);
    if (response->body && response->body_length > 0) {
        write(client_socket, response->body, response->body_length);
    }
}

void free_response(HttpResponse *response) {
    if (!response) return;
    free(response->status_code);
    free(response->headers);
    free(response->body);
    free(response);
}

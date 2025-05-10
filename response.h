#pragma once

typedef struct {
    char *status_code;
    char *headers;
    char *body;
    int body_length;
} HttpResponse;

HttpResponse *create_response(const char *status_code);

void set_response_header(HttpResponse *response, const char *header_key,
                         const char *header_value);

void set_response_body(HttpResponse *response, const char *body_content,
                       int body_length);

void send_response(int client_socket, HttpResponse *response);

void free_response(HttpResponse *response);

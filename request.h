#pragma once

typedef struct {
    char *method;
    char *path;
    char *http_version;
    char *headers;
    char *body;
    int body_length;
} HttpRequest;

HttpRequest *read_request_from_fd(int socket_fd);

void free_request(HttpRequest *request);

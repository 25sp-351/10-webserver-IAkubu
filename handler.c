#include "handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Output static files
HttpResponse *handle_static_request(const char *path) {
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "static%s", path + 7); // skip "/static"

    FILE *file = fopen(filepath, "rb");
    if (!file) {
        HttpResponse *response = create_response("404 Not Found");
        set_response_header(response, "Content-Type", "text/plain");
        set_response_body(response, "File not found", strlen("File not found"));
        return response;
    }

    fseek(file, 0, SEEK_END);
    int filesize = ftell(file);
    rewind(file);

    char *file_content = malloc(filesize);
    fread(file_content, 1, filesize, file);
    fclose(file);

    HttpResponse *response = create_response("200 OK");
    set_response_header(response, "Content-Type", "application/octet-stream");
    set_response_body(response, file_content, filesize);
    free(file_content);
    return response;
}

// Perform calculations
HttpResponse *handle_calculation_request(const char *path) {
    int num1, num2;
    char operation[16];

    if (sscanf(path, "/calc/%15[^/]/%d/%d", operation, &num1, &num2) != 3) {
        HttpResponse *response = create_response("400 Bad Request");
        set_response_header(response, "Content-Type", "text/plain");
        set_response_body(response, "Invalid calculation format", strlen("Invalid calculation format"));
        return response;
    }

    int result = 0;
    if (strcmp(operation, "add") == 0) {
        result = num1 + num2;
    } else if (strcmp(operation, "mul") == 0) {
        result = num1 * num2;
    } else if (strcmp(operation, "div") == 0) {
        if (num2 == 0) {
            HttpResponse *response = create_response("400 Bad Request");
            set_response_header(response, "Content-Type", "text/plain");
            set_response_body(response, "Division by zero", strlen("Division by zero"));
            return response;
        }
        result = num1 / num2;
    } else {
        HttpResponse *response = create_response("400 Bad Request");
        set_response_header(response, "Content-Type", "text/plain");
        set_response_body(response, "Unknown operation", strlen("Unknown operation"));
        return response;
    }

    char result_text[64];
    snprintf(result_text, sizeof(result_text), "%d", result);

    HttpResponse *response = create_response("200 OK");
    set_response_header(response, "Content-Type", "text/plain");
    set_response_body(response, result_text, strlen(result_text));
    return response;
}

// Main handler function
HttpResponse *handle_request(HttpRequest *request) {
    if (strcmp(request->method, "GET") != 0) {
        HttpResponse *response = create_response("405 Method Not Allowed");
        set_response_header(response, "Content-Type", "text/plain");
        set_response_body(response, "Only GET supported", strlen("Only GET supported"));
        return response;
    }

    if (strncmp(request->path, "/static/", 8) == 0) {
        return handle_static_request(request->path);
    } else if (strncmp(request->path, "/calc/", 6) == 0) {
        return handle_calculation_request(request->path);
    } else {
        HttpResponse *response = create_response("404 Not Found");
        set_response_header(response, "Content-Type", "text/plain");
        set_response_body(response, "Not Found", strlen("Not Found"));
        return response;
    }
}

#include "request.h"
#include "response.h"
#include "handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>

#define DEFAULT_PORT 8080

void *client_thread(void *arg) {
    int client_socket = *(int *)arg;
    free(arg);

    HttpRequest *request = read_request_from_fd(client_socket);
    if (request) {
        HttpResponse *response = handle_request(request);
        if (response) {
            send_response(client_socket, response);
            free_response(response);
        }
        free_request(request);
    }

    close(client_socket);
    return NULL;
}

int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;

    if (argc == 3 && strcmp(argv[1], "-p") == 0) {
        port = atoi(argv[2]);
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("[+] Listening on port %d...\n", port);

    while (1) {
        socklen_t addrlen = sizeof(address);
        int *client_socket = malloc(sizeof(int));
        *client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (*client_socket < 0) {
            perror("accept failed");
            free(client_socket);
            continue;
        }

        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, client_socket);
        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}

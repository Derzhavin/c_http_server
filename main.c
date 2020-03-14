#include "response_builder.h"
#include "config_parser.h"

#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <stdbool.h>

#define RECV_BUF_SIZE 1024
#define DEFAULT_CONFIG_NAME "config"

int main(int argc, char *argv[]) {
    char config_filename[255];
    
    if (argc > 1) {
       strcpy(config_filename, argv[1]);     
    } else {
        strcpy(config_filename, DEFAULT_CONFIG_NAME);
    }

    Config *config = parse_config(config_filename);

    if (!config) {
        puts("config failed");
        return 0;
    }
    
    printf("config: %s\n", config_filename);  
    printf("host: %s\n", config->host);
    printf("port: %d\n", config->port);
    printf("assets: %s\n", config->assets);
    printf("version: %s\n", config->http_version);
    printf("index: %s\n", config->index);

    struct sockaddr_in server_addr;

    DIR * dir;
    struct dirent *src;
    char path[strlen(config->assets) + 1 + 255];

    if (!(dir = opendir(config->assets))) {
        puts("opendir failed");
        return 0;
    }
    
    bool is_html_in_dir = false;

    while((src = readdir(dir))) {
        if (src->d_type == DT_REG) {
            char *extension = strchr(src->d_name, '.') + 1;
            if (is_extension_fit(extension, "html")) {
                is_html_in_dir = true;
            }
        }
    }

    if (!is_html_in_dir) {
        puts("there is no html in dir");
        return 0;
    }

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    int client_sock;

    inet_pton(AF_INET, config->host, &server_addr.sin_addr);
    server_addr.sin_port = htons(config->port);
    server_addr.sin_family = AF_INET;

    bind(server_sock, (struct sockaddr*) &server_addr, sizeof(server_addr));
    listen(server_sock, 2);
    
    struct sockaddr_in client_addr;
    socklen_t socklen = sizeof(struct sockaddr_in);
    char buf_recv[RECV_BUF_SIZE];
    char bad_response[50];
    sprintf(bad_response, "%s 404 Not Found\r\n", config->http_version);

    for(;;) {
        puts("============================================");
        puts("Waiting for connection...");

        client_sock = accept(server_sock, (struct sockaddr*) &client_addr, &socklen);
       
        printf("Connection from: %s.\n", inet_ntoa(client_addr.sin_addr));
        recv(client_sock, buf_recv, RECV_BUF_SIZE, 0);
        puts(buf_recv);

        char *src_filename = parse_src_filename(buf_recv, config->http_version);
        char *response = NULL;

        if (src_filename) {
            if (!strcmp(src_filename, "/")) {
                strcpy(src_filename, config->index);
            }

            sprintf(path, "%s/%s", config->assets, src_filename);
            char request_line[50];
            sprintf(request_line, "%s 200 OK", config->http_version);
            response = read_src(path, request_line, config->http_version, "utf-8");
            
            if (response) {
                send(client_sock, (char*)response, strlen(response), 0);
            }
            else {
                send(client_sock, (char*)bad_response, strlen(bad_response), 0);
            }
        } else {
             send(client_sock, (char*)bad_response, strlen(bad_response), 0);
        }

        
        close(client_sock);
        
        if (response) {
            free(response);
        }
        puts("Connection closed.");
    }
    
    free(config);
    return 0;
}

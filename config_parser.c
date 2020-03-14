#include "config_parser.h"

Config *parse_config(const char *filename) {
    Config *config = (Config *) malloc(sizeof(Config));
    FILE *config_file = fopen(filename, "r");

    if (!config_file) {
        return NULL;
    }

    fseek(config_file, 0L, SEEK_END);
    long int file_size = ftell(config_file);
    rewind(config_file);
    char *buffer = (char*) malloc(file_size);
    fread(buffer, 1, file_size, config_file);

    char str_port[10];
    strcpy(config->host, strtok(buffer + 6, "\n"));
    strcpy(str_port, strtok(NULL, "\n") + 6);
    config->port = atoi(str_port);
    strcpy(config->assets, strtok(NULL, "\n") + 8);
    strcpy(config->http_version, strtok(NULL, "\n") + 9);
    strcpy(config->index, strtok(NULL, "\n") + 7);
   
   free(buffer);
   return config;
}

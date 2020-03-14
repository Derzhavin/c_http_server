#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <stdbool.h>

typedef struct Config Config; 
struct Config {
    char host[15];
    unsigned short port;
    char assets[255];
    char http_version[10];
    char index[255];
};

Config *parse_config(const char *filename);



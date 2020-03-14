#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <stdbool.h>

#define TEXT_TYPES_NUM 2

typedef struct Pair Pair;
struct Pair {
    char extension[20];
    char text_type[20];
};

bool is_extension_fit(char *pattern, char *text);
char *read_src(const char *filename, char *request_line, char *version, char *charset);
char *parse_src_filename(char *buf_recv, char *http_version);

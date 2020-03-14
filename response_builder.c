#include "response_builder.h"

bool is_extension_fit(char *pattern, char *text) {
    unsigned short extension_len = strlen(pattern);
    return !strcmp(text, pattern) && extension_len == strlen(text);
}

char *parse_src_filename(char *buf_recv, char *http_version) {
    if (!strncmp(buf_recv, "GET ", 4)) {
        char *p_src_end = strchr(buf_recv + 4, ' ');
        if (!strncmp(p_src_end + 1, http_version, strlen(http_version))) {
            char *p_src_begin = strtok(buf_recv + 4, " ");
            return p_src_begin != p_src_end ? p_src_begin: NULL;
        }
    } 

    return NULL;
}

char *read_src(const char *filename, char *request_line, char *version, char *charset) {
    Pair pairs[TEXT_TYPES_NUM] = {{"html\0", "html\0"}, {"js\0", "javascript\0"}};
    
    char* text_type;
    char *extension = strchr(filename, '.') + 1;

    for(int i = 0; i < TEXT_TYPES_NUM; i++) {
        if (is_extension_fit(extension, pairs[i].extension)) {
            text_type = pairs[i].text_type;        
        }
    }

    FILE *page = fopen(filename, "r");
    
    if (!page) {
        return NULL;
    }

    fseek(page, 0L, SEEK_END);
    long int page_size = ftell(page);
    rewind(page);

    char page_size_str[100];
    sprintf(page_size_str, "%lu", page_size);

    unsigned long long int response_size = strlen(request_line)
                                         + strlen("\r\n")
                                         + strlen("Version: ")
                                         + strlen(version)
                                         + strlen("\r\n")
                                         + strlen("Content-Type: text/")
                                         + strlen(text_type)
                                         + strlen("; charset=")
                                         + strlen(charset)
                                         + strlen("\r\n")
                                         + strlen("Content-Length: ")
                                         + strlen(page_size_str)
                                         + strlen("\r\n\r\n")
                                         + page_size;
    
    char *dest = (char*) malloc(response_size);
    sprintf(dest, "%s\r\nVersion: %s\r\nContent-Type: text/%s; charset=%s\r\nContent-Length: %lu\r\n\r\n",
            request_line,
            version,
            text_type,
            charset,
            page_size
           );
     
    fread(&dest[response_size - page_size], 1, page_size, page);

    return dest;
}

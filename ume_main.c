/* 
 * ume_main.c
 * 
 * this file contains the main function of ume application
 */

#include "ume.h"

/*
 * forward declaration
 */
char ** um_read_patterns_from_file(char *file_name);
um_match_url_from_file(char *file_name, struct um_ctx *ctx);

int main(int ac, char **av) {

    char *pre;
    char *suf;
    char **filters = NULL;
    struct um_ctx *ctx = NULL;

    if ( ac != 3 ) {
        printf(" Usage : ume <pattern file> <url file>\n");
        exit(EXIT_FAILURE);
    }

    if ( ( (filters = um_read_patterns_from_file(av[1])) == NULL) || 
       ( (ctx = um_compile(filters)) == NULL ) )  {
        printf("ERROR : error processing pattern file : file=%s\n", av[1]);
        exit(EXIT_FAILURE);
    }
    free(filters);

    if ( um_match_url_from_file(av[2],ctx) == EXIT_FAILURE ) {
        printf("ERROR : error processing url file : file=%s\n",av[2] );
        exit(EXIT_FAILURE);
    }

    um_free(ctx);

}

char ** um_read_patterns_from_file(char *file_name){
    FILE * fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char **patterns = NULL;
    int index=0;

    fp = fopen(file_name, "r");
    if (fp == NULL) {
        printf("ERROR : error opening file in read mode, file=%s\n",file_name);
        return NULL;
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        if ( patterns == NULL ){
            patterns = (char **) malloc(sizeof(char *));
        } else {
            patterns = (char **) realloc(patterns, (index+1) *sizeof(char*));
        }

        if ( patterns == NULL ) {
            printf("ERROR : error mallocing space for patterns : file=%s\n",file_name);
            return NULL;
        }

        char *dup =  strndup(line,read-1);
        if ( dup == NULL ) {
            printf("ERROR : error duplicating pattern buffer : file=%s\n",file_name);
            return NULL;
        }
        patterns[index] = dup;
 
        index++;
    }

    /*
     * pattern has to be NULL terminated
     */
    patterns = (char **) realloc(patterns, (index+1) * sizeof(char*));
    if ( patterns == NULL ) {
        printf("ERROR : error mallocing last space for patterns : file=%s\n",file_name);
        return NULL;
    }

    patterns[index] = NULL;

    fclose(fp);

    return patterns;
}

int um_match_url_from_file(char *file_name, struct um_ctx *ctx){
    FILE * fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(file_name, "r");
    if (fp == NULL) {
        printf("ERROR : error opening file in read mode, file=%s\n",file_name);
        return EXIT_FAILURE;
    }

    if (ctx == NULL){
        printf("ERROR : invalid ctx\n");
        return EXIT_FAILURE;
    }

    while ((read = getline(&line, &len, fp)) != -1) {

        bool results[ctx->count];
        char *dup =  strndup(line,read-1);

        um_match(ctx,dup,results);

        printf("URL %s",dup);
        for(int i=0; i<ctx->count;i++){
            if ( results[i] == true ) {
                printf(",%d",i);
            }
        }
        printf("\n");
        free(dup);
    }

    fclose(fp);

    return EXIT_SUCCESS;
}
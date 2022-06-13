/* 
 * ume_main.c
 * 
 * this file contains the main function of ume application
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "ume.h"

/*
 * forward declaration of local functions
 */
char ** um_read_patterns_from_file(char *file_name);
int um_match_url_from_file(char *file_name, struct um_ctx *ctx);
static void patterns_free(char **patterns);

/*
 * Function:  main 
 * ----------------------------------------------------------------------------
 * main reads patterns from pattern file, creates a context 
 * and calls function to match URLs in url file
 *
 *  ac: number of parameter
 *  av: array of strings contain command line parameters. In this case we 
 *      expect two parameters :
 *      1. name of the file containing patterns
 *      2. name of the file containing URLs
 *
 *  returns: EXIT_SUCCESS if successful
 *           EXIT_FAILURE in case of an error
 */
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
       ( (ctx = um_compile((const char **)filters)) == NULL ) )  {
        printf("ERROR : error processing pattern file : file=%s\n", av[1]);
        exit(EXIT_FAILURE);
    }
    patterns_free(filters);

    if ( um_match_url_from_file(av[2],ctx) == EXIT_FAILURE ) {
        printf("ERROR : error processing url file : file=%s\n",av[2] );
        exit(EXIT_FAILURE);
    }

    um_free(ctx);

}

/*
 * Function:  um_read_patterns_from_file 
 * ----------------------------------------------------------------------------
 * This function reads patterns from file and returns them as array of strings.
 * It assumes that there is one pattern per line
 *
 *  file_name : IN pattern file name
 *
 *  returns: char** if successful
 *           NULL in case of an error
 */
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

    if (line) free(line);
    fclose(fp);

    return patterns;
}

/*
 * Function:  um_match_url_from_file 
 * ----------------------------------------------------------------------------
 * This function reads URLS from file, looks for a match with patterns in 
 * contextstructure and prints all the match indexes to the stdout. It assumes 
 * one URL per line.
 *
 *  file_name : IN file contain URLs
 *  ctx       : IN pattern context
 *
 *  returns: EXIT_SUCCESS if successful
 *           EXIT_FAILURE in case of an error
 */

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

    if (line) free(line);
    fclose(fp);

    return EXIT_SUCCESS;
}

/*
 * Function:  patterns_free 
 * ----------------------------------------------------------------------------
 * This function frees patterns array and associated character buffers.
 *
 *  patterns : IN pattern array
 *
 *  returns: none
 */
static void patterns_free(char **patterns) {
    if (patterns == NULL) return;

    char **iter = patterns;
    while (*iter != NULL) {
       free(*iter);
       iter++;
    }

    free(patterns);
}

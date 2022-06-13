/* 
 * ume_api.c
 * 
 * this file contains definition of the major UME APIs
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "ume.h"

static bool is_match(char * s, char * p);

struct um_ctx *um_compile(const char **patterns){

    int count = 0;
    const char **iter;

    /* check size of patterns */
    iter = patterns;
    while (*iter != NULL) {
        iter++;
        count++;
    }

    /* malloc space for context */
    struct um_ctx *ctx = (struct um_ctx *) malloc(sizeof(struct um_ctx));
    if ( ctx == NULL ){
        return NULL;
    }

    /* malloc space for pattern sub structure */
    ctx->count = count;
    ctx->patterns = (struct um_pattern *) malloc(count * sizeof(struct um_pattern));
    if (ctx->patterns == NULL) {
        free(ctx);
        return NULL;
    }

    /* initialize each pattern sub structure */
    iter = patterns;
    struct um_pattern *um_pattern = ctx->patterns;
    while( *iter != NULL) {
        char *pre, *suf;
        um_tokenize(*iter,&pre,&suf);
        um_pattern->pref = pre;
        um_pattern->suff = suf;

        iter++;
        um_pattern++;
    } 

    return ctx;
}  

int um_match(const struct um_ctx *ctx, const char *url, bool *results){

    char *pre, *suf;

    um_tokenize(url,&pre,&suf);

    for (int i=0;i<ctx->count;i++){
        if ( is_match(pre, ctx->patterns[i].pref) && is_match(suf,ctx->patterns[i].suff) ){
            results[i] = true;
        } else {
            results[i] = false;
        }
    }

    if (pre) free(pre);
    if (suf) free(suf);

    return EXIT_SUCCESS;
}


void um_free(struct um_ctx *ctx) {

    if ( ctx == NULL ) return;

    for (int i=0;i<ctx->count;i++){
        struct um_pattern pattern = ctx->patterns[i];
        if (pattern.pref) free(pattern.pref);
        if (pattern.suff) free(pattern.suff);
    }

    free(ctx->patterns);
    free(ctx);
}

/*
 * Function:  um_tokenize 
 * ----------------------------------------------------------------------------
 * This function returns prefix and suffix of a string where prefix and suffix
 * are delimited by first occurence of '/' character. In case where there is no
 * '/', suffix will be empty.
 *
 *  s   : IN string that needs to be tokenized
 *  pre : OUT prefix
 *  suf : OUT suffix 
 *
 *  returns: true if successful
 *           false in case of an error
 */
bool um_tokenize(const char *s, char **pre, char **suf){

    char *copy = strdup(s);
    char *rest = copy;

    char *temp = strtok_r(rest, "/", &rest);

    /*
     * malloc a new buffers for prefix and suffix 
     */
    *pre = strdup(temp);
    if (*pre == NULL){
        return false;
    }

    /*
     * suffix could be NULL
     */
    if ( rest != NULL ){
        *suf = strdup(rest);
        if (*suf == NULL){
            return false;
        }
    }else *suf = NULL;

    if (copy) free(copy);

    return true;
}

/*
 * Function:  is_match 
 * ----------------------------------------------------------------------------
 * This function uses top-down dynamic programing algorithm to checks if there
 * is a match between string and a pattern string taking into account a 
 * wildcard character '*' in pattern string. 
 *
 *  s   : IN string that needs to be tokenized
 *  p   : IN pattern string that can have '*' character
 *
 *  returns: true  : if there was a match
 *           false : otherwise 
 */
static bool is_match(char * s, char * p){

    /* empty string matchs empty pattern */
    if ( s==NULL && p==NULL) return true;

    if ( s==NULL || p==NULL) return false;

    int m = strlen(s);
    int n = strlen(p);
 
    /* zero length string matchs zero length pattern */
    if (  n == 0  ) return (m == 0);
    
    bool dp[m+1][n+1];
    
    memset(dp,false,sizeof(dp));

    /* by definition wildcard pattern matchs empty string */   
    dp[0][0] = true;
    for(int j=0;j<n;j++){
        if (p[j] == '*'){
            dp[0][j+1] = dp[0][j];
        }
    }
     
    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
            if (p[j] == '*' ){
                dp[i+1][j+1] = dp[i][j+1] || dp[i+1][j];
            } else if ( s[i] == p[j]  ){
                dp[i+1][j+1] = dp[i][j];
            } else {
                dp[i+1][j+1] = false;
            }
        }
    }
    
    return dp[m][n];
}

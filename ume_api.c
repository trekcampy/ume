/* 
 * ume_api.c
 * 
 * this file contains definition of the major UME APIs
 */

#include "ume.h"
#include "string.h"

bool isMatch(char * s, char * p);

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
        if ( isMatch(pre, ctx->patterns[i].pref) && isMatch(suf,ctx->patterns[i].suff) ){
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

bool isMatch(char * s, char * p){

    if ( s==NULL && p==NULL) return true;

    if ( s==NULL || p==NULL) return false;

    int m = strlen(s);
    int n = strlen(p);

    if (  n == 0  ) return (m == 0);
    
    bool dp[m+1][n+1];
    int i,j;
    
    memset(dp,false,sizeof(dp));
    
    dp[0][0] = true;
    for(j=0;j<n;j++){
        if (p[j] == '*'){
            dp[0][j+1] = dp[0][j];
        }
    }
     
    for(i=0;i<m;i++){
        for(j=0;j<n;j++){
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

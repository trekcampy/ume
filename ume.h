/* 
 * ume.h
 * 
 * this file contains structure and function declarations 
 */



struct um_pattern {
    char *pref;
    char *suff;
};

struct um_ctx {
    int count;
    struct um_pattern *patterns;
};


struct um_ctx *um_compile(const char **patterns);  

int um_match(const struct um_ctx *ctx, const char *url, bool *results);  

void um_free(struct um_ctx *ctx); 

bool um_tokenize(const char *s, char **pre, char **suf);
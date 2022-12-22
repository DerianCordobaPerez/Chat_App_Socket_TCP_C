typedef struct {
    const char  *_short,
                *_large,
                *description;
    void *value;
    int index;
} ArgumentInfo;

typedef struct Argument {
    ArgumentInfo *info;
    int length;
    struct Argument *next;
} Argument;

const void transform(); 

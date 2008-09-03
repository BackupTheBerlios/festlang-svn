/* Fake header */

#define PARAM_STRING 0

static inline void
ParamGetValue(char *name, int type, char *res)
{
    strcpy(res, "tree.dat");
}


/* verify that yajl_tree_parse limits nesting depth and reports the cause */

#include <yajl/yajl_tree.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *nested_objects(size_t depth)
{
    const char prefix[] = "{\"k\":";
    const size_t prefix_len = sizeof(prefix) - 1;
    char *json = malloc((prefix_len + 1) * depth + 2);
    char *p = json;
    size_t i;

    if (json == NULL)
        return NULL;

    for (i = 0; i < depth; i++) {
        memcpy(p, prefix, prefix_len);
        p += prefix_len;
    }
    *p++ = '0';
    for (i = 0; i < depth; i++)
        *p++ = '}';
    *p = '\0';

    return json;
}

int main(void)
{
    char error[256];
    char expected[128];
    char *json;
    yajl_val tree;

    json = nested_objects(YAJL_MAX_DEPTH);
    if (json == NULL)
        return 1;
    tree = yajl_tree_parse(json, error, sizeof(error));
    free(json);
    if (tree == NULL) {
        fprintf(stderr, "depth %d unexpectedly failed: %s\n",
                YAJL_MAX_DEPTH, error);
        return 1;
    }
    yajl_tree_free(tree);

    json = nested_objects(YAJL_MAX_DEPTH + 1);
    if (json == NULL)
        return 1;
    tree = yajl_tree_parse(json, error, sizeof(error));
    free(json);
    if (tree != NULL) {
        fprintf(stderr, "depth %d unexpectedly succeeded\n",
                YAJL_MAX_DEPTH + 1);
        yajl_tree_free(tree);
        return 1;
    }
    snprintf(expected, sizeof(expected),
             "Maximum JSON nesting depth (%d) exceeded", YAJL_MAX_DEPTH);
    if (strstr(error, expected) == NULL) {
        fprintf(stderr, "unexpected error: %s\n", error);
        return 1;
    }

    return 0;
}

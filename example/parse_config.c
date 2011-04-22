#include <stdio.h>
#include <string.h>

#include "yajl/yajl_tree.h"

static unsigned char fileData[65536];

int
main(void)
{
    size_t rd;
    yajl_value_t * node;
    char errbuf[1024];

    /* null plug buffers */
    fileData[0] = errbuf[0] = 0;

    /* read the entire config file */
    rd = fread((void *) fileData, 1, sizeof(fileData) - 1, stdin);

    if (rd == 0 && !feof(stdin)) {
        fprintf(stderr, "error encountered on file read\n");
        return 1;
    }

    if (rd >= sizeof(fileData) - 1) {
        fprintf(stderr, "config file too big\n");
        return 1;
    }

    /* we have the whole config file in memory.  let's parse it */ 
    node = yajl_tree_parse((const char *) fileData, errbuf, sizeof(errbuf));

    /* error handling */
    if (node == NULL) {
        fprintf(stderr, "parse_error: ");
        if (strlen(errbuf)) fprintf(stderr, " %s", errbuf);
        else fprintf(stderr, "unknown error");
        fprintf(stderr, "\n");
        return 1;
    }

    /* now extract a nested value from the config file */
    {
        const char * path[] = { "Logging", "timeFormat", (const char *) 0 };
        yajl_value_t * v = yajl_tree_get(node, path, YAJL_TYPE_STRING);
        if (v) printf("Logging/timeFomat: %s\n", YAJL_TO_STRING(v));
        else   printf("no such node: %s/%s\n", path[0], path[1]);
    }

    yajl_tree_free(node);

    return 0;
}

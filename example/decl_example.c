/*
 * Copyright (c) 2012, Daniel Calandria <dcalandria@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>

#include <yajl/yajl_decl.h>

/* usage reporting config struct */
typedef struct
{
  int enabled;
  int url;
  int id;
} usage_config_t;

/* logging config struct */

typedef struct
{
  char *level;
  char *dest;
} logging_config_t;

/* config struct */
typedef struct
{
  char *BuildType;
  char *DistServer;
  char **SecondaryDistServers;
  unsigned int SDS_size;

  
  usage_config_t *UsageReporting;
  logging_config_t *Logging;
} config_t;


/* DECLARE JSON OBJECT */
YAJL_OBJECT_BEGIN(usage_config_t)
  YAJL_FIELD_BOOLEAN(enabled)
  YAJL_FIELD_BOOLEAN(url)
  YAJL_FIELD_BOOLEAN(id)
YAJL_OBJECT_END(usage_config_t)

YAJL_OBJECT_BEGIN(logging_config_t)
  YAJL_FIELD_STRING(level)
  YAJL_FIELD_STRING(dest)
YAJL_OBJECT_END(logging_config_t)

YAJL_OBJECT_BEGIN(config_t)
  YAJL_FIELD_STRING(BuildType)
  YAJL_FIELD_STRING(DistServer)
  YAJL_ARRAY_STRING_S(SecondaryDistServers, SDS_size )
  YAJL_FIELD_OBJECT(usage_config_t,   UsageReporting )
  YAJL_FIELD_OBJECT(logging_config_t, Logging )
YAJL_OBJECT_END(config_t)

static unsigned char fileData[65536];

int main ( int argc, char **argv )
{
  size_t rd;
  char errbuf[1024];
  config_t *config = NULL;
  int i = 0;
  
  /* null plug buffers */
  fileData[0] = errbuf[0] = 0;

  /* read the entire config file */
  rd = fread((void *) fileData, 1, sizeof(fileData) - 1, stdin);

  /* file read error handling */
  if (rd == 0 && !feof(stdin))
    {
      fprintf(stderr, "error encountered on file read\n");
      return 1;
    }
  else if (rd >= sizeof(fileData) - 1)
    {
      fprintf(stderr, "config file too big\n");
      return 1;
    }

  config = YAJL_PARSE_BEGIN(config_t);
  YAJL_PARSE ( config_t, fileData, strlen(fileData) );
  YAJL_PARSE_END(config_t);

  /* printf results */
  printf("BuildType:       %s\n", config->BuildType );
  printf("DistServer:      %s\n", config->DistServer );
  printf("SecondaryDistServers:\n" );
  for (i = 0; i < config->SDS_size; ++i)
    printf("  %d:  %s\n", i, config->SecondaryDistServers[i]);
  printf("usage reporting: \n");
  printf("  enabled:   %s\n", (config->UsageReporting->enabled ? "TRUE": "FALSE"));
  printf("  url:       %s\n", (config->UsageReporting->url ? "TRUE": "FALSE"));
  printf("  id:        %s\n", (config->UsageReporting->id ? "TRUE": "FALSE"));
  printf("logging config: \n");
  printf("  level:     %s\n", config->Logging->level );
  printf("  dest:      %s\n", config->Logging->dest );

  /*
    TODO:
       free the memory allocated by decl api could be tedious.
          ---> provide a bargabe collector to free memory!!!
   */
  
  return 0;
}

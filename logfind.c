#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include "dbg.h"

#define MAX_ARGS 5
#define MAX_LOG_FILES 100
#define LOGS_SRC ((const char *) ".logfind")

struct Logfiles {
    int count;
    char **files;
};

int get_term_count(char **terms, size_t count)
{
    int total = 0;
    for ( int i = 0; i < count; i++ )
    {
        total += terms[i] != NULL ? 1 : 0;
    }

    return total;
}

int search_file(char *filename, char **terms, int num_terms, int OR_FIND)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    
    FILE *f = fopen(filename, "r");
    check(f, "Failed to open %s", filename);

    while ( (read = getline(&line, &len, f)) != -1 )
    {
        int matches = 0;
        for ( int i = 0; i < num_terms; i++ )
        {
            char *found = strstr(line, terms[i]);
            if ( !OR_FIND && !found )
            { // if our search is AND
                break;
            }
            else if ( found )
            {
                matches++;
                if ( OR_FIND )
                { // we've found a match on OR
                    printf("%s", line);
                    break;
                }
            }
        }

        if ( !OR_FIND && matches == num_terms )
        {
            printf("%s", line);
        }
    }

    if ( f ) fclose(f);
    if ( line ) free(line);
    return 0;
error:
    if ( f ) fclose(f);
    if ( line ) free(line);
    return -1;
}

int get_logfiles(struct Logfiles *logs)
{
    char *line = NULL;
    char *copy = NULL;
    int glob_flags = GLOB_DOOFFS;
    int rc = 0;
    size_t len = 0;
    ssize_t read;
    glob_t globbuf;
    globbuf.gl_offs = 0;
    
    logs->files = malloc(sizeof(char *)*MAX_LOG_FILES);
    
    FILE *logconf = fopen(LOGS_SRC, "r");
    check(logconf, "Failed to open %s.", LOGS_SRC);

    while ( (read = getline(&line, &len, logconf)) != -1 && logs->count <= MAX_LOG_FILES )
    {
        // http://stackoverflow.com/a/17250527/1893525
        // strip new line
        line[strcspn(line, "\n")] = 0;
        if ( strstr(line, "*") != NULL )
        {
            // make glob append if after the first time
            if(globbuf.gl_pathc > 0) glob_flags |= GLOB_APPEND;

            rc = glob(line, glob_flags, NULL, &globbuf);
            check(rc == 0 || rc == GLOB_NOMATCH, "Converting glob %s to files failed", line);
        }
        else
        {
            copy = malloc(len);
            check_mem(copy);

            memcpy(copy, line, len);
            logs->files[logs->count] = copy;
            logs->count++;
        }
    }

    if ( logs->count < MAX_LOG_FILES && globbuf.gl_pathc > 0 )
    {
        for ( int i = 0; i < globbuf.gl_pathc; i++ )
        {
            len = strlen(globbuf.gl_pathv[i]);
            copy = malloc(len);
            check_mem(copy);

            memcpy(copy, globbuf.gl_pathv[i], len);
            logs->files[logs->count] = copy;
            logs->count++;

            if ( logs->count == MAX_LOG_FILES ) break;
        }
    }

    globfree(&globbuf);
    if ( logconf ) fclose(logconf);
    if ( copy ) free(copy);
    
    return 0;

error:
    globfree(&globbuf);
    if ( logconf ) fclose(logconf);
    if ( copy ) free(copy);
    return -1;
}

int main(int argc, char *argv[])
{
    check(argc > 1 && argc < MAX_ARGS+2,
            "\nUSAGE: logfind [-o] <search_term(s)>...\n[-o] Use OR instead of AND for terms\nMax terms: 4");

    int OR_FIND = 0;
    int cur_index = 0;
    int rc = 0;
    char *terms[MAX_ARGS-1];
    size_t items = 0;

    for ( int i = 1; i < argc; i++ )
    {
        if ( strncmp(argv[i], "-o", 2) == 0 )
        {
            OR_FIND = 1;
        }
        else
        {
            cur_index = OR_FIND ? i - 2 : i - 1;
            terms[cur_index] = argv[i];
        }
    }

    if ( argc == 2 && OR_FIND ) printf("At least 1 search term must be provided\n");
    
    items = sizeof(terms)/sizeof(terms[0]);
    int num_terms = get_term_count(terms, items);
    struct Logfiles logs = {.count = 0};

    rc = get_logfiles(&logs);
    check(rc == 0, "Searching for log files failed");
    
    for ( int i = 0; i < MAX_LOG_FILES; i++ )
    {
        if ( logs.files[i] == NULL ) break;
        printf("File: %s\n", logs.files[i]);
        rc = search_file(logs.files[i], terms, num_terms, OR_FIND);
        check(rc == 0, "File search failed");
    }

    free(logs.files);
    return 0;

error:
    return -1;
}

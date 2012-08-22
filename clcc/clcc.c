#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "compiler.h"

int main(int argc, char **argv)
{
    //const char source[] = "__kernel svoid test(__global int *input, __global int *output) { int i = get_global_id(0); output[i] = input[i]; }";
    char *source;
    //const char options[] = "-cl-nv-verbose -cl-nv-arch sm_12 --cl-nv-cstd=CL1.0";
    char *options;
    //const char *strings[] = { source };
    const char *strings[1];
    //const size_t lengths[] = { sizeof(source) };
    size_t lengths[1];
    const unsigned int count = 1;
    char *binary = NULL;
    char *log = NULL;
    int result;
    FILE *sourceFile = NULL;
    char *sourceFilename = NULL;
    FILE *binaryFile = NULL;
    char *binaryFilename = NULL;
    struct stat sourceStat;
    size_t size;

    if (argc < 3 || argc > 4)
    {
        printf("Usage: clcc [\"compiler-options\"] input.cl output.ptx\n");
        exit(EXIT_SUCCESS);
    }
    sourceFilename = argv[argc - 2];
    binaryFilename = argv[argc - 1];
    if (argc > 3)
    {
        options = argv[1];
    }
    else
    {
        options = "";
    }

    sourceFile = fopen(sourceFilename, "r");
    if (sourceFile == NULL)
    {
        perror(sourceFilename);
        exit(EXIT_FAILURE);
    }

    result = stat(sourceFilename, &sourceStat);
    if (result != 0)
    {
        perror(sourceFilename);
        exit(EXIT_FAILURE);
    }

    source = malloc(sourceStat.st_size + 1);
    if (source == NULL)
    {
        fprintf(stderr, "malloc: Unable to allocate memory for source file.");
        exit(EXIT_FAILURE);
    }
    source[sourceStat.st_size] = 0;

    size = fread(source, sizeof(*source), sourceStat.st_size / sizeof(*source), sourceFile);
    if (size != sourceStat.st_size / sizeof(*source))
    {
        perror(sourceFilename);
        exit(EXIT_FAILURE);
    }

    strings[0] = source;
    lengths[0] = strlen(source);
    result = NvCliCompileProgram(strings, count, lengths, options, &log, &binary);
    
    if (result != 0)
    {
        //printf("\n%s", log); // TODO: replace <program source> with better one (like the drivers do)
         
        char* temp;
        temp = strtok (log,"\n");
         while (temp != NULL)
  	    {
  	    	if(temp[0] == ':') printf("%s", sourceFilename);	
    		printf ("%s\n",temp);
    		temp = strtok (NULL, "\n");    	
    	}	
        NvCliCompileLogFree(log);
        exit(EXIT_FAILURE);
        
    }
    assert(binary != NULL);


    binaryFile = fopen(binaryFilename, "w");
    if (binaryFile == NULL)
    {
        perror(binaryFilename);
        exit(EXIT_FAILURE);
    }

    size = fwrite(binary, sizeof(*binary), strlen(binary), binaryFile);
    if (size != strlen(binary))
    {
        perror(binaryFilename);
        exit(EXIT_FAILURE);
    }

    free(source);
    fclose(sourceFile);
    NvCliCompiledProgramFree(binary);
    fclose(binaryFile);

    return EXIT_SUCCESS;
}
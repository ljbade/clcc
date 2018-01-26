#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "compiler.h"

int doHelp = 0;

void parseOptions(int argc, char* argv[])
{
    for (int i=0; i < argc; i++)
    {
        if (strcmp(argv[i], "--help") == 0)
            doHelp = 1;
    }
    
    if (argc < 3 || argc > 4)
        doHelp = 1;
}

void usage()
{
    printf("Usage: clcc [\"compiler-options\"] input.cl output.ptx\n");
    printf("Where options can be...\n");
    printf("\n");
    printf("-cl-nv-verbose\n");
    printf("\n");

    // ??
    printf("-cl-opt-disable\n");
    printf("\n");
    printf("-cl-single-precision-constant\n");
    printf("\n");
    printf("-cl-denorms-are-zero\n");
    printf("\n");
    printf("-cl-fp32-correctly-rounded-divide-sqrt\n");
    printf("\n");
    printf("-cl-mad-enable\n");
    printf("\n");
    printf("-cl-no-signed-zeros\n");
    printf("\n");
    printf("-cl-unsafe-math-optimizations\n");
    printf("\n");
    printf("-cl-finite-math-only\n");
    printf("\n");
    printf("-cl-fast-relaxed-math\n");
    printf("\n");
    // ?
    
    printf("-cl-nv-arch\n");
    printf("\n");

    printf("-cl-nv-cstd=<version>\n");
    printf("   version is the OpenCL version supported such as CL1.0 or CL1.1 ");
    printf("\n");
    printf("-cl-nv-maxrregcount <N>\n");
    printf("   Passed on to ptxas as --maxrregcount <N>\n");
    printf("   N is a positive integer.\n");
    printf("   Specify the maximum number of registers that GPU functions can use.\n");
    printf("\n");
    printf("-cl-nv-opt-level <N>\n");
    printf("   Passed on to ptxas as --opt-level <N>\n");
    printf("   N is a positive integer, or 0 (no optimization).\n");
    printf("\n");
    printf("-cl-nv-verbose\n");
    printf("   Passed on to ptxas as --verbose\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
	// Some test arguments:
    // const char source[] = "__kernel void test(__global int *input, __global int *output) { int i = get_global_id(0); output[i] = input[i]; }";
    // const char options[] = "-cl-nv-verbose -cl-nv-arch sm_12 -cl-nv-cstd=CL1.0";
    // const char *strings[] = { source };
    // const size_t lengths[] = { sizeof(source) };

    char *source;
    char *options;
    const char *strings[1];
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

    parseOptions(argc, argv);
    
    if (doHelp)
        usage();
    
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

    sourceFile = fopen(sourceFilename, "rb");
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

    source = (char *)malloc(sourceStat.st_size + 1);
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
    
    //if (result != 0)
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
        
        if (result != 0)
            exit(EXIT_FAILURE);
    }
    assert(binary != NULL);


    binaryFile = fopen(binaryFilename, "w+b");
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

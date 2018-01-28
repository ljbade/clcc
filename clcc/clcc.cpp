#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "OpenCLUtils.h"

#include "compiler.h"

int doHelp = 0;
int doOpenCLRuntime = 0;

char *options;
char *sourceFilename = NULL;
char *binaryFilename = NULL;

void compileWithOpenCLRuntime(const char *strings[], size_t lengths[], const unsigned int count);
void compileWithNativeLibrary(const char *strings[], size_t lengths[], const unsigned int count);

void parseOptions(int argc, char* argv[])
{
    for (int i=0; i < argc; i++)
    {
        if (strcmp(argv[i], "--help") == 0)
        {
            doHelp = 1;
        }
        else if (strcmp(argv[i], "--opencl-runtime") == 0)
        {
            doOpenCLRuntime = 1;
        }   
        else if (strcmp(argv[i], "--flags") == 0)
        {
            i++;
            options = argv[i];
        }
        else if (strcmp(argv[i], "-i") == 0)
        {
            i++;
            sourceFilename = argv[i];    
        }
        else if (strcmp(argv[i], "-o") == 0)
        {
            i++;
            binaryFilename = argv[i];
        }
    }

    if (doHelp)
        return;
    
    if (sourceFilename == NULL)
    {
        fprintf(stderr, "No input file specified\n");
        exit(-1);
    }
    if (binaryFilename == NULL)
    {
        fprintf(stderr, "No output file specified\n");
        exit(-1);
    }
        
}

void usage()
{
    printf("Usage: clcc [options] --flags [\"compiler-options\"] -i input.cl -o output.ptx\n");
    
    printf("\n");
    printf("Where options are..\n");
    printf("\n");
    printf("--help\n");
    printf("\n");
    printf("--opencl-runtime\n");
    printf("\n");

    printf("\n");
    printf("Where flags can be...\n");
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
    char *source;
    const char *strings[1];
    size_t lengths[1];
    const unsigned int count = 1;

    FILE *sourceFile = NULL;
    struct stat sourceStat;
    size_t size;

    parseOptions(argc, argv);
    
    if (doHelp)
        usage();
    
    sourceFile = fopen(sourceFilename, "rb");
    if (sourceFile == NULL)
    {
        perror(sourceFilename);
        exit(EXIT_FAILURE);
    }

    int result = stat(sourceFilename, &sourceStat);
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
    
    if (doOpenCLRuntime)
        compileWithOpenCLRuntime(strings, lengths, count);
    else
        compileWithNativeLibrary(strings, lengths, count);
    
    free(source);
    fclose(sourceFile);

    return EXIT_SUCCESS;
}

    
void compileWithOpenCLRuntime(const char *strings[], size_t lengths[], const unsigned int count)
{
    cl_uint spi = 0;
    cl_uint sdi = 0;

    cl_platform_id pi = selectPlatform(spi);
    cl_device_id di = selectDevice(pi, sdi);
    cl_context ct = createContext(pi, di);
    
    cl_int ret;
    cl_program program = clCreateProgramWithSource(ct, 1, strings, lengths, &ret);
    SAMPLE_CHECK_ERRORS(ret);

    
    ret = clBuildProgram(program, 1, &di, options, NULL, NULL);

    //if (ret == CL_BUILD_PROGRAM_FAILURE) 
    {
        // Determine the size of the log
        size_t log_size;
        ret = clGetProgramBuildInfo(program, di, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        SAMPLE_CHECK_ERRORS(ret);

        // Allocate memory for the log
        char *log = (char *) malloc(log_size);

        // Get the log
        ret = clGetProgramBuildInfo(program, di, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        SAMPLE_CHECK_ERRORS(ret);

        // Print the log
        printf("Compilation Flags:\n");
        printf(options);
        printf("\n");
        printf("Compilation Output Log >>>>>\n");
        printf(log);
        printf("\n<<<<<<\n");
    }

    SAMPLE_CHECK_ERRORS(ret);

}

void compileWithNativeLibrary(const char *strings[], size_t lengths[], const unsigned int count)
{
    char *log = NULL;
    char *binary = NULL;
    FILE *binaryFile = NULL;
    size_t size;

    int result = NvCliCompileProgram(strings, count, lengths, options, &log, &binary);
    
    //if (result != 0)
    {
        //printf("\n%s", log); // TODO: replace <program source> with better one (like the drivers do)
         
        printf(log);	
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

    NvCliCompiledProgramFree(binary);
    fclose(binaryFile);
    
}
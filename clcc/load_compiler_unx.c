#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

#include "compiler.h"

void *libnvidiacompiler = NULL;
tNvCliCompileProgram pNvCliCompileProgram;
tNvCliCompileLogFree pNvCliCompileLogFree;
tNvCliCompiledProgramFree pNvCliCompiledProgramFree;
int loaded = 0;

void unload_compiler(void)
{
    if (!loaded)
    {
        return;
    }

    pNvCliCompileProgram = NULL;
    pNvCliCompileLogFree = NULL;
    pNvCliCompiledProgramFree = NULL;

    dlclose(libnvidiacompiler);
    libnvidiacompiler = NULL;

    loaded = 0;
}

void load_compiler()
{
    if (loaded)
    {
        return;
    }

    libnvidiacompiler = dlopen("libnvidia-compiler.so", RTLD_GLOBAL | RTLD_LAZY);
    if (libnvidiacompiler == NULL)
    {
        fprintf(stderr, "%s\n", dlerror());
        fprintf(stderr, "You may need to create a symlink to /usr/lib/libnvidia-compiler.so.XXX.XX.XX\n");
        exit(EXIT_FAILURE);
    }

    pNvCliCompileProgram = (tNvCliCompileProgram*) dlsym(libnvidiacompiler, "NvCliCompileProgram");
    if (pNvCliCompileProgram == NULL)
    {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    pNvCliCompileLogFree = (tNvCliCompileLogFree*) dlsym(libnvidiacompiler, "NvCliCompileLogFree");
    if (pNvCliCompileLogFree == NULL)
    {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    pNvCliCompiledProgramFree = (tNvCliCompiledProgramFree*) dlsym(libnvidiacompiler, "NvCliCompiledProgramFree");
    if (pNvCliCompiledProgramFree == NULL)
    {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    atexit(unload_compiler);

    loaded = 1;
}
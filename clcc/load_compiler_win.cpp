#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include "compiler.h"

HMODULE nvcompiler = NULL;
tNvCliCompileProgram pNvCliCompileProgram;
tNvCliCompileLogFree pNvCliCompileLogFree;
tNvCliCompiledProgramFree pNvCliCompiledProgramFree;
int loaded = 0;

void unload_compiler(void)
{
    if (!loaded)
    {
        return; // 0;
    }

    pNvCliCompileProgram = NULL;
    pNvCliCompileLogFree = NULL;
    pNvCliCompiledProgramFree = NULL;

    FreeLibrary(nvcompiler);
    nvcompiler = NULL;

    loaded = 0;

	// return 0;
}

void print_error()
{
    char *message = NULL;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        GetLastError(),
        0,
        (LPSTR)&message,
        0,
        NULL);
#ifdef _WIN64
    fprintf(stderr, "nvcompiler.dll: %s\n", message);
#else
    fprintf(stderr, "nvcompiler32.dll: %s\n", message);
#endif
    LocalFree(message);
}

void load_compiler()
{
    if (loaded)
    {
        return;
    }

#ifdef _WIN64
    nvcompiler = LoadLibrary("nvcompiler.dll");
#else 
    nvcompiler = LoadLibrary("nvcompiler32.dll");
#endif
    if (nvcompiler == NULL)
    {
        print_error();
        exit(EXIT_FAILURE);
    }

    pNvCliCompileProgram = (tNvCliCompileProgram) GetProcAddress(nvcompiler, "NvCliCompileProgram");
    if (pNvCliCompileProgram == NULL)
    {
        print_error();
        exit(EXIT_FAILURE);
    }

    pNvCliCompileLogFree = (tNvCliCompileLogFree) GetProcAddress(nvcompiler, "NvCliCompileLogFree");
    if (pNvCliCompileLogFree == NULL)
    {
        print_error();
        exit(EXIT_FAILURE);
    }

    pNvCliCompiledProgramFree = (tNvCliCompiledProgramFree) GetProcAddress(nvcompiler, "NvCliCompiledProgramFree");
    if (pNvCliCompiledProgramFree == NULL)
    {
        print_error();
        exit(EXIT_FAILURE);
    }

    //_onexit(unload_compiler);
    atexit(unload_compiler);

    loaded = 1;
}
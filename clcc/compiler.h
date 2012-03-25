int NvCliCompileProgram(const char **sourceStrings, unsigned int sourceStringsCount, const size_t *sourceStringsLengths, const char *compilerOptions, char **compileLogRet, char **compiledProgramRet);
void NvCliCompileLogFree(const char *compileLog);
void NvCliCompiledProgramFree(const char *compiledProgram);

typedef int (*tNvCliCompileProgram) (const char **sourceStrings, unsigned int sourceStringsCount, const size_t *sourceStringsLengths, const char *compilerOptions, char **compileLogRet, char **compiledProgramRet);
typedef void (*tNvCliCompileLogFree) (const char *compileLog);
typedef void (*tNvCliCompiledProgramFree) (const char *compiledProgram);
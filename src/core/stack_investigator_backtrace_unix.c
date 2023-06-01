//
// file:		crash_investigator_backtrace_unix.cpp
// path:		src/core/backtrace/crash_investigator_backtrace_unix.cpp
// created by:	Davit Kalantaryan (davit.kalataryan@desy.de)
// created on:	2021 Nov 25
//


#ifdef _MSC_VER
#pragma warning(disable:4206)
#endif

#ifndef STACK_INVEST_DO_NOT_USE_STACK_INVESTIGATION
#if !defined(_WIN32) || defined(__INTELLISENSE__)

#include <stack_investigator/export_symbols.h>

#if !defined(__EMSCRIPTEN__)
#define CRASH_INVESTEXECINFO_DEFINED
#endif

#if defined(__linux__) || defined(__linux)
#define CRASH_INVEST_PRCTL_DEFINED
#endif

#include <stack_investigator/investigator.h>
#include "stack_investigator_private_internal.h"
#include <string.h>
#include <alloca.h>
#include <stdio.h>
#include <unistd.h>
#ifdef CRASH_INVESTEXECINFO_DEFINED
#include <execinfo.h>
#else
static int backtrace(void **buffer, int size){
    (void)buffer;
    (void)size;
    return 0;
}
static char **backtrace_symbols(void *const *buffer, int size){
    (void)buffer;
    (void)size;
    return NULL;
}
#endif
#ifdef CRASH_INVEST_PRCTL_DEFINED
#include <sys/prctl.h>
#include <sys/wait.h>
#endif

#ifdef CRASH_INVEST_PRCTL_DEFINED

STACK_INVEST_EXPORT void StackInvestPrintTrace(void)
{
    char pid_buf[30];
    snprintf(pid_buf,29, "%d", getpid());
    char name_buf[512];
    name_buf[readlink("/proc/self/exe", name_buf, 511)]=0;
    prctl(PR_SET_PTRACER, PR_SET_PTRACER_ANY, 0, 0, 0);
    int child_pid = fork();
    if (!child_pid) {
        dup2(2,1); // redirect output to stderr - edit: unnecessary?
        execl("/usr/bin/gdb", "gdb", "--batch", "-n", "-ex", "thread", "-ex", "bt", name_buf, pid_buf, NULL);
        abort(); /* If gdb failed to start */
    } else {
        waitpid(child_pid,NULL,0);
    }
}

#else

STACK_INVEST_EXPORT void StackInvestPrintTrace(void){}

#endif


#define STACK_INVEST_SYMBOLS_COUNT_MAX  256

STACK_INVEST_EXPORT struct StackInvestBacktrace* StackInvestInitBacktraceDataForCurrentStack(int a_goBackInTheStackCalc)
{
	struct StackInvestBacktrace* pReturn = CPPUTILS_STATIC_CAST(struct StackInvestBacktrace*, STACK_INVEST_ANY_ALLOC(sizeof(struct StackInvestBacktrace)));
    if(!pReturn){return CPPUTILS_NULL;}

    const int cnMaxSymbolCount = STACK_INVEST_SYMBOLS_COUNT_MAX +a_goBackInTheStackCalc;

    pReturn->reserved01 = 0;

    void** ppBuffer = CPPUTILS_STATIC_CAST(void**,alloca(CPPUTILS_STATIC_CAST(size_t,cnMaxSymbolCount)*sizeof(void*)));
    int nInitialDeepness = backtrace(ppBuffer,cnMaxSymbolCount);
    if(nInitialDeepness>a_goBackInTheStackCalc){
        pReturn->stackDeepness = nInitialDeepness-a_goBackInTheStackCalc;

    }
    else{
        pReturn->stackDeepness = nInitialDeepness;
    }
    pReturn->ppBuffer = CPPUTILS_STATIC_CAST(void**, STACK_INVEST_ANY_ALLOC(CPPUTILS_STATIC_CAST(size_t,pReturn->stackDeepness)*sizeof(void*)));
    if(!(pReturn->ppBuffer)){STACK_INVEST_ANY_FREE(pReturn);return CPPUTILS_NULL;}
    pReturn->hashIsNotValid = 1;
    memcpy(pReturn->ppBuffer,&(ppBuffer[a_goBackInTheStackCalc]), CPPUTILS_STATIC_CAST(size_t,pReturn->stackDeepness)*sizeof(void*));

    return pReturn;
}


STACK_INVEST_EXPORT void StackInvestConvertBacktraceToNamesRaw(const struct StackInvestBacktrace* a_data, struct StackInvestStackItem* a_pStack, size_t a_bufferSize)
{
    if(a_data){
        size_t i =0;
        const size_t cunSynbols = CPPUTILS_STATIC_CAST(size_t,a_data->stackDeepness)>a_bufferSize?a_bufferSize:CPPUTILS_STATIC_CAST(size_t,a_data->stackDeepness);
        char** ppStrings = backtrace_symbols(a_data->ppBuffer,a_data->stackDeepness);
        if(!ppStrings){return;}

        for(; i < cunSynbols; ++i){
            a_pStack[i].address = a_data->ppBuffer[i];
            a_pStack[i].binFile = strdup(ppStrings[i]);
            a_pStack[i].funcName = strdup("");
            a_pStack[i].sourceFile = strdup("");
			a_pStack[i].reserved01 = 0;
			a_pStack[i].line = -1;
        }

        STACK_INVEST_C_LIB_FREE_NO_CLBK(ppStrings);
    }
}





#endif  // #ifndef _WIN32
#endif // #ifndef CRASH_INVEST_DO_NOT_USE_AT_ALL

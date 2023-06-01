//
// file:		crash_investigator_backtrace_unix.cpp
// path:		src/core/backtrace/crash_investigator_backtrace_unix.cpp
// created by:	Davit Kalantaryan (davit.kalataryan@desy.de)
// created on:	2021 Nov 25
//


#ifdef _MSC_VER
#pragma warning(disable:4206)
#endif

#if !defined(_WIN32) || defined(__INTELLISENSE__)

#include <stack_investigator/export_symbols.h>

#if !defined(__EMSCRIPTEN__)
#define CRASH_INVESTEXECINFO_DEFINED
#endif

#if !defined(STACK_INVEST_NOT_USE_LIBDWARF) && !defined(STACK_INVEST_LIBDWARF_USED)
#if defined(__linux__) || defined(__linux)
#define STACK_INVEST_LIBDWARF_USED
#endif
#endif  //  #if !defined(STACK_INVEST_NOT_USE_LIBDWARF) && !defined(STACK_INVEST_LIBDWARF_USED)

#include <stack_investigator/investigator.h>
#include "stack_investigator_private_internal.h"
#include <string.h>
#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef CRASH_INVESTEXECINFO_DEFINED
#include <execinfo.h>
#endif
#ifdef STACK_INVEST_LIBDWARF_USED
#include <cinternal/hash/dllhash.h>
#include <pthread.h>
#include <libdwarf/libdwarf.h>
#include <libdwarf/dwarf.h>
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


#ifdef STACK_INVEST_LIBDWARF_USED


struct SModuleDwarfEntry{
    size_t              offset;
    CinternalDLLHash_t  addrItemHash;
};


static CinternalDLLHash_t   s_dwarfModulesHash = CPPUTILS_NULL;
static pthread_mutex_t      s_dwarfMutex;


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


static void DeallocateIemFromDwarfModulesHashInline(struct SModuleDwarfEntry* a_pDwarfModule) CPPUTILS_NOEXCEPT
{
    (void)a_pDwarfModule;
}


static void DeallocateIemFromDwarfModulesHash(void* a_pDwarfModule) CPPUTILS_NOEXCEPT
{
    DeallocateIemFromDwarfModulesHashInline(CPPUTILS_STATIC_CAST(struct SModuleDwarfEntry*,a_pDwarfModule));
}


#else   //  #ifdef STACK_INVEST_LIBDWARF_USED


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


#endif  //  #ifdef STACK_INVEST_LIBDWARF_USED

static void stack_investigator_backtrace_unix_cleanup(void){
        
#ifdef STACK_INVEST_LIBDWARF_USED
    
    CInternalDLLHashDestroyEx(s_dwarfModulesHash,&DeallocateIemFromDwarfModulesHash);
    pthread_mutex_destroy(&s_dwarfMutex);
    
#endif  //  #ifdef STACK_INVEST_LIBDWARF_USED
    
}

CPPUTILS_CODE_INITIALIZER(stack_investigator_backtrace_unix_initialize){    

#ifdef STACK_INVEST_LIBDWARF_USED
    
    if(pthread_mutex_init(&s_dwarfMutex,CPPUTILS_NULL)){
        exit(1);
    }
    
#endif  //  #ifdef STACK_INVEST_LIBDWARF_USED
    
    s_dwarfModulesHash = CInternalDLLHashCreateExRawMem(1024,& STACK_INVEST_ANY_ALLOC, & STACK_INVEST_ANY_FREE);
    if(!s_dwarfModulesHash){
        pthread_mutex_destroy(&s_dwarfMutex);
        exit(1);
    }
    
    atexit(&stack_investigator_backtrace_unix_cleanup);

}


#endif  // #ifndef _WIN32

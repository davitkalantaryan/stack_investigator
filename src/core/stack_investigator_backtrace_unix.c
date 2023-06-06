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

#include <stack_investigator/investigator.h>
#include "stack_investigator_private_internal.h"
#include "stack_investigator_private_addr_to_details_unix.h"
#include <string.h>
#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef CRASH_INVESTEXECINFO_DEFINED
#include <execinfo.h>
#endif


CPPUTILS_BEGIN_C

#ifdef STACK_INVEST_LIBDWARF_USED
    #define StackInvestDetailsFromFrameAddressMacro StackInvestDetailsFromFrameAddress_Dwarf
#elif defined(STACK_INVEST_ADDRTOLINE_USED)
    #define StackInvestDetailsFromFrameAddressMacro StackInvestDetailsFromFrameAddress_addrtoline
#else
    #define StackInvestDetailsFromFrameAddressMacro StackInvestDetailsFromFrameAddress_empty
#endif

#define STACK_INVEST_SYMBOLS_COUNT_MAX  256

static char* StackInvestStrdupInline(const char* a_cpcString){
    const size_t cunStrLenPlus1 = strlen(a_cpcString)+1;
    char* pcRetStr = CPPUTILS_STATIC_CAST(char*,STACK_INVEST_ANY_ALLOC(cunStrLenPlus1));
    if(!pcRetStr){return CPPUTILS_NULL;}
    memcpy(pcRetStr,a_cpcString,cunStrLenPlus1);
    return pcRetStr;
}


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


STACK_INVEST_EXPORT int StackInvestConvertBacktraceToNamesRaw(const struct StackInvestBacktrace* a_data, size_t a_offset, struct StackInvestStackItem* a_pStack, size_t a_bufferSize)
{
    if(a_data){
        size_t cunBinLen;
        char* pcBinName;
        size_t i =0, j;
        const size_t cunOffset = (a_offset>CPPUTILS_STATIC_CAST(size_t,a_data->stackDeepness))?0:a_offset;
        const size_t cunCountAfterOffset = CPPUTILS_STATIC_CAST(size_t,a_data->stackDeepness)-cunOffset;
        const size_t cunSynbols = cunCountAfterOffset>a_bufferSize?a_bufferSize:cunCountAfterOffset;
        char** ppStrings = backtrace_symbols(a_data->ppBuffer + cunOffset,CPPUTILS_STATIC_CAST(int,cunSynbols));
        if(!ppStrings){return 1;}

        for(; i < cunSynbols; ++i){
            CPPUTILS_STATIC_CAST(void,a_pStack[i].reserved01);
            a_pStack[i].address = a_data->ppBuffer[i+cunOffset];
            cunBinLen = strcspn(ppStrings[i],"([\0");
            pcBinName = CPPUTILS_STATIC_CAST(char*,STACK_INVEST_ANY_ALLOC(cunBinLen+1));
            if(!pcBinName){
                for(j=0;j<i;++j){
                    STACK_INVEST_ANY_FREE(CPPUTILS_CONST_CAST(char*,a_pStack[j].binFile));
                }
                STACK_INVEST_C_LIB_FREE_NO_CLBK(ppStrings);
                return 1;
            }
            memcpy(pcBinName,ppStrings[i],cunBinLen);
            pcBinName[cunBinLen]=0;
            a_pStack[i].binFile = pcBinName;
            if(StackInvestDetailsFromFrameAddressMacro(&a_pStack[i])){
                for(j=0;j<i;++j){
                    STACK_INVEST_ANY_FREE(CPPUTILS_CONST_CAST(char*,a_pStack[j].binFile));
                }
                STACK_INVEST_C_LIB_FREE_NO_CLBK(ppStrings);
                return 1;
            }

            if(a_pStack[i].line<0){
                STACK_INVEST_ANY_FREE(CPPUTILS_CONST_CAST(char*,a_pStack[i].binFile));
                a_pStack[i].binFile = StackInvestStrdupInline(ppStrings[i]);
                if(!a_pStack[i].binFile){
                    for(j=0;j<i;++j){
                        STACK_INVEST_ANY_FREE(CPPUTILS_CONST_CAST(char*,a_pStack[j].binFile));
                    }
                    STACK_INVEST_C_LIB_FREE_NO_CLBK(ppStrings);
                    return 1;
                }
            }  //  if(a_pStack[i].line<0){
        }  //  for(; i < cunSynbols; ++i){

        STACK_INVEST_C_LIB_FREE_NO_CLBK(ppStrings);
        return 0;
    }
    
    return 1;
}


static void stack_investigator_backtrace_unix_cleanup(void){    
}

CPPUTILS_CODE_INITIALIZER(stack_investigator_backtrace_unix_initialize){    
    atexit(&stack_investigator_backtrace_unix_cleanup);
}


CPPUTILS_END_C


#endif  // #ifndef _WIN32

//
// file:		stack_investigator_backtrace_addr_to_details_unix_empty.c
// path:		src/core/stack_investigator_backtrace_addr_to_details_unix_empty.c
// created by:	Davit Kalantaryan (davit.kalataryan@desy.de)
// created on:	2023 Jun 03
//


#ifdef _MSC_VER
#pragma warning(disable:4206)
#endif

#include <stack_investigator/export_symbols.h>

#if !defined(_WIN32) || defined(__INTELLISENSE__)

#include "stack_investigator_private_addr_to_details_unix.h"
#ifndef STACK_INVEST_EMPTY_USED

#include "stack_investigator_private_internal.h"
#include <string.h>


CPPUTILS_BEGIN_C

static char* StackInvestStrdupInline(const char* a_cpcString){
    const size_t cunStrLenPlus1 = strlen(a_cpcString)+1;
    char* pcRetStr = CPPUTILS_STATIC_CAST(char*,STACK_INVEST_ANY_ALLOC(cunStrLenPlus1));
    if(!pcRetStr){return CPPUTILS_NULL;}
    memcpy(pcRetStr,a_cpcString,cunStrLenPlus1);
    return pcRetStr;
}


CPPUTILS_DLL_PRIVATE int StackInvestDetailsFromFrameAddress_empty(struct StackInvestStackItem* a_pStack)
{
    a_pStack->funcName = StackInvestStrdupInline("UnknownFunction");
    if(!(a_pStack->funcName)){return 1;}
    a_pStack->sourceFile = StackInvestStrdupInline("UnknownSource");
    if(!(a_pStack->sourceFile)){
        STACK_INVEST_ANY_FREE(CPPUTILS_CONST_CAST(char*,a_pStack->funcName));
        return 1;
    }
    a_pStack->line = -1;
    return 0;
}

CPPUTILS_END_C


#endif  //  #ifndef STACK_INVEST_EMPTY_USED
#endif  //  #if !defined(_WIN32) || defined(__INTELLISENSE__)

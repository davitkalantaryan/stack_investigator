//
// file:			investigator.h
// path:			include/stack_investigator/investigator.h
// created on:		2022 Jun 09
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef STACK_INVEST_INCLUDE_STACK_INVESTIGATOR_H
#define STACK_INVEST_INCLUDE_STACK_INVESTIGATOR_H

#include <stack_investigator/internal_header.h>
#include <stddef.h>
#include <stdbool.h>

CPPUTILS_BEGIN_C

struct StackInvestBacktrace {
    void**  ppBuffer;
    size_t  hash;
    int     stackDeepness;
    int     hashIsNotValid : 2;
    int     reserved01 : 32;
};

struct StackInvestStackItem {
    const void*     address;
    const char*     binFile;     // exe, or dll
    const char*     funcName;
    const char*     sourceFile;  // empty means unavailable
    int             line;      // <0 means unknown (no debug info available)
    int             reserved01;
};

STACK_INVEST_EXPORT bool IsTheSameStack(const struct StackInvestBacktrace* a_stack1, const struct StackInvestBacktrace* a_stack2);
STACK_INVEST_EXPORT size_t HashOfTheStack(struct StackInvestBacktrace* a_stack);
STACK_INVEST_EXPORT void FreeBacktraceData(struct StackInvestBacktrace* a_data);


CPPUTILS_END_C

#endif  // #ifndef STACK_INVEST_INCLUDE_STACK_INVEST_INTERNAL_HEADER_H

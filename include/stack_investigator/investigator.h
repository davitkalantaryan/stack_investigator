//
// file:			investigator.h
// path:			include/stack_investigator/investigator.h
// created on:		2022 Jun 09
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef STACK_INVEST_INCLUDE_STACK_INVESTIGATOR_H
#define STACK_INVEST_INCLUDE_STACK_INVESTIGATOR_H

#include <stack_investigator/export_symbols.h>
#include <stddef.h>
#include <stdbool.h>

CPPUTILS_BEGIN_C

struct StackInvestBacktrace {
    void**  ppBuffer;
    size_t  hash;
    int     stackDeepness;
    int     hashIsNotValid : 2;
    int     reserved01 : 30;
};

struct StackInvestStackItem {
    const void*     address;
    const char*     binFile;     // exe, or dll
    const char*     funcName;
    const char*     sourceFile;  // empty means unavailable
    int             line;      // <0 means unknown (no debug info available)
    int             reserved01;
};

struct StackInvestOptimalPrint;


STACK_INVEST_EXPORT struct StackInvestBacktrace* StackInvestInitBacktraceDataForCurrentStack(int a_goBackInTheStackCalc);
STACK_INVEST_EXPORT struct StackInvestBacktrace* StackInvestCloneBackTrace(const struct StackInvestBacktrace* a_btr);
STACK_INVEST_EXPORT bool StackInvestIsTheSameStack(const struct StackInvestBacktrace* a_stack1, const struct StackInvestBacktrace* a_stack2);
STACK_INVEST_EXPORT size_t StackInvestHashOfTheStack(struct StackInvestBacktrace* a_stack);
STACK_INVEST_EXPORT void StackInvestFreeBacktraceData(struct StackInvestBacktrace* a_data);
STACK_INVEST_EXPORT int  StackInvestConvertBacktraceToNamesRaw(const struct StackInvestBacktrace* CPPUTILS_ARG_NN a_data, size_t a_offset, struct StackInvestStackItem* a_pStack, size_t a_bufferSize);
STACK_INVEST_EXPORT void StackInvestFreeStackItemData(struct StackInvestStackItem* a_pStack);
STACK_INVEST_EXPORT const struct StackInvestOptimalPrint*  StackInvestOptimalPrintCreate(const struct StackInvestBacktrace* a_data, size_t a_offset,size_t a_count);
STACK_INVEST_EXPORT void StackInvestOptimalPrintPrint(const struct StackInvestOptimalPrint* CPPUTILS_ARG_NN a_opPrintData);
STACK_INVEST_EXPORT void StackInvestOptimalPrintClean(const struct StackInvestOptimalPrint* CPPUTILS_ARG_NN a_opPrintData);

#define StackInvestConvertBacktraceToNames(_data,_pStack)  StackInvestConvertBacktraceToNamesRaw((_data),0,(_pStack),CPPUTILS_STATIC_CAST(size_t,(_data)->stackDeepness))


CPPUTILS_END_C

#endif  // #ifndef STACK_INVEST_INCLUDE_STACK_INVEST_INTERNAL_HEADER_H

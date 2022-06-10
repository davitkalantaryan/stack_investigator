//
// file:		stack_investigator_backtrace_common.cpp
// path:		src/core/stack_investigator_backtrace_common.cpp
// created by:	Davit Kalantaryan (davit.kalataryan@desy.de)
// created on:	2021 Nov 25
//

#ifndef STACK_INVEST_DO_NOT_USE_STACK_INVESTIGATION

#include <stack_investigator/investigator.h>
#include "stack_investigator_private_internal.h"
#include <string.h>

CPPUTILS_BEGIN_C


STACK_INVEST_EXPORT struct StackInvestBacktrace* StackInvestCloneBackTrace(const struct StackInvestBacktrace* a_btr)
{
	if (a_btr) {
		struct StackInvestBacktrace* pReturn = CPPUTILS_STATIC_CAST(struct StackInvestBacktrace*, STACK_INVEST_ANY_ALLOC(sizeof(struct StackInvestBacktrace)));
		if (!pReturn) {
			return pReturn;
		}

		pReturn->ppBuffer = CPPUTILS_STATIC_CAST(void**, STACK_INVEST_ANY_ALLOC(CPPUTILS_STATIC_CAST(size_t, a_btr->stackDeepness) * sizeof(void*)));
		if (!pReturn->ppBuffer) {
			STACK_INVEST_ANY_FREE(pReturn);
			return CPPUTILS_NULL;
		}

		pReturn->stackDeepness = a_btr->stackDeepness;
		pReturn->hash = a_btr->hash;
		pReturn->hashIsNotValid = a_btr->hashIsNotValid;
		pReturn->reserved01 = a_btr->reserved01;

		memcpy(pReturn->ppBuffer, a_btr->ppBuffer, CPPUTILS_STATIC_CAST(size_t, a_btr->stackDeepness) * sizeof(void*));
		return pReturn;
	}
	return CPPUTILS_NULL;
}


STACK_INVEST_EXPORT bool StackInvestIsTheSameStack(const struct StackInvestBacktrace* a_stack1, const struct StackInvestBacktrace* a_stack2)
{
	return (a_stack1->stackDeepness > 0) && (a_stack1->stackDeepness == a_stack2->stackDeepness) &&
		(memcmp(a_stack1->ppBuffer, a_stack2->ppBuffer, CPPUTILS_STATIC_CAST(size_t, a_stack1->stackDeepness)*sizeof(void*)) == 0);
}


STACK_INVEST_EXPORT size_t StackInvestHashOfTheStack(struct StackInvestBacktrace* a_stack)
{
#ifndef _WIN32
    if (a_stack->hashIsNotValid) {
        int i = 0;
        size_t unMult = 1;
        a_stack->hash = 0;
        for (; i < a_stack->stackDeepness; ++i, unMult *= 1000) {
            a_stack->hash += ((size_t)a_stack->ppBuffer[i]) * unMult;
        }
        a_stack->hashIsNotValid = 0;
    }
#endif
    return a_stack->hash;
}


STACK_INVEST_EXPORT void StackInvestFreeBacktraceData(struct StackInvestBacktrace* a_data)
{
    if(a_data){
        STACK_INVEST_ANY_FREE(a_data->ppBuffer);
        STACK_INVEST_ANY_FREE(a_data);
    }
}


STACK_INVEST_EXPORT void StackInvestFreeStackItemData(struct StackInvestStackItem* a_pStack)
{
	STACK_INVEST_C_LIB_FREE_NO_CLBK(CPPUTILS_CONST_CAST(char*,a_pStack->binFile));
	STACK_INVEST_C_LIB_FREE_NO_CLBK(CPPUTILS_CONST_CAST(char*,a_pStack->funcName));
	STACK_INVEST_C_LIB_FREE_NO_CLBK(CPPUTILS_CONST_CAST(char*,a_pStack->sourceFile));
}


CPPUTILS_END_C


#endif // #ifndef STACK_INVEST_DO_NOT_USE_STACK_INVESTIGATION

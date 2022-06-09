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


STACK_INVEST_EXPORT bool IsTheSameStack(const struct StInvestBacktrace* a_stack1, const struct StInvestBacktrace* a_stack2)
{
	return (a_stack1->stackDeepness > 0) && (a_stack1->stackDeepness == a_stack2->stackDeepness) &&
		(memcmp(a_stack1->ppBuffer, a_stack2->ppBuffer, CPPUTILS_STATIC_CAST(size_t, a_stack1->stackDeepness)*sizeof(void*)) == 0);
}


STACK_INVEST_EXPORT size_t HashOfTheStack(struct StInvestBacktrace* a_stack)
{
    if (a_stack->hashIsNotValid) {
        int i = 0;
        size_t unMult = 1;
        a_stack->hash = 0;
        for (; i < a_stack->stackDeepness; ++i, unMult *= 1000) {
            a_stack->hash += ((size_t)a_stack->ppBuffer[i]) * unMult;
        }
        a_stack->hashIsNotValid = 0;
    }
    return a_stack->hash;
}


STACK_INVEST_EXPORT void FreeBacktraceData(struct StInvestBacktrace* a_data)
{
    if(a_data){
        STACK_INVEST_FREE(a_data->ppBuffer);
        STACK_INVEST_FREE(a_data);
    }
}


CPPUTILS_END_C


#endif // #ifndef STACK_INVEST_DO_NOT_USE_STACK_INVESTIGATION

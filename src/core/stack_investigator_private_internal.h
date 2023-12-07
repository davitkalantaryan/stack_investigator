//
// file:			stack_investigator_private_internal.h
// path:			src/core/stack_investigator_private_internal.h
// created on:		2022 Jun 09
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef STACK_INVEST_SRC_CORE_STACK_INVESTIGATOR_PRIVATE_INTERNAL_H
#define STACK_INVEST_SRC_CORE_STACK_INVESTIGATOR_PRIVATE_INTERNAL_H

#include <stack_investigator/export_symbols.h>
#include <stddef.h>

CPPUTILS_BEGIN_C

#ifdef STACK_INVEST_ANY_ALLOC
extern void* STACK_INVEST_ANY_ALLOC(size_t);
#define STACK_INVEST_ANY_ALLOC_D
#else
#define STACK_INVEST_ANY_ALLOC malloc
#endif


#ifdef STACK_INVEST_ANY_REALLOC
extern void* STACK_INVEST_ANY_REALLOC(void*,size_t);
#define STACK_INVEST_ANY_REALLOC_D
#else
#define STACK_INVEST_ANY_REALLOC realloc
#endif


#ifdef STACK_INVEST_ANY_FREE
extern void STACK_INVEST_ANY_FREE(void*);
#define STACK_INVEST_ANY_FREE_D
#else
#define STACK_INVEST_ANY_FREE free
#endif

#ifdef STACK_INVEST_C_LIB_FREE_NO_CLBK
extern void STACK_INVEST_C_LIB_FREE_NO_CLBK(void*);
#define STACK_INVEST_C_LIB_FREE_NO_CLBK_D
#else
#define STACK_INVEST_C_LIB_FREE_NO_CLBK free
#endif


#if !defined(STACK_INVEST_ANY_ALLOC_D) || !defined(STACK_INVEST_FREE_D)  || !defined(STACK_INVEST_C_LIB_FREE_NO_CLBK_D)
#include <stdlib.h>
#endif


CPPUTILS_END_C


#endif  // #ifndef STACK_INVEST_SRC_CORE_STACK_INVESTIGATOR_PRIVATE_INTERNAL_H

//
// file:			stack_investigator_private_internal.h
// path:			src/core/stack_investigator_private_internal.h
// created on:		2022 Jun 09
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef STACK_INVEST_SRC_CORE_STACK_INVESTIGATOR_PRIVATE_INTERNAL_H
#define STACK_INVEST_SRC_CORE_STACK_INVESTIGATOR_PRIVATE_INTERNAL_H

#include <stack_investigator/internal_header.h>
#include <stddef.h>

#if !defined(STACK_INVEST_MALLOC) || !defined(STACK_INVEST_FREE)
#include <stdlib.h>
#endif

#ifdef STACK_INVEST_MALLOC
extern void* STACK_INVEST_MALLOC(size_t);
#else
#define STACK_INVEST_MALLOC malloc
#endif


#ifdef STACK_INVEST_FREE
extern void STACK_INVEST_FREE(void*);
#else
#define STACK_INVEST_FREE free
#endif


#endif  // #ifndef STACK_INVEST_SRC_CORE_STACK_INVESTIGATOR_PRIVATE_INTERNAL_H

//
// file:			stack_investigator_private_mutex.h
// path:			src/core/stack_investigator_private_mutex.h
// created on:		2023 Jun 01
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef STACK_INVEST_SRC_CORE_STACK_INVESTIGATOR_PRIVATE_MUTEX_H
#define STACK_INVEST_SRC_CORE_STACK_INVESTIGATOR_PRIVATE_MUTEX_H

#include <stack_investigator/export_symbols.h>


CPPUTILS_BEGIN_C


#ifdef STACK_INVEST_RW_MUTEX_CREATE
extern void* STACK_INVEST_RW_MUTEX_CREATE(void);
#define STACK_INVEST_RW_MUTEX_CREATE_D
#else
extern CPPUTILS_DLL_PRIVATE void* STACK_INVEST_RW_MUTEX_CREATE_function(void);
#define STACK_INVEST_RW_MUTEX_CREATE STACK_INVEST_RW_MUTEX_CREATE_function
#ifndef STACK_INVEST_RW_MUTEX_ANY_D
#define STACK_INVEST_RW_MUTEX_ANY_D
#endif
#endif

#ifdef STACK_INVEST_RW_MUTEX_DESTROY
extern void STACK_INVEST_RW_MUTEX_DESTROY(void*);
#define STACK_INVEST_RW_MUTEX_DESTROY_D
#else
extern CPPUTILS_DLL_PRIVATE void STACK_INVEST_RW_MUTEX_DESTROY_function(void*);
#define STACK_INVEST_RW_MUTEX_DESTROY STACK_INVEST_RW_MUTEX_DESTROY_function
#ifndef STACK_INVEST_RW_MUTEX_ANY_D
#define STACK_INVEST_RW_MUTEX_ANY_D
#endif
#endif

#ifdef STACK_INVEST_RW_MUTEX_RD_LOCK
extern void STACK_INVEST_RW_MUTEX_RD_LOCK(void*);
#define STACK_INVEST_RW_MUTEX_RD_LOCK_D
#else
extern CPPUTILS_DLL_PRIVATE void STACK_INVEST_RW_MUTEX_RD_LOCK_function(void*);
#define STACK_INVEST_RW_MUTEX_RD_LOCK STACK_INVEST_RW_MUTEX_RD_LOCK_function
#ifndef STACK_INVEST_RW_MUTEX_ANY_D
#define STACK_INVEST_RW_MUTEX_ANY_D
#endif
#endif

#ifdef STACK_INVEST_RW_MUTEX_WR_LOCK
extern void STACK_INVEST_RW_MUTEX_WR_LOCK(void*);
#define STACK_INVEST_RW_MUTEX_WR_LOCK_D
#else
extern CPPUTILS_DLL_PRIVATE void STACK_INVEST_RW_MUTEX_WR_LOCK_function(void*);
#define STACK_INVEST_RW_MUTEX_WR_LOCK STACK_INVEST_RW_MUTEX_WR_LOCK_function
#ifndef STACK_INVEST_RW_MUTEX_ANY_D
#define STACK_INVEST_RW_MUTEX_ANY_D
#endif
#endif

#ifdef STACK_INVEST_RW_MUTEX_RD_UNLOCK
extern void STACK_INVEST_RW_MUTEX_RD_UNLOCK(void*);
#define STACK_INVEST_RW_MUTEX_RD_UNLOCK_D
#else
extern CPPUTILS_DLL_PRIVATE void STACK_INVEST_RW_MUTEX_RD_UNLOCK_function(void*);
#define STACK_INVEST_RW_MUTEX_RD_UNLOCK STACK_INVEST_RW_MUTEX_RD_UNLOCK_function
#ifndef STACK_INVEST_RW_MUTEX_ANY_D
#define STACK_INVEST_RW_MUTEX_ANY_D
#endif
#endif

#ifdef STACK_INVEST_RW_MUTEX_WR_UNLOCK
extern void STACK_INVEST_RW_MUTEX_WR_UNLOCK(void*);
#define STACK_INVEST_RW_MUTEX_WR_UNLOCK_D
#else
extern CPPUTILS_DLL_PRIVATE void STACK_INVEST_RW_MUTEX_WR_UNLOCK_function(void*);
#define STACK_INVEST_RW_MUTEX_WR_UNLOCK STACK_INVEST_RW_MUTEX_WR_UNLOCK_function
#ifndef STACK_INVEST_RW_MUTEX_ANY_D
#define STACK_INVEST_RW_MUTEX_ANY_D
#endif
#endif


CPPUTILS_END_C

#endif  // #ifndef STACK_INVEST_SRC_CORE_STACK_INVESTIGATOR_PRIVATE_MUTEX_H

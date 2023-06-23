//
// file:		stack_investigator_backtrace_mutex_unix.cpp
// path:		src/core/backtrace/stack_investigator_backtrace_mutex_unix.cpp
// created by:	Davit Kalantaryan (davit.kalataryan@desy.de)
// created on:	2023 Jun 01
//


#ifdef _MSC_VER
#pragma warning(disable:4206)
#endif

#include <stack_investigator/export_symbols.h>

#if (!defined(_WIN32)) || defined(__INTELLISENSE__)

#include "stack_investigator_private_mutex.h"
#ifdef STACK_INVEST_RW_MUTEX_ANY_D
#include <stdlib.h>
#include <pthread.h>


CPPUTILS_BEGIN_C


#ifndef STACK_INVEST_RW_MUTEX_CREATE_D

CPPUTILS_DLL_PRIVATE void* STACK_INVEST_RW_MUTEX_CREATE_function(void)
{
    pthread_rwlock_t* pRwLock = CPPUTILS_STATIC_CAST(pthread_rwlock_t*,malloc(sizeof(pthread_rwlock_t)));
    if(!pRwLock){
        return CPPUTILS_NULL;
    }
    if(pthread_rwlock_init(pRwLock,CPPUTILS_NULL)){
        free(pRwLock);
        return CPPUTILS_NULL;
    }
    
    return pRwLock;
}

#endif //  #ifndef STACK_INVEST_RW_MUTEX_CREATE_D


#ifndef STACK_INVEST_RW_MUTEX_DESTROY_D

CPPUTILS_DLL_PRIVATE void STACK_INVEST_RW_MUTEX_DESTROY_function(void* a_pRwMutex)
{
    pthread_rwlock_t* pRwLock = CPPUTILS_STATIC_CAST(pthread_rwlock_t*,a_pRwMutex);
    pthread_rwlock_destroy(pRwLock);
    free(pRwLock);
}

#endif //  #ifndef STACK_INVEST_RW_MUTEX_DESTROY_D


#ifndef STACK_INVEST_RW_MUTEX_RD_LOCK_D

CPPUTILS_DLL_PRIVATE void STACK_INVEST_RW_MUTEX_RD_LOCK_function(void* a_pRwMutex)
{
    pthread_rwlock_t* pRwLock = CPPUTILS_STATIC_CAST(pthread_rwlock_t*,a_pRwMutex);
    pthread_rwlock_rdlock(pRwLock);
}

#endif //  #ifndef STACK_INVEST_RW_MUTEX_RD_LOCK_D


#ifndef STACK_INVEST_RW_MUTEX_WR_LOCK_D

CPPUTILS_DLL_PRIVATE void STACK_INVEST_RW_MUTEX_WR_LOCK_function(void* a_pRwMutex)
{
    pthread_rwlock_t* pRwLock = CPPUTILS_STATIC_CAST(pthread_rwlock_t*,a_pRwMutex);
    pthread_rwlock_wrlock(pRwLock);
}

#endif //  #ifndef STACK_INVEST_RW_MUTEX_WR_LOCK_D


#ifndef STACK_INVEST_RW_MUTEX_RD_UNLOCK_D

CPPUTILS_DLL_PRIVATE void STACK_INVEST_RW_MUTEX_RD_UNLOCK_function(void* a_pRwMutex)
{
    pthread_rwlock_t* pRwLock = CPPUTILS_STATIC_CAST(pthread_rwlock_t*,a_pRwMutex);
    pthread_rwlock_unlock(pRwLock);
}

#endif //  #ifndef STACK_INVEST_RW_MUTEX_RD_UNLOCK_D



#ifndef STACK_INVEST_RW_MUTEX_WR_UNLOCK_D

CPPUTILS_DLL_PRIVATE void STACK_INVEST_RW_MUTEX_WR_UNLOCK_function(void* a_pRwMutex)
{
    pthread_rwlock_t* pRwLock = CPPUTILS_STATIC_CAST(pthread_rwlock_t*,a_pRwMutex);
    pthread_rwlock_unlock(pRwLock);
}

#endif //  #ifndef STACK_INVEST_RW_MUTEX_WR_UNLOCK_D




CPPUTILS_END_C


#endif  //  #ifdef STACK_INVEST_RW_MUTEX_ANY_D
#endif  //  #if (!defined(_WIN32)) || defined(__INTELLISENSE__)

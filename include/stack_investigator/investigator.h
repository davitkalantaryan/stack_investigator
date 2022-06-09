//
// file:			investigator.h
// path:			include/stack_investigator/investigator.h
// created on:		2022 Jun 09
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef STACK_INVEST_INCLUDE_STACK_INVESTIGATOR_H
#define STACK_INVEST_INCLUDE_STACK_INVESTIGATOR_H

#include <stack_investigator/internal_header.h>


#if defined(STACK_INVEST_COMPILING_SHARED_LIB)
    #define STACK_INVEST_EXPORT CPPUTILS_DLL_PUBLIC
#elif defined(STACK_INVEST_USING_STATIC_LIB_OR_OBJECTS)
    #ifdef STACK_INVEST_DEFAULT_VISIBILITY
        #define STACK_INVEST_EXPORT
    #else
        #define STACK_INVEST_EXPORT CPPUTILS_DLL_PRIVATE
    #endif
#else
    #define STACK_INVEST_EXPORT CPPUTILS_IMPORT_FROM_DLL
#endif



#endif  // #ifndef STACK_INVEST_INCLUDE_STACK_INVEST_INTERNAL_HEADER_H

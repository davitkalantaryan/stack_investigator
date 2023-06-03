//
// file:			stack_investigator_private_addr_to_details_unix.h
// path:			src/core/stack_investigator_private_addr_to_details_unix.h
// created on:		2023 Jun 01
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef STACK_INVEST_SRC_CORE_STACK_INVESTIGATOR_PRIVATE_ADDR_TO_DETAILS_UNIX_H
#define STACK_INVEST_SRC_CORE_STACK_INVESTIGATOR_PRIVATE_ADDR_TO_DETAILS_UNIX_H

#include <stack_investigator/export_symbols.h>

#if !defined(_WIN32) || defined(__INTELLISENSE__)

#include <stack_investigator/investigator.h>

#if !defined(STACK_INVEST_NOT_USE_LIBDWARF) && !defined(STACK_INVEST_LIBDWARF_USED)
#if defined(__linux__) || defined(__linux)
#define STACK_INVEST_LIBDWARF_USED
#endif
#endif  //  #if !defined(STACK_INVEST_NOT_USE_LIBDWARF) && !defined(STACK_INVEST_LIBDWARF_USED)


CPPUTILS_BEGIN_C

#ifdef STACK_INVEST_LIBDWARF_USED
CPPUTILS_DLL_PRIVATE int StackInvestDetailsFromFrameAddress_Dwarf(struct StackInvestStackItem* a_pStack);  //  ret 0 is ok
#endif


#ifdef STACK_INVEST_ADDRTOLINE_USED
CPPUTILS_DLL_PRIVATE int StackInvestDetailsFromFrameAddress_addrtoline(struct StackInvestStackItem* a_pStack);  //  ret 0 is ok
#endif


#ifndef STACK_INVEST_EMPTY_USED
CPPUTILS_DLL_PRIVATE int StackInvestDetailsFromFrameAddress_empty(struct StackInvestStackItem* a_pStack);  //  ret 0 is ok
#endif


CPPUTILS_END_C


#endif  //  #if !defined(_WIN32) || defined(__INTELLISENSE__)
#endif  //  #ifndef STACK_INVEST_SRC_CORE_STACK_INVESTIGATOR_PRIVATE_ADDR_TO_DETAILS_UNIX_H

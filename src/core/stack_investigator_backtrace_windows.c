//
// file:		crash_investigator_backtrace_unix.cpp
// path:		src/core/backtrace/crash_investigator_backtrace_unix.cpp
// created by:	Davit Kalantaryan (davit.kalataryan@desy.de)
// created on:	2021 Nov 25
//

#ifndef STACK_INVEST_DO_NOT_USE_STACK_INVESTIGATION
#if defined(_WIN32) || defined(__INTELLISENSE__)

#include <stack_investigator/investigator.h>
#include "stack_investigator_private_internal.h"
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <DbgHelp.h>


// see: https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/bb204633(v=vs.85)?redirectedfrom=MSDN

#define STACK_INVEST_SYMBOLS_COUNT_MAX  62 // Windows Server 2003 and Windows XP: The sum of the FramesToSkip and FramesToCapture parameters must be less than 63

CPPUTILS_DLL_PRIVATE struct StackInvestBacktrace* InitBacktraceDataForCurrentStack(int a_goBackInTheStackCalc)
{
	struct StackInvestBacktrace* pReturn;
	void* vpBuffer[STACK_INVEST_SYMBOLS_COUNT_MAX];
	WORD countOfStacks;
	ULONG ulBtrHash;

	++a_goBackInTheStackCalc;
	countOfStacks = CaptureStackBackTrace(
		CPPUTILS_STATIC_CAST(DWORD, a_goBackInTheStackCalc),
		CPPUTILS_STATIC_CAST(DWORD, STACK_INVEST_SYMBOLS_COUNT_MAX - a_goBackInTheStackCalc),
		vpBuffer, &ulBtrHash);
	if (countOfStacks < 1){return CPPUTILS_NULL;}

	pReturn = CPPUTILS_STATIC_CAST(struct StackInvestBacktrace*, STACK_INVEST_MALLOC(sizeof(struct StackInvestBacktrace)));
	if(!pReturn){return CPPUTILS_NULL;}

	pReturn->stackDeepness = CPPUTILS_STATIC_CAST(int,countOfStacks);

	pReturn->ppBuffer = CPPUTILS_STATIC_CAST(void **, STACK_INVEST_MALLOC(CPPUTILS_STATIC_CAST(size_t,pReturn->stackDeepness) * sizeof(void *)));
	if (!(pReturn->ppBuffer)){
		STACK_INVEST_FREE(pReturn);
		return CPPUTILS_NULL;
	}

	memcpy(pReturn->ppBuffer, vpBuffer, CPPUTILS_STATIC_CAST(size_t,pReturn->stackDeepness) * sizeof(void *));
	return pReturn;
}

static void GetSymbolInfo(struct StackInvestStackItem* a_pItem);

CPPUTILS_DLL_PRIVATE void ConvertBacktraceToNames(const struct StackInvestBacktrace* a_data, struct StackInvestStackItem* a_pStack, size_t a_bufferSize)
{
	size_t i = 0;
	const size_t cunSynbols = CPPUTILS_STATIC_CAST(size_t, a_data->stackDeepness) > a_bufferSize ? a_bufferSize : CPPUTILS_STATIC_CAST(size_t, a_data->stackDeepness);

	for (; i < cunSynbols; ++i) {
		a_pStack[i].reserved01 = 0;
		a_pStack[i].address = a_data->ppBuffer[i];
		GetSymbolInfo(&a_pStack[i]);
	}
}

static HANDLE s_currentProcess = CPPUTILS_NULL;

static void StackInvestCleanupRoutine(void)
{
	if (s_currentProcess) {
		SymCleanup(s_currentProcess);
		s_currentProcess = CPPUTILS_NULL;
	}
}

CPPUTILS_ALLOC_FREE_INITIALIZER(StackInvestInitializationRoutine)
{
	s_currentProcess = GetCurrentProcess();
	if (!SymInitialize(s_currentProcess, CPPUTILS_NULL, TRUE)) {
		// SymInitialize failed
		s_currentProcess = CPPUTILS_NULL;
		DWORD error = GetLastError();
		fprintf(stderr, "SymInitialize returned error : %d\n", CPPUTILS_STATIC_CAST(int,error));
		return;
	}
	atexit(StackInvestCleanupRoutine);
}


#ifdef _WIN64
typedef DWORD64  DWORD_ci;
#else
typedef DWORD  DWORD_ci;
#endif


static void GetSymbolInfo(struct StackInvestStackItem* a_pStackItem)
{
	// https://docs.microsoft.com/en-us/windows/win32/debug/retrieving-symbol-information-by-address
	const DWORD_ci  dwAddress = CPPUTILS_STATIC_CAST(DWORD_ci, CPPUTILS_REINTERPRET_CAST(size_t,a_pStackItem->address));

	
	{
		DWORD64  dwDisplacement = 0;
		char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
		PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;

		pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		pSymbol->MaxNameLen = MAX_SYM_NAME;

		if (SymFromAddr(s_currentProcess, dwAddress, &dwDisplacement, pSymbol)) {
			a_pStackItem->funcName = pSymbol->Name;
		}
		else {
			a_pStackItem->funcName = CPPUTILS_NULL;
		}
	}

	
	
	{
		DWORD  dwDisplacement;
		IMAGEHLP_LINE64 line;

		SymSetOptions(SYMOPT_LOAD_LINES);

		line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

		if (SymGetLineFromAddr64(s_currentProcess, dwAddress, &dwDisplacement, &line)){
			if (line.FileName) {
				a_pStackItem->sourceFile = _strdup(line.FileName);
			}
			else {
				a_pStackItem->sourceFile = CPPUTILS_NULL;
			}
			a_pStackItem->line = CPPUTILS_STATIC_CAST(int,line.LineNumber);
		}
		else{
			// SymGetLineFromAddr64 failed
			a_pStackItem->line = -1;
			a_pStackItem->sourceFile = CPPUTILS_NULL;
		}
	}



	{
		IMAGEHLP_MODULE aModuleInfo;
		aModuleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE);

		if (SymGetModuleInfo(s_currentProcess, dwAddress, &aModuleInfo)) {
			a_pStackItem->binFile = _strdup(aModuleInfo.ImageName);
		}
		else {
			a_pStackItem->binFile = CPPUTILS_NULL;
		}
	}


}



#endif // #if defined(_WIN32) || defined(__INTELLISENSE__)
#endif // #ifndef STACK_INVEST_DO_NOT_USE_STACK_INVESTIGATION

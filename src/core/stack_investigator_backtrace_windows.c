//
// file:		crash_investigator_backtrace_unix.cpp
// path:		src/core/backtrace/crash_investigator_backtrace_unix.cpp
// created by:	Davit Kalantaryan (davit.kalataryan@desy.de)
// created on:	2021 Nov 25
//

#if defined(_WIN32) || defined(__INTELLISENSE__)

#include <stack_investigator/investigator.h>
#include "stack_investigator_private_internal.h"
#include <cinternal/logger.h>
#include <cinternal/lw_mutex_recursive.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cinternal/disable_compiler_warnings.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <DbgHelp.h>
#include <cinternal/undisable_compiler_warnings.h>

#ifdef _MSC_VER
#pragma comment (lib, "Dbghelp.lib")
#endif


// see: https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/bb204633(v=vs.85)?redirectedfrom=MSDN

#define STACK_INVEST_SYMBOLS_COUNT_MAX  62 // Windows Server 2003 and Windows XP: The sum of the FramesToSkip and FramesToCapture parameters must be less than 63


static cinternal_lw_recursive_mutex_t  s_mutex_for_dbg_functions;


STACK_INVEST_EXPORT struct StackInvestBacktrace* StackInvestInitBacktraceDataForCurrentStack(int a_goBackInTheStackCalc)
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

	pReturn = CPPUTILS_STATIC_CAST(struct StackInvestBacktrace*, STACK_INVEST_ANY_ALLOC(sizeof(struct StackInvestBacktrace)));
	if(!pReturn){return CPPUTILS_NULL;}

	pReturn->stackDeepness = CPPUTILS_STATIC_CAST(int,countOfStacks);

	pReturn->ppBuffer = CPPUTILS_STATIC_CAST(void **, STACK_INVEST_ANY_ALLOC(CPPUTILS_STATIC_CAST(size_t,pReturn->stackDeepness) * sizeof(void *)));
	if (!(pReturn->ppBuffer)){
		STACK_INVEST_ANY_FREE(pReturn);
		return CPPUTILS_NULL;
	}

	memcpy(pReturn->ppBuffer, vpBuffer, CPPUTILS_STATIC_CAST(size_t,pReturn->stackDeepness) * sizeof(void *));
	pReturn->hash = CPPUTILS_STATIC_CAST(size_t,ulBtrHash);
	pReturn->reserved01 = 0;
	return pReturn;
}

static void StackInvestGetSymbolInfo(struct StackInvestStackItem* a_pItem);

STACK_INVEST_EXPORT int StackInvestConvertBacktraceToNamesRaw(const struct StackInvestBacktrace* CPPUTILS_ARG_NN a_data, size_t a_offset, struct StackInvestStackItem* a_pStack, size_t a_bufferSize)
{
    if(a_data){
        size_t i = 0;
        const size_t cunOffset = (a_offset>CPPUTILS_STATIC_CAST(size_t,a_data->stackDeepness))?0:a_offset;
        const size_t cunCountAfterOffset = CPPUTILS_STATIC_CAST(size_t,a_data->stackDeepness)-cunOffset;
        const size_t cunSynbols = cunCountAfterOffset>a_bufferSize?a_bufferSize:cunCountAfterOffset;
        
        for (; i < cunSynbols; ++i) {
            CPPUTILS_STATIC_CAST(void,a_pStack[i].reserved01);
            a_pStack[i].address = a_data->ppBuffer[i-cunOffset];
            StackInvestGetSymbolInfo(&a_pStack[i]);
        }
        
    }  //  if(a_data){

	return 0;
}


CPPUTILS_DLL_PRIVATE int StackInvestDetailsFromFrameAddress_Windows(struct StackInvestStackItem* CPPUTILS_ARG_NN a_pStack)
{
#if 0
	struct StackInvestBacktrace {
		void** ppBuffer;
		size_t  hash;
		int     stackDeepness;
		int     hashIsNotValid : 2;
		int     reserved01 : 30;
	};
#endif
	struct StackInvestBacktrace aData;
	void* vpBuffer[2] = { CPPUTILS_CONST_CAST(void*,a_pStack->address),CPPUTILS_NULL };
	aData.ppBuffer = vpBuffer;
	aData.hash = 0;
	aData.hashIsNotValid = 1;
	CPPUTILS_STATIC_CAST(void, aData.reserved01);
	return StackInvestConvertBacktraceToNamesRaw(&aData, 0, a_pStack, 1);
}


struct StackInvestOptimalPrint {
	size_t count;
	struct StackInvestStackItem* m_item_p;
};


STACK_INVEST_EXPORT const struct StackInvestOptimalPrint* StackInvestOptimalPrintCreate(const struct StackInvestBacktrace* a_data, size_t a_offset, size_t a_count)
{
	//struct StackInvestOptimalPrint* pRet = (struct StackInvestOptimalPrint*)
	if (a_offset < a_count) {
		const size_t itemsCount = a_count - a_offset;
		struct StackInvestOptimalPrint* pRet = CPPUTILS_STATIC_CAST(struct StackInvestOptimalPrint*,STACK_INVEST_ANY_ALLOC(sizeof(struct StackInvestOptimalPrint)));
		if (!pRet) { return CPPUTILS_NULL; }
		pRet->m_item_p = CPPUTILS_STATIC_CAST(struct StackInvestStackItem*,STACK_INVEST_ANY_ALLOC(sizeof(struct StackInvestStackItem)* itemsCount));
		if (!(pRet->m_item_p)) {
			STACK_INVEST_ANY_FREE(pRet);
			return CPPUTILS_NULL;
		}
		pRet->count = itemsCount;
		if (StackInvestConvertBacktraceToNamesRaw(a_data, a_offset, pRet->m_item_p, itemsCount)) {
			STACK_INVEST_ANY_FREE(pRet->m_item_p);
			STACK_INVEST_ANY_FREE(pRet);
			return CPPUTILS_NULL;
		}
		return pRet;
	}

	return CPPUTILS_NULL;
	
}

STACK_INVEST_EXPORT void StackInvestOptimalPrintPrint(const struct StackInvestOptimalPrint* CPPUTILS_ARG_NN a_opPrintData)
{
	size_t i;
	CinternalMakeLogNoExtraData(CinternalLogTypeInfo, false, "---");
	CinternalLogPrintDateAndTime(CinternalLogTypeInfo, false);
	CinternalMakeLogNoExtraData(CinternalLogTypeInfo, false, "\n");
	for (i = 0; i < (a_opPrintData->count); ++i) {
		CinternalMakeLogNoExtraData(CinternalLogTypeInfo, false, 
			"    fl: \"%s\", ln: %d, fn: %s\n", 
			(a_opPrintData->m_item_p)[i].sourceFile, (a_opPrintData->m_item_p)[i].line,(a_opPrintData->m_item_p)[i].funcName);
	}
	CinternalMakeLogNoExtraData(CinternalLogTypeInfo, true, "\n");
}


STACK_INVEST_EXPORT void StackInvestOptimalPrintClean(const struct StackInvestOptimalPrint* CPPUTILS_ARG_NN a_opPrintData)
{
	STACK_INVEST_ANY_FREE(a_opPrintData->m_item_p);
	STACK_INVEST_ANY_FREE(CPPUTILS_CONST_CAST(struct StackInvestOptimalPrint* ,a_opPrintData));
}


static HANDLE s_currentProcess = CPPUTILS_NULL;

static void StackInvestCleanupRoutine(void)
{
	if (s_currentProcess) {
		cinternal_lw_recursive_mutex_lock(&s_mutex_for_dbg_functions);
		SymCleanup(s_currentProcess);
		cinternal_lw_recursive_mutex_unlock(&s_mutex_for_dbg_functions);
		s_currentProcess = CPPUTILS_NULL;
	}

	cinternal_lw_recursive_mutex_destroy(&s_mutex_for_dbg_functions);
}

CPPUTILS_C_CODE_INITIALIZER(StackInvestInitializationRoutine)
{
	if (s_currentProcess) { return; }
	if (cinternal_lw_recursive_mutex_create(&s_mutex_for_dbg_functions)) {
		CInternalLogError("Unable create mutex\n");
		exit(1);
	}
	s_currentProcess = GetCurrentProcess();
	cinternal_lw_recursive_mutex_lock(&s_mutex_for_dbg_functions);
	if (!SymInitialize(s_currentProcess, CPPUTILS_NULL, TRUE)) {
		// SymInitialize failed
		cinternal_lw_recursive_mutex_unlock(&s_mutex_for_dbg_functions);
		s_currentProcess = CPPUTILS_NULL;
		DWORD error = GetLastError();
		CInternalLogError("SymInitialize returned error : %d\n", CPPUTILS_STATIC_CAST(int,error));
		exit(1);
	}
	cinternal_lw_recursive_mutex_unlock(&s_mutex_for_dbg_functions);
	atexit(StackInvestCleanupRoutine);
}


#ifdef _WIN64
typedef DWORD64  DWORD_ci;
#else
typedef DWORD  DWORD_ci;
#endif


static void StackInvestGetSymbolInfo(struct StackInvestStackItem* a_pStackItem)
{
	// https://docs.microsoft.com/en-us/windows/win32/debug/retrieving-symbol-information-by-address
	const DWORD_ci  dwAddress = CPPUTILS_STATIC_CAST(DWORD_ci, CPPUTILS_REINTERPRET_CAST(size_t,a_pStackItem->address));

	cinternal_lw_recursive_mutex_lock(&s_mutex_for_dbg_functions);
	
	{
		DWORD64  dwDisplacement = 0;
		char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
		PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;

		pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		pSymbol->MaxNameLen = MAX_SYM_NAME;

		if (SymFromAddr(s_currentProcess, dwAddress, &dwDisplacement, pSymbol)) {
			a_pStackItem->funcName = _strdup(pSymbol->Name);
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

	cinternal_lw_recursive_mutex_unlock(&s_mutex_for_dbg_functions);


}



#endif // #if defined(_WIN32) || defined(__INTELLISENSE__)

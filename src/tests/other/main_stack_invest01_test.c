//
// file:		main_stack_invest02_test.c
// path:		src/tests/other/main_stack_invest02_test.c
// created by:	Davit Kalantaryan (davit.kalataryan@desy.de)
// created on:	2022 Jun 10
//

#include <stdio.h>
#include <stdlib.h>
#include <stack_investigator/investigator.h>


static void PrintStack(struct StackInvestStackItem* pItems, int a_frames);

int main(void)
{
    int i=0;
    struct StackInvestBacktrace* pStack;
    struct StackInvestStackItem* pItems;

	printf("If debugging is needed, then connect with debugger, then press enter to proceed  ! ");
	fflush(stdout);
	getchar();

	pStack = StackInvestInitBacktraceDataForCurrentStack(0);
	if ((!pStack) || (pStack->stackDeepness<1)) {fprintf(stderr, "Unable to get stack\n");return 1;}
	pItems = CPPUTILS_STATIC_CAST(struct StackInvestStackItem*,malloc(sizeof(struct StackInvestStackItem)*(pStack->stackDeepness)));
	StackInvestConvertBacktraceToNames(pStack, pItems);
	PrintStack(pItems, pStack->stackDeepness);
	
	for(; i<1000;++i){
		//int* pI = new int;
		//printf("%.4d  pI=%p\n",i,static_cast<void*>(pI));
	}
	
	return 0;
}


static void PrintStack(struct StackInvestStackItem* pFrames, int a_frames)
{
    int i=0;
	for (; i < a_frames; ++i) {
		fprintf(stderr, "\t%p, bin:\"%s\", fnc:\"%s\", src:\"%s\", ln:%d\n",
			pFrames[i].address, pFrames[i].binFile, pFrames[i].funcName,
			pFrames[i].sourceFile, pFrames[i].line);
	}
	StackInvestPrintTrace();
}

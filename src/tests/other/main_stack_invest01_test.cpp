//
// file:		main_stack_invest01_test.cpp
// path:		src/tests/other/main_stack_invest01_test.cpp
// created by:	Davit Kalantaryan (davit.kalataryan@desy.de)
// created on:	2022 Jun 09
//

#include <stdio.h>
#include <stack_investigator/investigator.h>


static void PrintStack(struct StackInvestStackItem* pItems, int a_frames);

int main()
{
	printf("If debugging is needed, then connect with debugger, then press enter to proceed  ! ");
	fflush(stdout);
	getchar();

	struct StackInvestBacktrace* pStack = StackInvestInitBacktraceDataForCurrentStack(0);
	if ((!pStack) || (pStack->stackDeepness<1)) {fprintf(stderr, "Unable to get stack\n");return 1;}
	struct StackInvestStackItem* pItems = new StackInvestStackItem[size_t(pStack->stackDeepness)];
	StackInvestConvertBacktraceToNames(pStack, pItems);
	PrintStack(pItems, pStack->stackDeepness);
	
	for(int i(0); i<1000;++i){
		//int* pI = new int;
		//printf("%.4d  pI=%p\n",i,static_cast<void*>(pI));
	}
	
	return 0;
}


static void PrintStack(struct StackInvestStackItem* pFrames, int a_frames)
{
	for (int i(0); i < a_frames; ++i) {
		fprintf(stderr, "\t%p, bin:\"%s\", fnc:\"%s\", src:\"%s\", ln:%d\n",
			pFrames[i].address, pFrames[i].binFile, pFrames[i].funcName,
			pFrames[i].sourceFile, pFrames[i].line);
	}
	StackInvestPrintTrace();
}

//
// file:		main_stack_invest01_test.cpp
// path:		src/tests/other/main_stack_invest01_test.cpp
// created by:	Davit Kalantaryan (davit.kalataryan@desy.de)
// created on:	2022 Jun 09
//

#include <stdio.h>
#include <stdlib.h>
#include <stack_investigator/cinvestigator.hpp>


static void PrintStack(void);

int main(void)
{
    int i=0;

	printf("If debugging is needed, then connect with debugger, then press enter to proceed  ! ");
	fflush(stdout);
	getchar();

	PrintStack();

	for(; i<1000;++i){
		//int* pI = new int;
		//printf("%.4d  pI=%p\n",i,static_cast<void*>(pI));
	}

	return 0;
}


static void PrintStack(void)
{
    stack_investigator::StackItems aItems;
    const size_t cunFrames = aItems.numberOfFrames();
    const stack_investigator::StackItems::CStackItemPtr& pFrames = aItems.stackItems();
	for (size_t i(0); i < cunFrames; ++i) {
		fprintf(stderr, "\t%p, bin:\"%s\", fnc:\"%s\", src:\"%s\", ln:%d\n",
			pFrames[i].address, pFrames[i].binFile, pFrames[i].funcName,
			pFrames[i].sourceFile, pFrames[i].line);
	}
}

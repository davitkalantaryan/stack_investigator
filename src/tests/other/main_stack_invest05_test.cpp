//
// file:		main_stack_invest01_test.cpp
// path:		src/tests/other/main_stack_invest01_test.cpp
// created by:	Davit Kalantaryan (davit.kalataryan@desy.de)
// created on:	2022 Jun 09
//

#include <stdio.h>
#include <stdlib.h>
#include <stack_investigator/cinvestigator.hpp>


int main(void)
{
    int i=0;

	printf("If debugging is needed, then connect with debugger, then press enter to proceed  ! ");
	fflush(stdout);
	getchar();

    stack_investigator::StackItems().printStack(stderr,fprintf);

	for(; i<1000;++i){
		//int* pI = new int;
		//printf("%.4d  pI=%p\n",i,static_cast<void*>(pI));
	}

	return 0;
}

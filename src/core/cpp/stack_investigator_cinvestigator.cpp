//
// file:			stack_investigator_cinvestigator.cpp
// path:			src/core/cpp/stack_investigator_cinvestigator.cpp
// created on:		2022 Jun 10
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <stack_investigator/cinvestigator.hpp>
#include <stack_investigator/investigator.h>
#ifdef _MSC_VER
#pragma warning(disable:4464)  // relative include path contains '..'
#endif
#include "../stack_investigator_private_internal.h"
#include <stdlib.h>


namespace stack_investigator{


class CPPUTILS_DLL_PRIVATE Backtrace_p
{
public:
    Backtrace_p(int a_goBackInTheStack);
    Backtrace_p(const Backtrace_p& cM);
    StackInvestBacktrace* m_pStack;
};


class CPPUTILS_DLL_PRIVATE StackItems_p
{
public:
    size_t                      m_itemsCount;
    StackInvestStackItem*       m_pStackItems;
};


/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

Backtrace::~Backtrace() CPPUTILS_NOEXCEPT
{
    if(m_bt_data_ptr){
        StackInvestFreeBacktraceData(m_bt_data_ptr->m_pStack);
        delete m_bt_data_ptr;
    }
}


Backtrace::Backtrace(int a_goBackInTheStack)
    :
      m_bt_data_ptr(new Backtrace_p(a_goBackInTheStack))
{
}


Backtrace::Backtrace(const Backtrace& a_cM)
    :
      m_bt_data_ptr(new Backtrace_p(*a_cM.m_bt_data_ptr))
{
}


Backtrace::Backtrace(Backtrace&& a_mM)CPPUTILS_NOEXCEPT
    :
      m_bt_data_ptr(a_mM.m_bt_data_ptr)
{
    a_mM.m_bt_data_ptr = nullptr;
}


Backtrace& Backtrace::operator=(const Backtrace& a_cM)
{
    StackInvestFreeBacktraceData(m_bt_data_ptr->m_pStack);
    m_bt_data_ptr->m_pStack = StackInvestCloneBackTrace(a_cM.m_bt_data_ptr->m_pStack);
    if (!m_bt_data_ptr->m_pStack) {
        throw "Low memory"; // todo: replace this with proper exception
    }
    return *this;
}


Backtrace& Backtrace::operator=(Backtrace&& a_mM)CPPUTILS_NOEXCEPT
{
    Backtrace_p* this_data_ptr = m_bt_data_ptr;
    m_bt_data_ptr = a_mM.m_bt_data_ptr;
    a_mM.m_bt_data_ptr = this_data_ptr;
    return *this;
}


const Backtrace::CVoidPtr* Backtrace::frames()const
{
    return m_bt_data_ptr->m_pStack->ppBuffer;
}


size_t Backtrace::hash()const
{
    return StackInvestHashOfTheStack(m_bt_data_ptr->m_pStack);
}


int Backtrace::numberOfFrames()const
{
    return m_bt_data_ptr->m_pStack->stackDeepness;
}


/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


StackItems::~StackItems()
{
    for(size_t i(0); i<m_st_itms_data_ptr->m_itemsCount;++i){
        StackInvestFreeStackItemData(&m_st_itms_data_ptr->m_pStackItems[i]);
    }

    STACK_INVEST_ANY_FREE(m_st_itms_data_ptr->m_pStackItems);
    delete m_st_itms_data_ptr;
}


StackItems::StackItems(const Backtrace& a_st)
    :
      m_st_itms_data_ptr(new StackItems_p())
{
    m_st_itms_data_ptr->m_itemsCount = static_cast<size_t>(a_st.numberOfFrames());
    m_st_itms_data_ptr->m_pStackItems = static_cast<StackInvestStackItem*>(STACK_INVEST_ANY_ALLOC(m_st_itms_data_ptr->m_itemsCount*sizeof(StackInvestStackItem)));
    if(!m_st_itms_data_ptr->m_pStackItems){
        m_st_itms_data_ptr->m_itemsCount = 0;
        throw "Low memory"; // todo: replace this with proper exception
    }

    StackInvestConvertBacktraceToNamesRaw(a_st.m_bt_data_ptr->m_pStack,m_st_itms_data_ptr->m_pStackItems,m_st_itms_data_ptr->m_itemsCount);
}


const StackItems::CStackItemPtr& StackItems::stackItems()const
{
    return m_st_itms_data_ptr->m_pStackItems;
}


size_t StackItems::numberOfFrames()const
{
    return m_st_itms_data_ptr->m_itemsCount;
}


void StackItems::printStack(const TypePrint1& a_fnc)const
{
    const StackInvestStackItem*const& pFrames = m_st_itms_data_ptr->m_pStackItems;
	for (size_t i(0); i < m_st_itms_data_ptr->m_itemsCount; ++i) {
		a_fnc("\t%p, bin:\"%s\", fnc:\"%s\", src:\"%s\", ln:%d\n",
			pFrames[i].address, pFrames[i].binFile, pFrames[i].funcName,
			pFrames[i].sourceFile, pFrames[i].line);
	}
	StackInvestPrintTrace();


    //printStack((FILE*)a_fnc,[](FILE* a_fl, const char* a_frm, ...){
    //    StackItems::TypePrint1 fn = (StackItems::TypePrint1)a_fl;
    //    return fn(a_frm);
    //});
    /// todo: switch to unique code base

}


void StackItems::printStack(FILE* a_file, const TypePrint2& a_fnc)const
{
    const StackInvestStackItem*const& pFrames = m_st_itms_data_ptr->m_pStackItems;
	for (size_t i(0); i < m_st_itms_data_ptr->m_itemsCount; ++i) {
		a_fnc(a_file,"\t%p, bin:\"%s\", fnc:\"%s\", src:\"%s\", ln:%d\n",
			pFrames[i].address, pFrames[i].binFile, pFrames[i].funcName,
			pFrames[i].sourceFile, pFrames[i].line);
	}
	StackInvestPrintTrace();
}


/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

Backtrace_p::Backtrace_p(int a_goBackInTheStack)
    :
      m_pStack(StackInvestInitBacktraceDataForCurrentStack(a_goBackInTheStack))
{
    if(!m_pStack){
        throw "Low memory"; // todo: replace this with proper exception
    }
}


Backtrace_p::Backtrace_p(const Backtrace_p& a_cM)
    :
      m_pStack(StackInvestCloneBackTrace(a_cM.m_pStack))
{
    if(!m_pStack){
        throw "Low memory"; // todo: replace this with proper exception
    }
}


}  // namespace stack_investigator{

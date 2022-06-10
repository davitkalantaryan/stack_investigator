//
// file:			cinvestigator.hpp
// path:			include/stack_investigator/cinvestigator.hpp
// created on:		2022 Jun 10
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#pragma once

#include <stack_investigator/internal_header.h>
#include <stack_investigator/investigator.h>
#include <stddef.h>
#include <stdio.h>
//#include <functional>

namespace stack_investigator{


class STACK_INVEST_CPP_EXPORT StackItems;
class CPPUTILS_DLL_PRIVATE Backtrace_p;

class STACK_INVEST_CPP_EXPORT Backtrace
{
public:
    typedef const void*const CVoidPtr;

public:
    ~Backtrace()CPPUTILS_NOEXCEPT;
    Backtrace(int goBackInTheStack=1);
    Backtrace(const Backtrace& cM);
    Backtrace(Backtrace&& cM)CPPUTILS_NOEXCEPT;

    Backtrace& operator=(const Backtrace& cM);
    Backtrace& operator=(Backtrace&& cM)CPPUTILS_NOEXCEPT;
    const CVoidPtr* frames()const;
    size_t hash()const;
    int    numberOfFrames()const;

private:
    Backtrace_p*  m_bt_data_ptr;

    friend class StackItems;
};



class CPPUTILS_DLL_PRIVATE StackItems_p;

class STACK_INVEST_CPP_EXPORT StackItems
{
public:
    typedef StackInvestStackItem*const CStackItemPtr;
    //typedef std::function<int(const char*,...)>  TypePrint1;
    //typedef std::function<int(FILE*,const char*,...)>  TypePrint2;
    typedef int (*TypePrint1)(const char*,...);
    typedef int (*TypePrint2)(FILE*,const char*,...);

public:
    ~StackItems()CPPUTILS_NOEXCEPT;
    StackItems(const Backtrace& st = Backtrace(1));
    StackItems(const StackItems&)=delete;
    StackItems& operator=(const StackItems&)=delete;

    const CStackItemPtr& stackItems()const;
    size_t numberOfFrames()const;
    void printStack(const TypePrint1&)const;
    void printStack(FILE*, const TypePrint2&)const;

private:
    StackItems_p*const  m_st_itms_data_ptr;
};


}  // namespace stack_investigator{

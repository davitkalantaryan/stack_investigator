//
// file:		stack_investigator_backtrace_addr_to_details_unix_dwarf.c
// path:		src/core/stack_investigator_backtrace_addr_to_details_unix_dwarf.c
// created by:	Davit Kalantaryan (davit.kalataryan@desy.de)
// created on:	2023 Jun 03
//


#ifdef _MSC_VER
#pragma warning(disable:4206)
#endif

#include <stack_investigator/export_symbols.h>

#if !defined(_WIN32) || defined(__INTELLISENSE__)
#include "stack_investigator_private_addr_to_details_unix.h"
#ifdef STACK_INVEST_LIBDWARF_USED

#include "stack_investigator_private_internal.h"
#include "stack_investigator_private_mutex.h"
#include <cinternal/hash/dllhash.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <dlfcn.h>
#include <libdwarf/libdwarf.h>
#include <libdwarf/dwarf.h>


CPPUTILS_BEGIN_C


struct SModuleDwarfEntry{
    bool                hasDebugInfo;
    bool                reserved01[7];
    size_t              offset;
    CinternalDLLHash_t  addrItemHash;
};

struct StackInvestStackItemBasic {
    const char*     funcName;
    const char*     sourceFile;  // empty means unavailable
    int             line;      // <0 means unknown (no debug info available)
    int             reserved01;
};


static CinternalDLLHash_t   s_dwarfModulesHash = CPPUTILS_NULL;
static void*                s_pDwarfMutex = CPPUTILS_NULL;


struct SModuleDwarfEntry* CreateDwarfEntryForModule(const char* a_cpcModuleName, const void* a_pAnyAddress);

static inline CinternalDLLHashItem_t CInternalDLLHashAddDataWithKnownHashInNotExist(CinternalDLLHash_t CPPUTILS_ARG_NN a_hashTbl, const void* a_data, const void* a_key, size_t a_keySize, size_t a_hash){
    CPPUTILS_STATIC_CAST(void,a_hash);
    return CInternalDLLHashAddDataIfNotExists(a_hashTbl,a_data,a_key,a_keySize);
}


static inline void DeallocateItemFromDwarfModulesHashInline(struct SModuleDwarfEntry* a_pDwarfModule) CPPUTILS_NOEXCEPT
{
    CPPUTILS_STATIC_CAST(void,a_pDwarfModule);
}


static char* StackInvestStrdupInline(const char* a_cpcString){
    const size_t cunStrLenPlus1 = strlen(a_cpcString)+1;
    char* pcRetStr = CPPUTILS_STATIC_CAST(char*,STACK_INVEST_ANY_ALLOC(cunStrLenPlus1));
    if(!pcRetStr){return CPPUTILS_NULL;}
    memcpy(pcRetStr,a_cpcString,cunStrLenPlus1);
    return pcRetStr;
}


CPPUTILS_DLL_PRIVATE int StackInvestDetailsFromFrameAddress_Dwarf(struct StackInvestStackItem* a_pStack)
{
    struct SModuleDwarfEntry* pDwEntry;
    struct StackInvestStackItemBasic* pStackInfoItem;
    CinternalDLLHashItem_t moduleItem;
    CinternalDLLHashItem_t stackItem;
    size_t unHash;
    const size_t cunBinFlNameLenPlus1 = strlen(a_pStack->binFile) + 1;
    STACK_INVEST_RW_MUTEX_RD_LOCK(s_pDwarfMutex);
    moduleItem = CInternalDLLHashFindEx(s_dwarfModulesHash,a_pStack->binFile,cunBinFlNameLenPlus1,&unHash);
    STACK_INVEST_RW_MUTEX_RD_UNLOCK(s_pDwarfMutex);
    
    if(moduleItem){
        pDwEntry = CPPUTILS_STATIC_CAST(struct SModuleDwarfEntry*,moduleItem->data);
    }
    else{
        pDwEntry = CreateDwarfEntryForModule(a_pStack->binFile,a_pStack->address);
        if(!pDwEntry){
            return 1;
        }
        STACK_INVEST_RW_MUTEX_WR_LOCK(s_pDwarfMutex);
        moduleItem = CInternalDLLHashAddDataWithKnownHashInNotExist(s_dwarfModulesHash,pDwEntry,a_pStack->binFile,cunBinFlNameLenPlus1,unHash);
        STACK_INVEST_RW_MUTEX_WR_UNLOCK(s_pDwarfMutex);
        if(!moduleItem){
            // some other thread added this module
            DeallocateItemFromDwarfModulesHashInline(pDwEntry);
            STACK_INVEST_RW_MUTEX_RD_LOCK(s_pDwarfMutex);
            moduleItem = CInternalDLLHashFindEx(s_dwarfModulesHash,a_pStack->binFile,cunBinFlNameLenPlus1,&unHash);
            STACK_INVEST_RW_MUTEX_RD_UNLOCK(s_pDwarfMutex);
            if(!moduleItem){return 1;} // this should never happen and this can be replaced by assert
            pDwEntry = CPPUTILS_STATIC_CAST(struct SModuleDwarfEntry*,moduleItem->data);
        }
    }
    
    if(pDwEntry->hasDebugInfo){
        stackItem = CInternalDLLHashFindEx(s_dwarfModulesHash,a_pStack->address,0,&unHash);
        if(!stackItem){return 1;} // this should never happen and this can be replaced by assert
        pStackInfoItem = CPPUTILS_STATIC_CAST(struct StackInvestStackItemBasic*,stackItem->data);
        a_pStack->funcName = StackInvestStrdupInline(pStackInfoItem->funcName);
        if(!(a_pStack->funcName)){return 1;}
        a_pStack->sourceFile = StackInvestStrdupInline(pStackInfoItem->sourceFile);
        if(!(a_pStack->sourceFile)){
            STACK_INVEST_ANY_FREE(CPPUTILS_CONST_CAST(char*,a_pStack->funcName));
            return 1;
        }
        a_pStack->line = pStackInfoItem->line;
        return 0;
    }
    
    a_pStack->funcName = StackInvestStrdupInline("UnknownFunction");
    if(!(a_pStack->funcName)){return 1;}
    a_pStack->sourceFile = StackInvestStrdupInline("UnknownSource");
    if(!(a_pStack->sourceFile)){
        STACK_INVEST_ANY_FREE(CPPUTILS_CONST_CAST(char*,a_pStack->funcName));
        return 1;
    }
    a_pStack->line = -1;
    return 0;
}


static int CollectDwarfInfo(int a_modFd, struct SModuleDwarfEntry* a_pDwarfEntry);


struct SModuleDwarfEntry* CreateDwarfEntryForModule(const char* a_cpcModuleName, const void* a_pAnyAddress)
{
    Dl_info dlInfo;
    int modFd;
    struct SModuleDwarfEntry* pDwarfEntry = CPPUTILS_STATIC_CAST(struct SModuleDwarfEntry*,STACK_INVEST_ANY_ALLOC(sizeof(struct SModuleDwarfEntry)));
    if(!pDwarfEntry){
        return CPPUTILS_NULL;
    }
    
    if(!dladdr(a_pAnyAddress,&dlInfo)){
        pDwarfEntry->hasDebugInfo = false;
        return pDwarfEntry;
    }
    
    modFd = open(a_cpcModuleName, O_RDONLY);
    if (modFd < 0) {
        pDwarfEntry->hasDebugInfo = false;
        return pDwarfEntry;
    }
    
    pDwarfEntry->hasDebugInfo = true;
    pDwarfEntry->addrItemHash = CInternalDLLHashCreateExSmlInt(1024,& STACK_INVEST_ANY_ALLOC, & STACK_INVEST_ANY_FREE);
    pDwarfEntry->offset = CPPUTILS_STATIC_CAST(size_t,dlInfo.dli_fbase);
    CollectDwarfInfo(modFd,pDwarfEntry);
    close(modFd);
    
    return pDwarfEntry;
}


static inline char* GetFunctionNames(Dwarf_Addr addr, Dwarf_Debug dbg, Dwarf_Die cu_die)
{
    Dwarf_Die child_die;
    Dwarf_Error error;
    
    // assuming cu_die is the DIE of a compilation unit
    if (dwarf_child(cu_die, &child_die, &error) == DW_DLV_OK) {
        Dwarf_Die child_die_next=CPPUTILS_NULL;
        do {
            if(child_die_next){
                dwarf_dealloc(dbg, child_die, DW_DLA_DIE);
                child_die = child_die_next;
            }
            Dwarf_Half tag;
            if (dwarf_tag(child_die, &tag, &error) != DW_DLV_OK) {
                continue;
            }
            
            if (tag == DW_TAG_subprogram) {
                Dwarf_Addr low_pc, high_pc;
                Dwarf_Attribute attr;
    
                if (dwarf_attr(child_die, DW_AT_low_pc, &attr, &error) == DW_DLV_OK) {
                    dwarf_formaddr(attr, &low_pc, &error);
                    dwarf_dealloc(dbg, attr, DW_DLA_ATTR);
                }else{
                    continue;
                }
                
                if (dwarf_attr(child_die, DW_AT_high_pc, &attr, &error) == DW_DLV_OK) {
                    dwarf_formaddr(attr, &high_pc, &error);
                    dwarf_dealloc(dbg, attr, DW_DLA_ATTR);
                }
                else{
                    continue;
                }
    
                // Assuming addr is the address you are interested in
                if (addr >= low_pc && addr < high_pc) {
                    char* die_name;
                    if (dwarf_diename(child_die, &die_name, &error) == DW_DLV_OK) {
                        //printf("Function name: %s\n", die_name);
                        //dwarf_dealloc(dbg, die_name, DW_DLA_STRING);
                        return die_name;
                    }
                }
            }
        } while (dwarf_siblingof(dbg, child_die, &child_die_next, &error) == DW_DLV_OK);
        
    }
        
        return CPPUTILS_NULL;
    
}


static int CollectDwarfInfo(int a_modFd, struct SModuleDwarfEntry* a_pDwarfEntry)
{
    struct StackInvestStackItemBasic* pItem;
    Dwarf_Debug dbg;
    Dwarf_Error err;
    Dwarf_Line *linebuf;
    Dwarf_Signed linecount;
    Dwarf_Unsigned cu_header_length, abbrev_offset, next_cu_header;
    Dwarf_Half version_stamp, address_size;   
    Dwarf_Die cu_die = 0;
    Dwarf_Signed i;
    Dwarf_Die no_die = 0;

    if (dwarf_init(a_modFd, DW_DLC_READ, 0, 0, &dbg, &err) != DW_DLV_OK) {
        fprintf(stderr, "Failed to initialize libdwarf.\n");
        return 1;
    }
    
    while (1) {
        int res = dwarf_next_cu_header(dbg, &cu_header_length, &version_stamp,&abbrev_offset, 
                                       &address_size, &next_cu_header,&err);
        if (res == DW_DLV_ERROR){
            dwarf_finish(dbg, &err);
            return 1;
        }
        else if (res == DW_DLV_NO_ENTRY){
            break;
        }
                
        if (dwarf_siblingof(dbg, no_die, &cu_die, &err) == DW_DLV_OK) {
            // now cu_die is the top DIE of the current compile unit
            // you can use libdwarf functions on cu_die to inspect its attributes, 
            // iterate over its children, etc.
            
            
            if (dwarf_srclines(cu_die, &linebuf, &linecount, &err) != DW_DLV_OK) {
                continue;
            }
        
            for (i = 0; i < linecount; ++i) {
                Dwarf_Addr lineaddr;
                if (dwarf_lineaddr(linebuf[i], &lineaddr, &err) != DW_DLV_OK) {
                    continue;
                }
        
                char *filename;
                if (dwarf_linesrc(linebuf[i], &filename, &err) != DW_DLV_OK) {
                    continue;
                }
        
                Dwarf_Unsigned lineno;
                if (dwarf_lineno(linebuf[i], &lineno, &err) != DW_DLV_OK) {
                    continue;
                }
                
                char *funcName = GetFunctionNames(lineaddr,dbg,cu_die);
                
                //printf("0x%llx %s:%llu, fn:%s\n", lineaddr, filename, lineno,(funcName?funcName:"null"));
                pItem = CPPUTILS_STATIC_CAST(struct StackInvestStackItemBasic*,STACK_INVEST_ANY_ALLOC(sizeof(struct StackInvestStackItemBasic)));
                if(pItem){
                    pItem->funcName = StackInvestStrdupInline(funcName?funcName:"UnknownFunction");
                    if(!(pItem->funcName)){
                        STACK_INVEST_ANY_FREE(pItem);
                        return 1;
                    }
                    
                    pItem->sourceFile = StackInvestStrdupInline(filename);
                    if(!(pItem->sourceFile)){
                        STACK_INVEST_ANY_FREE(CPPUTILS_CONST_CAST(char*,pItem->funcName));
                        STACK_INVEST_ANY_FREE(pItem);
                        return 1;
                    }
                    
                    pItem->line = CPPUTILS_STATIC_CAST(int,lineno);
                    CPPUTILS_STATIC_CAST(void,pItem->reserved01);
                    
                    if(!CInternalDLLHashAddDataIfNotExists(a_pDwarfEntry->addrItemHash,pItem,((char*)(lineaddr))+a_pDwarfEntry->offset,0)){
                        // we already added this address
                        STACK_INVEST_ANY_FREE(CPPUTILS_CONST_CAST(char*,pItem->sourceFile));
                        STACK_INVEST_ANY_FREE(CPPUTILS_CONST_CAST(char*,pItem->funcName));
                        STACK_INVEST_ANY_FREE(pItem);
                    }
                }  //  if(pItem){
        
                
                if(funcName){
                    dwarf_dealloc(dbg, funcName, DW_DLA_STRING);
                }
                dwarf_dealloc(dbg, filename, DW_DLA_STRING);
            }
        
            dwarf_srclines_dealloc(dbg, linebuf, linecount);
            
            // deallocate the die after using it
            dwarf_dealloc(dbg, cu_die, DW_DLA_DIE);
        }  //  if (dwarf_siblingof(dbg, no_die, &cu_die, &err) == DW_DLV_OK) {

        
    }

    if (dwarf_finish(dbg, &err) != DW_DLV_OK) {
        fprintf(stderr, "Failed to finalize libdwarf.\n");
        return 1;
    }
    
    return 0;
}


static void DeallocateItemFromDwarfModulesHash(void* a_pDwarfModule) CPPUTILS_NOEXCEPT
{
    DeallocateItemFromDwarfModulesHashInline(CPPUTILS_STATIC_CAST(struct SModuleDwarfEntry*,a_pDwarfModule));
}


static void stack_investigator_backtrace_addr_to_details_unix_dwarf_cleanup(void){    
    CInternalDLLHashDestroyEx(s_dwarfModulesHash,&DeallocateItemFromDwarfModulesHash);
    STACK_INVEST_RW_MUTEX_DESTROY(s_pDwarfMutex);
    s_pDwarfMutex = CPPUTILS_NULL;   
}

CPPUTILS_CODE_INITIALIZER(stack_investigator_backtrace_addr_to_details_unix_dwarf_initialize){        
    s_pDwarfMutex = STACK_INVEST_RW_MUTEX_CREATE();
    
    if(!s_pDwarfMutex){
        exit(1);
    }
        
    s_dwarfModulesHash = CInternalDLLHashCreateExRawMem(1024,& STACK_INVEST_ANY_ALLOC, & STACK_INVEST_ANY_FREE);
    if(!s_dwarfModulesHash){
        STACK_INVEST_RW_MUTEX_DESTROY(s_pDwarfMutex);
        s_pDwarfMutex = CPPUTILS_NULL;
        exit(1);
    }
    
    atexit(&stack_investigator_backtrace_addr_to_details_unix_dwarf_cleanup);

}



CPPUTILS_END_C


#endif  //  #ifdef STACK_INVEST_LIBDWARF_USED
#endif  //  #if !defined(_WIN32) || defined(__INTELLISENSE__)

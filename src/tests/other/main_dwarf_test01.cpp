#include "libdwarf/libdwarf.h"
#include "libdwarf/dwarf.h"
#include <execinfo.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


static int s_nLineToReport = 0;

static char* GetFunctionNames(Dwarf_Addr addr, Dwarf_Debug dbg, Dwarf_Die cu_die)
{
    Dwarf_Die child_die;
    Dwarf_Error error;
    
    // assuming cu_die is the DIE of a compilation unit
    if (dwarf_child(cu_die, &child_die, &error) == DW_DLV_OK) {
        Dwarf_Die child_die_next=nullptr;
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
        
        return nullptr;
    
}


static void print_dwarf_info(int fd) {
    Dwarf_Debug dbg;
    Dwarf_Error err;
    Dwarf_Line *linebuf;
    Dwarf_Signed linecount;
    //Dwarf_Global* typep = 0;

    if (dwarf_init(fd, DW_DLC_READ, 0, 0, &dbg, &err) != DW_DLV_OK) {
        fprintf(stderr, "Failed to initialize libdwarf.\n");
        exit(EXIT_FAILURE);
    }

    Dwarf_Unsigned cu_header_length, abbrev_offset, next_cu_header;
    Dwarf_Half version_stamp, address_size;
    //Dwarf_Line *linebuf;
    //Dwarf_Signed linecount;
    
    Dwarf_Die cu_die = 0;
    Dwarf_Signed i;
    
    while (1) {
        int res = dwarf_next_cu_header(dbg, &cu_header_length, &version_stamp,
                                       &abbrev_offset, &address_size, &next_cu_header,
                                       &err);
        if (res == DW_DLV_ERROR)
            exit(EXIT_FAILURE);
        else if (res == DW_DLV_NO_ENTRY)
            break;
                
        Dwarf_Die no_die = 0;
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
        
                    //if(s_nLineToReport==int(lineno))
                    {
                        printf("0x%llx %s:%llu, fn:%s\n", lineaddr, filename, lineno,(funcName?funcName:"null"));
                    }
                    
                    if(funcName){
                        dwarf_dealloc(dbg, funcName, DW_DLA_STRING);
                    }
                    dwarf_dealloc(dbg, filename, DW_DLA_STRING);
                }
        
                dwarf_srclines_dealloc(dbg, linebuf, linecount);
                
                // deallocate the die after using it
                dwarf_dealloc(dbg, cu_die, DW_DLA_DIE);
            }

        
    }

    if (dwarf_finish(dbg, &err) != DW_DLV_OK) {
        fprintf(stderr, "Failed to finalize libdwarf.\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    int nAddresses;
    char** ppSymbols = NULL;
    void* vFames[1024];
    const char* cpcProgname = argv[0];
    if (argc > 1) {
        cpcProgname = argv[1];
    }
    
    nAddresses=backtrace(vFames,1024);
    s_nLineToReport= __LINE__ - 1;
    if(nAddresses>0){
        ppSymbols= backtrace_symbols(vFames,nAddresses);
        if(!ppSymbols){
            exit(1);
        }
    }

    int fd = open(cpcProgname, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    print_dwarf_info(fd);

    close(fd);
    
    for(int i(0);i<nAddresses;++i){
        printf("frame[%d] = %s[%p]\n",i,ppSymbols[i],vFames[i]);
    }
    
    free(ppSymbols);
    
    printf("If debugging is needed, then connect with debugger, then press enter to proceed  ! ");
	fflush(stdout);
	getchar();

    return 0;
}

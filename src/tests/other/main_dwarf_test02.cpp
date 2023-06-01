#include "libdwarf/libdwarf.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stack_investigator/investigator.h>

void print_dwarf_info(int fd) {
    Dwarf_Debug dbg;
    Dwarf_Error err;
    Dwarf_Line *linebuf;
    Dwarf_Signed linecount;

    if (dwarf_init(fd, DW_DLC_READ, 0, 0, &dbg, &err) != DW_DLV_OK) {
        fprintf(stderr, "Failed to initialize libdwarf.\n");
        exit(EXIT_FAILURE);
    }

    Dwarf_Unsigned cu_header_length, abbrev_offset, next_cu_header;
    Dwarf_Half version_stamp, address_size;
    //Dwarf_Line *linebuf;
    //Dwarf_Signed linecount;
    
    Dwarf_Die cu_die = 0;

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
        
                for (Dwarf_Signed i = 0; i < linecount; ++i) {
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
        
                    printf("0x%llx %s:%llu\n", lineaddr, filename, lineno);
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
    const char* cpcProgname = argv[0];
    if (argc > 1) {
        cpcProgname = argv[1];
    }
    
    StackInvestBacktrace*const pStack = StackInvestInitBacktraceDataForCurrentStack(0);
    printf("pStack=%p\n",(void*)pStack);
    if(pStack){
        for(int i=0; i<pStack->stackDeepness; ++i){
            printf("addr[%d]=%p\n",i,pStack->ppBuffer[i]);
        }
        StackInvestFreeBacktraceData(pStack);
    }

    int fd = open(cpcProgname, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    print_dwarf_info(fd);

    close(fd);

    return 0;
}

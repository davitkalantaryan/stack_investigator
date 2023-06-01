#include "libdwarf/libdwarf.h"
#include <fcntl.h>

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
    Dwarf_Line *linebuf;
    Dwarf_Signed linecount;

    while (1) {
        int res = dwarf_next_cu_header(dbg, &cu_header_length, &version_stamp,
                                       &abbrev_offset, &address_size, &next_cu_header,
                                       &err);
        if (res == DW_DLV_ERROR)
            exit(EXIT_FAILURE);
        else if (res == DW_DLV_NO_ENTRY)
            break;

        if (dwarf_srclines(dbg, &linebuf, &linecount, &err) != DW_DLV_OK) {
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
    }

    if (dwarf_finish(dbg, &err) != DW_DLV_OK) {
        fprintf(stderr, "Failed to finalize libdwarf.\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Expected a program name\n");
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    print_dwarf_info(fd);

    close(fd);

    return 0;
}

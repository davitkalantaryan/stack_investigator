//
// file:		crash_investigator_backtrace_unix.cpp
// path:		src/core/backtrace/crash_investigator_backtrace_unix.cpp
// created by:	Davit Kalantaryan (davit.kalataryan@desy.de)
// created on:	2021 Nov 25
//

#ifndef STACK_INVEST_DO_NOT_USE_STACK_INVESTIGATION
#if !defined(_WIN32) || defined(__INTELLISENSE__)

#include <stack_investigator/internal_header.h>

#if !defined(__EMSCRIPTEN__)
#define CRASH_INVESTEXECINFO_DEFINED
#endif

#if defined(__linux__) || defined(__linux)
#define CRASH_INVEST_PRCTL_DEFINED
#endif

#include <stack_investigator/investigator.h>
#include "stack_investigator_private_internal.h"
#include <string.h>
#include <alloca.h>
#include <stdio.h>
#include <unistd.h>
#ifdef CRASH_INVESTEXECINFO_DEFINED
#include <execinfo.h>
#endif
#ifdef CRASH_INVEST_PRCTL_DEFINED
#include <sys/prctl.h>
#include <sys/wait.h>
#endif

#ifdef CRASH_INVEST_PRCTL_DEFINED

STACK_INVEST_EXPORT void StackInvestPrintTrace(void)
{
    char pid_buf[30];
    snprintf(pid_buf,29, "%d", getpid());
    char name_buf[512];
    name_buf[readlink("/proc/self/exe", name_buf, 511)]=0;
    prctl(PR_SET_PTRACER, PR_SET_PTRACER_ANY, 0, 0, 0);
    int child_pid = fork();
    if (!child_pid) {
        dup2(2,1); // redirect output to stderr - edit: unnecessary?
        execl("/usr/bin/gdb", "gdb", "--batch", "-n", "-ex", "thread", "-ex", "bt", name_buf, pid_buf, NULL);
        abort(); /* If gdb failed to start */
    } else {
        waitpid(child_pid,NULL,0);
    }
}

#else

STACK_INVEST_EXPORT void StackInvestPrintTrace(void){}

#endif


#define STACK_INVEST_SYMBOLS_COUNT_MAX  256

STACK_INVEST_EXPORT struct StackInvestBacktrace* StackInvestInitBacktraceDataForCurrentStack(int a_goBackInTheStackCalc)
{
	struct StackInvestBacktrace* pReturn = CPPUTILS_STATIC_CAST(struct StackInvestBacktrace*, STACK_INVEST_ANY_ALLOC(sizeof(struct StackInvestBacktrace)));
    if(!pReturn){return CPPUTILS_NULL;}

    const int cnMaxSymbolCount = STACK_INVEST_SYMBOLS_COUNT_MAX +a_goBackInTheStackCalc;

    pReturn->reserved01 = 0;

    void** ppBuffer = CPPUTILS_STATIC_CAST(void**,alloca(CPPUTILS_STATIC_CAST(size_t,cnMaxSymbolCount)*sizeof(void*)));
    int nInitialDeepness = backtrace(ppBuffer,cnMaxSymbolCount);
    if(nInitialDeepness>a_goBackInTheStackCalc){
        pReturn->stackDeepness = nInitialDeepness-a_goBackInTheStackCalc;
        pReturn->ppBuffer = CPPUTILS_STATIC_CAST(void**, STACK_INVEST_ANY_ALLOC(CPPUTILS_STATIC_CAST(size_t,pReturn->stackDeepness)*sizeof(void*)));
        if(!(pReturn->ppBuffer)){STACK_INVEST_ANY_FREE(pReturn);return CPPUTILS_NULL;}
		memcpy(pReturn->ppBuffer,&(ppBuffer[a_goBackInTheStackCalc]), CPPUTILS_STATIC_CAST(size_t,pReturn->stackDeepness)*sizeof(void*));
    }
    else{
        pReturn->stackDeepness = nInitialDeepness;
        pReturn->ppBuffer = CPPUTILS_STATIC_CAST(void**, STACK_INVEST_ANY_ALLOC(CPPUTILS_STATIC_CAST(size_t,pReturn->stackDeepness)*sizeof(void*)));
        if(!(pReturn->ppBuffer)){STACK_INVEST_ANY_FREE(pReturn);return CPPUTILS_NULL;}
		memcpy(pReturn->ppBuffer,ppBuffer, CPPUTILS_STATIC_CAST(size_t,pReturn->stackDeepness)*sizeof(void*));
    }

    return pReturn;
}


STACK_INVEST_EXPORT void StackInvestConvertBacktraceToNamesRaw(const struct StackInvestBacktrace* a_data, struct StackInvestStackItem* a_pStack, size_t a_bufferSize)
{
    if(a_data){
        size_t i =0;
        const size_t cunSynbols = CPPUTILS_STATIC_CAST(size_t,a_data->stackDeepness)>a_bufferSize?a_bufferSize:CPPUTILS_STATIC_CAST(size_t,a_data->stackDeepness);
        char** ppStrings = backtrace_symbols(a_data->ppBuffer,a_data->stackDeepness);
        if(!ppStrings){return;}

        for(; i < cunSynbols; ++i){
            a_pStack[i].address = a_data->ppBuffer[i];
            a_pStack[i].binFile = strdup(ppStrings[i]);
            a_pStack[i].funcName = strdup("");
            a_pStack[i].sourceFile = strdup("");
			a_pStack[i].reserved01 = 0;
			a_pStack[i].line = -1;
        }

        STACK_INVEST_C_LIB_FREE_NO_CLBK(ppStrings);
    }
}



/*//// dwarf test*/

//#ifdef CRASH_INVEST_TRANSLATE_ADDRESSES_TO_LINES
#if 0

#include <libdwarf/dwarf.h>
#include <libdwarf/libdwarf.h>
#include <err.h>

static char unknown[] = { '?', '?', '\0' };
static Dwarf_Addr section_base;

//static
void
translate(Dwarf_Debug dbg, Elf *e, const char* addrstr)
{
	Dwarf_Die die, ret_die;
	Dwarf_Line *lbuf;
	Dwarf_Error de;
	Dwarf_Half tag;
	Dwarf_Unsigned lopc, hipc, addr, lineno, plineno;
	Dwarf_Signed lcount;
	Dwarf_Addr lineaddr, plineaddr;
	Dwarf_Off off;
	struct CU *cu;
	struct Func *f;
	const char *funcname;
	char *file, *file0, *pfile;
	char demangled[1024];
	int ec, i, ret;

	addr = strtoull(addrstr, NULL, 16);
	addr += section_base;
	lineno = 0;
	file = unknown;
	cu = NULL;
	die = NULL;

	while ((ret = dwarf_next_cu_header(dbg, NULL, NULL, NULL, NULL, NULL,
	    &de)) ==  DW_DLV_OK) {
		die = NULL;
		while (dwarf_siblingof(dbg, die, &ret_die, &de) == DW_DLV_OK) {
			if (die != NULL)
				dwarf_dealloc(dbg, die, DW_DLA_DIE);
			die = ret_die;
			if (dwarf_tag(die, &tag, &de) != DW_DLV_OK) {
				warnx("dwarf_tag failed: %s",
				    dwarf_errmsg(de));
				goto next_cu;
			}

			/* XXX: What about DW_TAG_partial_unit? */
			if (tag == DW_TAG_compile_unit)
				break;
		}
		if (ret_die == NULL) {
			warnx("could not find DW_TAG_compile_unit die");
			goto next_cu;
		}
		if (dwarf_attrval_unsigned(die, DW_AT_low_pc, &lopc, &de) ==
		    DW_DLV_OK) {
			if (dwarf_attrval_unsigned(die, DW_AT_high_pc, &hipc,
			   &de) == DW_DLV_OK) {
				/*
				 * Check if the address falls into the PC
				 * range of this CU.
				 */
				if (handle_high_pc(die, lopc, &hipc) !=
				    DW_DLV_OK)
					goto out;
			} else {
				/* Assume ~0ULL if DW_AT_high_pc not present */
				hipc = ~0ULL;
			}

			/*
			 * Record the CU in the hash table for faster lookup
			 * later.
			 */
			if (dwarf_dieoffset(die, &off, &de) != DW_DLV_OK) {
				warnx("dwarf_dieoffset failed: %s",
				    dwarf_errmsg(de));
				goto out;
			}
			HASH_FIND(hh, culist, &off, sizeof(off), cu);
			if (cu == NULL) {
				if ((cu = calloc(1, sizeof(*cu))) == NULL)
					err(EXIT_FAILURE, "calloc");
				cu->off = off;
				cu->lopc = lopc;
				cu->hipc = hipc;
				STAILQ_INIT(&cu->funclist);
				HASH_ADD(hh, culist, off, sizeof(off), cu);
			}

			if (addr >= lopc && addr < hipc)
				break;
		}

	next_cu:
		if (die != NULL) {
			dwarf_dealloc(dbg, die, DW_DLA_DIE);
			die = NULL;
		}
	}

	if (ret != DW_DLV_OK || die == NULL)
		goto out;

	switch (dwarf_srclines(die, &lbuf, &lcount, &de)) {
	case DW_DLV_OK:
		break;
	case DW_DLV_NO_ENTRY:
		/* If a CU lacks debug info, just skip it. */
		goto out;
	default:
		warnx("dwarf_srclines: %s", dwarf_errmsg(de));
		goto out;
	}

	plineaddr = ~0ULL;
	plineno = 0;
	pfile = unknown;
	for (i = 0; i < lcount; i++) {
		if (dwarf_lineaddr(lbuf[i], &lineaddr, &de)) {
			warnx("dwarf_lineaddr: %s", dwarf_errmsg(de));
			goto out;
		}
		if (dwarf_lineno(lbuf[i], &lineno, &de)) {
			warnx("dwarf_lineno: %s", dwarf_errmsg(de));
			goto out;
		}
		if (dwarf_linesrc(lbuf[i], &file0, &de)) {
			warnx("dwarf_linesrc: %s", dwarf_errmsg(de));
		} else
			file = file0;
		if (addr == lineaddr)
			goto out;
		else if (addr < lineaddr && addr > plineaddr) {
			lineno = plineno;
			file = pfile;
			goto out;
		}
		plineaddr = lineaddr;
		plineno = lineno;
		pfile = file;
	}

out:
	f = NULL;
	funcname = NULL;
	if (ret == DW_DLV_OK && (func || inlines) && cu != NULL) {
		if (cu->srcfiles == NULL)
			if (dwarf_srcfiles(die, &cu->srcfiles, &cu->nsrcfiles,
			    &de))
				warnx("dwarf_srcfiles: %s", dwarf_errmsg(de));
		if (STAILQ_EMPTY(&cu->funclist)) {
			collect_func(dbg, die, NULL, cu);
			die = NULL;
		}
		f = search_func(cu, addr);
		if (f != NULL)
			funcname = f->name;
	}

	if (print_addr) {
		if ((ec = gelf_getclass(e)) == ELFCLASSNONE) {
			warnx("gelf_getclass failed: %s", elf_errmsg(-1));
			ec = ELFCLASS64;
		}
		if (ec == ELFCLASS32) {
			if (pretty_print)
				printf("0x%08jx: ", (uintmax_t) addr);
			else
				printf("0x%08jx\n", (uintmax_t) addr);
		} else {
			if (pretty_print)
				printf("0x%016jx: ", (uintmax_t) addr);
			else
				printf("0x%016jx\n", (uintmax_t) addr);
		}
	}

	if (func) {
		if (funcname == NULL)
			funcname = unknown;
		if (demangle && !elftc_demangle(funcname, demangled,
		    sizeof(demangled), 0)) {
			if (pretty_print)
				printf("%s at ", demangled);
			else
				printf("%s\n", demangled);
		} else {
			if (pretty_print)
				printf("%s at ", funcname);
			else
				printf("%s\n", funcname);
		}
	}

	(void) printf("%s:%ju\n", base ? basename(file) : file,
	    (uintmax_t) lineno);

	if (ret == DW_DLV_OK && inlines && cu != NULL &&
	    cu->srcfiles != NULL && f != NULL && f->inlined_caller != NULL)
		print_inlines(cu, f->inlined_caller, f->call_file,
		    f->call_line);

	if (die != NULL)
		dwarf_dealloc(dbg, die, DW_DLA_DIE);

	/*
	 * Reset internal CU pointer, so we will start from the first CU
	 * next round.
	 */
	while (ret != DW_DLV_NO_ENTRY) {
		if (ret == DW_DLV_ERROR)
			errx(EXIT_FAILURE, "dwarf_next_cu_header: %s",
			    dwarf_errmsg(de));
		ret = dwarf_next_cu_header(dbg, NULL, NULL, NULL, NULL, NULL,
		    &de);
	}
}

#endif // #ifdef CRASH_INVEST_TRANSLATE_ADDRESSES_TO_LINES


#endif  // #ifndef _WIN32
#endif // #ifndef CRASH_INVEST_DO_NOT_USE_AT_ALL

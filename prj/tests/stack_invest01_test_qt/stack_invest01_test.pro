#
# File crash_investigateor.pro
# File created : 18 Nov 2021
# Created by : Davit Kalantaryan (davit.kalantaryan@desy.de)
# This file can be used to produce Makefile
# 


#DEFINES += DEBUG_APP

include("$${PWD}/../../common/common_qt/sys_common.pri")
include("$${PWD}/../../common/common_qt/flags_common.pri")
DESTDIR = $${artifactRoot}/$${SYSTEM_PATH}/$$CONFIGURATION/test

QT -= core
QT -= gui
CONFIG -= qt

#DEFINES += CRASH_INVEST_DO_NOT_USE_MAL_FREE
DEFINES += STACK_INVEST_USING_STATIC_LIB_OR_OBJECTS

SOURCES += "$${PWD}/../../../src/tests/other/main_stack_invest01_test.c"

INCLUDEPATH += $${PWD}/../../../include
INCLUDEPATH += $${PWD}/../../../contrib/cpputils/include

SOURCES += "$${PWD}/../../../src/core/stack_investigator_backtrace_common.c"
SOURCES += "$${PWD}/../../../src/core/stack_investigator_backtrace_unix.c"
SOURCES += "$${PWD}/../../../src/core/stack_investigator_backtrace_windows.c"

HEADERS += \
	"$${PWD}/../../../src/core/stack_investigator_private_internal.h"	\
	$$files($${PWD}/../../../include/*.h,true)

OTHER_FILES +=	\
	"$${PWD}/../../../src/core/analyze/crash_investigator_analyze_leak_only_new_delete.cpp"		\
	"$${PWD}/../../../contrib/cpputils/src/core/cpputils_hashtbl.cpp"				\
	"$${PWD}/../../../src/core/analyze/crash_investigator_analyze_leak_only_new_delete_std_hash.cpp"

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
DEFINES += STACK_INVEST_USING_STATIC_LIB_OR_OBJECTS_CPP

INCLUDEPATH += $${PWD}/../../../include
INCLUDEPATH += $${PWD}/../../../contrib/cpputils/include


SOURCES += "$${PWD}/../../../src/tests/other/main_stack_invest05_test.cpp"

SOURCES += "$${PWD}/../../../src/core/stack_investigator_backtrace_common.c"
SOURCES += "$${PWD}/../../../src/core/stack_investigator_backtrace_unix.c"
SOURCES += "$${PWD}/../../../src/core/stack_investigator_backtrace_windows.c"

SOURCES += "$${PWD}/../../../src/core/cpp/stack_investigator_cinvestigator.cpp"


HEADERS += \
	"$${PWD}/../../../src/core/stack_investigator_private_internal.h"	\
	$$files($${PWD}/../../../include/*.hpp,true)				\
	$$files($${PWD}/../../../include/*.h,true)

OTHER_FILES +=	\
	"$${PWD}/../../../src/core/analyze/crash_investigator_analyze_leak_only_new_delete.cpp"		\
	"$${PWD}/../../../contrib/cpputils/src/core/cpputils_hashtbl.cpp"				\
	"$${PWD}/../../../src/core/analyze/crash_investigator_analyze_leak_only_new_delete_std_hash.cpp"

#
# File crash_investigateor.pro
# File created : 18 Nov 2021
# Created by : Davit Kalantaryan (davit.kalantaryan@desy.de)
# This file can be used to produce Makefile for daqadcreceiver application

#DEFINES += DEBUG_APP

include("$${PWD}/../../common/common_qt/sys_common.pri")
include("$${PWD}/../../common/common_qt/flags_common.pri")
DESTDIR = $${artifactRoot}/$${SYSTEM_PATH}/$$CONFIGURATION/test

QT -= core
QT -= gui
CONFIG -= qt

win32 {
} else {
    LIBS += -ldwarf
    LIBS += -ldl
    LIBS += -pthread
}

#DEFINES += CRASH_INVEST_DO_NOT_USE_MAL_FREE
DEFINES += STACK_INVEST_USING_STATIC_LIB_OR_OBJECTS
DEFINES += STACK_INVEST_USING_STATIC_LIB_OR_OBJECTS_CPP

INCLUDEPATH += $${PWD}/../../../include
INCLUDEPATH += $${PWD}/../../../contrib/cpputils/include


SOURCES += "$${PWD}/../../../src/tests/other/main_dwarf_test02.cpp"

SOURCES += $$files($${PWD}/../../../src/core/*.c,false)
SOURCES += "$${cinternalRepoRoot}/src/core/cinternal_core_hash_dllhash.c"

SOURCES += "$${PWD}/../../../src/core/cpp/stack_investigator_cinvestigator.cpp"


HEADERS += \
	$$files($${PWD}/../../../src/core/*.h,true)				\
	$$files($${PWD}/../../../include/*.hpp,true)				\
	$$files($${PWD}/../../../include/*.h,true)

OTHER_FILES +=	\
	"$${PWD}/../../../src/core/analyze/crash_investigator_analyze_leak_only_new_delete.cpp"		\
	"$${PWD}/../../../contrib/cpputils/src/core/cpputils_hashtbl.cpp"				\
	"$${PWD}/../../../src/core/analyze/crash_investigator_analyze_leak_only_new_delete_std_hash.cpp"

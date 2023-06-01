#
# file:		memory_investigator_all.pro
# path:		workspaces/memory_investigator_all_qt/memory_investigator_all.pro
# created on:	2021 Nov 18
# creatd by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
# purpose:	Qt Project file
#


TEMPLATE = subdirs
CONFIG += ordered

include("$${PWD}/../../prj/common/common_qt/sys_common.pri")
include("$${PWD}/../../prj/common/common_qt/flags_common.pri")
repositoryRoot = $${PWD}/../..

SUBDIRS		+=	"$${repositoryRoot}/prj/tests/stack_invest01_test_qt/stack_invest01_test.pro"
SUBDIRS		+=	"$${repositoryRoot}/prj/tests/stack_invest02_test_qt/stack_invest02_test.pro"
SUBDIRS		+=	"$${repositoryRoot}/prj/tests/stack_invest03_test_qt/stack_invest03_test.pro"
SUBDIRS		+=	"$${repositoryRoot}/prj/tests/stack_invest04_test_qt/stack_invest04_test.pro"
SUBDIRS		+=	"$${repositoryRoot}/prj/tests/stack_invest05_test_qt/stack_invest05_test.pro"
SUBDIRS		+=	"$${repositoryRoot}/prj/tests/dwarf_test01_qt/dwarf_test01.pro"
SUBDIRS		+=	"$${repositoryRoot}/prj/tests/dwarf_test02_qt/dwarf_test02.pro"

includeCppUtils {
	SUBDIRS		+=	"$${repositoryRoot}/contrib/cinternal/workspaces/cinternal_all_qt/cinternal_all.pro"
}

testsFromWorkspace {
	SUBDIRS		+=	"$${repositoryRoot}/prj/tests/googletest_mult/googletest_getter.pro"
}



OTHER_FILES += $$files($${repositoryRoot}/scripts/*.sh,true)
OTHER_FILES += $$files($${repositoryRoot}/scripts/*.bat,true)
OTHER_FILES += $$files($${repositoryRoot}/scripts/.cicd/*.sh,true)
OTHER_FILES += $$files($${repositoryRoot}/scripts/.cicd/*.bat,true)
OTHER_FILES += $$files($${repositoryRoot}/scripts/.raw/*.sh,true)
OTHER_FILES += $$files($${repositoryRoot}/scripts/.raw/*.bat,true)
OTHER_FILES += $$files($${repositoryRoot}/docs/*.md,true)
OTHER_FILES += $$files($${repositoryRoot}/docs/*.txt,true)
OTHER_FILES += $$files($${repositoryRoot}/.github/*.yml,true)

OTHER_FILES	+=	\
	"$${repositoryRoot}/.gitattributes"									\
	"$${repositoryRoot}/.gitignore"										\
	"$${repositoryRoot}/.gitmodules"									\
	"$${repositoryRoot}/LICENSE"										\
	"$${repositoryRoot}/README.md"										\
	"$${repositoryRoot}/Makefile"										\
	"$${repositoryRoot}/scripts/run_under_invest"						

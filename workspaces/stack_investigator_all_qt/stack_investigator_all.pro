#
# file:			memory_investigator_all.pro
# path:			workspaces/memory_investigator_all_qt/memory_investigator_all.pro
# created on:	2021 Nov 18
# creatd by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
# purpose:		Qt Project file
#


TEMPLATE = subdirs
CONFIG += ordered


repositoryRoot = $${PWD}/../..


includeCppUtils {
	SUBDIRS		+=	"$${repositoryRoot}/contrib/cpputils/workspaces/cpputils_qt/cpputils.pro"
}

testsFromWorkspace {
	SUBDIRS		+=	"$${repositoryRoot}/prj/tests/double_free01_test_qt/double_free01_test.pro"
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

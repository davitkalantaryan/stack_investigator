#
# file:			googletest_getter.pro
# path:			prj/tests/googletest_mult/googletest_getter.pro
# created on:	2021 Mar 27
# created by:	Davit Kalantaryan
#

include ( "$${PWD}/../../common/common_qt/sys_common.pri" )

# we reevaluate this
DESTDIR = "$${artifactRoot}/$${SYSTEM_PATH}/test"

QT -= gui
QT -= core
QT -= widgets
CONFIG -= qt

win32{
} else {
	LIBS += "$${PWD}/../../../sys/$$CODENAME/$$CONFIGURATION/lib/libgtest_main$${nameExtension}.a"
	LIBS += "$${PWD}/../../../sys/$$CODENAME/$$CONFIGURATION/lib/libgtest$${nameExtension}.a"
	LIBS += -pthread
}

repoRootPath=$${PWD}/../../..
SRC_DIR=$${repoRootPath}/src/tests/googletest

INCLUDEPATH += "$${PWD}/../../../include"
INCLUDEPATH += "$${PWD}/../../../contrib/googletest/googletest/include"

COMMON_SRCS	= $$files($${repoRootPath}/src/core/*.cpp)  # I assume, that in core we will not have subdirs
COMMON_HDRS	= $$files($${repoRootPath}/include/*.h,true)
COMMON_HDRSPP	= $$files($${repoRootPath}/include/*.hpp,true)

UNITTEST_SRCS	= $$files($${SRC_DIR}/*.cpp,true)
UNITTEST_HDRS	= $$files($${SRC_DIR}/*.h,true)
UNITTEST_HDRSPP	= $$files($${SRC_DIR}/*.hpp,true)

SOURCES =  $$UNITTEST_SRCS
SOURCES += $$COMMON_SRCS
HEADERS =  $$UNITTEST_HDRS
HEADERS += $$UNITTEST_HDRSPP
HEADERS += $$COMMON_HDRS
HEADERS += $$COMMON_HDRSPP

OTHER_FILES +=	\
	"$${PWD}/unix.Makefile"										\
	"$${PWD}/windows.Makefile"									\
	"$${PWD}/packages.config"

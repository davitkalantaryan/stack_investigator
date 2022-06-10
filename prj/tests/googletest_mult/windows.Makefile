#
# file:			Makefile (to create sss library)
# created on:	2020 Dec 02
# created by:	
#
# purpose:		To build DOOCS client library without EPICS
# Examples of calling:
#		>nmake /f windows.Makefile /e Platform=x64
#		>nmake /f windows.Makefile /e Platform=x86
#		>nmake /f windows.Makefile 
#
# Please specify any platform target from following list
# arm
# arm64
# x64 
# x86
# Platform				= x64  # default
# PlatformTarget	# will be specified automatically
#
# below is the variables, that should be specified by parent Makefile, before including this
# TargetName				= libsss # this s defined by final makefile
# CallerMakeFilePath		= XXX  # this one you can provide $(MAKEDIR)\makeFileName
#
# here are default values - will be set if parent Makefile does not specify them'
# Platform				= x64  # default
# ObjectsDirBase		= $(CallerMakeFileDir)\.objects
# SourcesRootDir		= $(CallerMakeFileDir)
#
# @$(LINKER) $(LFLAGS) $(Objects) $(LIBS) /DLL $(LIBPATHS) /SUBSYSTEM:CONSOLE /OUT:$(TargetName).$(TargetExtension)
#  {$(SrcBaseDir)\}.cpp{$(ObjectsDir)}.obj:
#  @$(CPPC) /c $(CXXFLAGS) /Fo$(ObjectsDir)\$(@D)\ $(SrcBaseDir)\$*.cpp
#  @$(CPPC) /c $(CXXFLAGS) /Fo$(ObjectsDir)\$(@D)\ $*.cpp
#
# to clean google test run below commands

# one can redefine this by this nmake /e Platform=x86
Platform			= x64
Configuration			= Debug


# this is set automatically when needed
!IFNDEF MakeFileDir
#!IF "$(MakeFileDir)" == ""
MakeFileDir			= $(MAKEDIR)
!ENDIF
MakeFileName                    = windows.Makefile

RepoRootDir			= $(MakeFileDir)\..\..\..
SrcBaseDir			= $(MakeFileDir)\..\..\..\src
GoogleTestDir			= $(RepoRootDir)\contrib\googletest

TargetName			= unittest
TargetExtension			= exe
TargetCategory                  = test

LINKER        			= link
PDB_FILE_PATH			= $(TargetDirectory)\$(TargetName).pdb
DEFINES       			= $(DEFINES) /D "_WINDLL" /D "_MBCS" /D "CPPUTILS_USING_STATIC_LIB_OR_OBJECTS"
INCLUDE_PATHS			= $(INCLUDE_PATHS) /I"$(GoogleTestDir)\googletest\include"
INCLUDE_PATHS			= $(INCLUDE_PATHS) /I"$(RepoRootDir)\include"
CFLAGS				= $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES)
CXXFLAGS			= $(CXXFLAGS) $(CFLAGS)

LIBPATHS			= $(LIBPATHS) /LIBPATH:"$(RepoRootDir)\sys\win_$(Platform)\$(Configuration)\lib"
LIBS				=
LIBS				= $(LIBS) "gtest_main$(LibrariesExtension).lib"
LIBS				= $(LIBS) "gtest$(LibrariesExtension).lib"
LIBS				= $(LIBS) "Ws2_32.lib"

LFLAGS				= $(LFLAGS) /OUT:"$(TargetDirectory)\$(TargetFileName)" 
LFLAGS				= $(LFLAGS) /MANIFEST /NXCOMPAT /PDB:"$(TargetDirectory)\$(TargetName).pdb" 
LFLAGS				= $(LFLAGS) /DYNAMICBASE $(LIBS) 
LFLAGS				= $(LFLAGS) /DEBUG /MACHINE:$(Platform) /INCREMENTAL  
LFLAGS				= $(LFLAGS) /SUBSYSTEM:CONSOLE /MANIFESTUAC:"level='asInvoker' uiAccess='false'" 
LFLAGS				= $(LFLAGS) /ERRORREPORT:PROMPT /NOLOGO $(LIBPATHS) /TLBID:1


#SourcesToCompile	=
#DirectoriesToCompile	= $(DirectoriesToCompile) nr-build\gen\cpp\sss\ssslang\antlr
DirectoriesToCompile	=
DirectoriesToCompile	= tests\googletest
DirectoriesToCompile	= $(DirectoriesToCompile) core

default: googletest


googletest: __preparationForSetObjects __setObjects


__buildGoogleTestLib:
	@cd $(MakeFileDir)
	@echo "!!!!!! 'msbuild -t:restore -p:RestorePackagesConfig=true' is not necessary anymore"

!include <$(RepoRootDir)\prj\common\common_mkfl\windows.common.Makefile>

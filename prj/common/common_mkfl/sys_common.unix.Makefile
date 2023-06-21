

mkfile_path			=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir			=  $(shell dirname $(mkfile_path))

ifndef stackInvestigatorRepoRoot
        stackInvestigatorRepoRoot	:= $(shell curDir=`pwd` && cd $(mkfile_dir)/../../.. && pwd && cd ${curDir})
endif

ifndef repositoryRoot
        repositoryRoot	= $(stackInvestigatorRepoRoot)
endif

ifndef artifactRoot
        artifactRoot	= $(repositoryRoot)
endif

ifndef cinternalRepoRoot
        cinternalRepoRoot	= $(stackInvestigatorRepoRoot)/contrib/cinternal
endif


include $(stackInvestigatorRepoRoot)/ENVIRONMENT

COMMON_FLAGS	+= -DCRASH_INVEST_VERSION_NUM=$(CRASH_INVEST_VERSION_ENV)
COMMON_FLAGS	+= -DCRASH_INVEST_VERSION_STR="\"$(CRASH_INVEST_VERSION_ENV)\""


include $(cinternalRepoRoot)/prj/common/common_mkfl/sys_common_unix.Makefile

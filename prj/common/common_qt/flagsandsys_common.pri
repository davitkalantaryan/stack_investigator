#
# repo:			stack_investigator
# file:			flags_common.pri
# path:			prj/common/common_qt/flags_common.pri    
# created on:		2023 Jan 10
# Created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
#

message ("$${PWD}/flags_common.pri")
stackInvestigatorFlagsCommonIncluded = 1


isEmpty( stackInvestigatorRepoRoot ) {
    stackInvestigatorRepoRoot = $$(stackInvestigatorRepoRoot)
    isEmpty(stackInvestigatorRepoRoot) {
        stackInvestigatorRepoRoot = $${PWD}/../../..
    }
}

isEmpty( repositoryRoot ) {
    repositoryRoot = $$(repositoryRoot)
    isEmpty(repositoryRoot) {
        repositoryRoot = $${stackInvestigatorRepoRoot}
    }
}

isEmpty(artifactRoot) {
    artifactRoot = $$(artifactRoot)
    isEmpty(artifactRoot) {
        artifactRoot = $${repositoryRoot}
    }
}

isEmpty( cinternalRepoRoot ) {
    cinternalRepoRoot = $$(cinternalRepoRoot)
    isEmpty(cinternalRepoRoot) {
        cinternalRepoRoot=$${stackInvestigatorRepoRoot}/contrib/cinternal
    }
}

isEmpty( cinternalFlagsCommonIncluded ) {
    include ( "$${cinternalRepoRoot}/prj/common/common_qt/flagsandsys_common.pri" )
    cinternalFlagsCommonIncluded = 1
}

INCLUDEPATH += $${stackInvestigatorRepoRoot}/include
exists($${stackInvestigatorRepoRoot}/sys/$${CODENAME}/$$CONFIGURATION/lib) {
    LIBS += -L$${stackInvestigatorRepoRoot}/sys/$${CODENAME}/$$CONFIGURATION/lib
}
exists($${stackInvestigatorRepoRoot}/sys/$${CODENAME}/$$CONFIGURATION/tlib) {
    LIBS += -L$${stackInvestigatorRepoRoot}/sys/$${CODENAME}/$$CONFIGURATION/tlib
}

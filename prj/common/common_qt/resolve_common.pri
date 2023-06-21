#
# file:			resolve_common.pri
# path:			prj/common/common_qt/resolve_common.pri
# created on:		2023 Jan 21
# Created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
#

message ("$${PWD}/resolve_common.pri")

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

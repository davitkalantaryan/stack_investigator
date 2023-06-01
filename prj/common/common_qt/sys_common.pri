#
# file:			sys_common.pri
# path:			prj/common/common_qt/sys_common.pri    
# created on:		2023 Jan 10
# Created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
#

message ("$${PWD}/sys_common.pri")

isEmpty( stackInvestigatorRepoRoot ) {
	stackInvestigatorRepoRoot = $${PWD}/../../..
}

isEmpty( repositoryRoot ) {
	repositoryRoot = $${stackInvestigatorRepoRoot}
}

isEmpty( cinternalRepoRoot ) {
	cinternalRepoRoot=$${stackInvestigatorRepoRoot}/contrib/cinternal
}


include ( "$${cinternalRepoRoot}/prj/common/common_qt/sys_common.pri" )

#
# repo:			stack_investigator
# file:			sys_common.pri
# path:			prj/common/common_qt/sys_common.pri    
# created on:		2023 Jan 10
# Created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
#

message ("$${PWD}/sys_common.pri")
stackInvestigatorSysCommonIncluded = 1

isEmpty( stackInvestigatorResolveCommonIncluded ) {
        include("$${PWD}/resolve_common.pri")
	stackInvestigatorResolveCommonIncluded = 1
}

isEmpty( cinternalSysCommonIncluded ) {
        include ( "$${cinternalRepoRoot}/prj/common/common_qt/sys_common.pri" )
	cinternalSysCommonIncluded = 1
}

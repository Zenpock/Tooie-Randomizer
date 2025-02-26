#include "stdafx.h"
#include "LogicGroup.h"

int LogicGroup::lastGroupID = 0;

LogicGroup::LogicGroup()
{
	lastGroupID++;
	GroupID = lastGroupID;
	Requirements.push_back(RequirementSet());
}
LogicGroup::LogicGroup(int groupID)
{
	if (groupID > lastGroupID)
		lastGroupID = groupID;
	GroupID = groupID;
}

LogicGroup LogicGroup::GetLogicGroupFromGroupId(int groupID, std::unordered_map<int,LogicGroup>& logicGroups)
{
	if (logicGroups.count(groupID) > 0)
		return logicGroups[groupID];
	return NULL;
}
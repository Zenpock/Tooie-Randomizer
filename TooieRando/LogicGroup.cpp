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

LogicGroup LogicGroup::GetLogicGroupContainingObjectId(int objectID, std::vector<LogicGroup> logicGroups)
{
	for (int i = 0; i < logicGroups.size(); i++)
	{
		for (int j = 0; j < logicGroups[i].objectIDsInGroup.size(); j++)
		{
			if (logicGroups[i].objectIDsInGroup[j] == objectID)
				return logicGroups[i];
		}
	}
	return NULL;
}

LogicGroup LogicGroup::GetLogicGroupContainingMoveId(int moveID, std::vector<LogicGroup> logicGroups)
{
	for (int i = 0; i < logicGroups.size(); i++)
	{
		if (logicGroups[i].containedMove == moveID)
			return logicGroups[i];
	}
	return NULL;
}

LogicGroup LogicGroup::GetLogicGroupFromGroupId(int groupID, std::vector<LogicGroup> logicGroups)
{
	for (int i = 0; i < logicGroups.size(); i++)
	{
		if (logicGroups[i].GroupID == groupID)
			return logicGroups[i];
	}
	return NULL;
}
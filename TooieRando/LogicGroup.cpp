#include "stdafx.h"
#include "LogicGroup.h"

int LogicGroup::lastGroupID = 0;

LogicGroup::LogicGroup()
{
	lastGroupID++;
	GroupID = lastGroupID;
}
LogicGroup::LogicGroup(int groupID)
{
	if (groupID > lastGroupID)
		lastGroupID = groupID;
	GroupID = groupID;
}
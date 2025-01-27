#pragma once
#include <string>
#include <vector>
#include "RandomizedObject.h"
class LogicGroup
{
private:
	static int lastGroupID;
public:
	std::string GroupName = "Group";
	std::vector<int> RequiredAbilities;
	std::vector<std::string> RequiredItems;
	std::vector<int> RequiredItemsCount;
	std::vector<LogicGroup*> DependentGroups;
	std::vector<RandomizedObject*> objectsInGroup;
	std::vector<int> objectIDsInGroup; //Stores the object IDs in the group
	std::vector<int> dependentGroupIDs; //Stores the object IDs in the group

	int GroupID = -1;
	LogicGroup();
	LogicGroup(int groupID);
};

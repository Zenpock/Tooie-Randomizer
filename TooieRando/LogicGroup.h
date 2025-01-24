#pragma once
#include <string>
#include <vector>
#include "RandomizedObject.h"
class LogicGroup
{
public:
	std::string GroupName = "Group";
	std::vector<int> RequiredAbilities;
	std::vector<std::string> RequiredItems;
	std::vector<int> RequiredItemsCount;
	std::vector<LogicGroup*> DependentGroups;
	std::vector<RandomizedObject*> objectsInGroup;
	std::vector<int> objectIDsInGroup; //Stores the object IDs in the group

	int GroupID = -1;
	LogicGroup()
	{
		static int lastGroupID;
		lastGroupID++;
		GroupID = lastGroupID;
	}
	std::vector<int> GetDependentGroupsIDs()
	{
		std::vector<int> group;
		for (int i = 0; i < DependentGroups.size(); i++)
		{
			group.push_back(DependentGroups[i]->GroupID);
		}
		return group;
	}
};


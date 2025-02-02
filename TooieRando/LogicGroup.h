#pragma once
#include <string>
#include <vector>
#include "RandomizedObject.h"
class LogicGroup
{
	
private:
	static int lastGroupID;
public:
	class RequirementSet
	{
		public:
		std::string SetName = "Default";
		std::vector<int> RequiredAbilities;
		std::vector<std::string> RequiredItems;
		std::vector<int> RequiredItemsCount;
		std::vector<std::string> RequiredKeys;//Unique Identifiers used to indicate whether a switch or something has been set, used for mumbo and humba flags in levels
	};

	std::string GroupName = "Group";
	std::vector<RequirementSet> Requirements;//Group distinct requirements together, so if you can achieve a goal with grenade eggs or bill drill you would make 2 different requirement sets
	std::vector<RandomizedObject*> objectsInGroup;
	std::vector<int> objectIDsInGroup; //Stores the object IDs in the group
	std::vector<int> dependentGroupIDs; //Stores the group IDs in the group
	std::string key; //The key rewarded after the group has fulfilled at least 1 requirement set
	int GroupID = -1;
	LogicGroup();
	LogicGroup(int groupID);

};


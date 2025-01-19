#pragma once
#include <string>
#include <vector>

class LogicGroup
{
public:
	std::string GroupName = "Group";
	std::vector<int> RequiredAbilities;
	std::vector<int> RequiredItems;
	std::vector<int> RequiredItemsCount;
	std::vector<LogicGroup*> DependentGroups;
};


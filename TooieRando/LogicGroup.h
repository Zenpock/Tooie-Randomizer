#pragma once
#include <string>
#include <vector>
#include "RandomizedObject.h"
#include <unordered_map>
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
	int DependentShuffleGroup = -1; //This refers to the shufflegroups for entrances which this logic group will find one of the entrances within and make it a dependent group
	int AssociatedWarp = -1; //The EntranceID that unlocks this group
	std::string GroupName = "Group";
	std::string SpecialTag = "";//This is a tag used to edit specific groups when randomizing. e.g. changing the note requirement for certain moves/adjusting jiggy requirements based on options
	std::vector<RequirementSet> Requirements;//Group distinct requirements together, so if you can achieve a goal with grenade eggs or bill drill you would make 2 different requirement sets
	std::vector<int> objectIDsInGroup; //Stores the object IDs in the group
	std::vector<int> dependentGroupIDs; //Stores the group IDs in the group
	int containedMove = -1; //The Move ID that is contained within this group. There aren't enough moves in the game for it to matter that you can only have 1 move in a group.
	std::string key; //The key rewarded after the group has fulfilled at least 1 requirement set
	int GroupID = -1;
	static LogicGroup GetLogicGroupFromGroupId(int groupID, std::unordered_map<int,LogicGroup>& logicGroups);

	LogicGroup();
	LogicGroup(int groupID);

};


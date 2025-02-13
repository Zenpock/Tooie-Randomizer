#include "stdafx.h"
#include "LogicHandler.h"

void GetAllAvailableLocations(LogicGroup* startingGroup, LogicGroup::RequirementSet )
{

}

void GetAllAvailableMoveLocations(LogicGroup* startingGroup)
{

}

/// <summary>
/// Get all of the items currently accessible with the given starting location and moves/inventory
/// </summary>
/// <param name="startingGroup"></param>
/// <returns></returns>
LogicHandler::AccessibleThings LogicHandler::GetAllTotals(LogicGroup startingGroup,std::vector<LogicGroup> logicGroups, LogicHandler::AccessibleThings start, std::vector<RandomizedObject> objects,std::vector<MoveObject> moves, std::vector<int>& seenLogicGroups, std::vector<int>& nextLogicGroups)
{
	LogicHandler::AccessibleThings state=GetAccessibleRecursive(startingGroup, logicGroups, start, objects, moves, seenLogicGroups, nextLogicGroups);
	return std::move(state);
}

/// <summary>
/// Add all of the objects from accessible dependents
/// </summary>
/// <param name="startingGroup"></param>
/// <param name="logicGroups"></param>
/// <param name="start"></param>
/// <param name="objects"></param>
/// <param name="moves"></param>
/// <returns></returns>
LogicHandler::AccessibleThings LogicHandler::GetAccessibleRecursive(LogicGroup startingGroup, std::vector<LogicGroup> logicGroups, LogicHandler::AccessibleThings start, std::vector<RandomizedObject> objects, std::vector<MoveObject> moves, std::vector<int>& seenLogicGroups, std::vector<int>& nextLogicGroups)
{
	LogicHandler::AccessibleThings accessible;
	accessible.Add(start);
	auto it = std::find(seenLogicGroups.begin(), seenLogicGroups.end(), startingGroup.GroupID);
	if (it == seenLogicGroups.end()) //Check if we've already been here
	{
		accessible.Add(startingGroup, objects, moves);
		seenLogicGroups.push_back(startingGroup.GroupID);
		for (int i = 0; i < startingGroup.dependentGroupIDs.size(); i++)
		{
			LogicGroup group = LogicGroup::GetLogicGroupFromGroupId(startingGroup.dependentGroupIDs[i], logicGroups);
			if (LogicHandler::FulfillsRequirements(group, start))
			{
				accessible.Add(GetAccessibleRecursive(group, logicGroups, start, objects, moves, seenLogicGroups,nextLogicGroups));
			}
			else
			{
				nextLogicGroups.push_back(startingGroup.dependentGroupIDs[i]);
			}

		}
	}
	return std::move(accessible);
}

/// <summary>
/// Return whether the current state fulfills the requirements to unlock the group
/// </summary>
/// <param name="startingGroup"></param>
/// <param name=""></param>
/// <param name="parent"></param>
/// <returns></returns>
bool LogicHandler::FulfillsRequirements(LogicGroup groupToUnlock, LogicHandler::AccessibleThings state)
{
	bool fulfilled = true;
	for (int i = 0; i < groupToUnlock.Requirements.size(); i++)
	{
		fulfilled = true;
		for (int j = 0; j < groupToUnlock.Requirements[i].RequiredAbilities.size(); j++)
		{
			int ability = groupToUnlock.Requirements[i].RequiredAbilities[j];
			auto matchesAbility = [ability](MoveObject move) {return move.MoveID == ability; };
			auto it = std::find_if(state.Abilities.begin(), state.Abilities.end(), matchesAbility);
			if (it != state.Abilities.end())
			{
				continue;
			}
			else
			{
				fulfilled = false;
				break;
			}
		}
		if (fulfilled == false)
		{
			continue;
		}
		for (int j = 0; j < groupToUnlock.Requirements[i].RequiredItems.size(); j++)
		{
			std::string name = groupToUnlock.Requirements[i].RequiredItems[j];
			auto it = std::find_if(state.ContainedItems.begin(), state.ContainedItems.end(), [name](std::tuple<std::string, int> Item) {return std::get<0>(Item) == name; });
			if (it == state.ContainedItems.end())
			{
				fulfilled = false;
				break;
			}
			else
			{
				if (std::get<1>(state.ContainedItems[it - state.ContainedItems.begin()]) >= groupToUnlock.Requirements[i].RequiredItemsCount[j])
				{
					continue;
				}
				fulfilled = false;
				break;
			}
		}
		if (fulfilled == false)
		{
			continue;
		}
		if (!ContainsRequiredKeys(state, groupToUnlock.Requirements[i]))
		{
			fulfilled = false;
			continue;
		}
		return fulfilled;
	}
	return fulfilled;
}

/// <summary>
/// Return whether or not the accessible spots provided can possibly open the next group based on the amount of available move locations and item locations
/// </summary>
bool LogicHandler::CanFulfillRequirements(LogicHandler::AccessibleThings accessibleSpots, LogicGroup groupToOpen)
{
	bool canFulfill = true;
	for (int i = 0; i < groupToOpen.Requirements.size(); i++)
	{
		canFulfill = true;
		if (accessibleSpots.Abilities.size() < groupToOpen.Requirements[i].RequiredAbilities.size())
		{
			canFulfill = false;
			continue;
		}
		for (int j = 0; j < groupToOpen.Requirements[i].RequiredItems.size(); j++)
		{
			int neededSpots=0;//Number of locations required to fulfill the requirement
			if (groupToOpen.Requirements[i].RequiredItems[j] == "Note")
			{
				int paidAmount = 0;
				for(int levelIndex = 0;levelIndex< accessibleSpots.GetLevels().size();levelIndex++)
				{ 
					paidAmount+=20; //Always try a treble
					neededSpots++;
					int usedNotes = 0; //Number of normal note nests that are available to grab in the entire level
					//Keep trying to add notes until we reach the required value making sure the number of notes does not exceed the maximum notes in the level
					while (paidAmount < groupToOpen.Requirements[i].RequiredItemsCount[j] && usedNotes < 16 && usedNotes+1 < accessibleSpots.GetLocationsFromMap(accessibleSpots.GetLevels()[levelIndex]).size())
					{
						paidAmount += 5;
						neededSpots++;
						usedNotes++;
					}
				}
				if (paidAmount < groupToOpen.Requirements[i].RequiredItemsCount[j]) //If we cannot meet the quota we cannot meet the requirements
				{
					canFulfill = false;
					continue;
				}
			}
			else
			{
				neededSpots += groupToOpen.Requirements[i].RequiredItemsCount[j];
			}
			if (neededSpots > accessibleSpots.Items.size())
			{
				canFulfill = false;
				continue;
			}
		}
		if (accessibleSpots.Items.size() < groupToOpen.Requirements[i].RequiredItems.size())
		{
			canFulfill = false;
			continue;
		}
		if (!ContainsRequiredKeys(accessibleSpots,groupToOpen.Requirements[i]))
		{
			canFulfill = false;
			continue;
		}
	}
	return canFulfill;
}

bool LogicHandler::ContainsRequiredKeys(LogicHandler::AccessibleThings state, LogicGroup::RequirementSet requirements)
{
	bool foundKeys = true;
	for (int i = 0; i < requirements.RequiredKeys.size(); i++)
	{
		foundKeys = true;
			auto it = std::find(state.Keys.begin(), state.Keys.end(), requirements.RequiredKeys[i]);
			if (it == state.Keys.end())
			{
				foundKeys = false;
				break;
			}
	}
	return foundKeys;
}
void LogicHandler::TryRoute(LogicGroup startingGroup,std::vector<LogicGroup> logicGroups, std::vector<int> lookedAtLogicGroups, LogicHandler::AccessibleThings initialState, std::vector<int> viableLogicGroups, std::vector<RandomizedObject> objects, std::vector<MoveObject> moves)
{
	LogicHandler newLogicHandler;
	std::vector<int> nextLogicGroups;
	std::vector<int> lookedLogicGroups;
	LogicHandler::AccessibleThings newState = newLogicHandler.GetAllTotals(startingGroup, logicGroups, initialState, objects, moves, lookedLogicGroups, nextLogicGroups);
	auto it = std::find(viableLogicGroups.begin(), viableLogicGroups.end(), startingGroup.GroupID);
	viableLogicGroups.erase(it);


	for (int i = 0; i < nextLogicGroups.size(); i++)
	{
		auto it = std::find(viableLogicGroups.begin(), viableLogicGroups.end(), nextLogicGroups[i]);
		if (it == viableLogicGroups.end())
		{
			bool canFulfill = LogicHandler::CanFulfillRequirements(newState, LogicGroup::GetLogicGroupFromGroupId(nextLogicGroups[i], logicGroups));
			if (canFulfill)
				viableLogicGroups.push_back(nextLogicGroups[i]);
		}
	}
	if(viableLogicGroups.size()==0)
		OutputDebugString("End");

	for (int i = 0; i < viableLogicGroups.size(); i++)
	{
		LogicHandler::AccessibleThings state;
		LogicGroup viableGroup = LogicGroup::GetLogicGroupFromGroupId(viableLogicGroups[i], logicGroups);
		state.Add(viableGroup, objects, moves);
		TryRoute(viableGroup,logicGroups, lookedAtLogicGroups,state, viableLogicGroups, objects,moves);
	}

}
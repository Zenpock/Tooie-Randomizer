#include "stdafx.h"
#include "LogicHandler.h"

int LogicHandler::seed = 0;
std::vector<RandomizedObject> LogicHandler::objectsList;

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
		auto it = std::find(nextLogicGroups.begin(), nextLogicGroups.end(), startingGroup.GroupID);
		if (it != nextLogicGroups.end())
			nextLogicGroups.erase(it);
		for (int i = 0; i < startingGroup.dependentGroupIDs.size(); i++) //Check dependents
		{
			LogicGroup group = LogicGroup::GetLogicGroupFromGroupId(startingGroup.dependentGroupIDs[i], logicGroups);
			if (LogicHandler::FulfillsRequirements(group, accessible))
			{
				accessible.Add(GetAccessibleRecursive(group, logicGroups, accessible, objects, moves, seenLogicGroups,nextLogicGroups));
			}
			else
			{
				nextLogicGroups.push_back(startingGroup.dependentGroupIDs[i]);
			}

		}
		
	}

	std::vector<int> tempLogicGroups = nextLogicGroups;
	for (int i = 0; i < tempLogicGroups.size(); i++) //Check if old next groups can be visited
	{
		LogicGroup group = LogicGroup::GetLogicGroupFromGroupId(tempLogicGroups[i], logicGroups);
		if (LogicHandler::FulfillsRequirements(group, accessible))
		{
			accessible.Add(GetAccessibleRecursive(group, logicGroups, accessible, objects, moves, seenLogicGroups, nextLogicGroups));
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
			auto matchesAbility = [ability](std::tuple<int, MoveObject> move) {return (std::get<1>(move)).Ability == ability; };
			auto it = std::find_if(state.SetAbilities.begin(), state.SetAbilities.end(), matchesAbility);
			if (it != state.SetAbilities.end())
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
		canFulfill = accessibleSpots.CanFulfill(groupToOpen.Requirements[i]);
		if (canFulfill)
			return true;
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
void LogicHandler::TryRoute(LogicGroup startingGroup,std::vector<LogicGroup> logicGroups, std::vector<int> lookedAtLogicGroups, std::vector<int> nextLogicGroups , LogicHandler::AccessibleThings initialState, std::vector<int> viableLogicGroups, std::vector<RandomizedObject> objects, std::vector<MoveObject> moves)
{
	LogicHandler newLogicHandler;
	LogicHandler::AccessibleThings newState = newLogicHandler.GetAllTotals(startingGroup, logicGroups, initialState, objects, moves, lookedAtLogicGroups, nextLogicGroups);
	auto it = std::find(viableLogicGroups.begin(), viableLogicGroups.end(), startingGroup.GroupID);
	if(it != viableLogicGroups.end())
		viableLogicGroups.erase(it);
	std::vector<int> tempViableLogicGroups;

	for (int i = 0; i < viableLogicGroups.size(); i++) //Iterate through the viable groups and check if they're still viable
	{
		bool canFulfill = LogicHandler::CanFulfillRequirements(newState, LogicGroup::GetLogicGroupFromGroupId(viableLogicGroups[i], logicGroups));
		if (canFulfill)
			tempViableLogicGroups.push_back(viableLogicGroups[i]);
	}
	viableLogicGroups = tempViableLogicGroups;

	for (int i = 0; i < nextLogicGroups.size(); i++) //Iterate through all of the dependant groups for the currently unlocked groups and see if they can be fulfilled with the available locations
	{
		auto it = std::find(viableLogicGroups.begin(), viableLogicGroups.end(), nextLogicGroups[i]);
		if (it == viableLogicGroups.end())
		{
			LogicGroup group = LogicGroup::GetLogicGroupFromGroupId(nextLogicGroups[i], logicGroups);
			bool canFulfill = LogicHandler::CanFulfillRequirements(newState, group);
			if (canFulfill)
				viableLogicGroups.push_back(nextLogicGroups[i]);
		}
	}
	if (viableLogicGroups.size() == 0) //If we've run out of viable logic groups this means the chain has ended
	{
		//CString str;
		//str.Format("Ended Chain of Size %X At Group %s \n", lookedAtLogicGroups.size(),startingGroup.GroupName.c_str());
		//OutputDebugString(str);
	}
		

	for (int i = 0; i < viableLogicGroups.size(); i++) //Iterate through all other viable logic groups (meaning groups that can actually have their requirements fulfilled by the number of objects and ability locations available for object placement)
	{
		LogicGroup viableGroup = LogicGroup::GetLogicGroupFromGroupId(viableLogicGroups[i], logicGroups);
		for (int j = 0; j < viableGroup.Requirements.size(); j++)
		{
			if (newState.CanFulfill(viableGroup.Requirements[j]))
			{
				LogicHandler::AccessibleThings state;
					state.Add(newState);
					state.AddAbilities(viableGroup.Requirements[j], moves);
					state.AddItems(viableGroup.Requirements[j]);
					state.UpdateCollectables();
					TryRoute(viableGroup, logicGroups, lookedAtLogicGroups,nextLogicGroups, state, viableLogicGroups, objects, moves);
			}
		}
	}

}
#include "stdafx.h"
#include "LogicHandler.h"
#include <iostream>

std::vector<OptionData>* LogicHandler::options;
std::unordered_map<int,RandomizedObject> LogicHandler::objectsList;
std::unordered_map<int, Entrance> LogicHandler::EntranceList;
bool LogicHandler::alreadySetup = false;
bool LogicHandler::debug = true; //Set this value to true to activate the debug prints in the logic handler

std::unordered_map<int, std::vector<int>> LogicHandler::normalLevelObjectsMapAll; //List of all objects sorted int groups by level

std::unordered_map<int, std::vector<int>> LogicHandler::shuffleGroups; //List of all entrances sorted by shuffle group

std::unordered_map<int, int> LogicHandler::entranceAssociations; //<EntrancedID,LogicGroupID>

bool LogicHandler::objectsNotRandomized; //Whether the objects not randomized options is set

const int groupsToTraverseBeforeBacktrack = 4000;
std::vector<std::string>  LogicHandler::WorldTags{"World1","World2","World3","World4","World5","World6","World7","World8","World9","Hag1" };
std::vector<int>  LogicHandler::notePrices{ 25,30,35,45,85,95,110,160,170,180,200,265,275,290,315,390,405,420,525,545,590,640,660,765 };
std::vector<int>  LogicHandler::glowboPrices{ 0x2,0x4,0x6,0x7,0x9,0xB,0xD,0xF };
//The number of silos in the associated world
std::unordered_map<int, int>  LogicHandler::siloIndexStep = {
		{1, 3}, {2, 2}, {3, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 2}, {8, 1},{9, 0}
};
std::unordered_map<int, std::string>  LogicHandler::WorldPrefixes{ {1,"MT"},{2,"GGM"},{3,"WW"},{4,"JRL"},{5,"TDL" },{6,"GI"},{7,"HFP"},{8,"CCL"},{9,"CK"},{10,"Hag1"} };


/// <summary>
/// This is the list of entrances as they refer to worlds entrances from outside the world (EntranceID,VanillaWorldIndex)
/// </summary>
std::unordered_map<int, int>  LogicHandler::EntranceToWorld {{0x1, 0x1},{0x3, 0x2},{0x5, 0x3},{0x7, 0x4},{0xB, 0x5},{0xF, 0x6},{0x9, 0x7},{0xD, 0x8},{0x11, 0x9} };

/// <summary>
/// List of the internal entrances within a world tied to their associated world (EntranceID,VanillaWorldIndex)
/// </summary>
std::unordered_map<int, int>  LogicHandler::EntranceInWorld{ {0x2, 0x1},{0x4, 0x2},{0x6, 0x3},{0x8, 0x4},{0xC, 0x5},{0x10, 0x6},{0xA, 0x7},{0xE, 0x8},{0x12, 0x9} };


std::vector<int> LogicHandler::NoRandomizationIDs;
std::vector<int> LogicHandler::worldPrices;

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
LogicHandler::AccessibleThings LogicHandler::GetAllTotals(LogicGroup startingGroup,std::unordered_map<int,LogicGroup>& logicGroups, LogicHandler::AccessibleThings start, std::vector<RandomizedObject>& objects,std::vector<MoveObject>& moves, std::vector<int>& seenLogicGroups, std::vector<int>& nextLogicGroups, std::vector<int>& viableLogicGroups)
{
	LogicHandler::AccessibleThings state=GetAccessibleRecursive(startingGroup, logicGroups, start, objects, moves, seenLogicGroups, nextLogicGroups,viableLogicGroups);
	return state;
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
LogicHandler::AccessibleThings LogicHandler::GetAccessibleRecursive(LogicGroup& startingGroup, std::unordered_map<int, LogicGroup>& logicGroups, LogicHandler::AccessibleThings& start, std::vector<RandomizedObject>& objects, std::vector<MoveObject>& moves, std::vector<int>& seenLogicGroups, std::vector<int>& nextLogicGroups, std::vector<int>& viableLogicGroups)
{
	//OutputDebugString(("Entering " + startingGroup.GroupName + "\n").c_str());
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
			auto it = std::find(seenLogicGroups.begin(), seenLogicGroups.end(), startingGroup.dependentGroupIDs[i]);
			if (it == seenLogicGroups.end()) //Check if we've already added this group
			{
				LogicGroup group = LogicGroup::GetLogicGroupFromGroupId(startingGroup.dependentGroupIDs[i], logicGroups);
				HandleSpecialTags(&group, &accessible);
				if (LogicHandler::FulfillsRequirements(&group, &accessible))
				{

					auto it = std::find(nextLogicGroups.begin(), nextLogicGroups.end(), group.GroupID);
					if (it != nextLogicGroups.end())
					{
						nextLogicGroups.erase(it);
					}
					auto viableIterator = std::find(viableLogicGroups.begin(), viableLogicGroups.end(), group.GroupID);
					if (viableIterator != viableLogicGroups.end())
						viableLogicGroups.erase(viableIterator);

					//OutputDebugString(("Group Requirements Already Fulfilled " + group.GroupName + "\n").c_str());
					accessible.Add(GetAccessibleRecursive(group, logicGroups, accessible, objects, moves, seenLogicGroups, nextLogicGroups, viableLogicGroups));
				}
				else
				{
					auto nextLogicGroupIt = std::find(nextLogicGroups.begin(), nextLogicGroups.end(), startingGroup.dependentGroupIDs[i]);
					if (nextLogicGroupIt == nextLogicGroups.end()) //Check that we haven't already placed this in the next logic groups list
					{
						//OutputDebugString(("Add next group " + group.GroupName + "\n").c_str());
						nextLogicGroups.push_back(startingGroup.dependentGroupIDs[i]);
					}
				}
			}
		}
		
	}

	std::vector<int> tempLogicGroups = nextLogicGroups;
	for (int i = 0; i < tempLogicGroups.size(); i++) //Check if old next groups can be visited
	{
		LogicGroup group = LogicGroup::GetLogicGroupFromGroupId(tempLogicGroups[i], logicGroups);
		HandleSpecialTags(&group, &accessible);
		if (LogicHandler::FulfillsRequirements(&group, &accessible))
		{
			auto it = std::find(nextLogicGroups.begin(), nextLogicGroups.end(), group.GroupID);
			if (it != nextLogicGroups.end())
			{
				auto viableIterator = std::find(viableLogicGroups.begin(), viableLogicGroups.end(), group.GroupID);
				if (viableIterator != viableLogicGroups.end())
					viableLogicGroups.erase(viableIterator);
				//OutputDebugString(("Old Group Requirements Already Fulfilled " + group.GroupName + "\n").c_str());
				nextLogicGroups.erase(it);
				accessible.Add(GetAccessibleRecursive(group, logicGroups, accessible, objects, moves, seenLogicGroups, nextLogicGroups, viableLogicGroups));
			}
		}
	}
	//OutputDebugString(("Exit " + startingGroup.GroupName + "\n").c_str());
	return accessible;
}

/// <summary>
/// Return whether the current state fulfills any of the requirement sets to unlock the group
/// </summary>
/// <param name="startingGroup"></param>
/// <param name=""></param>
/// <param name="parent"></param>
/// <returns></returns>
bool LogicHandler::FulfillsRequirements(LogicGroup* groupToUnlock, LogicHandler::AccessibleThings* state)
{
	HandleSpecialTags(groupToUnlock, state);

	bool fulfilled = true;
	for (int i = 0; i < groupToUnlock->Requirements.size(); i++)
	{
		fulfilled = true;
		for (int j = 0; j < groupToUnlock->Requirements[i].RequiredAbilities.size(); j++)
		{
			int ability = groupToUnlock->Requirements[i].RequiredAbilities[j];
			auto matchesAbility = [ability](std::tuple<int, MoveObject> move) {return (std::get<1>(move)).Ability == ability; };
			auto it = std::find_if(state->SetAbilities.begin(), state->SetAbilities.end(), matchesAbility);
			if (it != state->SetAbilities.end())
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
		for (int j = 0; j < groupToUnlock->Requirements[i].RequiredItems.size(); j++)
		{
			std::string name = groupToUnlock->Requirements[i].RequiredItems[j];
			auto it = std::find_if(state->ContainedItems.begin(), state->ContainedItems.end(), [name](std::tuple<std::string, int> Item) {return std::get<0>(Item) == name; });
			if (it == state->ContainedItems.end())
			{
				fulfilled = false;
				break;
			}
			else
			{
				if (std::get<1>(state->ContainedItems[it - state->ContainedItems.begin()]) >= groupToUnlock->Requirements[i].RequiredItemsCount[j])
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
		if (!ContainsRequiredKeys(state, &groupToUnlock->Requirements[i]))
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
bool LogicHandler::CanFulfillRequirements(LogicHandler::AccessibleThings* accessibleSpots, LogicGroup* groupToOpen)
{
	HandleSpecialTags(groupToOpen, accessibleSpots);

	bool canFulfill = true;
	for (int i = 0; i < groupToOpen->Requirements.size(); i++)
	{
		canFulfill = accessibleSpots->CanFulfill(&groupToOpen->Requirements[i]);
		if (canFulfill)
			return true;
	}
	return canFulfill;
}

bool LogicHandler::ContainsRequiredKeys(LogicHandler::AccessibleThings* state, LogicGroup::RequirementSet* requirements)
{
	bool foundKeys = true;
	for (int i = 0; i < requirements->RequiredKeys.size(); i++)
	{
		foundKeys = true;
			auto it = std::find(state->Keys.begin(), state->Keys.end(), requirements->RequiredKeys[i]);
			if (it == state->Keys.end())
			{
				foundKeys = false;
				break;
			}
	}
	return foundKeys;
}

/// <summary>
/// Get the index for the world at the given world index so if GGM is at world 1 then the return value would be 2
/// </summary>
int LogicHandler::GetWorldAtOrder(LogicHandler::AccessibleThings* state, int worldNumber)
{
	for (int i = 0; i < state->SetWarps.size(); i++)
	{
		if (EntranceToWorld[state->SetWarps[i].first] == worldNumber)
		{
			return EntranceInWorld[state->SetWarps[i].second];
		}
	}
	return -1;
}

/// <summary>
/// Get the list of all of the worlds that have been setup so far in order
/// </summary>
std::vector<int> LogicHandler::GetWorldsInOrder(LogicHandler::AccessibleThings* state)
{
	std::vector<int> WorldOrder;
	for (int i = 1; i <= 9; i++)
	{
		int world = GetWorldAtOrder(state, i);
		WorldOrder.push_back(world);
	}
	return WorldOrder;
}


bool checkWorldAndTag(int compare, int world, std::string tag,LogicGroup* group)
{
	return ((compare == world) && (tag == group->SpecialTag));
}
void LogicHandler::HandleSpecialTags(LogicGroup* group, LogicHandler::AccessibleThings* state)
{
	if (group->SpecialTag == "")
	{
		return;
	}
	std::vector<int> worlds = GetWorldsInOrder(state);

	for (int i = 0;i<worldPrices.size();i++)
	{
		
		if (group->SpecialTag == WorldTags[i])
		{
			if (i > 0 && worlds[i - 1] == -1)
				return;
			group->Requirements[0].RequiredItemsCount[0] = worldPrices[i];
			return;
		}
	}
	//I'm assuming that the only required Item is a note/glowbo
	//Prices are set really high in the logic so they are not traversed before the world is unlocked
	int siloIndex = 0;
	int glowboIndex = 0;
	for (int i = 0; i < worlds.size(); i++)
	{
		if (worlds[i] == -1) //If we are looking past the point where the worlds are setup return
			return;
		int currentWorld = worlds[i];
		std::string currentTag = group->SpecialTag;

		if (WorldPrefixes[currentWorld] + "Glowbo" == currentTag && glowboPrices.size() < glowboIndex)
		{
			group->Requirements[0].RequiredItemsCount[0] = glowboPrices[glowboIndex];
			return;
		}
		int inLevelIndex = -1;
		if (WorldPrefixes[currentWorld] + "Silo1" == currentTag)
		{
			inLevelIndex = 0;
		}
		else if (WorldPrefixes[currentWorld] + "Silo2" == currentTag)
		{
			inLevelIndex = 1;
		}
		else if (WorldPrefixes[currentWorld] + "Silo3" == currentTag)
		{
			inLevelIndex = 2;
		}
		if (inLevelIndex >= 0)
		{
			group->Requirements[0].RequiredItemsCount[0] = notePrices[siloIndex + inLevelIndex];
			return;
		}

		siloIndex += siloIndexStep[worlds[i]];

		glowboIndex++;
		if (i == 2)
		{
			if (group->SpecialTag == "IOHGlowbo")
			{
				group->Requirements[0].RequiredItemsCount[0] = glowboPrices[glowboIndex];
				return;
			}
			glowboIndex++;
		}
		if (i < 4) //Handle changing all of the ioh silo notes
		{

			if (group->SpecialTag == "IOHSilo" + std::to_string(i + 1))
			{
				group->Requirements[0].RequiredItemsCount[0] = notePrices[siloIndex];
				return;
			}
			siloIndex++;
		}
	}
	
}

LogicHandler::AccessibleThings LogicHandler::TryRoute(LogicGroup startingGroup,std::unordered_map<int,LogicGroup>& logicGroups, std::vector<int> lookedAtLogicGroups, std::vector<int> nextLogicGroups , LogicHandler::AccessibleThings initialState, std::vector<int> viableLogicGroups, std::vector<RandomizedObject> objects, std::vector<MoveObject> moves, int depth, std::default_random_engine& rng)
{
	groupsTraversed++;
	if (groupsTraversed > groupsToTraverseBeforeBacktrack && depth > 1)
	{
		LogicHandler::AccessibleThings revertState;
		revertState.depthToLeave=30;
		groupsTraversed = 0;
		DebugPrint("Reached Group Traversal Limit at depth: " + std::to_string(depth) + ", in Group: " + startingGroup.GroupName);
		return revertState;
	}
	DebugPrint("Recursion Depth: " + std::to_string(depth) + ", Processing Group: " + startingGroup.GroupName);

	LogicHandler::AccessibleThings newState = GetAllTotals(startingGroup, logicGroups, initialState, objects, moves, lookedAtLogicGroups, nextLogicGroups,viableLogicGroups);
	auto it = std::find(viableLogicGroups.begin(), viableLogicGroups.end(), startingGroup.GroupID);
	if(it != viableLogicGroups.end())
		viableLogicGroups.erase(it);
	std::vector<int> tempViableLogicGroups;

	for (int i = 0; i < viableLogicGroups.size(); i++) //Iterate through the viable groups and check if they're still viable
	{
		LogicGroup group = LogicGroup::GetLogicGroupFromGroupId(viableLogicGroups[i], logicGroups);
		HandleSpecialTags(&group, &newState);
		bool canFulfill = LogicHandler::CanFulfillRequirements(&newState, &LogicGroup::GetLogicGroupFromGroupId(viableLogicGroups[i], logicGroups));
		auto it = std::find(lookedAtLogicGroups.begin(), lookedAtLogicGroups.end(), group.GroupID);
		if (it != viableLogicGroups.end())
		if (canFulfill)
			tempViableLogicGroups.push_back(viableLogicGroups[i]);
	}
	viableLogicGroups = tempViableLogicGroups;

	std::vector<int> tempLogicGroups = nextLogicGroups;
	std::shuffle(tempLogicGroups.begin(), tempLogicGroups.end(), rng);

	for (int i = 0; i < tempLogicGroups.size(); i++) //Iterate through all of the dependant groups for the currently unlocked groups and see if they can be fulfilled with the available locations
	{

		auto it = std::find(viableLogicGroups.begin(), viableLogicGroups.end(), tempLogicGroups[i]);
		if (it == viableLogicGroups.end())
		{
			LogicGroup group = LogicGroup::GetLogicGroupFromGroupId(tempLogicGroups[i], logicGroups);
			HandleSpecialTags(&group, &newState);
			bool canFulfill = LogicHandler::CanFulfillRequirements(&newState, &group);

			if (canFulfill)
				viableLogicGroups.push_back(tempLogicGroups[i]);
		}
	}

	auto keyIterator = std::find(newState.Keys.begin(), newState.Keys.end(), "Grunty Defeated");
	if (keyIterator != newState.Keys.end()) //End State
	{
		//::MessageBox(NULL, "Valid Path Found", "Completed", NULL);
		newState.done = true;
		return newState;
	}

	tempLogicGroups = viableLogicGroups;
	

	std::shuffle(tempLogicGroups.begin(), tempLogicGroups.end(), rng);


	for (int i = 0; i < tempLogicGroups.size(); i++) //Iterate through all other viable logic groups (meaning groups that can actually have their requirements fulfilled by the number of objects and ability locations available for object placement)
	{
		LogicGroup viableGroup = LogicGroup::GetLogicGroupFromGroupId(tempLogicGroups[i], logicGroups);
		HandleSpecialTags(&viableGroup, &newState);
		std::vector<LogicGroup::RequirementSet> requirements = viableGroup.Requirements;
		std::shuffle(requirements.begin(), requirements.end(), rng);

		std::vector<int> shuffleGroup = shuffleGroups[viableGroup.DependentShuffleGroup];
		std::shuffle(shuffleGroup.begin(), shuffleGroup.end(), rng);

		for (int j = 0; j < requirements.size(); j++)
			{
				if (newState.CanFulfill(&requirements[j]))
				{

					if (viableGroup.DependentShuffleGroup != -1)
					{
						for (int i = 0; i < shuffleGroup.size(); i++)
						{
							LogicHandler::AccessibleThings state;

							int exitID = shuffleGroup[i];
							auto matchesEntrance = [exitID](std::pair<int, int> entrancePair) {return (std::get<0>(entrancePair)) == exitID || std::get<1>(entrancePair) == exitID; };
							auto it = std::find_if(newState.SetWarps.begin(), newState.SetWarps.end(), matchesEntrance);
							state.Add(newState);
							if (it == newState.SetWarps.end()) //Check if this entrance has been set before
							{
								state.SetWarps.push_back(std::make_pair(viableGroup.AssociatedWarp, exitID));
							}
								//Get the Group Associated with this entrance
								DebugPrint("Set Warp " + std::to_string(viableGroup.AssociatedWarp) + " to " + std::to_string(exitID) + " at depth " + std::to_string(depth + 1));
								LogicGroup tempGroup = viableGroup;
								tempGroup.dependentGroupIDs.push_back(entranceAssociations[exitID]);
								
								state.AddAbilities(requirements[j], moves,rng);

								state.AddItems(requirements[j],rng);
								state.UpdateCollectables();
								DebugPrint("Recursing into Group: " + viableGroup.GroupName + " at depth " + std::to_string(depth + 1));

								LogicHandler::AccessibleThings doneState = TryRoute(tempGroup, logicGroups, lookedAtLogicGroups, nextLogicGroups, state, tempLogicGroups, objects, moves, depth + 1,rng);
								if (doneState.done || (doneState.depthToLeave > 0 && depth > 1))
								{
									doneState.depthToLeave--;
									return doneState;
								}
							
						}
					}
					else
					{
						LogicHandler::AccessibleThings state;
						state.Add(newState);
						state.AddAbilities(requirements[j], moves,rng);

						state.AddItems(requirements[j],rng);
						state.UpdateCollectables();
						DebugPrint("Recursing into Group: " + viableGroup.GroupName + " at depth " + std::to_string(depth + 1));

						LogicHandler::AccessibleThings doneState = TryRoute(viableGroup, logicGroups, lookedAtLogicGroups, nextLogicGroups, state, tempLogicGroups, objects, moves, depth + 1,rng);
						if (doneState.done || (doneState.depthToLeave > 0 && depth > 1))
						{
							doneState.depthToLeave--;
							return doneState;
						}
					}
				}
			}
		
	}

	DebugPrint("Backtracking from Group: " + startingGroup.GroupName + " at depth " + std::to_string(depth));
	LogicHandler::AccessibleThings state;
	return state;
}

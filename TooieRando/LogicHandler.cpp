#include "stdafx.h"
#include "LogicHandler.h"
#include <iostream>
#include <map>


std::vector<OptionData>* LogicHandler::options;
//Index the RandomizedObject by their RandoObjectId
std::unordered_map<int,RandomizedObject> LogicHandler::objectsList;
std::unordered_map<int, Entrance> LogicHandler::EntranceList;
bool LogicHandler::alreadySetup = false;
bool LogicHandler::generousJiggies = false;

//Set this value to true to activate the debug prints in the logic handler
bool LogicHandler::debug = false; 
bool LogicHandler::printLog = false;
bool LogicHandler::saveLogging = true;
int LogicHandler::debugLevel = 6;

//List of all normal objects sorted into groups by level
std::unordered_map<int, std::set<int>> LogicHandler::normalLevelObjectsMapAll; 

//List of all objects sorted into groups by level
std::unordered_map<int, std::set<int>> LogicHandler::levelObjectsMapAll;

//List of all normal locations
std::set<int> LogicHandler::normalAll;

//List of all objects
std::set<int> LogicHandler::allObjects;

//List of all spawnable objects
std::set<int> LogicHandler::allSpawnableObjects;


//List of all entrances sorted by shuffle group
std::unordered_map<int, std::vector<int>> LogicHandler::shuffleGroups; 

//<EntrancedID,LogicGroupID>
std::unordered_map<int, int> LogicHandler::entranceAssociations; 

//Ability,RandoObjectID
std::unordered_map<int, int> LogicHandler::AbilityItems;

//Whether the objects not randomized options is set
bool LogicHandler::objectsNotRandomized; 

const int groupsToTraverseBeforeBacktrack = 0x500;
std::vector<std::string>  LogicHandler::WorldTags{"World1","World2","World3","World4","World5","World6","World7","World8","World9","Hag1" };
std::vector<int>  LogicHandler::notePrices{ 25,30,35,45,85,95,110,160,170,180,200,265,275,290,315,390,405,420,525,545,590,640,660,765 };

//The number of silos in the associated world
std::unordered_map<int, int>  LogicHandler::siloIndexStep = {
		{1, 3}, {2, 2}, {3, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 2}, {8, 1},{9, 0}
};
std::unordered_map<int, std::string>  LogicHandler::WorldPrefixes{ {0,"IOH"}, {1,"MT"},{2,"GGM"},{3,"WW"},{4,"JRL"},{5,"TDL" },{6,"GI"},{7,"HFP"},{8,"CCL"},{9,"CK"},{10,"Hag1"} };


/// <summary>
/// This is the list of entrances as they refer to worlds entrances from outside the world (EntranceID,VanillaWorldIndex)
/// </summary>
std::unordered_map<int, int>  LogicHandler::EntranceToWorld {{0x1, 0x1},{0x3, 0x2},{0x5, 0x3},{0x7, 0x4},{0xB, 0x5},{0xF, 0x6},{0x9, 0x7},{0xD, 0x8},{0x11, 0x9} };

/// <summary>
/// List of the internal entrances within a world tied to their associated world (EntranceID,VanillaWorldIndex)
/// </summary>
std::unordered_map<int, int>  LogicHandler::EntranceInWorld{ {0x2, 0x1},{0x4, 0x2},{0x6, 0x3},{0x8, 0x4},{0xC, 0x5},{0x10, 0x6},{0xA, 0x7},{0xE, 0x8},{0x12, 0x9} };


std::set<int> LogicHandler::NoRandomizationIDs;
std::set<int> LogicHandler::HintBlacklist;
std::set<int> LogicHandler::LevelRestrictedIDs;
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
LogicHandler::AccessibleThings LogicHandler::GetAllTotals(LogicGroup startingGroup,std::unordered_map<int,LogicGroup>& logicGroups, LogicHandler::AccessibleThings start, const std::vector<RandomizedObject>& objects, std::set<int>& seenLogicGroups, std::set<int>& nextLogicGroups, std::set<int>& viableLogicGroups)
{
	LogicHandler::AccessibleThings state=GetAccessibleRecursive(startingGroup, logicGroups, start, objects, seenLogicGroups, nextLogicGroups,viableLogicGroups);
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
LogicHandler::AccessibleThings LogicHandler::GetAccessibleRecursive(LogicGroup& startingGroup, std::unordered_map<int, LogicGroup>& logicGroups, LogicHandler::AccessibleThings& start, const std::vector<RandomizedObject>& objects, std::set<int>& seenLogicGroups, std::set<int>& nextLogicGroups, std::set<int>& viableLogicGroups)
{

	//OutputDebugString(("Entering " + startingGroup.GroupName + "\n").c_str());
	LogicHandler::AccessibleThings accessible(start);
	DebugPrintPriority("Attempt Traverse: " + startingGroup.GroupName,2);

	if (seenLogicGroups.find(startingGroup.GroupID) == seenLogicGroups.end()) //Check if we've already been here
	{
		DebugPrintPriority("Add new group Accessible Recursive: " + startingGroup.GroupName,2);
		accessible.Add(startingGroup, objects);
		seenLogicGroups.insert(startingGroup.GroupID);
		nextLogicGroups.erase(startingGroup.GroupID);
		for (int i = 0; i < startingGroup.dependentGroupIDs.size(); i++) //Check dependents
		{
			if (seenLogicGroups.find(startingGroup.dependentGroupIDs[i]) == seenLogicGroups.end()) //Check if we've already added this group
			{
				LogicGroup group = LogicGroup::GetLogicGroupFromGroupId(startingGroup.dependentGroupIDs[i], logicGroups);
				DebugPrintPriority("Check if dependant group is already fulfilled: " + group.GroupName,2);
				HandleSpecialTags(&group, &accessible);
				if (LogicHandler::FulfillsRequirements(&group, &accessible))
				{
					nextLogicGroups.erase(group.GroupID);
					viableLogicGroups.erase(group.GroupID);
					accessible.Add(GetAccessibleRecursive(group, logicGroups, accessible, objects, seenLogicGroups, nextLogicGroups, viableLogicGroups));
				}
				else
				{
					if (nextLogicGroups.find(startingGroup.dependentGroupIDs[i]) == nextLogicGroups.end()) //Check that we haven't already placed this in the next logic groups list
					{
						DebugPrintPriority("Could not fulfill keep for next logic group: " + group.GroupName,2);
						nextLogicGroups.insert(startingGroup.dependentGroupIDs[i]);
					}
				}
			}
		}
		
	}
	else
	{
		return accessible;
	}
	DebugPrintPriority("Check old Groups " + startingGroup.GroupName,2);
	//Whenever a new key/unrandomized move is added recheck the old groups otherwise we can just do this once
	if (accessible.checkedOldGroups)
	{
		return accessible;
	}
	std::set<int> tempLogicGroups = nextLogicGroups;
	for (int logicGroup : tempLogicGroups) //Check if old next groups can be visited
	{
		LogicGroup group = LogicGroup::GetLogicGroupFromGroupId(logicGroup, logicGroups);
		DebugPrintPriority("Check if old group is already fulfilled: " + group.GroupName,2);

		HandleSpecialTags(&group, &accessible);
		if (LogicHandler::FulfillsRequirements(&group, &accessible))
		{
			if (nextLogicGroups.erase(group.GroupID) != 0)
			{
				viableLogicGroups.erase(group.GroupID);
				accessible.Add(GetAccessibleRecursive(group, logicGroups, accessible, objects, seenLogicGroups, nextLogicGroups, viableLogicGroups));
			}
		}
	}
	accessible.checkedOldGroups = true;
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
			auto matchesAbility = [ability](std::tuple<int, int> move) {return (std::get<1>(move)) == ability; };
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
			CollectableId type = groupToUnlock->Requirements[i].RequiredItems[j];
			auto it = std::find_if(state->ContainedItems.begin(), state->ContainedItems.end(), [type](std::pair<CollectableId, int> Item) {return Item.first == type; });
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
bool LogicHandler::CanFulfillRequirements(LogicHandler::AccessibleThings* accessibleSpots, LogicGroup* groupToOpen, std::unordered_map<int, int>& unusedNormalGlobal)
{
	HandleSpecialTags(groupToOpen, accessibleSpots);

	bool canFulfill = true;
	for (int i = 0; i < groupToOpen->Requirements.size(); i++)
	{
		if (groupToOpen->Requirements[i].Incidental)
			return false;

		canFulfill = accessibleSpots->CanFulfill(&groupToOpen->Requirements[i],unusedNormalGlobal);
		if (canFulfill)
			return true;
	}
	return canFulfill;
}

bool LogicHandler::ContainsRequiredKeys(const LogicHandler::AccessibleThings* state, const LogicGroup::RequirementSet* requirements)
{
	bool foundKeys = true;
	for (const auto& key: requirements->RequiredKeys)
	{
		foundKeys = true;
		if (state->Keys.count(key)==0)
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
int LogicHandler::GetWorldAtOrder(const LogicHandler::AccessibleThings* state, int worldNumber)
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

bool LogicHandler::ContainsEntrance(const LogicHandler::AccessibleThings* state, int entranceID)
{
	for (int i = 0; i < state->SetWarps.size(); i++)
	{
		if (state->SetWarps[i].first == entranceID || state->SetWarps[i].second == entranceID)
		{
			return true;
		}
	}
	return false;
}

/// <summary>
/// Get the list of all of the worlds that have been setup so far in order
/// </summary>
std::vector<int> LogicHandler::GetWorldsInOrder(const LogicHandler::AccessibleThings* state)
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
void LogicHandler::HandleSpecialTags(LogicGroup* group,const LogicHandler::AccessibleThings* state)
{
	if (group->SpecialTag == "")
	{
		return;
	}
	std::vector<int> worlds = GetWorldsInOrder(state);

	//Normally worlds would
	for (int i = 0;i<worldPrices.size();i++)
	{
		if (group->SpecialTag == WorldTags[i])
		{
			if (i > 0 && worlds[i - 1] == -1)
				return;
			int thisEntrance = group->AssociatedWarp;
			auto matchesEntrance = [thisEntrance](std::pair<int, int> entrancePair) {return (std::get<0>(entrancePair)) == thisEntrance || std::get<1>(entrancePair) == thisEntrance; };
			auto foundEntrance = std::find_if(state->SetWarps.begin(), state->SetWarps.end(), matchesEntrance);
			if(foundEntrance != state->SetWarps.end()&& group->dependentGroupIDs.size()==0)
			{
				group->dependentGroupIDs.push_back(entranceAssociations[(*foundEntrance).second]);
			}
			int jiggiesRequiredInLogic = worldPrices[i];
			if (generousJiggies)
			{
				if (jiggiesRequiredInLogic > 5)
				{
					if (jiggiesRequiredInLogic < 15)
					{
						jiggiesRequiredInLogic*=1.1;
					}
					else if (jiggiesRequiredInLogic < 35)
					{
						jiggiesRequiredInLogic *= 1.15;
					}
					else if (jiggiesRequiredInLogic < 45)
					{
						jiggiesRequiredInLogic *= 1.2;
					}
					else if (jiggiesRequiredInLogic < 55)
					{
						jiggiesRequiredInLogic *= 1.2;
					}
					else if (jiggiesRequiredInLogic < 65)
					{
						jiggiesRequiredInLogic *= 1.2;
					}
					else
					{
						jiggiesRequiredInLogic *= 1.2;
					}
				}
				if (jiggiesRequiredInLogic > 90)
					jiggiesRequiredInLogic = 90;
			}
			group->Requirements[0].RequiredItemsCount[0] = jiggiesRequiredInLogic;
			return;
		}
	}
	//I'm assuming that the only required Item is a note/glowbo
	//Prices are set really high in the logic so they are not traversed before the world is unlocked
	int siloIndex = 0;
	for (int i = 0; i < worlds.size(); i++)
	{
		if (worlds[i] == -1) //If we are looking past the point where the worlds are setup return
			return;
		int currentWorld = worlds[i];
		std::string currentTag = group->SpecialTag;
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

LogicHandler::AccessibleThings LogicHandler::TryRoute(LogicGroup startingGroup, std::unordered_map<int, LogicGroup>& logicGroups, std::set<int> lookedAtLogicGroups, std::set<int> nextLogicGroups, LogicHandler::AccessibleThings initialState, std::set<int> viableLogicGroups, const std::vector<RandomizedObject> objects, int depth, std::default_random_engine& rng)
{
	
		initialState.lastTraversed.push_back(startingGroup.GroupID);
		if (debug)
		{
			std::string groups = "{";
			for (int i = 0; i < initialState.lastTraversed.size(); i++)
			{
				if (i != 0)
					groups.append(",");
				groups.append(IntToHexString(initialState.lastTraversed[i]));

			}
			groups.append("}");
			DebugPrintPriority("Groups Traversed " + groups, 5);
		}
		groupsTraversed++;

		if (groupsTraversed > groupsToTraverseBeforeBacktrack && depth > 1)
		{
			LogicHandler::AccessibleThings revertState;
			revertState.depthToLeave = 30;
			groupsTraversed = 0;
			DebugPrint("Backtracking Reached Group Traversal Limit at depth: " + std::to_string(depth) + ", in Group: " + startingGroup.GroupName);
			return revertState;
		}
		//DebugPrint("Recursion Depth: " + std::to_string(depth) + ", Processing Group: " + startingGroup.GroupName);
		//Get all item locations currently accessible given the current owned items
		LogicHandler::AccessibleThings newState = GetAllTotals(startingGroup, logicGroups, initialState, objects, lookedAtLogicGroups, nextLogicGroups, viableLogicGroups);
		viableLogicGroups.erase(startingGroup.GroupID);
		std::set<int> tempViableLogicGroups;

		if (debug)
		{
			std::string ownedCollectables = "{";
			for (int i = 0; i < newState.ContainedItems.size(); i++)
			{
				if (i != 0)
					ownedCollectables.append(",");
				auto& collect = GetCollectibleFromCollectibleId(newState.ContainedItems[i].first);
				ownedCollectables.append("(" + collect.Name + "," + std::to_string(newState.ContainedItems[i].second) + ")");
			}
			ownedCollectables.append("}");
			DebugPrintPriority("Owned Collectables " + ownedCollectables, 5);

			std::string ownedMoves = "{";
			for (int i = 0; i < newState.SetAbilities.size(); i++)
			{
				if (newState.OwnedLocations.count(newState.SetAbilities[i].first) != 0)
				{
					ownedMoves.append(std::to_string(newState.SetAbilities[i].second) + ",");
				}
			}
			ownedMoves.append("}");
			DebugPrintPriority("Owned Moves " + ownedMoves, 5);
		}
		//Check if we have completed the game
		if (newState.Keys.count("Grunty Defeated") != 0) //End State
		{
			newState.done = true;
			return newState;
		}

		//If we run out of places that we can place items and we can't beat the game we have failed and need to backtrack
		if (newState.ItemLocations.size() == 0)
		{
			DebugPrintPriority("Ran out of item locations Backtracking from Group: " + startingGroup.GroupName + " at depth " + std::to_string(depth) + "\n", 5);
			LogicHandler::AccessibleThings state;
			return state;
		}

		if (debug)
		{
			std::string availableLocations = "{";
			for (int i = 0; i < newState.ItemLocations.size(); i++)
			{
				if (i != 0)
					availableLocations.append(",");
				availableLocations.append(IntToHexString(newState.ItemLocations[i]));

			}
			availableLocations.append("}");
			DebugPrintPriority("Available Locations " + availableLocations, 5);
		}

		std::vector<int> levels = newState.GetLevels();

		//Group all of the unused locations in each level not taking into account the accessibility of those items based on the level's id e.g. 0 = IOH
		std::unordered_map<int, int> unusedNormalGlobalLocations{};

		for (int i = 0; i < levels.size(); i++)
		{
			unusedNormalGlobalLocations[levels[i]] = newState.GetUnusedNormalGlobalLocationsFromLevel(levels[i]);
			DebugPrintPriority("LevelInt: "+std::to_string(levels[i])+" Unused Normal Global " + std::to_string(unusedNormalGlobalLocations[levels[i]]), 6);
		}

		//Look through all of the groups that were options last iteration and make sure that none of them have become non traversable
		for (int logicGroup : viableLogicGroups) //Iterate through the viable groups and check if they're still viable
		{
			LogicGroup group = LogicGroup::GetLogicGroupFromGroupId(logicGroup, logicGroups);
			DebugPrintPriority("Checking if group: " + group.GroupName + " is still viable", 2);

			HandleSpecialTags(&group, &newState);
			bool canFulfill = LogicHandler::CanFulfillRequirements(&newState, &group, unusedNormalGlobalLocations);
			if (lookedAtLogicGroups.find(group.GroupID) == lookedAtLogicGroups.end())
			{
				if (canFulfill)
					tempViableLogicGroups.insert(logicGroup);
				else
					DebugPrintPriority(group.GroupName + " is no longer viable", 2);
			}
		}
		viableLogicGroups = tempViableLogicGroups;
		std::set<int> tempLogicGroups = nextLogicGroups;
		//Iterate through all of the dependant groups for the currently unlocked groups and see if they can be fulfilled with the available locations
		for (int logicGroup : tempLogicGroups)
		{
			//Check if this group is already in the viable logic groups list
			if (viableLogicGroups.find(logicGroup) == viableLogicGroups.end())
			{
				LogicGroup group = LogicGroup::GetLogicGroupFromGroupId(logicGroup, logicGroups);
				//DebugPrintPriority("Checking if group: " + group.GroupName + " is viable", 5);

				HandleSpecialTags(&group, &newState);
				bool canFulfill = LogicHandler::CanFulfillRequirements(&newState, &group, unusedNormalGlobalLocations);

				if (canFulfill)
				{
					viableLogicGroups.insert(logicGroup);
					DebugPrintPriority(group.GroupName + " was found viable", 2);
				}
			}
		}

		tempLogicGroups.clear();
		//Get Move and associated Group
		std::unordered_map<int, std::vector<int>> MoveGroups;
		for (int logicGroup : viableLogicGroups)
		{
			LogicGroup viableGroup = LogicGroup::GetLogicGroupFromGroupId(logicGroup, logicGroups);
			bool foundGroup = false;
			for (int j = 0; j < viableGroup.Requirements.size(); j++)
			{
				if (!viableGroup.Requirements[j].Incidental)
				{
					if (viableGroup.Requirements[j].RequiredAbilities.size() > 0)
					{
						for (int abilityIndex = 0; abilityIndex < viableGroup.Requirements[j].RequiredAbilities.size(); abilityIndex++)
						{
							if (viableGroup.Requirements[j].RequiredItems.size() > 0)
							{
								MoveGroups[viableGroup.Requirements[j].RequiredAbilities[abilityIndex]].push_back(logicGroup);
								tempLogicGroups.insert(logicGroup);
								foundGroup = true;
								break;
							}
							else if (MoveGroups[viableGroup.Requirements[j].RequiredAbilities[abilityIndex]].size() == 0)
							{
								MoveGroups[viableGroup.Requirements[j].RequiredAbilities[abilityIndex]].push_back(logicGroup);
								tempLogicGroups.insert(logicGroup);
								foundGroup = true;
								break;
							}
						}
						if (foundGroup)
							break;
					}
					else
					{
						tempLogicGroups.insert(logicGroup);
					}
				}
			}
		}

		//tempLogicGroups = viableLogicGroups;
		std::vector<int> groupsToTraverse = { tempLogicGroups.begin(), tempLogicGroups.end() };
		std::shuffle(groupsToTraverse.begin(), groupsToTraverse.end(), rng);

		if (debug)
		{
			std::string validPaths;
			for (const int& validGroup : groupsToTraverse)
			{
				if (!validPaths.empty())
					validPaths.append(",");
				validPaths.append(IntToHexString(validGroup));
			}
			DebugPrintPriority("Valid Possible Groups: " + validPaths, 5);
		}

		//Iterate through all other viable logic groups (meaning groups that can actually have their requirements fulfilled by the number of objects and ability locations available for object placement)
		for (int i = 0; i < groupsToTraverse.size(); i++)
		{
			LogicGroup viableGroup = LogicGroup::GetLogicGroupFromGroupId(groupsToTraverse[i], logicGroups);
			HandleSpecialTags(&viableGroup, &newState);
			std::vector<LogicGroup::RequirementSet> requirements = viableGroup.Requirements;
			std::shuffle(requirements.begin(), requirements.end(), rng);

			std::vector<int> shuffleGroup = shuffleGroups[viableGroup.DependentShuffleGroup];
			std::shuffle(shuffleGroup.begin(), shuffleGroup.end(), rng);

			//DebugPrint("Checking requirements Recursion Depth: " + std::to_string(depth) + ", Processing Group: " + viableGroup.GroupName);

			//Try each requirement set
			for (int j = 0; j < requirements.size(); j++)
			{
				//Make sure we can fulfill this requirement still
				if (newState.CanFulfill(&requirements[j], unusedNormalGlobalLocations))
				{
					DebugPrint("\n");
					LogicHandler::AccessibleThings state(newState);

					state.AddItems(requirements[j], rng);

					if (debug)
					{
						int numItemsAdded = state.SetItems.size() - newState.SetItems.size();
						if (numItemsAdded > 0)
						{
							std::string ItemsAdded;
							for (int i = 0; i < numItemsAdded; i++)
							{
								if (i != 0)
								{
									ItemsAdded.append(",");
								}
								ItemsAdded.append("(" + IntToHexString(state.SetItems[i + newState.SetItems.size()].first) + "," + IntToHexString(state.SetItems[i + newState.SetItems.size()].second) + ")");
							}
							DebugPrintPriority("Items Added (Loc,Source): " + ItemsAdded, 6);
						}
					}

					state.UpdateMoves();
					state.UpdateCollectables();

					DebugPrintPriority("Recursing into Group: " + viableGroup.GroupName + " at depth " + std::to_string(depth + 1) + " Requirement Name " + requirements[j].SetName + " Requirement #" + std::to_string(j), 6);
					state.lastTraversed = initialState.lastTraversed;
					LogicHandler::AccessibleThings doneState = TryRoute(viableGroup, logicGroups, lookedAtLogicGroups, nextLogicGroups, state, tempLogicGroups, objects, depth + 1, rng);

					if (doneState.done || (doneState.depthToLeave > 0 && depth > 0))
					{
						if (doneState.depthToLeave == 1)
						{
							doneState.depthToLeave--;
							DebugPrintPriority("Finished Large Backtrack returned to " + startingGroup.GroupName + " at depth " + std::to_string(depth) + "\n", 5);
						}
						else
						{
							doneState.depthToLeave--;
							return doneState;
						}
					}
				}
			}

		}


		DebugPrintPriority("Exhausted potential paths Backtracking from Group: " + startingGroup.GroupName + " at depth " + std::to_string(depth) + "\n", 5);
		LogicHandler::AccessibleThings state;
		return state;
	
}

//Logic Group,Vector of possible Paths
std::map<int, std::set<std::set<int>>> LogicPaths;
LogicHandler::AccessibleThings LogicHandler::AssumedFill(LogicGroup startingGroup, std::vector<int>objectsToPlace, std::unordered_map<int, LogicGroup>& logicGroups, LogicHandler::AccessibleThings initialState, const std::vector<RandomizedObject> objects, std::default_random_engine& rng)
{
	//Objects that both cannot be rewards and are locked to a level
	std::vector<int> normalLevelRestricted;
	//Objects that can be rewards but are locked to a level
	std::vector<int> genericLevelRestrictedObjects;
	//Nonspawning Objects
	std::vector<int> normalObjects;
	//Objects that can be placed anywhere
	std::vector<int> genericObjects;

	//The Last ObjectId that was placed that was successful
	int lastPlacedObject = -1;
	int lastPlacedSize = -1;

	//The Next ObjectId to test
	int nextObjectToTest = -1;

	int itemsSinceLast = 0;
	int lastTestedSize = -1;

	std::vector<int> LevelRestricted;
	LogicHandler::AccessibleThings ownedState(initialState);


	LogicHandler::AccessibleThings checkPossible(initialState);
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i].Ability == -1)
			checkPossible.AddCollectable(objects[i].collectableId, objects[i].ItemAmount);
		else
			checkPossible.SetAbilities.push_back(std::make_pair(-1, objects[i].Ability));
	}
	checkPossible.keepCollectables = true;

	std::set<int>lookedAtLogicGroups_TEMP;
	std::set<int>nextLogicGroups_TEMP;
	std::set<int>viableLogicGroups_TEMP;

	LogicHandler::AccessibleThings possibleState = LogicHandler::GetAllTotals(startingGroup, logicGroups, checkPossible, objects, lookedAtLogicGroups_TEMP, nextLogicGroups_TEMP, viableLogicGroups_TEMP);
	DebugPrintPriority("Possible Locations " + std::to_string(possibleState.OwnedLocations.size()), 3);
	DebugPrintPriority("Objects to Place at start Locations " + std::to_string(objectsToPlace.size()), 3);

	for (int i = 0; i < objectsToPlace.size(); i++)
	{
		if (possibleState.OwnedLocations.count(objectsToPlace[i]) == 0)
		{
			DebugPrintPriority("Failed to find item in Logic " + objectsList[objectsToPlace[i]].LocationName, 3);
			ownedState.AddSetItem(objectsToPlace[i], objectsToPlace[i]);
			continue;
		}
		RandomizedObject& item = objectsList[objectsToPlace[i]];
		bool FoundNoRando = NoRandomizationIDs.count(item.ObjectID)==1;
		if (!item.Randomized || FoundNoRando == true) //If the object is not randomized Set it to equal itself and continue
		{
			ownedState.AddSetItem(item.RandoObjectID, item.RandoObjectID);
			continue;
		}
		bool foundLevelRestricted = LevelRestrictedIDs.count(item.ObjectID) == 1;
		if (item.ItemTag == "Note" || (foundLevelRestricted==true && !item.thisCanBeReward()))
		{
			normalLevelRestricted.push_back(item.RandoObjectID);
		}
		else if (foundLevelRestricted == true && item.thisCanBeReward())
		{
			genericLevelRestrictedObjects.push_back(item.RandoObjectID);
		}
		else if (!item.thisCanBeReward())
		{
			normalObjects.push_back(item.RandoObjectID);
		}
		else
		{
			genericObjects.push_back(item.RandoObjectID);
		}
	}
	std::shuffle(normalLevelRestricted.begin(), normalLevelRestricted.end(), rng);
	std::shuffle(genericLevelRestrictedObjects.begin(), genericLevelRestrictedObjects.end(), rng);
	std::shuffle(normalObjects.begin(), normalObjects.end(), rng);
	std::shuffle(genericObjects.begin(), genericObjects.end(), rng);
	objectsToPlace.clear();
	DebugPrintPriority("normalLevelRestricted size " + std::to_string(normalLevelRestricted.size()), 3);
	DebugPrintPriority("genericLevelRestrictedObjects size " + std::to_string(genericLevelRestrictedObjects.size()), 3);
	DebugPrintPriority("normalObjects size " + std::to_string(normalObjects.size()), 3);
	DebugPrintPriority("genericObjects size " + std::to_string(genericObjects.size()), 3);

	std::move(genericObjects.begin(), genericObjects.end(), std::back_inserter(objectsToPlace));
	std::move(normalObjects.begin(), normalObjects.end(), std::back_inserter(objectsToPlace));
	std::move(genericLevelRestrictedObjects.begin(), genericLevelRestrictedObjects.end(), std::back_inserter(objectsToPlace));
	std::move(normalLevelRestricted.begin(), normalLevelRestricted.end(), std::back_inserter(objectsToPlace));
	DebugPrintPriority("Objects already placed " + std::to_string(ownedState.SetItems.size()), 3);

	DebugPrintPriority("Objects to Place before placement " + std::to_string(objectsToPlace.size()), 3);

	lastPlacedObject = ownedState.SetItems.back().second;
	bool testThisObject = true;

	int lastSuccessful = -1;
	AccessibleThings lastSuccessfulState;
	while (!objectsToPlace.empty())
	{
		//debugLevel = 3;
		RandomizedObject& item = objectsList[objectsToPlace.back()];
		DebugPrintPriority("\nLooking to place " + objectsList[objectsToPlace.back()].LocationName + "(" + IntToHexString(objectsToPlace.back()) + ") Amount Left "+std::to_string(objectsToPlace.size()), 5);

		RandoStatusBox->SetWindowText(("Placing Id " + IntToHexString(item.RandoObjectID)+  "\n Remaining " + std::to_string(objectsToPlace.size())).c_str());

		objectsToPlace.pop_back();
		LogicHandler::AccessibleThings inverseState;
		inverseState.Add(initialState);
		inverseState.SetItems = ownedState.SetItems;
		inverseState.UsedItems = ownedState.UsedItems;
		ownedState.UpdateCollectables();
		ownedState.UpdateMoves();
		for (int i = 0; i < ownedState.SetItems.size(); i++)
		{
			if (objectsList[ownedState.SetItems[i].second].Ability == -1)
				inverseState.AddCollectable(objectsList[ownedState.SetItems[i].second].collectableId, objectsList[ownedState.SetItems[i].second].ItemAmount);
			else
				inverseState.SetAbilities.push_back(std::make_pair(-1, objectsList[ownedState.SetItems[i].second].Ability));
		}

		for (int i = 0; i < objects.size(); i++)
		{
			RandomizedObject& checkItem = objectsList[objects[i].RandoObjectID];
			if (checkItem.RandoObjectID == item.RandoObjectID)
				continue;
			if (checkItem.Ability == -1)
				inverseState.AddCollectable(checkItem.collectableId, checkItem.ItemAmount);
			else
				inverseState.SetAbilities.push_back(std::make_pair(-1, checkItem.Ability));
		}
		inverseState.keepCollectables = true;
		lookedAtLogicGroups_TEMP.clear();
		nextLogicGroups_TEMP.clear();
		viableLogicGroups_TEMP.clear();
		LogicHandler::AccessibleThings newState = LogicHandler::GetAllTotals(startingGroup, logicGroups, inverseState, objects, lookedAtLogicGroups_TEMP, nextLogicGroups_TEMP, viableLogicGroups_TEMP);

		std::vector<int>validLocations = AccessibleThings::GetValidLocationsForItemVector(item);
		std::vector<int>validAndReachable;
		for (int validIndex = 0; validIndex < validLocations.size(); validIndex++)
		{
			for (int availLocation = 0; availLocation < newState.ItemLocations.size(); availLocation++)
			{
				if (validLocations[validIndex] == newState.ItemLocations[availLocation])
				{
					validAndReachable.push_back(validLocations[validIndex]);
				}
			}
		}
		DebugPrintPriority("Found Valid Locations " + std::to_string(validLocations.size()), 3);
		DebugPrintPriority("Found ItemLocations "+ std::to_string(newState.ItemLocations.size()), 3);
		DebugPrintPriority("Found ValidAndReachable Locations " + std::to_string(validAndReachable.size()), 3);

		if (validAndReachable.size() == 0)
		{
			DebugPrintPriority("\nFailed Placement Due to lack of valid locations Remaining Items:", 0);

			DebugPrintPriority(objectsList[item.RandoObjectID].LocationName, 0);
			for (int object:objectsToPlace)
			{
				DebugPrintPriority(objectsList[object].LocationName, 0);
			}
			DebugPrintPriority("\nSet Items", 0);

			for (auto object : ownedState.SetItems)
			{
				DebugPrintPriority(objectsList[object.first].LocationName+" contains "+ objectsList[object.second].ItemTag + " from "+ objectsList[object.second].LocationName, 0);
			}
			DebugPrintPriority("\nOwned Locations", 0);

			for (auto object : ownedState.OwnedLocations)
			{
				DebugPrintPriority(objectsList[object].LocationName, 0);
			}

			DebugPrintPriority("\nOwned Abilities", 0);

			for (auto object : ownedState.SetItems)
			{
				if(objectsList[object.second].Ability!=-1)	
					DebugPrintPriority(objectsList[object.second].MoveName, 0);
			}

			::MessageBox(NULL, "Failed to Find Valid and Reachable", "Error", NULL);
			return LogicHandler::AccessibleThings();
		}
		std::shuffle(validAndReachable.begin(), validAndReachable.end(), rng);

		bool successful = false;
		bool override = false;

		if (debug)
		{
			std::string availableLocations = "{";
			for (int i = 0; i < validAndReachable.size(); i++)
			{
				if (i != 0)
					availableLocations.append(",");
				availableLocations.append(IntToHexString(validAndReachable[i]));

			}
			availableLocations.append("}");
			DebugPrintPriority("Valid and Reachable Available Locations " + availableLocations, 5);
		}

		for (int i = 0; i < validAndReachable.size(); i++)
		{
			bool continueMark = false;
			inverseState = LogicHandler::AccessibleThings();
			inverseState.Add(initialState);
			int locationId = validAndReachable[i];

			if (override||(lastSuccessful != -1&&(item.Ability != -1 && i == 25 || item.Ability == -1 && i == 8)))
			{
				locationId = lastSuccessful;
			}
			auto tempState = ownedState;
			tempState.AddSetItem(locationId, item.RandoObjectID);
			DebugPrintPriority("Attempt Place "+(objectsList[item.RandoObjectID].Ability == -1 ? (objectsList[item.RandoObjectID].LocationName + " (" + objectsList[item.RandoObjectID].ItemTag + ")") : objectsList[item.RandoObjectID].MoveName) + " at " + objectsList[locationId].LocationName, 5);

			tempState.UpdateCollectables();
			tempState.UpdateMoves();

			//Iterate through the rest of the items and make sure they can still be placed
			if (objectsToPlace.size() > 0)
			{
				/*
				if (objectsToPlace.size() < 11)
				{
					debugLevel = 0;
					DebugPrintPriority("\nSet Items Before Route for objects left "+ std::to_string(objectsToPlace.size()), 0);

					for (auto object : tempState.SetItems)
					{
						DebugPrintPriority(objectsList[object.first].LocationName + " contains " + objectsList[object.second].ItemTag + " from " + objectsList[object.second].LocationName, 0);
					}
				}*/
				groupsTraversed = 0;
				auto doneState = TryRoute(startingGroup, logicGroups, {}, {}, tempState, {}, objects, 0, rng);

				if (objectsToPlace.size() < 11)
				{
					//debugLevel = 3;
				}
				if (doneState.done == false)
				{
					continue;
				}
				DebugPrintPriority("Next Item will succeed if placed like", 5);
				bool found = false;
				for (const auto& setpair : doneState.SetItems)
				{
					//Get the next item we are placing
					if (setpair.second == objectsToPlace.back())
					{
						found = true;
						DebugPrintPriority(objectsList[setpair.first].LocationName +"(" + IntToHexString(setpair.first) + ") contains " + objectsList[setpair.second].ItemTag + " from " + objectsList[setpair.second].LocationName+"(" + IntToHexString(setpair.second) + ")", 5);
						lastSuccessful = setpair.first;
						break;
					}
				}
				if (!found)
				{
					//TODO: Find a location not in the done state
					RandomizedObject& item = objectsList[objectsToPlace.back()];
					std::vector<int>validLocations = AccessibleThings::GetValidLocationsForItemVector(item);
					std::set<int> valids(validLocations.begin(), validLocations.end());
					for (const auto& setpair : doneState.SetItems)
					{
						valids.erase(setpair.first);
					}
					if (valids.size() > 0)
					{
						lastSuccessful = *valids.begin();
						DebugPrintPriority("1 "+objectsList[lastSuccessful].LocationName + "(" + IntToHexString(lastSuccessful) + ") contains " + objectsList[objectsToPlace.back()].ItemTag + " from " + objectsList[objectsToPlace.back()].LocationName + "(" + IntToHexString(objectsToPlace.back()) + ")", 3);
					}
					else
					{
						lastSuccessful = -1;
						DebugPrintPriority("2 "+objectsList[objectsToPlace.back()].LocationName + "(" + IntToHexString(objectsToPlace.back()) + ") was not found in done state", 3);
					}
				}

				DebugPrintPriority("Successfully Placed " + IntToHexString(item.RandoObjectID) + " " + std::to_string(objectsToPlace.size()), 3);

			}
			successful = true;
			//DebugPrintPriority("Objects Left to place "+std::to_string(objectsToPlace.size()), 3);
			//DebugPrintPriority("Set Items " + std::to_string(ownedState.SetItems.size()), 3);
			DebugPrintPriority((objectsList[item.RandoObjectID].Ability==-1? (objectsList[item.RandoObjectID].LocationName+" ("+ objectsList[item.RandoObjectID].ItemTag+")") : objectsList[item.RandoObjectID].MoveName) + " Placed at " + objectsList[locationId].LocationName, 5);
			
			ownedState = tempState;
			break;
		}
		if (!successful)
		{

			debugLevel = 3;
			DebugPrintPriority("Could not place "+(objectsList[item.RandoObjectID].Ability == -1 ? (objectsList[item.RandoObjectID].LocationName + " (" + objectsList[item.RandoObjectID].ItemTag + ")") : objectsList[item.RandoObjectID].MoveName),3);
			
			DebugPrintPriority(objectsList[item.RandoObjectID].LocationName, 3);
			DebugPrintPriority("\nObjects Left to place", 3);
			for (int object : objectsToPlace)
			{
				DebugPrintPriority(objectsList[object].LocationName, 3);
			}
			DebugPrintPriority("\nSet Items", 3);

			for (auto object : ownedState.SetItems)
			{
				DebugPrintPriority(objectsList[object.first].LocationName + " contains " + objectsList[object.second].ItemTag + " from " + objectsList[object.second].LocationName,3);
			}

			DebugPrintPriority("\nOwned Locations",3);

			for (auto object : ownedState.OwnedLocations)
			{
				DebugPrintPriority(objectsList[object].LocationName,3);
			}

			DebugPrintPriority("\nOwned Abilities", 3);

			for (auto object : ownedState.SetItems)
			{
				if (objectsList[object.second].Ability != -1)
					DebugPrintPriority(objectsList[object.second].MoveName, 3);
			}
			
			debugLevel = 5;
			DebugPrintPriority("\nPrevious State", 0);

			auto doneState = TryRoute(startingGroup, logicGroups, {}, {}, ownedState, {}, objects, 0, rng);
			if (doneState.done == true)
			{
				DebugPrintPriority("Completed", 0);
			}
			else
			{
				DebugPrintPriority("Failed", 0);

			}
		
		}
	}
	auto doneState = TryRoute(startingGroup, logicGroups, {}, {}, ownedState, {}, objects, 0, rng);
	ownedState.done = true;
	DebugPrintPriority("Complete", 1);
	return ownedState;
}

std::unordered_set<int> visited;

//Try and find every group combination that directly leads into this one
void LogicHandler::FindLeadingGroups(LogicGroup startingGroup, std::unordered_map<int, LogicGroup>& logicGroups, LogicHandler::AccessibleThings& initialState, const std::vector<RandomizedObject> objects, std::default_random_engine& rng)
{
	bool alreadyTraversed = false;
	HandleSpecialTags(&startingGroup, &initialState);
	
	if (visited.count(startingGroup.GroupID) != 0)
		alreadyTraversed = true;
	//Check if we've already handled this group's dependents so we do not try and traverse them twice
	if (alreadyTraversed == false)
	{
		visited.insert(startingGroup.GroupID);
	}
	
	//Iterate through all of the dependent groups
	for (int dependentGroup: startingGroup.dependentGroupIDs)
	{
		//Find all of the groups associated with keys needed to fulfill a dependent
		std::set<std::set<int>> keys;
		for (auto& requirement : logicGroups[dependentGroup].Requirements)
		{
			//Gather all of the groups that provide the keys for this requirement set
			std::set<int> keyGroups;
			if (requirement.RequiredKeys.size() > 0)
			{
				for (auto& Group : logicGroups)
				{
					for (auto& key : requirement.RequiredKeys)
					{
						if (Group.second.key == key && dependentGroup != Group.first)
						{
							keyGroups.insert(Group.first);
							break;
						}
					}
				}
			}
			keys.insert(keyGroups);
		}
		if (logicGroups[dependentGroup].Requirements.size() == 0)
		{
			LogicPaths[dependentGroup].insert({ startingGroup.GroupID});
		}
		//For every keygroup you need a different set of required groups
		for (auto& keyGroup:keys)
		{
			std::set<int> insertGroup = keyGroup;
			insertGroup.insert(startingGroup.GroupID);
			LogicPaths[dependentGroup].insert(insertGroup);
		}
		if (!alreadyTraversed)
		{
			FindLeadingGroups(logicGroups[dependentGroup], logicGroups, initialState, objects, rng);
		}
	}
}

std::set<std::set<int>> LogicHandler::FindLogicChain(int startingGroup, std::unordered_map<int, LogicGroup>& logicGroups, std::map<int, std::list<ChainState>>& RemainingChains, std::map<int, std::set<std::set<int>>>& FinalChains,std::set<int> visited)
{
	auto& remaining = RemainingChains[startingGroup];
	std::vector<std::vector<ChainState>::iterator> LogicGroupChains;
	auto it = remaining.begin();
	while(it !=remaining.end())
	{
		bool foundVisited = false;
		for (auto group : it->exploredGroups)
		{
			if (visited.count(group) > 0)
			{
				foundVisited = true;
				break;
			}
		}
		for (auto group : it->unexploredGroups)
		{
			if (visited.count(group) > 0)
			{
				foundVisited = true;
				break;
			}
		}
		if (foundVisited)
		{
			++it;
			//Get all of the chains that do not rely on groups we have already visited
			continue;
		}
	
		DebugPrintPriority("Number of Remaining Logic Chains for " + logicGroups[startingGroup].GroupName + " " + std::to_string(LogicGroupChains.size()), 1);
	
		//Check if there are any unexplored groups
		while (!it->unexploredGroups.empty())
		{
			//auto groupChain = LogicGroupChains.front();
			int groupToExplore = *it->unexploredGroups.begin();
			DebugPrintPriority("Explored size " + std::to_string(it->exploredGroups.size()), 1);
			DebugPrintPriority("Unexplored size " + std::to_string(it->unexploredGroups.size()), 1);
			DebugPrintPriority("Exploring Group " + logicGroups[groupToExplore].GroupName, 1);

			it->unexploredGroups.erase(groupToExplore);
			it->exploredGroups.insert(groupToExplore);
			//LogicGroupChains.front() = groupChain;
			//Insert the group from this layer of recursion so we do not try and visit paths that would cause cycling
			visited.insert(startingGroup);
			auto returnChain = FindLogicChain(groupToExplore, logicGroups, RemainingChains, FinalChains, visited);
			if (returnChain.empty())
			{
				returnChain.insert({ {groupToExplore} });
			}
				
			auto OriginalChain = *it;

			for (auto& chain : returnChain)
			{
				if (chain.count(0x48) == 0)
				{
					DebugPrintPriority("Failed to find Valid Chain", 1);
				}
				std::string chainString = "Chain Found: {";
				bool firstGroup = true;
				for (auto& group : chain)
				{
					if (!firstGroup)
					{
						chainString = chainString + ",";
					}
					else
					{
						firstGroup = false;
					}
					chainString = chainString + IntToHexString(group);
				}
				chainString += "}";
				DebugPrintPriority(chainString, 1);
			}
			bool first = true;
			for (auto& chain : returnChain)
			{
				if (first)
				{
					first = false;
					it->exploredGroups.insert(chain.begin(), chain.end());
				}
				else
				{
					auto copyChain = OriginalChain;
					copyChain.exploredGroups.insert(chain.begin(), chain.end());
					remaining.push_back(copyChain);
				}
			}
			
		}
		FinalChains[startingGroup].insert(it->exploredGroups);
		//I still need to find a way to erase the correct group from the remaining chains
		it = remaining.erase(it);
	}
	DebugPrintPriority("Number of Final Logic Chains for " + logicGroups[startingGroup].GroupName + " " + std::to_string(FinalChains[startingGroup].size()), 1);

	return FinalChains[startingGroup];
}

//Unused
LogicHandler::AccessibleThings LogicHandler::RandomFill(LogicGroup startingGroup, std::vector<int>objectsToPlace, std::unordered_map<int, LogicGroup>& logicGroups, LogicHandler::AccessibleThings initialState, const std::vector<RandomizedObject> objects, std::default_random_engine& rng)
{

	//Objects that both cannot be rewards and are locked to a level
	std::vector<int> normalLevelRestricted;
	//Objects that can be rewards but are locked to a level
	std::vector<int> genericLevelRestrictedObjects;
	//Nonspawning Objects
	std::vector<int> normalObjects;
	//Objects that can be placed anywhere
	std::vector<int> genericObjects;

	std::vector<int> LevelRestricted;
	LogicHandler::AccessibleThings ownedState;

	LogicHandler::AccessibleThings checkPossible(initialState);

	checkPossible.keepCollectables = true;

	std::set<int>lookedAtLogicGroups_TEMP;
	std::set<int>nextLogicGroups_TEMP;
	std::set<int>viableLogicGroups_TEMP;

	for (int i = 0; i < objectsToPlace.size(); i++)
	{
		RandomizedObject& item = objectsList[objectsToPlace[i]];
		auto foundNoRando = std::find(NoRandomizationIDs.begin(), NoRandomizationIDs.end(), item.ObjectID);
		if (!item.Randomized || foundNoRando != NoRandomizationIDs.end()) //If the object is not randomized Set it to equal itself and continue
		{
			ownedState.AddSetItem(item.RandoObjectID, item.RandoObjectID);
			continue;
		}
		auto foundLevelRestricted = std::find(LevelRestrictedIDs.begin(), LevelRestrictedIDs.end(), item.ObjectID);
		if (item.ItemTag == "Note" || (foundLevelRestricted != LevelRestrictedIDs.end() && !item.thisCanBeReward()))
		{
			normalLevelRestricted.push_back(item.RandoObjectID);
		}
		else if (foundLevelRestricted != LevelRestrictedIDs.end() && item.thisCanBeReward())
		{
			genericLevelRestrictedObjects.push_back(item.RandoObjectID);
		}
		else if (!item.thisCanBeReward())
		{
			normalObjects.push_back(item.RandoObjectID);
		}
		else
		{
			genericObjects.push_back(item.RandoObjectID);
		}
	}
	std::shuffle(normalLevelRestricted.begin(), normalLevelRestricted.end(), rng);
	std::shuffle(genericLevelRestrictedObjects.begin(), genericLevelRestrictedObjects.end(), rng);
	std::shuffle(normalObjects.begin(), normalObjects.end(), rng);
	std::shuffle(genericObjects.begin(), genericObjects.end(), rng);
	objectsToPlace.clear();
	std::move(genericObjects.begin(), genericObjects.end(), std::back_inserter(objectsToPlace));
	std::move(normalObjects.begin(), normalObjects.end(), std::back_inserter(objectsToPlace));
	std::move(genericLevelRestrictedObjects.begin(), genericLevelRestrictedObjects.end(), std::back_inserter(objectsToPlace));
	std::move(normalLevelRestricted.begin(), normalLevelRestricted.end(), std::back_inserter(objectsToPlace));

	while (objectsToPlace.size() > 0)
	{
		RandomizedObject& item = objectsList[objectsToPlace.back()];
		objectsToPlace.pop_back();
		initialState.GetValidLocationsForItem(item);
	}
	ownedState.done = true;
	return ownedState;
}

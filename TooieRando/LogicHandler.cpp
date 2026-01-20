#include "stdafx.h"
#include "LogicHandler.h"
#include <iostream>

std::vector<OptionData>* LogicHandler::options;
//Index the RandomizedObject by their RandoObjectId
std::unordered_map<int,RandomizedObject> LogicHandler::objectsList;
std::unordered_map<int, Entrance> LogicHandler::EntranceList;
bool LogicHandler::alreadySetup = false;

//Set this value to true to activate the debug prints in the logic handler
bool LogicHandler::debug = false; 
bool LogicHandler::saveLogging = false;

//List of all objects sorted into groups by level
std::unordered_map<int, std::vector<int>> LogicHandler::normalLevelObjectsMapAll; 

//List of all entrances sorted by shuffle group
std::unordered_map<int, std::vector<int>> LogicHandler::shuffleGroups; 

//<EntrancedID,LogicGroupID>
std::unordered_map<int, int> LogicHandler::entranceAssociations; 

//Ability,RandoObjectID
std::unordered_map<int, int> LogicHandler::AbilityItems;

//Whether the objects not randomized options is set
bool LogicHandler::objectsNotRandomized; 

const int groupsToTraverseBeforeBacktrack = 900;
std::vector<std::string>  LogicHandler::WorldTags{"World1","World2","World3","World4","World5","World6","World7","World8","World9","Hag1" };
std::vector<int>  LogicHandler::notePrices{ 25,30,35,45,85,95,110,160,170,180,200,265,275,290,315,390,405,420,525,545,590,640,660,765 };
std::vector<int>  LogicHandler::glowboPrices{ 0x2,0x4,0x6,0x7,0x9,0xB,0xD,0xF,0x11 };

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
std::vector<int> LogicHandler::LevelRestrictedIDs;
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
LogicHandler::AccessibleThings LogicHandler::GetAllTotals(LogicGroup startingGroup,std::unordered_map<int,LogicGroup>& logicGroups, LogicHandler::AccessibleThings start, const std::vector<RandomizedObject>& objects, std::vector<int>& seenLogicGroups, std::vector<int>& nextLogicGroups, std::vector<int>& viableLogicGroups)
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
LogicHandler::AccessibleThings LogicHandler::GetAccessibleRecursive(LogicGroup& startingGroup, std::unordered_map<int, LogicGroup>& logicGroups, LogicHandler::AccessibleThings& start, const std::vector<RandomizedObject>& objects, std::vector<int>& seenLogicGroups, std::vector<int>& nextLogicGroups, std::vector<int>& viableLogicGroups)
{

	//OutputDebugString(("Entering " + startingGroup.GroupName + "\n").c_str());
	LogicHandler::AccessibleThings accessible;
	accessible.Add(start);
	DebugPrint("Attempt Traverse: " + startingGroup.GroupName);

	auto it = std::find(seenLogicGroups.begin(), seenLogicGroups.end(), startingGroup.GroupID);
	if (it == seenLogicGroups.end()) //Check if we've already been here
	{
		DebugPrint("Add new group Accessible Recursive: " + startingGroup.GroupName);

		accessible.Add(startingGroup, objects);
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
				DebugPrint("Check if dependant group is already fulfilled: " + group.GroupName);
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
					accessible.Add(GetAccessibleRecursive(group, logicGroups, accessible, objects, seenLogicGroups, nextLogicGroups, viableLogicGroups));
				}
				else
				{
					auto nextLogicGroupIt = std::find(nextLogicGroups.begin(), nextLogicGroups.end(), startingGroup.dependentGroupIDs[i]);
					if (nextLogicGroupIt == nextLogicGroups.end()) //Check that we haven't already placed this in the next logic groups list
					{
						DebugPrint("Could not fulfill keep for next logic group: " + group.GroupName);
						nextLogicGroups.push_back(startingGroup.dependentGroupIDs[i]);
					}
				}
			}
		}
		
	}
	else
	{
		return accessible;
	}
	DebugPrint("Check old Groups " + startingGroup.GroupName);
	//Whenever a new key/unrandomized move is added recheck the old groups otherwise we can just do this once
	if (accessible.checkedOldGroups)
	{
		return accessible;
	}
	std::vector<int> tempLogicGroups = nextLogicGroups;
	for (int i = 0; i < tempLogicGroups.size(); i++) //Check if old next groups can be visited
	{
		LogicGroup group = LogicGroup::GetLogicGroupFromGroupId(tempLogicGroups[i], logicGroups);
		DebugPrint("Check if old group is already fulfilled: " + group.GroupName);

		HandleSpecialTags(&group, &accessible);
		if (LogicHandler::FulfillsRequirements(&group, &accessible))
		{
			auto it = std::find(nextLogicGroups.begin(), nextLogicGroups.end(), group.GroupID);
			if (it != nextLogicGroups.end())
			{
				auto viableIterator = std::find(viableLogicGroups.begin(), viableLogicGroups.end(), group.GroupID);
				if (viableIterator != viableLogicGroups.end())
					viableLogicGroups.erase(viableIterator);
				nextLogicGroups.erase(it);
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
		if (WorldPrefixes[currentWorld] + "Glowbo" == currentTag && glowboPrices.size() > glowboIndex)
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
		if(currentWorld != 0x9)
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

LogicHandler::AccessibleThings LogicHandler::TryRoute(LogicGroup startingGroup,std::unordered_map<int,LogicGroup>& logicGroups, std::vector<int> lookedAtLogicGroups, std::vector<int> nextLogicGroups , LogicHandler::AccessibleThings initialState, std::vector<int> viableLogicGroups, const std::vector<RandomizedObject> objects, int depth, std::default_random_engine& rng)
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

	LogicHandler::AccessibleThings newState = GetAllTotals(startingGroup, logicGroups, initialState, objects, lookedAtLogicGroups, nextLogicGroups,viableLogicGroups);
	auto it = std::find(viableLogicGroups.begin(), viableLogicGroups.end(), startingGroup.GroupID);
	if(it != viableLogicGroups.end())
		viableLogicGroups.erase(it);
	std::vector<int> tempViableLogicGroups;
	
	
	std::vector<int> levels = newState.GetLevels();
	
	std::unordered_map<int, int> unusedNormalGlobalLocations{};

	for (int i = 0; i < levels.size(); i++)
	{
		unusedNormalGlobalLocations[levels[i]] = newState.GetUnusedNormalGlobalLocationsFromLevel(levels[i]);
	}
	


	for (int i = 0; i < viableLogicGroups.size(); i++) //Iterate through the viable groups and check if they're still viable
	{
		LogicGroup group = LogicGroup::GetLogicGroupFromGroupId(viableLogicGroups[i], logicGroups);
		DebugPrint("Checking if group: " + group.GroupName + " is still viable");

		HandleSpecialTags(&group, &newState);
		bool canFulfill = LogicHandler::CanFulfillRequirements(&newState, &LogicGroup::GetLogicGroupFromGroupId(viableLogicGroups[i], logicGroups), unusedNormalGlobalLocations);
		auto it = std::find(lookedAtLogicGroups.begin(), lookedAtLogicGroups.end(), group.GroupID);
		if (it == lookedAtLogicGroups.end())
		{
			if (canFulfill)
				tempViableLogicGroups.push_back(viableLogicGroups[i]);
			else
				DebugPrint(group.GroupName + " is no longer viable");
		}
	}
	viableLogicGroups = tempViableLogicGroups;

	std::vector<int> tempLogicGroups = nextLogicGroups;
	std::shuffle(tempLogicGroups.begin(), tempLogicGroups.end(), rng);
	
	//Iterate through all of the dependant groups for the currently unlocked groups and see if they can be fulfilled with the available locations
	for (int i = 0; i < tempLogicGroups.size(); i++) 
	{
		//Check if this group is already in the viable logic groups list
		auto it = std::find(viableLogicGroups.begin(), viableLogicGroups.end(), tempLogicGroups[i]);
		if (it == viableLogicGroups.end())
		{
			LogicGroup group = LogicGroup::GetLogicGroupFromGroupId(tempLogicGroups[i], logicGroups);
			DebugPrint("Checking if group: " + group.GroupName+ " is viable");

			HandleSpecialTags(&group, &newState);
			bool canFulfill = LogicHandler::CanFulfillRequirements(&newState, &group, unusedNormalGlobalLocations);

			if (canFulfill)
			{
				viableLogicGroups.push_back(tempLogicGroups[i]);
				DebugPrint( group.GroupName + " was found viable");
			}
		}
	}

	auto keyIterator = std::find(newState.Keys.begin(), newState.Keys.end(), "Grunty Defeated");
	if (keyIterator != newState.Keys.end()) //End State
	{
		//::MessageBox(NULL, "Valid Path Found", "Completed", NULL);
		newState.done = true;
		return newState;
	}
	tempLogicGroups.clear();
	//Get Move and associated Group
	std::unordered_map<int, std::vector<int>> MoveGroups;
	for (int i = 0; i < viableLogicGroups.size(); i++)
	{
		LogicGroup viableGroup = LogicGroup::GetLogicGroupFromGroupId(viableLogicGroups[i], logicGroups);
		bool foundGroup = false;
		for(int j = 0;j<viableGroup.Requirements.size();j++)
		{
			if (!viableGroup.Requirements[j].Incidental)
			{
				if (viableGroup.Requirements[j].RequiredAbilities.size() > 0)
				{
					for (int abilityIndex = 0; abilityIndex < viableGroup.Requirements[j].RequiredAbilities.size(); abilityIndex++)
					{
						if (viableGroup.Requirements[j].RequiredItems.size() > 0)
						{
							MoveGroups[viableGroup.Requirements[j].RequiredAbilities[abilityIndex]].push_back(viableLogicGroups[i]);
							tempLogicGroups.push_back(viableLogicGroups[i]);
							foundGroup = true;
							break;
						}
						else if (MoveGroups[viableGroup.Requirements[j].RequiredAbilities[abilityIndex]].size() == 0)
						{
							MoveGroups[viableGroup.Requirements[j].RequiredAbilities[abilityIndex]].push_back(viableLogicGroups[i]);
							tempLogicGroups.push_back(viableLogicGroups[i]);
							foundGroup = true;
							break;
						}
					}
					if (foundGroup)
						break;
				}
				else
				{
					tempLogicGroups.push_back(viableLogicGroups[i]);
				}
			}
		}
	}

	//tempLogicGroups = viableLogicGroups;

	std::shuffle(tempLogicGroups.begin(), tempLogicGroups.end(), rng);

	//Iterate through all other viable logic groups (meaning groups that can actually have their requirements fulfilled by the number of objects and ability locations available for object placement)
	for (int i = 0; i < tempLogicGroups.size(); i++) 
	{
		LogicGroup viableGroup = LogicGroup::GetLogicGroupFromGroupId(tempLogicGroups[i], logicGroups);
		HandleSpecialTags(&viableGroup, &newState);
		std::vector<LogicGroup::RequirementSet> requirements = viableGroup.Requirements;
		std::shuffle(requirements.begin(), requirements.end(), rng);

		std::vector<int> shuffleGroup = shuffleGroups[viableGroup.DependentShuffleGroup];
		std::shuffle(shuffleGroup.begin(), shuffleGroup.end(), rng);

		DebugPrint("Checking requirements Recursion Depth: " + std::to_string(depth) + ", Processing Group: " + viableGroup.GroupName);

		//Try each requirement set
		for (int j = 0; j < requirements.size(); j++)
		{
		//Make sure we can fulfill this requirement still
			if (newState.CanFulfill(&requirements[j], unusedNormalGlobalLocations))
			{
				//Check if the group has a warp connection
				if (viableGroup.DependentShuffleGroup != -1)
				{
					int numSkipped = 0;
					for (int i = 0; i < shuffleGroup.size(); i++)
					{
						LogicHandler::AccessibleThings state;

						int exitID = shuffleGroup[i];
						int thisEntrance = viableGroup.AssociatedWarp;
						auto matchesEntrance = [thisEntrance](std::pair<int, int> entrancePair) {return (std::get<0>(entrancePair)) == thisEntrance || std::get<1>(entrancePair) == thisEntrance; };
						auto matchesExit = [exitID](std::pair<int, int> entrancePair) {return (std::get<0>(entrancePair)) == exitID || std::get<1>(entrancePair) == exitID; };
						
						auto foundEntrance = std::find_if(newState.SetWarps.begin(), newState.SetWarps.end(), matchesEntrance);
						auto foundExit = std::find_if(newState.SetWarps.begin(), newState.SetWarps.end(), matchesExit);
						//Stops trying different worlds
						bool stopSearching = false;

						state.Add(newState);
						//Check if this entrance has been set before
						if (foundEntrance == newState.SetWarps.end() && foundExit == newState.SetWarps.end())
						{
							//If neither the current warp nor the targeted entrance are taken use them
							exitID = shuffleGroup[i];
							state.SetWarps.push_back(std::make_pair(viableGroup.AssociatedWarp, exitID));
						}
						else if(foundEntrance != newState.SetWarps.end())
						{
							//This Entrance is already setup
							exitID = (*foundEntrance).second;
							stopSearching = true;
						}
						else
						{
							//The exit we tried to use is already taken
							continue;
						}
							//Get the Group Associated with this entrance
							DebugPrint("Set Warp " + std::to_string(viableGroup.AssociatedWarp) + " to " + std::to_string(exitID) + " at depth " + std::to_string(depth + 1));
							LogicGroup tempGroup = viableGroup;
							//Dependent GroupIDs are already setup in handle special tags in certain cases and warps should not have multiple dependent groups
							if(tempGroup.dependentGroupIDs.size()==0)
								tempGroup.dependentGroupIDs.push_back(entranceAssociations[exitID]);
								
							state.AddItems(requirements[j], rng);
							state.UpdateMoves();
							state.UpdateCollectables();
							DebugPrint("Recursing into Group: " + viableGroup.GroupName + " at depth " + std::to_string(depth + 1));

							LogicHandler::AccessibleThings doneState = TryRoute(tempGroup, logicGroups, lookedAtLogicGroups, nextLogicGroups, state, tempLogicGroups, objects, depth + 1,rng);
							if (doneState.done || (doneState.depthToLeave > 0 && depth > 1))
							{
								doneState.depthToLeave--;
								return doneState;
							}
							if (stopSearching)
								break;
					}
				}
				else
				{
					LogicHandler::AccessibleThings state;
					state.Add(newState);
					state.AddItems(requirements[j],rng);
					state.UpdateMoves();
					state.UpdateCollectables();
					DebugPrint("Recursing into Group: " + viableGroup.GroupName + " at depth " + std::to_string(depth + 1));

					LogicHandler::AccessibleThings doneState = TryRoute(viableGroup, logicGroups, lookedAtLogicGroups, nextLogicGroups, state, tempLogicGroups, objects, depth + 1,rng);
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

LogicHandler::AccessibleThings LogicHandler::AssumedFill(LogicGroup startingGroup, std::unordered_map<int, LogicGroup>& logicGroups, std::vector<int> lookedAtLogicGroups, std::vector<int> nextLogicGroups, LogicHandler::AccessibleThings initialState, std::vector<int> viableLogicGroups, const std::vector<RandomizedObject> objects, int depth, std::default_random_engine& rng)
{
	
	std::vector<int> objectsToPlace;
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

	LogicHandler::AccessibleThings checkPossible;
	checkPossible.Add(initialState);
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i].Ability == -1)
			checkPossible.AddCollectable(objects[i].ItemTag, objects[i].ItemAmount);
		else
			checkPossible.SetAbilities.push_back(std::make_pair(-1, objects[i].Ability));
	}
	checkPossible.keepCollectables = true;

	std::vector<int>lookedAtLogicGroups_TEMP;
	std::vector<int>nextLogicGroups_TEMP;
	std::vector<int>viableLogicGroups_TEMP;

	LogicHandler::AccessibleThings possibleState = LogicHandler::GetAllTotals(startingGroup, logicGroups, checkPossible, objects, lookedAtLogicGroups_TEMP, nextLogicGroups_TEMP, viableLogicGroups_TEMP);

	for (int i = 0; i < possibleState.ItemLocations.size(); i++)
	{
		RandomizedObject& item = objectsList[possibleState.ItemLocations[i]];
		auto foundNoRando = std::find(NoRandomizationIDs.begin(), NoRandomizationIDs.end(), item.ObjectID);
		if (!item.Randomized || foundNoRando != NoRandomizationIDs.end()) //If the object is not randomized Set it to equal itself and continue
		{
			ownedState.AddSetItem(item.RandoObjectID, item.RandoObjectID);
			continue;
		}
		auto foundLevelRestricted = std::find(LevelRestrictedIDs.begin(), LevelRestrictedIDs.end(), item.ObjectID);
		if (item.ItemTag=="Note"||(foundLevelRestricted != LevelRestrictedIDs.end() && !item.isReward()))
		{
			normalLevelRestricted.push_back(item.RandoObjectID);
		}
		else if (foundLevelRestricted != LevelRestrictedIDs.end() && item.isReward())
		{
			genericLevelRestrictedObjects.push_back(item.RandoObjectID);
		}
		else if (!item.isReward())
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
		
	std::move(genericObjects.begin(), genericObjects.end(), std::back_inserter(objectsToPlace));
	std::move(normalObjects.begin(), normalObjects.end(), std::back_inserter(objectsToPlace));
	std::move(genericLevelRestrictedObjects.begin(), genericLevelRestrictedObjects.end(), std::back_inserter(objectsToPlace));
	std::move(normalLevelRestricted.begin(), normalLevelRestricted.end(), std::back_inserter(objectsToPlace));
	
	while (objectsToPlace.size() > 0)
	{
		lookedAtLogicGroups_TEMP.clear();
		nextLogicGroups_TEMP.clear();
		viableLogicGroups_TEMP.clear();

		RandomizedObject& item = objectsList[objectsToPlace.back()];
		objectsToPlace.pop_back();
		LogicHandler::AccessibleThings inverseState;
		inverseState.Add(initialState);
		inverseState.SetItems = ownedState.SetItems;
		for (int i = 0; i < ownedState.SetItems.size(); i++)
		{
			if (objectsList[ownedState.SetItems[i].second].Ability == -1)
				inverseState.AddCollectable(objectsList[ownedState.SetItems[i].second].ItemTag, objectsList[ownedState.SetItems[i].second].ItemAmount);
			else
				inverseState.SetAbilities.push_back(std::make_pair(-1, objectsList[ownedState.SetItems[i].second].Ability));
		}

		for (int i = 0; i<objectsToPlace.size(); i++)
		{
			if (objectsList[objectsToPlace[i]].Ability == -1)
				inverseState.AddCollectable(objectsList[objectsToPlace[i]].ItemTag, objectsList[objectsToPlace[i]].ItemAmount);
			else
				inverseState.SetAbilities.push_back(std::make_pair(-1, objectsList[objectsToPlace[i]].Ability));
		}
		inverseState.keepCollectables = true;
		LogicHandler::AccessibleThings newState = LogicHandler::GetAllTotals(startingGroup, logicGroups, inverseState, objects, lookedAtLogicGroups_TEMP, nextLogicGroups_TEMP, viableLogicGroups_TEMP);

		std::vector<int>validLocations = newState.GetValidLocationsForItem(item);
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
		if (validAndReachable.size() == 0)
		{
			::MessageBox(NULL, "Failed to Find Valid and Reachable", "Error", NULL);
			return LogicHandler::AccessibleThings();
		}
		std::shuffle(validAndReachable.begin(), validAndReachable.end(), rng);


		for (int i = 0; i < validAndReachable.size(); i++)
		{
			bool continueMark = false;
			inverseState = LogicHandler::AccessibleThings();
			inverseState.Add(initialState);
			int locationId = validAndReachable[i];
			ownedState.AddSetItem(locationId, item.RandoObjectID);
			if (item.Ability != -1)
				ownedState.UpdateMoves();

			//Iterate through the rest of the items and make sure they can still be placed
			if (objectsToPlace.size() > 0)
			{
				inverseState.SetItems = ownedState.SetItems;
				for (int i = 0; i < ownedState.SetItems.size(); i++)
				{
					if (objectsList[ownedState.SetItems[i].second].Ability == -1)
						inverseState.AddCollectable(objectsList[ownedState.SetItems[i].second].ItemTag, objectsList[ownedState.SetItems[i].second].ItemAmount);
					else
						inverseState.SetAbilities.push_back(std::make_pair(-1, objectsList[ownedState.SetItems[i].second].Ability));
				}

				for (int i = 0; i < objectsToPlace.size(); i++)
				{
					if (objectsList[objectsToPlace[i]].Ability == -1)
						inverseState.AddCollectable(objectsList[objectsToPlace[i]].ItemTag, objectsList[objectsToPlace[i]].ItemAmount);
					else
						inverseState.SetAbilities.push_back(std::make_pair(-1, objectsList[objectsToPlace[i]].Ability));
				}
				inverseState.keepCollectables = true;

				lookedAtLogicGroups_TEMP.clear();
				nextLogicGroups_TEMP.clear();
				viableLogicGroups_TEMP.clear();

				LogicHandler::AccessibleThings reachableState = LogicHandler::GetAllTotals(startingGroup, logicGroups, inverseState, objects, lookedAtLogicGroups_TEMP, nextLogicGroups_TEMP, viableLogicGroups_TEMP);

				for (int i = (objectsToPlace.size()-1); i >=0 ; i--)
				{
					RandomizedObject& checkitem = objectsList[objectsToPlace[i]];
					std::vector<int>validLocations = reachableState.GetValidLocationsForItem(checkitem);
					std::vector<int>validAndReachable;
					for (int validIndex = 0; validIndex < validLocations.size(); validIndex++)
					{
						for (int availLocation = 0; availLocation < reachableState.ItemLocations.size(); availLocation++)
						{
							if (validLocations[validIndex] == reachableState.ItemLocations[availLocation])
							{
								validAndReachable.push_back(validLocations[validIndex]);
							}
						}
					}
					if (validAndReachable.size()>0)
					{
						auto foundLocation = std::find(reachableState.ItemLocations.begin(), reachableState.ItemLocations.end(), validAndReachable.back());
						if (foundLocation != reachableState.ItemLocations.end())
						{
							reachableState.ItemLocations.erase(foundLocation);
						}
					}
					else
					{
						continueMark = true;
						break;
					}
				}
				if (continueMark)
				{
					ownedState.SetItems.pop_back();
					continue;
				}
			}



			break;
		}
	}
	ownedState.done = true;
	return ownedState;
}

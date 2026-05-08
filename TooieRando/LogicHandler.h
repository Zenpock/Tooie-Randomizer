#pragma once
#include "LogicGroup.h"
#include "MoveObject.h"
#include "Entrance.h"
#include "HelperFunctions.h"
#include <tuple> 
#include <random>
#include <algorithm>

#include <chrono>

#include<unordered_map>
#include <unordered_set>
#include <windows.h>
#include <sstream>
#include "OptionData.h"
#include <set>
#include <map>
#include "Collectables.h"

typedef struct BarrierPlaces {
	int barrierCount;
	std::set<int> itemLocations;
	bool operator < (const BarrierPlaces& other) const { return barrierCount < other.barrierCount; }

};

typedef struct BarrierInterval {
	int minCount; //If we are between this number inclusive
	int maxCount; //And this number exclusive
	std::set<int> itemLocations; //These are the valid locations
	bool operator < (const BarrierInterval& other) const { return maxCount > other.maxCount; }
};

class LogicHandler
{
public:
	int groupsTraversed = 0;
	LogicHandler() {
		groupsTraversed = 0;
	}
	static bool LogicHandler::alreadySetup;
	static std::unordered_map<int,RandomizedObject> objectsList; //List of All Objects
	static std::unordered_map<int, Entrance> LogicHandler::EntranceList;
	static std::vector<OptionData>* LogicHandler::options;

	//Status Box to show the progress of the randomization
	CEdit* RandoStatusBox;

	static bool generousJiggies;

	static std::vector<int> worldPrices;

	static bool debug;
	static bool printLog;
	static bool saveLogging;
	static int debugLevel;

	static std::unordered_map<int, std::set<int>> normalLevelObjectsMapAll; //List of all objects sorted int groups by level

	static std::unordered_map<int, std::set<int>> levelObjectsMapAll; //List of all objects sorted int groups by level

	static std::set<int> normalAll;

	static std::set<int> allObjects;

	static std::set<int> allSpawnableObjects;

	static std::unordered_map<int, std::vector<int>> LogicHandler::shuffleGroups;

	static std::unordered_map<int, int> LogicHandler::entranceAssociations;

	static bool objectsNotRandomized; //Whether the objects not randomized options is set
	
	static std::set<int> NoRandomizationIDs;
	static std::set<int> LevelRestrictedIDs;
	static std::set<int> HintBlacklist;

	//Map of the Ability stored in an item and the associated RandoObjectID
	static std::unordered_map<int, int> LogicHandler::AbilityItems;

	static std::vector<std::string> WorldTags;
	static std::vector<int>  LogicHandler::notePrices;
	static std::unordered_map<int, int>  LogicHandler::siloIndexStep;
	static std::unordered_map<int, std::string>  LogicHandler::WorldPrefixes;
	static std::unordered_map<int, int> LogicHandler::EntranceToWorld;
	static std::unordered_map<int, int> LogicHandler::EntranceInWorld;


	static void DebugPrintPriority(const std::string& message, int priority)
	{
		if (!debug||priority != debugLevel)
			return;
		static bool savedBefore = false;
		if(printLog)
			OutputDebugStringA((message + "\n").c_str());
		if (saveLogging)
		{
			if (savedBefore == false)
			{
				remove(LoggingFile);
				savedBefore = true;
			}
			std::fstream myfile;
			myfile.open(LoggingFile, std::ios::app);
			std::string str = message + " \n";
			myfile << str;
		}

	}

	static void DebugPrint(const std::string& message)
	{
		DebugPrintPriority(message, 0);
	}

	class NoteState
	{
	public:
		bool TrebleUsed = false;
		int usedNotes = 0;
		NoteState()
		{
		}
	};

	class AccessibleThings
	{
	public:
		~AccessibleThings() {
		}
		bool checkedOldGroups = false;
		bool keepCollectables = false; //This command prevents collectables from being cleared for tracking purposes		
		//RandoObjectID paired with the ability that is placed there
		std::vector<std::pair<int,int>> SetAbilities; 
		//Contains every RandoObjectID currently available
		//Available Locations to place Objects
		std::vector<int> ItemLocations; 

		//Locations that have been set and reached
		std::set<int> OwnedLocations;
		int depthToLeave=0; //The amount of steps back to take
		
		/// <summary>
		/// LocationId,SourceID
		/// </summary>
		std::vector<std::pair<int, int>> SetItems;
		//This is just a set of the sources that have already been placed in set items for quicker retrieval
		std::unordered_set<int> UsedItems;
		//This is just a set of the Locations that have already been placed in set items for quicker retrieval
		std::unordered_set<int> UsedLocations;
		/// <summary>
		/// Entry Warp, Exit Warp
		/// </summary>
		std::vector<std::pair<int, int>> SetWarps;

		//Vector of the different notes stored by level int being the level index
		std::unordered_map<int, NoteState> levelNotes;

		/// <summary>
		/// Collectable Name paired with the number of them
		/// </summary>
		std::vector<std::pair<CollectableId,int>> ContainedItems;

		//Collectable Barriers
		std::set<std::pair<CollectableId, int>> Barriers;

		std::unordered_set<std::string> Keys;

		//Last few groups traversed
		std::vector<int> lastTraversed;

		bool done = false;
		/// <summary>
		/// Add all of the items from the given group to this one
		/// </summary>
		/// <param name="things"></param>
		void AccessibleThings::Add(AccessibleThings& things)
		{
			checkedOldGroups = things.checkedOldGroups;
			OwnedLocations.insert(things.OwnedLocations.begin(), things.OwnedLocations.end());
			for (int i = 0; i < things.SetAbilities.size(); i++)
			{
				int ability = std::get<1>(things.SetAbilities[i]);
				auto matchesAbility = [ability](std::pair<int,int> object) {return (std::get<1>(object)) == ability; };
				auto it = std::find_if(SetAbilities.begin(), SetAbilities.end(), matchesAbility);
				if (it == SetAbilities.end()) //See if we dont already have this ability
				{
					SetAbilities.push_back(things.SetAbilities[i]);
				}
			}
			for (int i = 0; i < things.ItemLocations.size(); i++)
			{
				auto it = std::find(ItemLocations.begin(), ItemLocations.end(), things.ItemLocations[i]);
				if (it == ItemLocations.end())
					ItemLocations.push_back(things.ItemLocations[i]);
			}

			for (int i = 0; i < things.SetItems.size(); i++)
			{
				int sourceObject = std::get<1>(things.SetItems[i]);

				if (UsedItems.count(sourceObject) == 0 && UsedLocations.count(std::get<0>(things.SetItems[i])) == 0)
				{
					AddSetItem(std::get<0>(things.SetItems[i]), std::get<1>(things.SetItems[i]));
				}
			}

			for (int i = 0; i < things.SetWarps.size(); i++)
			{
				int exitID = std::get<1>(things.SetWarps[i]);
				auto matchesEntrance = [exitID](std::pair<int, int> entrancePair) {return (std::get<0>(entrancePair)) == exitID || std::get<1>(entrancePair) == exitID; };
				auto it = std::find_if(SetWarps.begin(), SetWarps.end(), matchesEntrance);
				if (it == SetWarps.end()) //Check if this entrance has been set before
				{
					//Get the Group Associated with this entrance
					SetWarps.push_back(things.SetWarps[i]);
				}
			}

			for (auto key: things.Keys)
			{
				Keys.insert(key);
			}
			Barriers.insert(things.Barriers.begin(), things.Barriers.end());

			keepCollectables = things.keepCollectables;
			if(keepCollectables == false)
			{ 
				UpdateCollectables();
			}
			else
			{
				ContainedItems.clear();
				for (int i = 0; i < things.ContainedItems.size(); i++)
				{
					ContainedItems.push_back(things.ContainedItems[i]);
				}
			}

		}
		/// <summary>
		/// Add all of the items from the given group to this one
		/// </summary>
		/// <param name="group"></param>
		/// <param name="objects">The list of all of the objects that exist so additional data can be gathered from them</param>
		void AccessibleThings::Add(LogicGroup& group, const std::vector<RandomizedObject>& objects)
		{
			//Loop through all objects and add the locations from the group if they can be found in the main list
			int foundObjects = 0;
			for (int j = 0; j < objects.size(); j++)
			{
				for (int i = 0; i < group.objectIDsInGroup.size(); i++)
				{
					if (objects[j].RandoObjectID == group.objectIDsInGroup[i])
					{
						int locationObjectID = objects[j].RandoObjectID;
						auto matchesObject = [locationObjectID](std::pair<int, int> object) {return std::get<0>(object) == locationObjectID; };
						auto it = std::find_if(SetItems.begin(), SetItems.end(), matchesObject);
						if (it == SetItems.end())
						{
							foundObjects++;
							ItemLocations.push_back(objects[j].RandoObjectID);
						}
						else
						{
							DebugPrintPriority("Found Set Item "+ (objectsList[it->second].Ability==-1?objectsList[it->second].ItemTag: objectsList[it->second].MoveName) + " at "+ objectsList[it->first].LocationName, 0);
							checkedOldGroups = false;
						}
						OwnedLocations.insert(objects[j].RandoObjectID);
						break;
					}
				}
				if (foundObjects >= group.objectIDsInGroup.size())
					break;
			}

			if (keepCollectables == false)
			{
				UpdateCollectables();
				UpdateMoves();
			}

			if (!group.key.empty())
			{
				Keys.insert(group.key);
				checkedOldGroups = false;

			}
			for (LogicGroup::RequirementSet& set : group.Requirements)
			{
				for (int j = 0; j < set.RequiredItems.size(); j++)
				{
					if (group.Requirements.size() == 1)
					{
						Barriers.insert(std::make_pair(set.RequiredItems[j], set.RequiredItemsCount[j]));
					}
				}
			}
		}

		int AccessibleThings::GetCollectableCount(CollectableId name)
		{
			auto matchesName = [name](std::pair<CollectableId,int> Collectable) {return Collectable.first == name; };
			auto it = std::find_if(ContainedItems.begin(), ContainedItems.end(), matchesName);
			if (it != ContainedItems.end())
				return (*it).second;
			else
				return 0;
		}

		/// <summary>
		/// Find the required items in the requirement set and assign the items to available locations
		/// </summary>
		/// <param name="things"></param>
		void AccessibleThings::AddItems(const LogicGroup::RequirementSet& requirement,std::default_random_engine& rng)
		{
			DebugPrintPriority("Adding requirements from Requirement Set: " + requirement.SetName,6);

			checkedOldGroups = false;

			std::vector<int> outVector;
			outVector = ItemLocations; //Shuffle the locations
			DebugPrintPriority("Item Locations: " + std::to_string(ItemLocations.size()),6);

			std::shuffle(outVector.begin(), outVector.end(), rng);
			std::unordered_map<int, std::vector<int>> normalLevelObjectsMap;
			//sort the available normal locations into levels
			for (int shuffledIdx = 0; shuffledIdx < outVector.size(); shuffledIdx++)
			{
				if (!objectsList[outVector[shuffledIdx]].IsSpawnLocation)
					normalLevelObjectsMap[objectsList[outVector[shuffledIdx]].LevelIndex].push_back(outVector[shuffledIdx]);
			}

			
			std::map<int,int> slotIdsToRemove;
			//Get the amount of note slots to remove for each level so we can try and use those to fulfill any note needs
			std::vector<int> levels = GetLevels(); //Get the accessible levels
			bool NotesRandomize = NoRandomizationIDs.count(Prop_Note) == 0;
			if (NotesRandomize)
			{
				for (int levelIndex = 0; levelIndex < levels.size(); levelIndex++)
				{

					int levelInt = levels[levelIndex];
					int usedSlots = 0;
					for (const auto& item : SetItems)
					{
						if (objectsList[std::get<0>(item)].LevelIndex == levelInt && !objectsList[std::get<0>(item)].IsSpawnLocation)
							usedSlots++;
					}
					int unusedSlots = GetUnusedNormalGlobalLocationsFromLevel(levelInt);
					DebugPrintPriority("We have " + std::to_string(unusedSlots) + " unused normal slots in level " + std::to_string(levelInt),6);

					DebugPrintPriority("We have " + std::to_string(normalLevelObjectsMap[levelInt].size()) + " available unused normal slots in level " + std::to_string(levelInt),6);

					int slotsUnallocated = 17;
					slotsUnallocated -= levelNotes[levelInt].usedNotes;
					DebugPrintPriority("We have used " + std::to_string(levelNotes[levelInt].usedNotes) + " note slots in level " + std::to_string(levelInt),6);

					if (levelNotes[levelInt].TrebleUsed)
					{
						DebugPrintPriority("We have used a treble in level " + std::to_string(levelInt),6);
						slotsUnallocated--;
					}
					int slotsToRemove = 0;
					int availableNormalSlots = normalLevelObjectsMap[levelInt].size();
					slotsToRemove = availableNormalSlots-(unusedSlots-slotsUnallocated);
					if (unusedSlots < slotsUnallocated)
					{
						::MessageBox(NULL, "Logic Error Occured Ran out of allocated space for notes (please try a different seed)", "Error", NULL);

						//OutputDebugString(("We have run out of slots to allocate Unused Slots: " + std::to_string(unusedSlots) + " Note Slots Unallocated: " + std::to_string(slotsUnallocated)+"\n").c_str());
					}

					if (slotsToRemove < 0)
						slotsToRemove = 0;

					slotIdsToRemove[levelInt] = slotsToRemove;

					DebugPrintPriority("We need to remove " + std::to_string(slotsToRemove) + " slots from level " +std::to_string(levels[levelIndex]),6);
				}
			}

			//Add notes first
			for (int i = 0; i < requirement.RequiredItems.size(); i++)
			{
				if (requirement.RequiredItems[i] == Collect_Note)
				{
					int collectableAmount = GetCollectableCount(requirement.RequiredItems[i]);
					bool firstLoop = true;
					while (collectableAmount < requirement.RequiredItemsCount[i])
					{
						std::vector<int> levels = GetLevels(); //Get the accessible levels
						//Loop through all of the accessible levels to find places to put notes
						for (int levelIndex = 0; levelIndex < levels.size(); levelIndex++)
						{
							int levelInt = levels[levelIndex];
							if (firstLoop && slotIdsToRemove[levelInt] == 0)
								continue;
							bool trebleUsed = levelNotes[levelInt].TrebleUsed;
							int usedNotes = levelNotes[levelInt].usedNotes; //Number of normal note nests that have been used in this level
							int availableLocations = normalLevelObjectsMap[levelInt].size();
							if (trebleUsed == false && availableLocations > 0)
							{
								collectableAmount += 20; //Always try a treble
								int sourceObjectID = FindObjectOfType(requirement.RequiredItems[i], 20, levelInt);
								if (sourceObjectID != -1)
								{
									int objectID = normalLevelObjectsMap[levelInt][0];
									auto foundLocation = std::find(ItemLocations.begin(), ItemLocations.end(), objectID);
									if (foundLocation != ItemLocations.end())
									{
										AddSetItem(normalLevelObjectsMap[levelInt][0], sourceObjectID);
										slotIdsToRemove[levelInt]--;
										normalLevelObjectsMap[levelInt].erase(normalLevelObjectsMap[levelInt].begin());

										auto foundInShuffled = std::find(outVector.begin(), outVector.end(), objectID);
										outVector.erase(foundInShuffled);
										DebugPrintPriority("OutVector Size After Treble Placement: " + std::to_string(outVector.size()), 6);

										ItemLocations.erase(foundLocation);
										availableLocations--;
										levelNotes[levelInt].TrebleUsed = true;
										DebugPrintPriority("Added Treble " + objectsList[sourceObjectID].ItemTag + " at " + IntToHexString(objectID), 6);
									}
								}
								else
								{
									::MessageBox(NULL, "Could not Find Valid Treble", "Error", NULL);
								}
							}
							if (availableLocations > 0)
							{
								//Keep trying to add notes until we reach the required value making sure the number of notes does not exceed the maximum notes in the level
								while (collectableAmount < requirement.RequiredItemsCount[i] && usedNotes < 16 && availableLocations > 0)
								{
									collectableAmount += 5;
									int sourceObjectID = FindObjectOfType(requirement.RequiredItems[i], 5, levelInt);
									if (sourceObjectID != -1)
									{
										int objectID = normalLevelObjectsMap[levelInt][0];
										auto foundLocation = std::find(ItemLocations.begin(), ItemLocations.end(), objectID);
										if (foundLocation != ItemLocations.end())
										{
											AddSetItem(normalLevelObjectsMap[levelInt][0], sourceObjectID);
											slotIdsToRemove[levelInt]--;

											normalLevelObjectsMap[levelInt].erase(normalLevelObjectsMap[levelInt].begin());

											auto foundInShuffled = std::find(outVector.begin(), outVector.end(), objectID);
											outVector.erase(foundInShuffled);
											DebugPrintPriority("OutVector Size After Note Placement: " + std::to_string(outVector.size()), 6);

											ItemLocations.erase(foundLocation);
											availableLocations--;
											DebugPrintPriority("Added Note " + objectsList[sourceObjectID].ItemTag + " at " + IntToHexString(objectID), 6);
										}
									}
									usedNotes++;
								}
								levelNotes[levelInt].usedNotes = usedNotes;
							}
						}
						if (firstLoop == false && collectableAmount < requirement.RequiredItemsCount[i])
						{
							::MessageBox(NULL, "Failed to fill notes", "Error", NULL);
							break;
						}
						//Do the loop again if we couldn't fulfill the requirement with just slots that were going to be removed
						firstLoop = false;
					}
				}
			}

			//Remove the slots not used for notes
			for (int levelIndex = 0; levelIndex < levels.size(); levelIndex++)
			{
				int levelInt = levels[levelIndex];
				for (int i = 0; i < slotIdsToRemove[levelInt]; i++)
				{
					int foundId = -1;
					if (normalLevelObjectsMap[levelInt].size() > 0)
						foundId = normalLevelObjectsMap[levelInt][0];
					auto foundNormalMap = std::find(normalLevelObjectsMap[levelInt].begin(), normalLevelObjectsMap[levelInt].end(), foundId);
					auto foundLocation = std::find(outVector.begin(), outVector.end(), foundId);
					if (foundLocation != outVector.end())
					{
						normalLevelObjectsMap[levelInt].erase(foundNormalMap);
						outVector.erase(foundLocation);

						DebugPrintPriority("Removed slot from level " + std::to_string(levelInt) + "(" + IntToHexString(foundId) + ")", 6);
						DebugPrintPriority("1OutVector Size After Removal: " + std::to_string(outVector.size()), 6);

					}
					else
					{
						DebugPrintPriority("Failed to Remove slot from level " + std::to_string(levelInt), 6);
					}
				}
			}

			//Setup the remaining normal objects first
			for (int i = 0; i < requirement.RequiredItems.size(); i++) 
			{
				if (requirement.RequiredItems[i] != Collect_Note)
				{
					for (int j = GetCollectableCount(requirement.RequiredItems[i]); j < requirement.RequiredItemsCount[i]; j++)
					{
						int sourceObjectID = FindObjectOfType(requirement.RequiredItems[i], 1);
						if (sourceObjectID != -1 && objectsList[sourceObjectID].CanBeReward(requirement.RequiredItems[i]) == false)
						{
							int outVectorIndex = 0;
							//Find a location in the outvector that is not a spawn location
							while (outVectorIndex<outVector.size() && objectsList[outVector[outVectorIndex]].IsSpawnLocation)
							{
								outVectorIndex++;
							}
							if (outVector.size() > 0 && outVectorIndex < outVector.size())
							{
								int locationObjectID = objectsList[outVector[outVectorIndex]].RandoObjectID;
								auto foundLocation = std::find(ItemLocations.begin(), ItemLocations.end(), locationObjectID);
								if (foundLocation != ItemLocations.end())
								{
									AddSetItem(locationObjectID, sourceObjectID);

									outVector.erase(outVector.begin());
									DebugPrintPriority("3OutVector Size After Removal: " + std::to_string(outVector.size()), 6);

									ItemLocations.erase(foundLocation);
									DebugPrintPriority("Added Item " + objectsList[sourceObjectID].ItemTag + "("+ IntToHexString(sourceObjectID) +")" + " at " + IntToHexString(locationObjectID),6);
								}
							}
							else
							{
								::MessageBox(NULL, "Logic did not leave enough valid locations for notes outside of logic (please try a different seed)", "Error", NULL);

								//OutputDebugString("Out of valid locations as they Could Not Be Found\n");
							}
						}
					}
				}
			}

			//No other objects need to be kept in a level in the same way
			for (int i = 0; i < requirement.RequiredItems.size(); i++) 
			{
				if (requirement.RequiredItems[i] != Collect_Note)
				{
					int startCollectableAmount = GetCollectableCount(requirement.RequiredItems[i]);
					DebugPrintPriority("Collectables to add " + std::to_string(requirement.RequiredItemsCount[i]- startCollectableAmount), 6);
					for (int j = startCollectableAmount; j < requirement.RequiredItemsCount[i]; j++)
					{
						int sourceObjectID = FindObjectOfType(requirement.RequiredItems[i], 1);
						if (sourceObjectID != -1 && objectsList[sourceObjectID].CanBeReward(requirement.RequiredItems[i]))
						{
							if (outVector.size() > 0)
							{
								int objectID = objectsList[outVector[0]].RandoObjectID;
								auto foundLocation = std::find(ItemLocations.begin(), ItemLocations.end(), objectID);
								if (foundLocation != ItemLocations.end())
								{
									AddSetItem(objectsList[outVector[0]].RandoObjectID, sourceObjectID);

									outVector.erase(outVector.begin());
									DebugPrintPriority("4OutVector Size After Removal: " + std::to_string(outVector.size()), 6);

									ItemLocations.erase(foundLocation);
									DebugPrint("Added Item " + objectsList[sourceObjectID].ItemTag + "(" + IntToHexString(sourceObjectID) + ")" + " at " + IntToHexString(objectID));
								}

							}
							else
							{
								::MessageBox(NULL, "Logic did not leave enough valid locations for notes outside of logic (please try a different seed)", "Error", NULL);

								//OutputDebugString("Out of valid locations as they Could Not Be Found\n");
							}
						}
					}
				}
			}


			for (int i = 0; i < requirement.RequiredAbilities.size(); i++)
			{
				int requiredAbility = requirement.RequiredAbilities[i];

				//Check if we already have this ability
				auto matchesSetAbilty = [requiredAbility](std::pair<int, int> move) {return std::get<1>(move) == requiredAbility; };
				auto abilityAlreadyAcquired = std::find_if(SetAbilities.begin(), SetAbilities.end(), matchesSetAbilty);
				if (abilityAlreadyAcquired != SetAbilities.end() && OwnedLocations.count(abilityAlreadyAcquired->first) != 0) //If we already have the ability try the others
				{
					continue;
				}
				else if (abilityAlreadyAcquired != SetAbilities.end() && OwnedLocations.count(abilityAlreadyAcquired->first) == 0)
				{
					::MessageBox(NULL, "Add Items has been used incorrectly a required ability is already placed and not owned", "Error", NULL);
				}
				//Look through all of the available objects to find the ability
				if (AbilityItems.find(requiredAbility)!=AbilityItems.end()) //If the move was found try and add it
				{
					auto foundLocation = std::find(ItemLocations.begin(), ItemLocations.end(), outVector[0]);
					if (foundLocation != ItemLocations.end())
					{
						//Add the source ability item and the target to the setitems
						AddSetItem(outVector[0], AbilityItems[requiredAbility]);
						
						//Remove the Target Location from the overall list
						ItemLocations.erase(foundLocation);
						DebugPrintPriority("Added Ability " + objectsList[AbilityItems[requiredAbility]].MoveName + " at " + IntToHexString(outVector[0]),6);
						//Remove the Target Location from the shuffled list
						outVector.erase(outVector.begin());
						DebugPrintPriority("2OutVector Size After Removal: " + std::to_string(outVector.size()), 6);

					}
				}
			}
		}

		int AccessibleThings::FindObjectOfType(CollectableId objectName)
		{
			return FindObjectOfType(objectName, 1);
		}

		/// <summary>
		/// Get RandoObjectId or return -1 if one could not be found
		/// </summary>
		int AccessibleThings::FindObjectOfType(CollectableId objectName, int amount)
		{
			for (const auto& obj : objectsList)
			{
				int id = obj.first;
				const RandomizedObject& object = obj.second;
				if (UsedItems.count(object.RandoObjectID) == 0 && object.collectableId == objectName && object.ItemAmount == amount && object.Randomized)
				{
					return object.RandoObjectID;  // Return first match immediately
				}
			}

			return -1;  // No match found
		}

		/// <summary>
		/// Get RandoObjectId or return -1 if one could not be found
		/// </summary>
		/// <param name="objectName">Find objects of this specific collectable type</param>
		/// <param name="amount">The value of the specific item type to look for e.g. 20 for trebles, 5 for note nests</param>
		/// <param name="levelIndex">The level the source should be in</param>
		/// <returns></returns>
		int AccessibleThings::FindObjectOfType(CollectableId objectName, int amount, int levelIndex)
		{

			for (const auto& obj : objectsList)
			{
				int id = obj.first;
				const RandomizedObject& object = obj.second;
				if (UsedItems.count(object.RandoObjectID) == 0 && object.collectableId == objectName && object.ItemAmount == amount && object.Randomized && object.LevelIndex == levelIndex)
				{
					if(NoRandomizationIDs.find(object.ObjectID) == NoRandomizationIDs.end())
						return object.RandoObjectID;  // Return first match immediately
				}
			}

			return -1;  // No match found
		}

		std::set<int> AccessibleThings::FindObjectsOfType(CollectableId objectName, int amount)
		{
			std::set<int> returnObjects;

			for (const auto& obj : objectsList)
			{
				int id = obj.first;
				const RandomizedObject& object = obj.second;
				if (UsedItems.count(object.RandoObjectID) == 0 && object.collectableId == objectName && object.ItemAmount == amount && object.Randomized)
				{
					if (NoRandomizationIDs.find(object.ObjectID) == NoRandomizationIDs.end())
						returnObjects.insert(object.RandoObjectID);  // Return first match immediately
				}
			}

			return returnObjects;  // No match found
		}

		std::set<int> AccessibleThings::FindObjectsOfType(CollectableId objectName, int amount, int levelIndex)
		{
			std::set<int> returnObjects;

			for (const auto& obj : objectsList)
			{
				int id = obj.first;
				const RandomizedObject& object = obj.second;
				if (UsedItems.count(object.RandoObjectID) == 0 && object.collectableId == objectName && object.ItemAmount == amount && object.Randomized && object.LevelIndex == levelIndex)
				{
					if (NoRandomizationIDs.find(object.ObjectID) == NoRandomizationIDs.end())
						returnObjects.insert(object.RandoObjectID);  // Return first match immediately
				}
			}

			return returnObjects;  // No match found
		}

		void AccessibleThings::UpdateCollectables()
		{
			if (keepCollectables == false)
			{
				ContainedItems.clear();
				levelNotes.clear();
				for (int i = 0; i < SetItems.size(); i++)
				{
					const RandomizedObject& object = objectsList[std::get<1>(SetItems[i])];
					if (OwnedLocations.count(std::get<0>(SetItems[i])) > 0)
					{

						AddCollectable(object.collectableId, object.ItemAmount);

					}
					if (object.ItemTag == "Note")
					{
						if (object.ItemAmount == 20)
							levelNotes[object.LevelIndex].TrebleUsed = true;
						else
							levelNotes[object.LevelIndex].usedNotes++;;
					}
				}
			}
		}

		void AccessibleThings::UpdateMoves()
		{
			if (keepCollectables == false)
			{
				SetAbilities.clear();
				for (int i = 0; i < SetItems.size(); i++)
				{
					const RandomizedObject& sourceObject = objectsList[std::get<1>(SetItems[i])];
					if (sourceObject.ItemTag == "Move Item" && OwnedLocations.count(std::get<0>(SetItems[i])) > 0)
					{
						SetAbilities.push_back(std::make_pair(objectsList[std::get<0>(SetItems[i])].RandoObjectID, sourceObject.Ability));
					}
				}
			}
		}

		/// <summary>
		/// Determine whether the available locations for placement would allow the given requirement set to have its remaining missing elements placed into the state such that the requirement would be fulfilled
		/// </summary>
		/// <param name="requirement"></param>
		/// <returns></returns>
		bool AccessibleThings::CanFulfill(const LogicGroup::RequirementSet* requirement,std::unordered_map<int, int>& unusedNormalGlobal)
		{
			//Check if notes should be randomized
			bool NotesRandomize = (NoRandomizationIDs.count(Prop_Note)==0);

			int normalLocationsCount = GetNormalLocations().size();
			int neededSpots = 0;//Number of locations required to fulfill the requirement
			int neededNormalSpots = 0; //Number of nonspawning locations

			DebugPrintPriority(("Checking requirements for set "+ requirement->SetName),6);

			DebugPrintPriority(("Number of Available Normal Locations " + std::to_string(normalLocationsCount) + " for set " + requirement->SetName),2);
			int missingAbilities = 0;
			for (int i = 0; i < requirement->RequiredAbilities.size(); i++)
			{
				int ability = requirement->RequiredAbilities[i];
				DebugPrintPriority(("Ability Needed: " + objectsList[AbilityItems[ability]].MoveName), 0);

				for (auto setPair : SetItems)
				{
					if (objectsList[setPair.second].Ability == ability && OwnedLocations.count(setPair.first)==0)
					{
						//(("An ability we need is set to appear later making this route impossible right now. Set name:" + requirement->SetName), 5);
						return false;
					}
				}
				auto matchesAbility = [ability](std::pair<int,int> move) {return std::get<1>(move) == ability; };
				auto it = std::find_if(SetAbilities.begin(), SetAbilities.end(), matchesAbility);
				if (it == SetAbilities.end()) //If the ability is not already found in the set abilities
				{
					missingAbilities++;
				}
			}
			//Add the amount of spots needed for the missing abilities
			neededSpots += missingAbilities;
			
			//Early end if we have run out of spots
			if (ItemLocations.size()<neededSpots)
			{
				return false;
			}

			std::vector<int> levels = GetLevels(); //Get the accessible levels
			int totalNormalObjects = 0; //Sum of the total number of normal objects in the accessible levels
			std::unordered_map<int, NoteState> tempLevelNotes;

			//The number of normal locations found in each level. Normal being non spawning
			//Level Int, Amount of locations

			for (int levelIndex = 0; levelIndex < levels.size(); levelIndex++)
			{
				int levelInt = levels[levelIndex];
				totalNormalObjects += normalLevelObjectsMapAll[levelInt].size();
				tempLevelNotes[levelInt].usedNotes = levelNotes[levelInt].usedNotes;
				tempLevelNotes[levelInt].TrebleUsed = levelNotes[levelInt].TrebleUsed;
				//unusedNormalLocationsByLevel[levelInt] = GetUnusedNormalGlobalLocationsFromLevel(levelInt);
			}

			DebugPrintPriority(("Number of Total Available Normal Locations " + std::to_string(normalLocationsCount) + " for set" + requirement->SetName),2);

			int noteSpots = 0; //How Many Spots are notes using

			for (int j = 0; j < requirement->RequiredItems.size(); j++)
			{
				CollectableId collectableName = requirement->RequiredItems[j];
				Collectable collect = GetCollectibleFromCollectibleId(collectableName);
				int collectableAmount = GetCollectableCount(collectableName);
				if (requirement->RequiredItems[j] == Collect_Note)
				{
					for (int levelIndex = 0; levelIndex < levels.size(); levelIndex++)
					{

						int levelInt = levels[levelIndex];
						
						//Indicates a treble was added this check to ensure we do not try and allocate more than available
						int trebleAdded = 0; 
						
						int normalSize = GetNormalLocationsFromMap(levelInt);
						//If there's available space for a treble use it
						if (!levelNotes[levelInt].TrebleUsed && normalSize > 0 && FindObjectOfType(Collect_Note, 20, levelInt)!=-1)
						{
							collectableAmount += 20;
							tempLevelNotes[levelInt].TrebleUsed = true;
							trebleAdded = 1;
							neededSpots++;
							neededNormalSpots++;
						}
						int numAvailableNotes = FindObjectsOfType(Collect_Note, 5, levelInt).size();
						while (collectableAmount < requirement->RequiredItemsCount[j] &&
							tempLevelNotes[levelInt].usedNotes < 16 &&
							tempLevelNotes[levelInt].usedNotes + trebleAdded < normalSize &&
							numAvailableNotes > 0
							)
						{
							numAvailableNotes--;
							collectableAmount += 5;
							tempLevelNotes[levelInt].usedNotes++;
							neededSpots++;
							neededNormalSpots++;
						}
						DebugPrintPriority("Level: " + std::to_string(levelInt) + " Temp Used Notes " + std::to_string(tempLevelNotes[levelInt].usedNotes),2);
						DebugPrintPriority("Level: " + std::to_string(levelInt) + " Available Normal Locations " + std::to_string(normalSize),2);
						DebugPrintPriority("Needed Notes: " + std::to_string(requirement->RequiredItemsCount[j]) + " Collected Notes: " + std::to_string(GetCollectableCount(collectableName)),2);
						DebugPrintPriority(" Possible Amount " + std::to_string(collectableAmount),2);

						if (collectableAmount >= requirement->RequiredItemsCount[j])
						{
							DebugPrintPriority("Level: " + std::to_string(levelInt) + " Temp Used Notes " + std::to_string(tempLevelNotes[levelInt].usedNotes),2);
							break;
						}
					}
					if (collectableAmount < requirement->RequiredItemsCount[j]) //If we cannot meet the quota we cannot meet the requirements
					{
						//DebugPrintPriority("Could not meet note quota Required Amount: " + std::to_string(requirement->RequiredItemsCount[j]) + " Possible Amount " + std::to_string(collectableAmount),5);
						return false;

					}
				}
				//Check if it isn't a reward object
				else if (!RandomizedObject::CanBeReward(requirement->RequiredItems[j]))
				{
					int numAvailableItems = FindObjectsOfType(collectableName, 1).size();
					if (numAvailableItems < requirement->RequiredItemsCount[j] - collectableAmount)
					{
						//DebugPrintPriority("Ran out of "+ collect.Name +" to use for placement required amount: " + std::to_string(requirement->RequiredItemsCount[j])+" Collected amount: "+std::to_string(collectableAmount)+" num available to place "+ std::to_string(numAvailableItems), 5);
						return false;
					}
					DebugPrintPriority("We need " + collect.Name + " to use for placement required amount: " + std::to_string(requirement->RequiredItemsCount[j]) + " Collected amount: " + std::to_string(collectableAmount) + " num available to place " + std::to_string(numAvailableItems), 0);

					//Make sure that we do not allocate more spots for items we already can afford
					if (collectableAmount < requirement->RequiredItemsCount[j])
					{
						neededSpots+= (requirement->RequiredItemsCount[j] - collectableAmount);
						neededNormalSpots += (requirement->RequiredItemsCount[j] - collectableAmount);
					}
				}
				else
				{
					int numAvailableItems = FindObjectsOfType(collectableName, 1).size();
					if (numAvailableItems < requirement->RequiredItemsCount[j] - collectableAmount)
					{
						//DebugPrintPriority("Ran out of items to use for placement: " + std::to_string(requirement->RequiredItemsCount[j]), 5);
						return false;
					}
					//Make sure that we do not allocate more spots for items we already can afford
					if (collectableAmount < requirement->RequiredItemsCount[j])
						neededSpots += (requirement->RequiredItemsCount[j] - collectableAmount);
				}
			}

			//Do we have enough spots overall
			if (ItemLocations.size() < neededSpots)
			{
				return false;
			}

			//Do we have enough normal spots (Does not consider level restricted)
			if (normalLocationsCount < neededNormalSpots)
			{
				return false;
			}
			//How many accessible note slots will need to be kept empty (not including notes added for this requirement)
			int unusedNoteSlotsToRemove = 0;
			if (NotesRandomize) //If the object is not randomized Set it to equal itself and continue
			{

				for (int levelIndex = 0; levelIndex < levels.size(); levelIndex++)
				{
					int levelInt = levels[levelIndex];
					int newNotes = tempLevelNotes[levelInt].usedNotes - levelNotes[levelInt].usedNotes;
					if (tempLevelNotes[levelInt].TrebleUsed == true && levelNotes[levelInt].TrebleUsed == false)
						newNotes++;

					int unusedNotesSlots = 16 - tempLevelNotes[levelInt].usedNotes;
					if (!tempLevelNotes[levelInt].TrebleUsed)
						unusedNotesSlots++;

					int accessibleUnusedNormalInLevel = GetNormalLocationsFromMap(levelInt);
					int globalUnusedNormalInLevel = unusedNormalGlobal[levelInt];

					//New Notes is the amount of slots for notes we need to be currently accessible
					if (newNotes > accessibleUnusedNormalInLevel)
					{
						return false;
					}

					if (newNotes + unusedNotesSlots > globalUnusedNormalInLevel)
					{
						return false;
					}
					

					//This should never happen
					if (accessibleUnusedNormalInLevel > globalUnusedNormalInLevel)
					{
						::MessageBox(NULL, "Accessible Unused is larger than global unused. There must be an error in logic calculation", "Error", NULL);
						return false;
					}
					if (unusedNotesSlots > globalUnusedNormalInLevel)
					{
						::MessageBox(NULL, "Unused Note Slot exceeds global unused. There must be an error in logic calculation", "Error", NULL);
						return false;
					}

					DebugPrintPriority("Unused Slots " + std::to_string(unusedNotesSlots) + " in level " + std::to_string(levelInt), 6);
					DebugPrintPriority("Global Unused Slots " + std::to_string(globalUnusedNormalInLevel) + " in level " + std::to_string(levelInt), 6);
					DebugPrintPriority("Accessible Unused Slots " + std::to_string(accessibleUnusedNormalInLevel) + " in level " + std::to_string(levelInt), 6);
					//8-(16-15) = 7
					int slotsToRemove = accessibleUnusedNormalInLevel - (globalUnusedNormalInLevel - unusedNotesSlots);
					if (slotsToRemove > 0)
					{
						unusedNoteSlotsToRemove += slotsToRemove;
						DebugPrintPriority("Slots to Remove " + std::to_string(slotsToRemove) + " in level " + std::to_string(levelInt), 6);

					}
				}

			}
			//Remove all of the note slot locations that are unused but need to be available for note placement later
			if (normalLocationsCount - unusedNoteSlotsToRemove < neededNormalSpots)
			{
				return false;
			}

			if (ItemLocations.size() - unusedNoteSlotsToRemove < neededSpots)
			{
				return false;
			}

			if (!ContainsRequiredKeys(this, requirement))
			{
				return false;
			}
			DebugPrintPriority("Total UnusedNoteSlotsToRemove " + std::to_string(unusedNoteSlotsToRemove) + " Normal Needed Spots " + std::to_string(neededNormalSpots), 6);

			DebugPrintPriority("Num Normal Accessible " + std::to_string(normalLocationsCount) + " Normal Needed Spots " + std::to_string(neededNormalSpots), 6);
			DebugPrintPriority("Num Accessible " + std::to_string(ItemLocations.size()) + " Needed Spots " + std::to_string(neededSpots) + " Normal Needed Spots " + std::to_string(neededNormalSpots), 6);
			DebugPrintPriority(("Requirement Set Fulfillable " + requirement->SetName), 6);

			return true;
		}

		void AccessibleThings::SetCollectable(CollectableId collectableName, int value)
		{
			auto it = std::find_if(ContainedItems.begin(), ContainedItems.end(), [collectableName](std::pair<CollectableId, int> Item) {return Item.first == collectableName; });
			if (it == ContainedItems.end())
			{
				ContainedItems.push_back(std::make_pair(collectableName, value));
			}
			else
			{
				std::get<1>(ContainedItems[it - ContainedItems.begin()]) = value;
			}
		}

		void AccessibleThings::AddCollectable(CollectableId collectableName, int value)
		{
			auto it = std::find_if(ContainedItems.begin(), ContainedItems.end(), [collectableName](std::pair<CollectableId, int> Item) {return Item.first == collectableName; });
			if (it == ContainedItems.end())
			{
				ContainedItems.push_back(std::make_pair(collectableName,value));
			}
			else //If there is already some amount of the object in the list add it
			{
				std::get<1>(ContainedItems[it - ContainedItems.begin()]) += value;
			}
		}

		std::vector<int> AccessibleThings::GetLocationsFromMap(int LevelIndex)
		{
			std::vector<int> ValidLocations;
			for (int i = 0; i < ItemLocations.size(); i++)
			{
				if (objectsList[ItemLocations[i]].LevelIndex == LevelIndex)
				{
					ValidLocations.push_back(ItemLocations[i]);
				}
			}
			return ValidLocations;
		}

		/// <summary>
		/// Get the normal locations in the given level that are accessible
		/// </summary>
		/// <param name="LevelIndex"></param>
		/// <returns></returns>
		int AccessibleThings::GetNormalLocationsFromMap(int LevelIndex)
		{
			int size = 0;
			for (int i = 0; i < ItemLocations.size(); i++)
			{
				if (objectsList[ItemLocations[i]].LevelIndex == LevelIndex && objectsList[ItemLocations[i]].IsSpawnLocation == false)
				{
					size++;
				}
				if (UsedLocations.count(ItemLocations[i])||NoRandomizationIDs.count(objectsList[ItemLocations[i]].ObjectID)!=0|| !objectsList[ItemLocations[i]].Randomized)
				{
					DebugPrintPriority("FoundFail",6);
				}

			}
			return size;
		}

		//Get Locations that do not spawn objects from the accessible state
		std::vector<int> AccessibleThings::GetNormalLocations()
		{
			std::vector<int> ValidLocations;
			for (int i = 0; i < ItemLocations.size(); i++)
			{
				if (objectsList[ItemLocations[i]].IsSpawnLocation == false)
				{
					ValidLocations.push_back(ItemLocations[i]);
				}
			}
			return ValidLocations;
		}

		//Get All Locations regardless of accessibility
		static std::vector<int> AccessibleThings::GetNormalGlobalLocations()
		{
			std::vector<int> ValidLocations;
			for (const auto& obj : objectsList)
			{
				if (!obj.second.IsSpawnLocation)
				{
					ValidLocations.push_back(obj.second.RandoObjectID);
				}
			}
			return ValidLocations;
		}

		//Get All Normal Locations in level regardless of accessibility
		static std::set<int>& AccessibleThings::GetNormalGlobalLocationsFromLevel(int LevelIndex)
		{
			return normalLevelObjectsMapAll[LevelIndex];
		}

		//Get All Locations in level regardless of accessibility
		static std::set<int>& AccessibleThings::GetGlobalLocationsFromLevel(int LevelIndex)
		{
			return levelObjectsMapAll[LevelIndex];
		}
		//GetAllValidLocations
		static std::set<int>& AccessibleThings::GetValidLocationsForItem(RandomizedObject& object)
		{
			bool foundLevelRestricted = LevelRestrictedIDs.find(object.ObjectID) != LevelRestrictedIDs.end();
			bool foundNoRando = NoRandomizationIDs.find(object.ObjectID) != NoRandomizationIDs.end();
			if (foundNoRando ||!object.Randomized)
			{
				return std::set<int>{ object.RandoObjectID };
			}
			if (object.ItemTag == "Note" || (foundLevelRestricted && !object.thisCanBeReward()))
			{
				return GetNormalGlobalLocationsFromLevel(object.LevelIndex);
			}
			else if (foundLevelRestricted && object.thisCanBeReward())
			{
				return GetGlobalLocationsFromLevel(object.LevelIndex);
			}
			else if (!object.thisCanBeReward())
			{
				return normalAll;
			}
			else
			{
				return allObjects;
			}
		}
		static std::set<int> AccessibleThings::GetValidLocationsForItem(CollectableId type, int levelIndex, AccessibleThings state, std::map<CollectableId, int>& counts)
		{
			
				std::set<int> setReturn;
				std::vector<int> id = { GetCollectibleFromCollectibleId(type).ObjectId };
				bool foundLevelRestricted = LevelRestrictedIDs.count(id[0]) != 0;
				bool foundNoRando = NoRandomizationIDs.count(id[0]) !=0;
				if (foundNoRando)
				{
					setReturn = std::set<int>{ };
				}
				else if (type == Collect_Note || (foundLevelRestricted && !RandomizedObject::CanBeReward(type)))
				{
					setReturn = GetNormalGlobalLocationsFromLevel(levelIndex);
				}
				else if (foundLevelRestricted && RandomizedObject::CanBeReward(type))
				{
					setReturn = GetGlobalLocationsFromLevel(levelIndex);
					if (id[0] == 0x1F5)
					{
						int count = 0;
						for (auto& item : setReturn)
						{
							if (objectsList[item].collectableId == type)
								count++;
						}
						if(count==0)
							setReturn = std::set<int>{ };
					}
				}
				else if (!RandomizedObject::CanBeReward(type))
				{
					setReturn = normalAll;
				}
				else
				{
					setReturn = allObjects;
				}
				for (auto& item : setReturn)
				{
					counts[objectsList[item].collectableId]++;
				}
				
				if (state.SetItems.size() == 0)
				{
					return setReturn;
				}
				else
				{
					for (auto& setItem : state.SetItems)
					{
						setReturn.erase(setItem.first);
					}
					return setReturn;
				}

		}
		static std::set<int>& AccessibleThings::GetValidLocationsForItem(CollectableId type,int levelIndex)
		{
			std::map <CollectableId, int> counts;
			return GetValidLocationsForItem(type, levelIndex, {},  counts);
		}

		static std::set<int> AccessibleThings::GetValidItemsForLocation(RandomizedObject& object)
		{
			bool foundLevelRestricted = LevelRestrictedIDs.count(object.ObjectID) != 0;
			bool foundNoRando = NoRandomizationIDs.count(object.ObjectID)!= 0;
			if (foundNoRando || !object.Randomized)
			{
				return std::set<int>{ object.RandoObjectID };
			}
			if (object.IsSpawnLocation||object.isVirtualObject())
			{
				std::set<int> temp;
				for (auto& tempObject : allSpawnableObjects)
				{
					bool foundNoRandoForItem = NoRandomizationIDs.count(objectsList[tempObject].ObjectID) != 0;
					bool foundLevelRestrictedItem = LevelRestrictedIDs.count(objectsList[tempObject].ObjectID) != 0;

					if(foundNoRandoForItem == false|| (foundLevelRestrictedItem && object.LevelIndex == objectsList[tempObject].LevelIndex)|| !foundLevelRestrictedItem)
						temp.insert(tempObject);
				}
				return temp;
			}
			else
			{
				std::set<int> temp;
				for (auto& tempObject : allObjects)
				{
					bool foundNoRandoForItem = NoRandomizationIDs.count(objectsList[tempObject].ObjectID) != 0;
					bool foundLevelRestrictedItem = LevelRestrictedIDs.count(objectsList[tempObject].ObjectID) != 0;

					if (foundNoRandoForItem == false || (foundLevelRestrictedItem && object.LevelIndex == objectsList[tempObject].LevelIndex) || !foundLevelRestrictedItem)
						temp.insert(tempObject);
				}
				return temp;
			}
		}

		static std::vector<int> AccessibleThings::GetValidLocationsForItemVector(RandomizedObject& object)
		{
			std::set<int> temp = GetValidLocationsForItem(object);
			return std::vector<int>(temp.begin(), temp.end());
		}

		static std::map<CollectableId, int> AccessibleThings::GetOwnedFromBarrier(AccessibleThings state, BarrierInterval barrier)
		{
			std::map<CollectableId, int> result;
			DebugPrintPriority("Barrier Max "+ std::to_string(barrier.maxCount)+" Barrier Min " + std::to_string(barrier.minCount), 1);

			for (auto& item : state.SetItems)
			{
				auto& location = barrier.itemLocations.find(item.first);
				if (location != barrier.itemLocations.end())
				{
					result[objectsList[item.second].collectableId] += objectsList[item.second].ItemAmount;
					DebugPrintPriority("Item at "+ objectsList[item.first].LocationName + " Is Item " + objectsList[item.second].LocationName + " Item Owned " + GetCollectibleFromCollectibleId(objectsList[item.second].collectableId).Name, 1);
				}
			}
			return result;
		}

		static int AccessibleThings::GetLevelRestriction(RandomizedObject& object)
		{
			auto foundLevelRestricted = std::find(LevelRestrictedIDs.begin(), LevelRestrictedIDs.end(), object.ObjectID);
			auto foundNoRando = std::find(NoRandomizationIDs.begin(), NoRandomizationIDs.end(), object.ObjectID);
			if (foundNoRando != NoRandomizationIDs.end() || !object.Randomized)
			{
				return object.LevelIndex;
			}
			if (object.ItemTag == "Note" || (foundLevelRestricted != LevelRestrictedIDs.end() && !object.thisCanBeReward()))
			{
				return object.LevelIndex;
			}
			else if (foundLevelRestricted != LevelRestrictedIDs.end() && object.thisCanBeReward())
			{
				return object.LevelIndex;
			}
			else if (!object.thisCanBeReward())
			{
				return -1;
			}
			else
			{
				return -1;
			}
		}

		//Get All unused locations in level regardless of access
		int AccessibleThings::GetUnusedNormalGlobalLocationsFromLevel(int LevelIndex)
		{
			DebugPrintPriority("GetUnusedNormalGlobalLocationsFromLevel for level: "+std::to_string(LevelIndex),2);

			int size = 0;
			int spawns=0, wrongLevel=0, noRando=0, used=0;
			for (const auto& obj : normalLevelObjectsMapAll[LevelIndex])
			{
				const auto& object = objectsList[obj];
				bool foundNoRando  = NoRandomizationIDs.count(object.ObjectID)!= 0;
				bool foundUsed = UsedLocations.count(object.RandoObjectID)!=0;

				if (!object.IsSpawnLocation &&
					object.LevelIndex == LevelIndex &&
					foundUsed == false &&
					foundNoRando == false &&
					object.Randomized)
				{
					size++;
				}
				if (object.IsSpawnLocation)
				{
					spawns++;
				}
				if (object.LevelIndex != LevelIndex)
				{
					wrongLevel++;
				}
				if (foundNoRando)
				{
					noRando++;
				}
			}
			DebugPrintPriority("Total: "+ std::to_string(normalLevelObjectsMapAll[LevelIndex].size()) +" in Level : " + std::to_string(LevelIndex),2);


			DebugPrintPriority("Spawner: "+std::to_string(spawns)+" Wrong Level: " + std::to_string(wrongLevel) +" No Rando: " + std::to_string(noRando) + " in Level: " + std::to_string(LevelIndex),2);
			return size;
		}

		void AccessibleThings::AddSetItem(int location, int source, bool setOwned = false)
		{
			SetItems.push_back(std::make_pair(location, source));
			UsedItems.insert(source);
			UsedLocations.insert(location);
			if(setOwned)
				OwnedLocations.insert(location);
		}

		/// <summary>
		/// Get a vector of the level indexes represented in this state
		/// </summary>
		std::vector<int> AccessibleThings::GetLevels()
		{
			std::vector<int> levels;
			for (int i = 0; i < ItemLocations.size(); i++)
			{
				auto it = std::find(levels.begin(), levels.end(), objectsList[ItemLocations[i]].LevelIndex);
				if (it == levels.end())
				{
					levels.push_back(objectsList[ItemLocations[i]].LevelIndex);
				}
			}
			return levels;
		}

	};
	static LogicHandler::AccessibleThings GetAllTotals(LogicGroup startingGroup, std::unordered_map<int, LogicGroup>& logicGroups, LogicHandler::AccessibleThings stateStart, const std::vector<RandomizedObject>& objects, std::set<int>& seenLogicGroups, std::set<int>& nextLogicGroups, std::set<int>& viableLogicGroups);
	static LogicHandler::AccessibleThings GetAccessibleRecursive(LogicGroup& startingGroup, std::unordered_map<int,LogicGroup>& logicGroups, LogicHandler::AccessibleThings& start, const std::vector<RandomizedObject>& objects, std::set<int>& seenLogicGroups, std::set<int>& nextLogicGroups, std::set<int>& viableLogicGroups);

	static bool FulfillsRequirements(LogicGroup* groupToUnlock, LogicHandler::AccessibleThings* state);
	static bool CanFulfillRequirements(LogicHandler::AccessibleThings* accessibleSpots, LogicGroup* groupToOpen, std::unordered_map<int, int>&);
	static bool ContainsRequiredKeys(const LogicHandler::AccessibleThings* state, const LogicGroup::RequirementSet* requirements);
	static int GetWorldAtOrder(const LogicHandler::AccessibleThings* state, int worldNumber);
	bool ContainsEntrance(const LogicHandler::AccessibleThings* state, int entranceID);
	static std::vector<int> GetWorldsInOrder(const LogicHandler::AccessibleThings* state);
	static void HandleSpecialTags(LogicGroup* group, const LogicHandler::AccessibleThings* state);
	LogicHandler::AccessibleThings TryRoute(LogicGroup startingGroup, std::unordered_map<int,LogicGroup>& logicGroups, std::set<int> lookedAtLogicGroups, std::set<int> nextLogicGroups, LogicHandler::AccessibleThings initialState, std::set<int> viableLogicGroups,const std::vector<RandomizedObject> objects, int depth,std::default_random_engine& rng);
	LogicHandler::AccessibleThings AssumedFill(LogicGroup startingGroup, std::vector<int>objectsToPlace, std::unordered_map<int, LogicGroup>& logicGroups, LogicHandler::AccessibleThings initialState, const std::vector<RandomizedObject> objects, std::default_random_engine& rng);

	typedef struct {
		std::set<int> unexploredGroups;
		std::set<int> exploredGroups;
	} ChainState;

	void FindLeadingGroups(LogicGroup startingGroup, std::unordered_map<int, LogicGroup>& logicGroups, LogicHandler::AccessibleThings& initialState, const std::vector<RandomizedObject> objects, std::default_random_engine& rng);

	std::set<std::set<int>> FindLogicChain(int, std::unordered_map<int, LogicGroup>&, std::map<int, std::list<ChainState>>&, std::map<int, std::set<std::set<int>>>&, std::set<int> visited);

	LogicHandler::AccessibleThings RandomFill(LogicGroup startingGroup, std::vector<int> objectsToPlace, std::unordered_map<int, LogicGroup>& logicGroups, LogicHandler::AccessibleThings initialState, const std::vector<RandomizedObject> objects, std::default_random_engine& rng);



	
};


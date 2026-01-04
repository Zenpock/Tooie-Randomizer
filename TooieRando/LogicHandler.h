#pragma once
#include "LogicGroup.h"
#include "MoveObject.h"
#include "Entrance.h"
#include <tuple> 
#include <random>
#include <algorithm>

#include <chrono>

#include<unordered_map>
#include <unordered_set>
#include <windows.h>
#include <sstream>
#include "OptionData.h"

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

	static std::vector<int> worldPrices;

	static bool debug;
	static bool saveLogging;
	static std::unordered_map<int, std::vector<int>> normalLevelObjectsMapAll; //List of all objects sorted int groups by level

	static std::unordered_map<int, std::vector<int>> LogicHandler::shuffleGroups;

	static std::unordered_map<int, int> LogicHandler::entranceAssociations;

	static bool objectsNotRandomized; //Whether the objects not randomized options is set
	
	static std::vector<int> NoRandomizationIDs;

	//Map of the Ability stored in an item and the associated RandoObjectID
	static std::unordered_map<int, int> LogicHandler::AbilityItems;

	static std::vector<std::string> WorldTags;
	static std::vector<int>  LogicHandler::notePrices;
	static std::vector<int>  LogicHandler::glowboPrices;
	static std::unordered_map<int, int>  LogicHandler::siloIndexStep;
	static std::unordered_map<int, std::string>  LogicHandler::WorldPrefixes;
	static std::unordered_map<int, int> LogicHandler::EntranceToWorld;
	static std::unordered_map<int, int> LogicHandler::EntranceInWorld;


	static void DebugPrint(const std::string& message)
	{
		static bool savedBefore = false;
		if(debug)
			OutputDebugStringA((message + "\n").c_str());
		if (saveLogging)
		{
			if (savedBefore == false)
			{
				remove("Logging.txt");
				savedBefore = true;
			}
			std::fstream myfile;
			myfile.open("Logging.txt", std::ios::app);
			std::string str = message + " \n";
			myfile << str;
		}
		
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
		std::vector<MoveObject> AbilityLocations; //Available Locations to place moves
		
		//RandoObjectID paired with the ability that is placed there
		std::vector<std::pair<int,int>> SetAbilities; 
		//Contains every RandoObjectID currently available
		std::vector<int> ItemLocations; //Available Locations to place Objects
		
		int depthToLeave=0; //The amount of steps back to take
		
		/// <summary>
		/// LocationId,SourceID
		/// </summary>
		std::vector<std::pair<int, int>> SetItems;

		/// <summary>
		/// Entry Warp, Exit Warp
		/// </summary>
		std::vector<std::pair<int, int>> SetWarps;

		//Vector of the different notes stored by level int being the level index
		std::unordered_map<int, NoteState> levelNotes;

		/// <summary>
		/// Collectable Name paired with the number of them
		/// </summary>
		std::vector<std::pair<std::string,int>> ContainedItems;

		std::vector<std::string> Keys;

		bool done = false;
		/// <summary>
		/// Add all of the items from the given group to this one
		/// </summary>
		/// <param name="things"></param>
		void AccessibleThings::Add(AccessibleThings& things)
		{
			checkedOldGroups = things.checkedOldGroups;
			for (int i = 0; i < things.AbilityLocations.size(); i++)
			{
				int ability = things.AbilityLocations[i].Ability;
				auto matchesAbilty = [ability](MoveObject move) {return move.Ability == ability; };
				auto it = std::find_if(AbilityLocations.begin(), AbilityLocations.end(), matchesAbilty);
				if (it == AbilityLocations.end())
					AbilityLocations.push_back(things.AbilityLocations[i]);
			}
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
				auto foundNoRando = std::find(NoRandomizationIDs.begin(), NoRandomizationIDs.end(), objectsList[things.ItemLocations[i]].ObjectID);
				if (!objectsList[things.ItemLocations[i]].Randomized || foundNoRando != NoRandomizationIDs.end()) //If the object is not randomized Set it to equal itself and continue
				{
					int id = things.ItemLocations[i];
					////OutputDebugString(("No Rando Item added in Level: " + std::to_string(objectsList[id].LevelIndex) + " Rando Object ID: " + std::to_string(objectsList[id].RandoObjectID) + "\n").c_str());
					SetItems.push_back(std::make_pair(id, id));
					continue;
				}
				auto it = std::find(ItemLocations.begin(), ItemLocations.end(), things.ItemLocations[i]);
				if (it == ItemLocations.end())
					ItemLocations.push_back(things.ItemLocations[i]);
			}

			for (int i = 0; i < things.SetItems.size(); i++)
			{
				int locationObjectID = std::get<0>(things.SetItems[i]);
				auto matchesObject = [locationObjectID](std::pair<int,int> object) {return std::get<0>(object) == locationObjectID; };
				auto it = std::find_if(SetItems.begin(), SetItems.end(), matchesObject);
				if (it == SetItems.end())
				{
					SetItems.push_back(things.SetItems[i]);
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

			for (int i = 0; i < things.Keys.size(); i++)
			{
				std::string key = things.Keys[i];
				auto matchesKey = [key](std::string listkey) {return key == listkey; };
				auto it = std::find_if(Keys.begin(), Keys.end(), matchesKey);
				if (it == Keys.end())
					Keys.push_back(things.Keys[i]);
			}
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
		/// <param name="moves">The list of all of the moves that exist so additional data can be gathered from them</param>
		void AccessibleThings::Add(LogicGroup& group, const std::vector<RandomizedObject>& objects, const std::vector<MoveObject>& moves)
		{
			//Loop through all objects and add the locations from the group if they can be found in the main list
			int foundObjects = 0;
			for (int j = 0; j < objects.size(); j++)
			{
				for (int i = 0; i < group.objectIDsInGroup.size(); i++)
				{
					if (objects[j].RandoObjectID == group.objectIDsInGroup[i])
					{
						foundObjects++;
						if (!objects[j].Randomized)
						{
							SetItems.push_back(std::make_pair(objects[j].RandoObjectID, objects[j].RandoObjectID));
						}
						else
						{
							ItemLocations.push_back(objects[j].RandoObjectID);
						}
						break;
					}
				}
				if (foundObjects >= group.objectIDsInGroup.size())
					break;
			}

			if (!group.key.empty())
			{
				Keys.push_back(group.key);
				checkedOldGroups = false;

			}
		}

		/// <summary>
		/// Remove items and ability locations based on the requirements in the given group
		/// </summary>
		/// <param name="things"></param>
		void AccessibleThings::RemoveRequirements(LogicGroup::RequirementSet* requirement)
		{
			for (int i = 0; i < requirement->RequiredAbilities.size(); i++)
			{
				AbilityLocations.erase(AbilityLocations.begin());
			}
		}

		int AccessibleThings::GetCollectableCount(std::string name)
		{
			auto matchesName = [name](std::pair<std::string,int> Collectable) {return std::get<0>(Collectable) == name; };
			auto it = std::find_if(ContainedItems.begin(), ContainedItems.end(), matchesName);
			if (it != ContainedItems.end())
				return (*it).second;
			else
				return 0;
		}

		/// <summary>
		/// Find the required moves in the requirement set and assign the abilities to available locations
		/// </summary>
		/// <param name="things"></param>
		void AccessibleThings::AddAbilities(const LogicGroup::RequirementSet& requirement, const std::vector<RandomizedObject>& objects, std::default_random_engine& rng)
		{
			std::vector<int> outVector;
			outVector = ItemLocations;
			//Shuffle all of the remaining item locations
			std::shuffle(outVector.begin(), outVector.end(), rng);
			int outVectorIndex = 0;
			checkedOldGroups = false;

			for (int i = 0; i < requirement.RequiredAbilities.size(); i++)
			{
				
				int requiredAbility = requirement.RequiredAbilities[i];

				//Check if we already have this ability
				auto matchesSetAbilty = [requiredAbility](std::pair<int, int> move) {return std::get<1>(move) == requiredAbility; };
				auto abilityAlreadyAcquired = std::find_if(SetAbilities.begin(), SetAbilities.end(), matchesSetAbilty);
				if (abilityAlreadyAcquired != SetAbilities.end()) //If we already have the ability try the others
					continue;

				//Ensure that the required ability exists
				auto matchesAbilty = [requiredAbility](RandomizedObject object) {return object.Ability == requiredAbility; };
				
				//Look through all of the available objects to find the ability
				auto foundAbility = std::find_if(objects.begin(), objects.end(), matchesAbilty);
				if (foundAbility != objects.end()) //If the move was found try and add it
				{
					//Add the source ability item and the target to the setitems
					SetItems.push_back(std::make_pair(outVector[0], (*foundAbility).RandoObjectID));
					

					//Remove the Target Location from the overall list

					auto foundLocation = std::find(ItemLocations.begin(), ItemLocations.end(), outVector[0]);
					if (foundLocation != ItemLocations.end())
					{
						ItemLocations.erase(foundLocation);
						DebugPrint("Added Ability " + (*foundAbility).MoveName + " at " + std::to_string(outVector[0]));
					}

					//Remove the Target Location from the shuffled list
					outVector.erase(outVector.begin());
					
				}
			}
		}

		/// <summary>
		/// Find the required items in the requirement set and assign the items to available locations
		/// </summary>
		/// <param name="things"></param>
		void AccessibleThings::AddItems(const LogicGroup::RequirementSet& requirement,std::default_random_engine& rng)
		{
			checkedOldGroups = false;

			std::vector<int> outVector;
			outVector = ItemLocations; //Shuffle the locations
			std::shuffle(outVector.begin(), outVector.end(), rng);
			std::unordered_map<int, std::vector<int>> normalLevelObjectsMap;
			//sort the available normal locations into levels
			for (int shuffledIdx = 0; shuffledIdx < outVector.size(); shuffledIdx++)
			{
				if (!objectsList[outVector[shuffledIdx]].IsSpawnLocation)
					normalLevelObjectsMap[objectsList[outVector[shuffledIdx]].LevelIndex].push_back(outVector[shuffledIdx]);
			}

			//Add notes first
			for (int i = 0; i < requirement.RequiredItems.size(); i++) 
			{
				if (requirement.RequiredItems[i] == "Note")
				{
					int collectableAmount = GetCollectableCount(requirement.RequiredItems[i]);
					std::vector<int> levels = GetLevels(); //Get the accessible levels
					//Loop through all of the accessible levels to find places to put notes
					for (int levelIndex = 0; levelIndex < levels.size(); levelIndex++)
					{
						int levelInt = levels[levelIndex];
						bool trebleUsed = levelNotes[levelInt].TrebleUsed;
						int usedNotes = levelNotes[levelInt].usedNotes; //Number of normal note nests that have been used in this level
						int availableLocations = normalLevelObjectsMap[levelInt].size();
						if (trebleUsed == false && availableLocations > 0)
						{
							collectableAmount += 20; //Always try a treble
							int sourceObjectID = FindObjectOfType(requirement.RequiredItems[i], 20, levelInt);
							if (sourceObjectID != -1)
							{
								SetItems.push_back(std::make_pair((normalLevelObjectsMap[levelInt][0]), sourceObjectID));
								int objectID = normalLevelObjectsMap[levelInt][0];

								normalLevelObjectsMap[levelInt].erase(normalLevelObjectsMap[levelInt].begin());

								auto foundInShuffled = std::find(outVector.begin(), outVector.end(), objectID);
								outVector.erase(foundInShuffled);

								auto foundLocation = std::find(ItemLocations.begin(), ItemLocations.end(), objectID);
								ItemLocations.erase(foundLocation);
								availableLocations--;
								levelNotes[levelInt].TrebleUsed = true;
								DebugPrint("Added Treble " + objectsList[sourceObjectID].ItemTag + " at " + std::to_string(objectID));
							}
							else
							{
								::MessageBox(NULL,"Could not Find Valid Treble","Error",NULL);
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

									////OutputDebugString(("Set Note In Level: " + std::to_string(objectsList[normalLevelObjectsMap[levelInt][0]].LevelIndex) + " Rando Object ID: " + std::to_string(normalLevelObjectsMap[levelInt][0]) + "\n").c_str());
									SetItems.push_back(std::make_pair((normalLevelObjectsMap[levelInt][0]), sourceObjectID));
									int objectID = normalLevelObjectsMap[levelInt][0];

									normalLevelObjectsMap[levelInt].erase(normalLevelObjectsMap[levelInt].begin());

									auto foundInShuffled = std::find(outVector.begin(), outVector.end(), objectID);
									outVector.erase(foundInShuffled);

									auto foundLocation = std::find(ItemLocations.begin(), ItemLocations.end(), objectID);
									ItemLocations.erase(foundLocation);
									availableLocations--;
									DebugPrint("Added Note " + objectsList[sourceObjectID].ItemTag + " at " + std::to_string(objectID));
								}
								usedNotes++;
							}
							levelNotes[levelInt].usedNotes = usedNotes;
						}
					}
				}
			}

			//remove locations allocated for notes so they aren't used by non notes
			std::vector<int> levels = GetLevels(); //Get the accessible levels
			for (int levelIndex = 0; levelIndex < levels.size(); levelIndex++)
			{

				int levelInt = levels[levelIndex];
				int usedSlots = 0;
				for (const auto& item : SetItems)
				{
					if(objectsList[std::get<0>(item)].LevelIndex == levelInt && !objectsList[std::get<0>(item)].IsSpawnLocation)
						usedSlots++;
				}
				int unusedSlots = GetUnusedNormalGlobalLocationsFromLevel(levelInt);

				int slotsUnallocated = 17;
				slotsUnallocated -= levelNotes[levelInt].usedNotes;
				if (levelNotes[levelInt].TrebleUsed)
					slotsUnallocated--;
				int slotsToRemove = 0;
				int availableNormalSlots = normalLevelObjectsMap[levelInt].size();
					slotsToRemove = availableNormalSlots - (unusedSlots - slotsUnallocated);
					if (slotsToRemove > 17)
					{
						int numNormal = 0;
						for (int shuffledIdx = 0; shuffledIdx < outVector.size(); shuffledIdx++) //sort the available normal locations into levels
						{
							if (!objectsList[outVector[shuffledIdx]].IsSpawnLocation && objectsList[outVector[shuffledIdx]].LevelIndex == levelInt)
								numNormal++;
						}
					}

					if (unusedSlots < slotsUnallocated)
					{
						::MessageBox(NULL, "Logic Error Occured Ran out of allocated space for notes (please try a different seed)", "Error", NULL);

						//OutputDebugString(("We have run out of slots to allocate Unused Slots: " + std::to_string(unusedSlots) + " Note Slots Unallocated: " + std::to_string(slotsUnallocated)+"\n").c_str());
					}


				if (slotsToRemove < 0)
					slotsToRemove = 0;
				for (int i = 0; i < slotsToRemove; i++)
				{
					int foundId = -1;
					if(normalLevelObjectsMap[levelInt].size()>0)
						foundId = normalLevelObjectsMap[levelInt][0];
					auto foundNormalMap = std::find(normalLevelObjectsMap[levelInt].begin(), normalLevelObjectsMap[levelInt].end(), foundId);
					auto foundLocation = std::find(outVector.begin(), outVector.end(), foundId);
					if (foundLocation != outVector.end())
					{
						normalLevelObjectsMap[levelInt].erase(foundNormalMap);
						outVector.erase(foundLocation);
					}
				}

			}

			//Setup the remaining normal objects first
			for (int i = 0; i < requirement.RequiredItems.size(); i++) 
			{
				if (requirement.RequiredItems[i] != "Note")
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
								SetItems.push_back(std::make_pair(objectsList[outVector[outVectorIndex]].RandoObjectID, sourceObjectID));

								int objectID = objectsList[outVector[0]].RandoObjectID;
								outVector.erase(outVector.begin());

								auto foundLocation = std::find(ItemLocations.begin(), ItemLocations.end(), objectID);
								if (foundLocation != ItemLocations.end())
								{
									ItemLocations.erase(foundLocation);
									DebugPrint("Added Item " + objectsList[sourceObjectID].ItemTag + " at " + std::to_string(objectsList[outVector[outVectorIndex]].RandoObjectID));
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
				if (requirement.RequiredItems[i] != "Note")
				{
					for (int j = GetCollectableCount(requirement.RequiredItems[i]); j < requirement.RequiredItemsCount[i]; j++)
					{
						int sourceObjectID = FindObjectOfType(requirement.RequiredItems[i], 1);
						if (sourceObjectID != -1 && objectsList[sourceObjectID].CanBeReward(requirement.RequiredItems[i]))
						{
							if (outVector.size() > 0)
							{
								SetItems.push_back(std::make_pair(objectsList[outVector[0]].RandoObjectID, sourceObjectID));

								int objectID = objectsList[outVector[0]].RandoObjectID;
								outVector.erase(outVector.begin());

								auto foundLocation = std::find(ItemLocations.begin(), ItemLocations.end(), objectID);
								if (foundLocation != ItemLocations.end())
								{
									ItemLocations.erase(foundLocation);
									DebugPrint("Added Item " + objectsList[sourceObjectID].ItemTag + " at " + std::to_string(objectID));
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
				if (abilityAlreadyAcquired != SetAbilities.end()) //If we already have the ability try the others
					continue;

				//Look through all of the available objects to find the ability
				if (AbilityItems.find(requiredAbility)!=AbilityItems.end()) //If the move was found try and add it
				{
					//Add the source ability item and the target to the setitems
					SetItems.push_back(std::make_pair(outVector[0], AbilityItems[requiredAbility]));

					//Remove the Target Location from the overall list
					auto foundLocation = std::find(ItemLocations.begin(), ItemLocations.end(), outVector[0]);
					if (foundLocation != ItemLocations.end())
					{
						ItemLocations.erase(foundLocation);
						DebugPrint("Added Ability " + objectsList[AbilityItems[requiredAbility]].MoveName + " at " + std::to_string(outVector[0]));
					}

					//Remove the Target Location from the shuffled list
					outVector.erase(outVector.begin());

				}
			}
		}

		int AccessibleThings::FindObjectOfType(std::string objectName)
		{
			return FindObjectOfType(objectName, 1);
		}

		int AccessibleThings::FindObjectOfType(std::string objectName, int amount)
		{
			std::unordered_set<int> usedObjectIDs;
			usedObjectIDs.reserve(SetItems.size());
			for (const auto& item : SetItems)
			{
				usedObjectIDs.insert(objectsList[std::get<1>(item)].RandoObjectID);
			}

			for (const auto& obj : objectsList)
			{
				int id = obj.first;
				RandomizedObject object = obj.second;
				if (usedObjectIDs.count(object.RandoObjectID) == 0 && object.ItemTag == objectName && object.ItemAmount == amount && object.Randomized)
				{
					auto foundNoRando = std::find(NoRandomizationIDs.begin(), NoRandomizationIDs.end(), object.ObjectID);
					if (foundNoRando == NoRandomizationIDs.end())
						return object.RandoObjectID;  // Return first match immediately
				}
			}

			return -1;  // No match found
		}

		int AccessibleThings::FindObjectOfType(std::string objectName, int amount, int levelIndex)
		{
			std::unordered_set<int> usedObjectIDs;
			usedObjectIDs.reserve(SetItems.size());
			for (const auto& item : SetItems)
			{
				usedObjectIDs.insert(objectsList[std::get<1>(item)].RandoObjectID);
			}

			for (const auto& obj : objectsList)
			{
				int id = obj.first;
				RandomizedObject object = obj.second;
				if (usedObjectIDs.count(object.RandoObjectID) == 0 && object.ItemTag == objectName && object.ItemAmount == amount && object.Randomized && object.LevelIndex == levelIndex)
				{
					auto foundNoRando = std::find(NoRandomizationIDs.begin(), NoRandomizationIDs.end(), object.ObjectID);
					if(foundNoRando == NoRandomizationIDs.end())
						return object.RandoObjectID;  // Return first match immediately
				}
			}

			return -1;  // No match found
		}

		void AccessibleThings::UpdateCollectables()
		{
			ContainedItems.clear();
			levelNotes.clear();
			for (int i = 0; i < SetItems.size(); i++)
			{
				RandomizedObject object = objectsList[std::get<1>(SetItems[i])];
				AddCollectable(object.ItemTag, object.ItemAmount);
				if(object.ItemTag == "Note")
				{ 
					if (object.ItemAmount == 20)
						levelNotes[object.LevelIndex].TrebleUsed = true;
					else
						levelNotes[object.LevelIndex].usedNotes++;;
				}
			}
		}

		void AccessibleThings::UpdateMoves()
		{
			SetAbilities.clear();
			for (int i = 0; i < SetItems.size(); i++)
			{
				RandomizedObject sourceObject = objectsList[std::get<1>(SetItems[i])];
				if (sourceObject.ItemTag == "Move Item")
				{
					SetAbilities.push_back(std::make_pair(objectsList[std::get<0>(SetItems[i])].RandoObjectID,sourceObject.Ability));
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
			int normalLocationsCount = GetNormalLocations();
			int neededSpots = 0;//Number of locations required to fulfill the requirement
			int neededNormalSpots = 0; //Number of nonspawning locations

			DebugPrint(("Checking requirements for set "+ requirement->SetName));

			DebugPrint(("Number of Available Normal Locations " + std::to_string(normalLocationsCount) + " for set " + requirement->SetName));

			int missingAbilities = 0;
			for (int i = 0; i < requirement->RequiredAbilities.size(); i++)
			{
				int ability = requirement->RequiredAbilities[i];
				auto matchesAbility = [ability](std::pair<int,int> move) {return std::get<1>(move) == ability; };
				auto it = std::find_if(SetAbilities.begin(), SetAbilities.end(), matchesAbility);
				if (it == SetAbilities.end()) //If the ability is not already found in the set abilities
				{
					missingAbilities++;
				}
			}

			DebugPrint(("Missing " + std::to_string(missingAbilities)+ " Abilities for set " + requirement->SetName));

			//Add the amount of spots needed for the missing abilities
			neededSpots += missingAbilities;
			
			std::vector<int> levels = GetLevels(); //Get the accessible levels
			int totalNormalObjects = 0; //Sum of the total number of normal objects in the accessible levels
			std::unordered_map<int, NoteState> tempLevelNotes;

			for (int levelIndex = 0; levelIndex < levels.size(); levelIndex++)
			{
				int levelInt = levels[levelIndex];
				totalNormalObjects += normalLevelObjectsMapAll[levelInt].size();
				tempLevelNotes[levelInt].usedNotes = levelNotes[levelInt].usedNotes;
				tempLevelNotes[levelInt].TrebleUsed = levelNotes[levelInt].TrebleUsed;
				int unusedNormalLocationsInLevel = GetUnusedNormalGlobalLocationsFromLevel(levelInt);
				DebugPrint(("Unused Normal Locations in Level: " + std::to_string(unusedNormalLocationsInLevel) + " Used Notes: " + std::to_string(levelNotes[levelInt].usedNotes) + " Treble Used: " + std::to_string(levelNotes[levelInt].TrebleUsed) + " In Level: " + std::to_string(levelInt)));
			}

			DebugPrint(("Number of Total Available Normal Locations " + std::to_string(normalLocationsCount) + " for set" + requirement->SetName));

			int noteSpots = 0; //How Many Spots are notes using

			for (int j = 0; j < requirement->RequiredItems.size(); j++)
			{
				std::string collectableName = requirement->RequiredItems[j];
				
				int collectableAmount = GetCollectableCount(collectableName);
				if (requirement->RequiredItems[j] == "Note")
				{
					for (int levelIndex = 0; levelIndex < levels.size(); levelIndex++)
					{

						int levelInt = levels[levelIndex];

						int usedNormalSlots = 0;
						for (const auto& item : SetItems)
						{
							if (objectsList[std::get<1>(item)].LevelIndex == levelInt && !objectsList[std::get<1>(item)].IsSpawnLocation)
								usedNormalSlots++;
						}

						DebugPrint(("Used Normal Slots in Level " + std::to_string(usedNormalSlots) + " Total Normal Slots in Level " +std::to_string(levelInt)+": " + std::to_string(normalLevelObjectsMapAll[levelInt].size())));

						//The amount of slots that are unused regardless of accessibility
						int unusedSlots = usedNormalSlots-normalLevelObjectsMapAll[levelInt].size(); 
						
						//Indicates a treble was added this check to ensure we do not try and allocate more than available
						int trebleAdded = 0; 
						
						int normalSize = GetNormalLocationsFromMap(levelInt);
						//If there's available space for a treble use it
						if (!levelNotes[levelInt].TrebleUsed && normalSize > 0) 
						{
							collectableAmount += 20;
							tempLevelNotes[levelInt].TrebleUsed = true;
							trebleAdded = 1;
						}
						while (collectableAmount < requirement->RequiredItemsCount[j] &&
							tempLevelNotes[levelInt].usedNotes < 16 && 
							tempLevelNotes[levelInt].usedNotes + trebleAdded < normalSize)
						{
							collectableAmount += 5;
							tempLevelNotes[levelInt].usedNotes++;
						}
						DebugPrint("Level: " + std::to_string(levelInt) + " Temp Used Notes " + std::to_string(tempLevelNotes[levelInt].usedNotes));
						DebugPrint("Level: " + std::to_string(levelInt) + " Available Normal Locations " + std::to_string(normalSize));
						DebugPrint("Needed Notes: " + std::to_string(requirement->RequiredItemsCount[j]) + " Collected Notes: " + std::to_string(GetCollectableCount(collectableName)));
						DebugPrint(" Possible Amount " + std::to_string(collectableAmount));

						if (collectableAmount >= requirement->RequiredItemsCount[j])
						{
							DebugPrint("Level: " + std::to_string(levelInt) + " Temp Used Notes " + std::to_string(tempLevelNotes[levelInt].usedNotes));
							break;
						}
											}
					if (collectableAmount < requirement->RequiredItemsCount[j]) //If we cannot meet the quota we cannot meet the requirements
					{
						DebugPrint("Could not meet note quota Required Amount: " + std::to_string(requirement->RequiredItemsCount[j]) + " Possible Amount " + std::to_string(collectableAmount));
						return false;

					}

				}
				//Check if it isn't a reward object
				else if (!RandomizedObject::CanBeReward(requirement->RequiredItems[j]))
				{
					//Make sure that we do not allocate more spots for items we already can afford
					if (collectableAmount < requirement->RequiredItemsCount[j])
					{
						neededNormalSpots += (requirement->RequiredItemsCount[j] - collectableAmount);
					}
				}
				else
				{
					//Make sure that we do not allocate more spots for items we already can afford
					if (collectableAmount < requirement->RequiredItemsCount[j])
						neededSpots += (requirement->RequiredItemsCount[j] - collectableAmount);
				}
			}

			int availableNormalSpots = normalLocationsCount; //Get the amount of normal spots after allocating space for notes
			////OutputDebugString(("Normal Location Amount Pre Allocation " + std::to_string(normalLocationsCount) + "\n").c_str());
			int notesToAllocate = 0; //The number of slots that have yet to be used for notes
			
			
			for (int levelIndex = 0; levelIndex < levels.size(); levelIndex++)
			{
				int levelInt = levels[levelIndex];

				int newNotes = tempLevelNotes[levelInt].usedNotes - levelNotes[levelInt].usedNotes;

				int usedNoteSlots = levelNotes[levelInt].usedNotes;
				if (tempLevelNotes[levelInt].TrebleUsed == true && levelNotes[levelInt].TrebleUsed == false)
					newNotes++;
				if (levelNotes[levelInt].TrebleUsed)
					usedNoteSlots++;
				availableNormalSpots -= newNotes;
				int unusedNotes = 17 - (usedNoteSlots);
				int unusedNormalLocations = unusedNormalGlobal[levelInt];
				int unusedAccessibleNormalLocations = GetNormalLocationsFromMap(levelInt);
				int slotsToRemove = unusedAccessibleNormalLocations - (unusedNormalLocations - unusedNotes);
				if(slotsToRemove > 0)
				notesToAllocate += slotsToRemove;
			}

			//Try and determine if there will still be room to place leftover notes in appropriate levels
			
			if (neededNormalSpots > availableNormalSpots)
			{
				DebugPrint("Not enough slots available after reserving for notes.");
				return false;
			}

			if (neededNormalSpots > normalLocationsCount)
			{
				DebugPrint("There were not enough available non spawning locations Available normal Locations: " + std::to_string(normalLocationsCount) + " needed normal spots Count " + std::to_string(neededNormalSpots));
				return false;
			}

			int availableSpotTotal = availableNormalSpots - neededNormalSpots - notesToAllocate;
			////OutputDebugString(("Needed Spots " + std::to_string(neededSpots) + " Available spot total " + std::to_string(availableSpotTotal) +" Total available location count without removal "+std::to_string(ItemLocations.size()) + "\n").c_str());

			if (neededSpots > availableSpotTotal)
			{
				DebugPrint("There were not enough available item locations Available Locations: " + std::to_string(availableSpotTotal) + " needed spots Count " + std::to_string(neededSpots));
				return false;
			}

			if (!ContainsRequiredKeys(this, requirement))
			{
				DebugPrint("Could not fulfill Key Requirement");
				return false;
			}
			DebugPrint("Requirements Fulfillable");
			return true;
		}

		void AccessibleThings::AddCollectable(std::string collectableName, int value)
		{
			auto it = std::find_if(ContainedItems.begin(), ContainedItems.end(), [collectableName](std::tuple<std::string, int> Item) {return std::get<0>(Item) == collectableName; });
			if (it == ContainedItems.end())
			{
				ContainedItems.push_back(std::make_pair(collectableName,value));
			}
			else //If there is already some amount of the object in the list add it
			{
				std::get<1>(ContainedItems[it - ContainedItems.begin()]) += value;
			}
		}

		int AccessibleThings::GetLocationsFromMap(int LevelIndex)
		{
			int size = 0;
			for (int i = 0; i < ItemLocations.size(); i++)
			{
				if (objectsList[ItemLocations[i]].LevelIndex == LevelIndex)
				{
					size++;
				}
			}
			return size;
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
			}
			return size;
		}

		//Get Locations that do not spawn objects from the accessible state
		int AccessibleThings::GetNormalLocations()
		{
			int size = 0;
			for (int i = 0; i < ItemLocations.size(); i++)
			{
				if (objectsList[ItemLocations[i]].IsSpawnLocation == false)
				{
					size++;
				}
			}
			return size;
		}

		//Get All Locations regardless of accessibility
		static int AccessibleThings::GetNormalGlobalLocations()
		{
			int size = 0;
			for (const auto& obj : objectsList)
			{
				if (!obj.second.IsSpawnLocation)
				{
					size++;
				}
			}
			return size;
		}

		//Get All Locations in level regardless of accessibility
		static std::vector<int> AccessibleThings::GetNormalGlobalLocationsFromLevel(int LevelIndex)
		{
			return normalLevelObjectsMapAll[LevelIndex];
		}

		//Get All unused locations in level regardless of access
		int AccessibleThings::GetUnusedNormalGlobalLocationsFromLevel(int LevelIndex)
		{
			int size = 0;
			//objects.reserve(objectsList.size());
			std::vector<int> usedObjectIDs;
			usedObjectIDs.reserve(SetItems.size());
			for (const auto& item : SetItems)
			{
				usedObjectIDs.push_back(objectsList[item.first].RandoObjectID);
			}
			int spawns=0, wrongLevel=0, noRando=0, used=0;
			for (const auto& obj : normalLevelObjectsMapAll[LevelIndex])
			{
				auto foundNoRando  = std::find(NoRandomizationIDs.begin(), NoRandomizationIDs.end(), objectsList[obj].ObjectID);
				auto foundUsed = std::find(usedObjectIDs.begin(), usedObjectIDs.end(), objectsList[obj].RandoObjectID);

				if (!objectsList[obj].IsSpawnLocation &&
					objectsList[obj].LevelIndex == LevelIndex &&
					foundUsed == usedObjectIDs.end() &&
					foundNoRando == NoRandomizationIDs.end() &&
					objectsList[obj].Randomized)
				{
					size++;
				}
				if (objectsList[obj].IsSpawnLocation)
				{
					spawns++;
				}
				if (objectsList[obj].LevelIndex != LevelIndex)
				{
					wrongLevel++;
				}
				if (foundNoRando != NoRandomizationIDs.end())
				{
					noRando++;
				}
				if (foundUsed != usedObjectIDs.end())
				{
					used++;
				}
			}
			DebugPrint("Total: "+ std::to_string(normalLevelObjectsMapAll[LevelIndex].size()) +" in Level : " + std::to_string(LevelIndex));


			DebugPrint("Spawner: "+std::to_string(spawns)+" Wrong Level: " + std::to_string(wrongLevel) +" No Rando: " + std::to_string(noRando) + " Already Used: " + std::to_string(used)+" in Level: " + std::to_string(LevelIndex));
			return size;
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
	static LogicHandler::AccessibleThings GetAllTotals(LogicGroup startingGroup, std::unordered_map<int, LogicGroup>& logicGroups, LogicHandler::AccessibleThings stateStart, const std::vector<RandomizedObject>& objects, const std::vector<MoveObject>& moves, std::vector<int>& seenLogicGroups, std::vector<int>& nextLogicGroups, std::vector<int>& viableLogicGroups);
	static LogicHandler::AccessibleThings GetAccessibleRecursive(LogicGroup& startingGroup, std::unordered_map<int,LogicGroup>& logicGroups, LogicHandler::AccessibleThings& start, const std::vector<RandomizedObject>& objects, const std::vector<MoveObject>& moves, std::vector<int>& seenLogicGroups, std::vector<int>& nextLogicGroups, std::vector<int>& viableLogicGroups);

	static bool FulfillsRequirements(LogicGroup* groupToUnlock, LogicHandler::AccessibleThings* state);
	static bool CanFulfillRequirements(LogicHandler::AccessibleThings* accessibleSpots, LogicGroup* groupToOpen, std::unordered_map<int, int>&);
	static bool ContainsRequiredKeys(const LogicHandler::AccessibleThings* state, const LogicGroup::RequirementSet* requirements);
	static int GetWorldAtOrder(const LogicHandler::AccessibleThings* state, int worldNumber);
	static std::vector<int> GetWorldsInOrder(const LogicHandler::AccessibleThings* state);
	static void HandleSpecialTags(LogicGroup* group, const LogicHandler::AccessibleThings* state);
	LogicHandler::AccessibleThings TryRoute(LogicGroup startingGroup, std::unordered_map<int,LogicGroup>& logicGroups, std::vector<int> lookedAtLogicGroups, std::vector<int> nextLogicGroups, LogicHandler::AccessibleThings initialState, std::vector<int> viableLogicGroups,const std::vector<RandomizedObject> objects, const std::vector<MoveObject> moves, int depth,std::default_random_engine& rng);
};


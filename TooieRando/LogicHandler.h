#pragma once
#include "LogicGroup.h"
#include "MoveObject.h"
#include <tuple> 
#include <random>
#include <algorithm>

#include <chrono>

#include<unordered_map>
#include <unordered_set>
#include <windows.h>
#include <sstream>
class LogicHandler
{
public:
	static int seed; //The seed used for randomization
	static std::unordered_map<int,RandomizedObject> objectsList; //List of All Objects
	static bool debug;
	static std::unordered_map<int, std::vector<int>> normalLevelObjectsMapAll; //List of all objects sorted int groups by level

	static bool objectsNotRandomized; //Whether the objects not randomized options is set
	
	static std::vector<int> NoRandomizationIDs;

	static void DebugPrint(const std::string& message)
	{
		if(debug)
		OutputDebugStringA((message + "\n").c_str());
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
		std::vector<MoveObject> AbilityLocations; //Available Locations to place moves
		std::vector<std::pair<int,MoveObject>> SetAbilities; //Location Move ID paired with the move placed at that location
		std::vector<int> ItemLocations; //Available Locations to place Objects
		
		int depthToLeave=0; //The amount of steps back to take
		
		/// <summary>
		/// LocationId,SourceID
		/// </summary>
		std::vector<std::pair<int, int>> SetItems;

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
		void AccessibleThings::Add(AccessibleThings things)
		{
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
				int ability = std::get<1>(things.SetAbilities[i]).Ability;
				auto matchesAbility = [ability](std::pair<int,MoveObject> move) {return (std::get<1>(move)).Ability == ability; };
				auto it = std::find_if(SetAbilities.begin(), SetAbilities.end(), matchesAbility);
				if(it == SetAbilities.end())
					SetAbilities.push_back(things.SetAbilities[i]);
			}

			for (int i = 0; i < things.ItemLocations.size(); i++)
			{
				auto foundNoRando = std::find(NoRandomizationIDs.begin(), NoRandomizationIDs.end(), objectsList[things.ItemLocations[i]].objectID);
				if (!objectsList[things.ItemLocations[i]].randomized || foundNoRando != NoRandomizationIDs.end()) //If the object is not randomized Set it to equal itself and continue
				{
					int id = things.ItemLocations[i];
					//OutputDebugString(("No Rando Item added in Level: " + std::to_string(objectsList[id].LevelIndex) + " Rando Object ID: " + std::to_string(objectsList[id].RandoObjectID) + "\n").c_str());
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
			for (int i = 0; i < things.Keys.size(); i++)
			{
				std::string key = things.Keys[i];
				auto matchesKey = [key](std::string listkey) {return key == listkey; };
				auto it = std::find_if(Keys.begin(), Keys.end(), matchesKey);
				if (it == Keys.end())
					Keys.push_back(things.Keys[i]);
			}
			UpdateCollectables();

		}
		/// <summary>
		/// Add all of the items from the given group to this one
		/// </summary>
		/// <param name="group"></param>
		/// <param name="objects">The list of all of the objects that exist so additional data can be gathered from them</param>
		/// <param name="moves">The list of all of the moves that exist so additional data can be gathered from them</param>
		void AccessibleThings::Add(LogicGroup group,std::vector<RandomizedObject>& objects, std::vector<MoveObject>& moves)
		{
			for (int i = 0; i < group.objectIDsInGroup.size(); i++)
			{
				for (int j = 0; j < objects.size(); j++)
				{
					if (objects[j].RandoObjectID == group.objectIDsInGroup[i])
					{
						ItemLocations.push_back(objects[j].RandoObjectID);
					}
				}
			}
			for (int j = 0; j < moves.size(); j++)
			{
				if (group.containedMove == moves[j].MoveID)
					AbilityLocations.push_back(moves[j]);
			}
			if(!group.key.empty())
			Keys.push_back(group.key);
		}

		/// <summary>
		/// Remove items and ability locations based on the requirements in the given group
		/// </summary>
		/// <param name="things"></param>
		void AccessibleThings::RemoveRequirements(LogicGroup::RequirementSet requirement)
		{
			for (int i = 0; i < requirement.RequiredAbilities.size(); i++)
			{
				AbilityLocations.erase(AbilityLocations.begin());
			}
			/*for (int i = 0; i < things.Items.size(); i++)
			{
				Items.push_back(things.Items[i]);
			}
			for (int i = 0; i < things.ContainedItems.size(); i++)
			{
				std::string name = std::get<0>(things.ContainedItems[i]);
				AddCollectable(name, std::get<1>(things.ContainedItems[i]));
			}*/
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
		void AccessibleThings::AddAbilities(LogicGroup::RequirementSet requirement, std::vector<MoveObject>& moves)
		{
			std::vector<MoveObject> outVector;
			outVector = AbilityLocations;
			std::shuffle(outVector.begin(), outVector.end(), std::default_random_engine(seed));
			
			int OutVectorIndex = 0;
			
			for (int i = 0; i < requirement.RequiredAbilities.size(); i++)
			{
				int ability = requirement.RequiredAbilities[i];
				auto matchesAbilty = [ability](MoveObject move) {return move.Ability == ability; };
				auto matchesSetAbilty = [ability](std::pair<int, MoveObject> move) {return std::get<1>(move).Ability == ability; };
				auto abilityAlreadyAcquired = std::find_if(SetAbilities.begin(), SetAbilities.end(), matchesSetAbilty);
				if (abilityAlreadyAcquired != SetAbilities.end()) //If we already have the ability try the others
					continue;

				auto abilityExists = std::find_if(moves.begin(), moves.end(), matchesAbilty);
				if (abilityExists != moves.end()) //If the move was found try and add it
				{
					//DebugPrint("Adding Move: " + (*abilityExists).MoveName + " Ability ID: " + std::to_string(ability));

					int moveID = outVector[OutVectorIndex].MoveID;
					auto matchesMoveID = [moveID](MoveObject move) {return move.MoveID == moveID; };
					auto foundLocation = std::find_if(AbilityLocations.begin(), AbilityLocations.end(), matchesMoveID);
					AbilityLocations.erase(foundLocation);
					SetAbilities.push_back(std::make_pair(outVector[OutVectorIndex].MoveID, *abilityExists));
					OutVectorIndex++;
				}
			}
		}

		/// <summary>
		/// Find the required items in the requirement set and assign the items to available locations
		/// </summary>
		/// <param name="things"></param>
		void AccessibleThings::AddItems(LogicGroup::RequirementSet requirement)
		{
			//OutputDebugString(("Add Requirement Set: " + requirement.SetName + "\n").c_str());

			std::vector<int> outVector;
			outVector = ItemLocations; //Shuffle the locations
			std::shuffle(outVector.begin(), outVector.end(), std::default_random_engine(seed));
			std::unordered_map<int, std::vector<int>> normalLevelObjectsMap;
			for (int shuffledIdx = 0; shuffledIdx < outVector.size(); shuffledIdx++) //sort the available normal locations into levels
			{
				if (!objectsList[outVector[shuffledIdx]].isSpawnLocation)
					normalLevelObjectsMap[objectsList[outVector[shuffledIdx]].LevelIndex].push_back(outVector[shuffledIdx]);
			}

			for (int i = 0; i < requirement.RequiredItems.size(); i++) //Add notes first
			{
				if (requirement.RequiredItems[i] == "Note")
				{
					int collectableAmount = GetCollectableCount(requirement.RequiredItems[i]);
					std::vector<int> levels = GetLevels(); //Get the accessible levels
					for (int levelIndex = 0; levelIndex < levels.size(); levelIndex++)
					{
						int levelInt = levels[levelIndex];
						bool trebleUsed = levelNotes[levelInt].TrebleUsed;
						int usedNotes = levelNotes[levelInt].usedNotes; //Number of normal note nests that have been used in this level
						if (trebleUsed == false)
						{
							collectableAmount += 20; //Always try a treble
							int sourceObjectID = FindObjectOfType(requirement.RequiredItems[i], 20, levelInt);
							if (sourceObjectID != -1)
							{
								//OutputDebugString(("Set Note In Level: " + std::to_string(objectsList[normalLevelObjectsMap[levelInt][0]].LevelIndex) + " Rando Object ID: " + std::to_string(normalLevelObjectsMap[levelInt][0]) + "\n").c_str());
								SetItems.push_back(std::make_pair((normalLevelObjectsMap[levelInt][0]), sourceObjectID));
								int objectID = normalLevelObjectsMap[levelInt][0];

								normalLevelObjectsMap[levelInt].erase(normalLevelObjectsMap[levelInt].begin());

								auto foundInShuffled = std::find(outVector.begin(), outVector.end(), objectID);
								outVector.erase(foundInShuffled);

								auto foundLocation = std::find(ItemLocations.begin(), ItemLocations.end(), objectID);
								ItemLocations.erase(foundLocation);
							}
							levelNotes[levelInt].TrebleUsed = true;
						}
						int availableLocations = normalLevelObjectsMap[levelInt].size();
						//Keep trying to add notes until we reach the required value making sure the number of notes does not exceed the maximum notes in the level
						while (collectableAmount < requirement.RequiredItemsCount[i] && usedNotes < 16 && availableLocations > 0) 
						{
							collectableAmount += 5;
							int sourceObjectID = FindObjectOfType(requirement.RequiredItems[i], 5, levelInt);
							if (sourceObjectID != -1)
							{
								//OutputDebugString(("Set Note In Level: " + std::to_string(objectsList[normalLevelObjectsMap[levelInt][0]].LevelIndex) + " Rando Object ID: " + std::to_string(normalLevelObjectsMap[levelInt][0]) + "\n").c_str());
								SetItems.push_back(std::make_pair((normalLevelObjectsMap[levelInt][0]), sourceObjectID));
								int objectID = normalLevelObjectsMap[levelInt][0];

								normalLevelObjectsMap[levelInt].erase(normalLevelObjectsMap[levelInt].begin());

								auto foundInShuffled = std::find(outVector.begin(), outVector.end(), objectID);
								outVector.erase(foundInShuffled);
								availableLocations--;
								auto foundLocation = std::find(ItemLocations.begin(), ItemLocations.end(), objectID);
								ItemLocations.erase(foundLocation);
							}
							usedNotes++;
						}
						levelNotes[levelInt].usedNotes = usedNotes;
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
					if(objectsList[std::get<0>(item)].LevelIndex == levelInt && !objectsList[std::get<0>(item)].isSpawnLocation)
						usedSlots++;
				}
				int unusedSlots = GetUnusedNormalGlobalLocationsFromLevel(levelInt).size();

				int slotsUnallocated = 17;
				slotsUnallocated -= levelNotes[levelInt].usedNotes;
				if (levelNotes[levelInt].TrebleUsed)
					slotsUnallocated--;
				//10-(12-10) = 8
				int slotsToRemove = 0;
				int availableNormalSlots = normalLevelObjectsMap[levelInt].size();
					slotsToRemove = availableNormalSlots - (unusedSlots - slotsUnallocated);
					if (slotsToRemove > 17)
					{
						int numNormal = 0;
						for (int shuffledIdx = 0; shuffledIdx < outVector.size(); shuffledIdx++) //sort the available normal locations into levels
						{
							if (!objectsList[outVector[shuffledIdx]].isSpawnLocation && objectsList[outVector[shuffledIdx]].LevelIndex == levelInt)
								numNormal++;
						}
						OutputDebugString(("Huh Too Many Slots are being removed " + std::to_string(numNormal)).c_str());
					}
				//10-(25-17)

					if (unusedSlots < slotsUnallocated)
					{
						OutputDebugString(("We have run out of slots to allocate Unused Slots: " + std::to_string(unusedSlots) + " Note Slots Unallocated: " + std::to_string(slotsUnallocated)+"\n").c_str());
					}


				if (slotsToRemove < 0)
					slotsToRemove = 0;
				//OutputDebugString(("Level: "+std::to_string(levelInt)+" Available Locations in Level: " + std::to_string(availableNormalSlots) + " Note Slots Used: " + std::to_string(17 - slotsUnallocated) + " Note Slots Unallocated: " + std::to_string(slotsUnallocated) + " Unused Slots : " + std::to_string(unusedSlots) + " slots to Remove : " + std::to_string(slotsToRemove) + "\n").c_str());
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
					else
						OutputDebugString(("Could Not Remove object " + std::to_string(foundId) + "\n").c_str());
				}
			}
			
			for (int i = 0; i < requirement.RequiredItems.size(); i++) //No other objects need to be kept in a level in the same way
			{
				if (requirement.RequiredItems[i] != "Note")
				{
					for (int j = GetCollectableCount(requirement.RequiredItems[i]); j < requirement.RequiredItemsCount[i]; j++)
					{
						int sourceObjectID = FindObjectOfType(requirement.RequiredItems[i], 1);
						if (sourceObjectID != -1)
						{
							if (outVector.size() > 0)
							{
								//OutputDebugString(("Level: " + std::to_string(objectsList[outVector[0]].LevelIndex) +" Rando Object ID: "+ std::to_string(objectsList[outVector[0]].RandoObjectID) + "\n").c_str());
								SetItems.push_back(std::make_pair(objectsList[outVector[0]].RandoObjectID, sourceObjectID));

								int objectID = objectsList[outVector[0]].RandoObjectID;
								outVector.erase(outVector.begin());

								auto foundLocation = std::find(ItemLocations.begin(), ItemLocations.end(), objectID);
								if (foundLocation != ItemLocations.end())
									ItemLocations.erase(foundLocation);
							}
							else
								OutputDebugString("Out of valid locations as they Could Not Be Found\n");
						}
					}
				}
			}
		}

		int AccessibleThings::FindObjectOfType(std::string objectName)
		{
			return FindObjectOfType(objectName, 1);
		}

		int AccessibleThings::FindObjectOfType(std::string objectName, int amount)
		{
			std::vector<RandomizedObject> unusedObjects;

			std::unordered_set<int> usedObjectIDs;
			for (const auto& item : SetItems)
			{
				usedObjectIDs.insert(objectsList[std::get<1>(item)].RandoObjectID);
			}

			for (const auto& obj : objectsList)
			{
				int id = obj.first;
				RandomizedObject object = obj.second;
				if (usedObjectIDs.count(object.RandoObjectID) == 0 && object.ItemTag == objectName && object.ItemAmount == amount && object.randomized)
				{
					auto foundNoRando = std::find(NoRandomizationIDs.begin(), NoRandomizationIDs.end(), object.objectID);
					if (foundNoRando == NoRandomizationIDs.end())
						return object.RandoObjectID;  // Return first match immediately
				}
			}

			return -1;  // No match found
		}

		int AccessibleThings::FindObjectOfType(std::string objectName, int amount, int levelIndex)
		{
			std::vector<RandomizedObject> unusedObjects;

			std::unordered_set<int> usedObjectIDs;
			for (const auto& item : SetItems)
			{
				usedObjectIDs.insert(objectsList[std::get<1>(item)].RandoObjectID);
			}

			for (const auto& obj : objectsList)
			{
				int id = obj.first;
				RandomizedObject object = obj.second;
				if (usedObjectIDs.count(object.RandoObjectID) == 0 && object.ItemTag == objectName && object.ItemAmount == amount && object.randomized && object.LevelIndex == levelIndex)
				{
					auto foundNoRando = std::find(NoRandomizationIDs.begin(), NoRandomizationIDs.end(), object.objectID);
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

		/// <summary>
		/// Determine whether the available locations for placement would allow the given requirement set to have its remaining missing elements placed into the state such that the requirement would be fulfilled
		/// </summary>
		/// <param name="requirement"></param>
		/// <returns></returns>
		bool AccessibleThings::CanFulfill(LogicGroup::RequirementSet requirement)
		{
			//OutputDebugString(("Checking requirements for set "+ requirement.SetName + "\n").c_str());
			int missingAbilities = 0;
			for (int i = 0; i < requirement.RequiredAbilities.size(); i++)
			{
				int ability = requirement.RequiredAbilities[i];
				auto matchesAbility = [ability](std::pair<int,MoveObject> move) {return std::get<1>(move).Ability == ability; };
				auto it = std::find_if(SetAbilities.begin(), SetAbilities.end(), matchesAbility);
				if (it == SetAbilities.end()) //If the ability is not already found in the set abilities
				{
					missingAbilities++;
				}
			}
			if (AbilityLocations.size() < missingAbilities) //Check if the available abilty locations could allow for the requirement to be fulfilled
			{
				DebugPrint("There were not enough available move locations Available Locations: " + std::to_string(AbilityLocations.size())+" Missing Abilities Count " + std::to_string(missingAbilities));
				return false;
			}

			int normalLocationsCount = GetNormalLocations().size();
			int neededSpots = 0;//Number of locations required to fulfill the requirement
			int neededNormalSpots = 0; //Number of nonspawning locations
			std::vector<int> levels = GetLevels(); //Get the accessible levels
			int totalNormalObjects = 0; //Sum of the total number of normal objects in the accessible levels
			std::unordered_map<int, NoteState> tempLevelNotes;
			for (int levelIndex = 0; levelIndex < levels.size(); levelIndex++)
			{
				int levelInt = levels[levelIndex];
				totalNormalObjects += normalLevelObjectsMapAll[levelInt].size();
				tempLevelNotes[levelInt].usedNotes = levelNotes[levelInt].usedNotes;
				tempLevelNotes[levelInt].TrebleUsed = levelNotes[levelInt].TrebleUsed;
			}

			int noteSpots = 0; //How Many Spots are notes using

			for (int j = 0; j < requirement.RequiredItems.size(); j++)
			{
				std::string collectableName = requirement.RequiredItems[j];
				
				int collectableAmount = GetCollectableCount(collectableName);
				//OutputDebugString((collectableName+" Requirement Count: "+std::to_string(requirement.RequiredItemsCount[j]) + " Collectable Count: " + std::to_string(collectableAmount) +"\n").c_str());
				if (requirement.RequiredItems[j] == "Note")
				{
					for (int levelIndex = 0; levelIndex < levels.size(); levelIndex++)
					{

						int levelInt = levels[levelIndex];

						int usedNormalSlots = 0;
						for (const auto& item : SetItems)
						{
							if (objectsList[std::get<1>(item)].LevelIndex == levelInt && !objectsList[std::get<1>(item)].isSpawnLocation)
								usedNormalSlots++;
						}
						int unusedSlots = usedNormalSlots-normalLevelObjectsMapAll[levelInt].size(); //Gets the amount of slots that are unused regardless of accessibility
						if (!levelNotes[levelInt].TrebleUsed)
						{
							collectableAmount += 20;
							tempLevelNotes[levelInt].TrebleUsed = true;
						}
						while (collectableAmount < requirement.RequiredItemsCount[j] && tempLevelNotes[levelInt].usedNotes < 16 && tempLevelNotes[levelInt].usedNotes < GetNormalLocationsFromMap(levelInt).size())
						{
							collectableAmount += 5;
							tempLevelNotes[levelInt].usedNotes++;
						}
					}
					if (collectableAmount < requirement.RequiredItemsCount[j]) //If we cannot meet the quota we cannot meet the requirements
					{
						DebugPrint("Could not meet note quota Required Amount: " + std::to_string(requirement.RequiredItemsCount[j]) + " Possible Amount " + std::to_string(collectableAmount));
						return false;

					}

				}
				//Check if it isn't a reward object
				else if (!RandomizedObject::CanBeReward(requirement.RequiredItems[j]))
				{
					//Make sure that we do not allocate more spots for items we already can afford
					if (collectableAmount < requirement.RequiredItemsCount[j])
					{
						neededNormalSpots += (requirement.RequiredItemsCount[j] - collectableAmount);
					}
				}
				else
				{
					//Make sure that we do not allocate more spots for items we already can afford
					if (collectableAmount < requirement.RequiredItemsCount[j])
						neededSpots += (requirement.RequiredItemsCount[j] - collectableAmount);
				}
			}
			int availableNormalSpots = normalLocationsCount; //Get the amount of normal spots after allocating space for notes
			//OutputDebugString(("Normal Location Amount Pre Allocation " + std::to_string(normalLocationsCount) + "\n").c_str());

			for (int levelIndex = 0; levelIndex < levels.size(); levelIndex++)
			{
				int levelInt = levels[levelIndex];

				int newNotes = tempLevelNotes[levelInt].usedNotes - levelNotes[levelInt].usedNotes;
				//OutputDebugString(("Level " + std::to_string(levelInt) + " Start Temp Notes " + std::to_string(tempLevelNotes[levelInt].usedNotes) + " Old Notes " + std::to_string(levelNotes[levelInt].usedNotes)+ "\n").c_str());
				//OutputDebugString(("Start Temp Treble " + std::to_string(tempLevelNotes[levelInt].TrebleUsed) + " Old Treble " + std::to_string(levelNotes[levelInt].TrebleUsed) + "\n").c_str());

				int usedNoteSlots = levelNotes[levelInt].usedNotes;
				if (tempLevelNotes[levelInt].TrebleUsed == true && levelNotes[levelInt].TrebleUsed == false)
					newNotes++;
				if (levelNotes[levelInt].TrebleUsed)
					usedNoteSlots++;
				//OutputDebugString(("Level "+std::to_string(levelInt)+" Allocate New Notes " + std::to_string(newNotes) + " Old Notes " + std::to_string(usedNoteSlots) + " Unused Normal Items in Level Count " + std::to_string(GetUnusedNormalGlobalLocationsFromLevel(levelInt).size()) + "\n").c_str());

				availableNormalSpots -= newNotes;
			}

			//Try and determine if there will still be room to place leftover notes in appropriate levels
			
			//OutputDebugString(("Needed Normal Spots " + std::to_string(neededNormalSpots) + " Available normal spots " + std::to_string(availableNormalSpots) + "\n").c_str());




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

			int availableSpotTotal = availableNormalSpots - neededNormalSpots;
			//OutputDebugString(("Needed Spots " + std::to_string(neededSpots) + " Available spot total " + std::to_string(availableSpotTotal) +" Total available location count without removal "+std::to_string(ItemLocations.size()) + "\n").c_str());

			if (neededSpots > availableSpotTotal)
			{
				DebugPrint("There were not enough available item locations Available Locations: " + std::to_string(availableSpotTotal) + " needed spots Count " + std::to_string(neededSpots));
				return false;
			}

			if (!ContainsRequiredKeys(*this, requirement))
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

		std::vector<RandomizedObject> AccessibleThings::GetLocationsFromMap(int LevelIndex)
		{
			std::vector<RandomizedObject> objects;
			for (int i = 0; i < ItemLocations.size(); i++)
			{
				if (objectsList[ItemLocations[i]].LevelIndex == LevelIndex)
				{
					objects.push_back(objectsList[ItemLocations[i]]);
				}
			}
			return objects;
		}

		/// <summary>
		/// Get the normal locations in the given level that are accessible
		/// </summary>
		/// <param name="LevelIndex"></param>
		/// <returns></returns>
		std::vector<RandomizedObject> AccessibleThings::GetNormalLocationsFromMap(int LevelIndex)
		{
			std::vector<RandomizedObject> objects;
			for (int i = 0; i < ItemLocations.size(); i++)
			{
				if (objectsList[ItemLocations[i]].LevelIndex == LevelIndex && objectsList[ItemLocations[i]].isSpawnLocation == false)
				{
					objects.push_back(objectsList[ItemLocations[i]]);
				}
			}
			return objects;
		}

		//Get Locations that do not spawn objects from the accessible state
		std::vector<RandomizedObject> AccessibleThings::GetNormalLocations()
		{
			std::vector<RandomizedObject> objects;
			for (int i = 0; i < ItemLocations.size(); i++)
			{
				if (objectsList[ItemLocations[i]].isSpawnLocation == false)
				{
					objects.push_back(objectsList[ItemLocations[i]]);
				}
			}
			return objects;
		}

		//Get All Locations regardless of accessibility
		static std::vector<int> AccessibleThings::GetNormalGlobalLocations()
		{
			std::vector<int> objects;
			for (const auto& obj : objectsList)
			{
				if (!obj.second.isSpawnLocation)
				{
					objects.push_back(obj.first);
				}
			}
			return objects;
		}

		//Get All Locations in level regardless of accessibility
		static std::vector<int> AccessibleThings::GetNormalGlobalLocationsFromLevel(int LevelIndex)
		{
			return normalLevelObjectsMapAll[LevelIndex];
		}

		//Get All unused locations in level regardless of access
		std::vector<int> AccessibleThings::GetUnusedNormalGlobalLocationsFromLevel(int LevelIndex)
		{
			std::vector<int> objects;
			std::vector<int> usedObjectIDs;
			for (const auto& item : SetItems)
			{
				usedObjectIDs.push_back(objectsList[item.first].RandoObjectID);
			}

			for (const auto& obj : objectsList)
			{
				auto it = std::find(usedObjectIDs.begin(), usedObjectIDs.end(), obj.second.RandoObjectID);
				auto foundNoRando = std::find(NoRandomizationIDs.begin(), NoRandomizationIDs.end(), obj.second.objectID);

				if (!obj.second.isSpawnLocation && obj.second.LevelIndex == LevelIndex && it == usedObjectIDs.end() && foundNoRando == NoRandomizationIDs.end() && obj.second.randomized)
				{
					objects.push_back(obj.first);
				}
			}
			return objects;
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
	static LogicHandler::AccessibleThings GetAllTotals(LogicGroup startingGroup, std::unordered_map<int, LogicGroup>& logicGroups, LogicHandler::AccessibleThings stateStart, std::vector<RandomizedObject>& objects, std::vector<MoveObject>& moves, std::vector<int>& seenLogicGroups, std::vector<int>& nextLogicGroups);
	static LogicHandler::AccessibleThings GetAccessibleRecursive(LogicGroup& startingGroup, std::unordered_map<int,LogicGroup>& logicGroups, LogicHandler::AccessibleThings& start, std::vector<RandomizedObject>& objects, std::vector<MoveObject>& moves, std::vector<int>& seenLogicGroups, std::vector<int>& nextLogicGroups);

	static bool FulfillsRequirements(LogicGroup groupToUnlock, LogicHandler::AccessibleThings state);
	static bool CanFulfillRequirements(LogicHandler::AccessibleThings accessibleSpots, LogicGroup groupToOpen);
	static bool ContainsRequiredKeys(LogicHandler::AccessibleThings state, LogicGroup::RequirementSet requirements);
	LogicHandler::AccessibleThings TryRoute(LogicGroup startingGroup, std::unordered_map<int,LogicGroup>& logicGroups, std::vector<int> lookedAtLogicGroups, std::vector<int> nextLogicGroups, LogicHandler::AccessibleThings initialState, std::vector<int> viableLogicGroups, std::vector<RandomizedObject> objects, std::vector<MoveObject> moves, int depth);
};


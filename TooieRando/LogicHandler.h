#pragma once
#include "LogicGroup.h"
#include "MoveObject.h"
#include <tuple> 
#include <random>
#include <algorithm>

class LogicHandler
{
public:
	static int seed; //The seed used for randomization
	static std::vector<RandomizedObject> objectsList; //List of All Objects

	class AccessibleThings
	{
	public:
		~AccessibleThings() {
		}
		std::vector<MoveObject> AbilityLocations; //Available Locations to place moves
		std::vector<std::tuple<int,MoveObject>> SetAbilities; //Location Move ID paired with the move placed at that location
		std::vector<RandomizedObject> ItemLocations; //Available Locations to place Objects
		std::vector<std::tuple<int, RandomizedObject>> SetItems; //Location Object ID paired with the object placed at that location

		/// <summary>
		/// Collectable Name paired with the number of them
		/// </summary>
		std::vector<std::tuple<std::string,int>> ContainedItems;

		std::vector<std::string> Keys;

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
				auto matchesAbility = [ability](std::tuple<int,MoveObject> move) {return (std::get<1>(move)).Ability == ability; };
				auto it = std::find_if(SetAbilities.begin(), SetAbilities.end(), matchesAbility);
				if(it == SetAbilities.end())
					SetAbilities.push_back(things.SetAbilities[i]);
			}
			for (int i = 0; i < things.ItemLocations.size(); i++)
			{
				int locationObjectID = things.ItemLocations[i].ObjectID;
				auto matchesObject = [locationObjectID](RandomizedObject object) {return object.ObjectID == locationObjectID; };
				auto it = std::find_if(ItemLocations.begin(), ItemLocations.end(), matchesObject);
				if (it == ItemLocations.end())
					ItemLocations.push_back(things.ItemLocations[i]);
			}
			for (int i = 0; i < things.SetItems.size(); i++)
			{
				int locationObjectID = std::get<1>(things.SetItems[i]).ObjectID;
				auto matchesObject = [locationObjectID](std::tuple<int,RandomizedObject> object) {return std::get<1>(object).ObjectID == locationObjectID; };
				auto it = std::find_if(SetItems.begin(), SetItems.end(), matchesObject);
				if (it == SetItems.end())
					SetItems.push_back(things.SetItems[i]);
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
					if (objects[j].ObjectID == group.objectIDsInGroup[i])
					{
						ItemLocations.push_back(objects[j]);
						//AddCollectable(objects[j].ItemTag, objects[j].ItemAmount);
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
			auto matchesName = [name](std::tuple<std::string,int> Collectable) {return std::get<0>(Collectable) == name; };
			auto it = std::find_if(ContainedItems.begin(), ContainedItems.end(), matchesName);
			if (it != ContainedItems.end())
				return std::get<1>(*it);
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
			for (int i = 0; i < requirement.RequiredAbilities.size(); i++)
			{
				int ability = requirement.RequiredAbilities[i];
				auto matchesAbilty = [ability](MoveObject move) {return move.Ability == ability; };
				auto it = std::find_if(moves.begin(), moves.end(), matchesAbilty);
				if (it != moves.end())
				{
					int moveID = outVector[i].MoveID;
					auto matchesMoveID = [moveID](MoveObject move) {return move.MoveID == moveID; };
					auto foundLocation = std::find_if(AbilityLocations.begin(), AbilityLocations.end(), matchesMoveID);
					AbilityLocations.erase(foundLocation);
					SetAbilities.push_back(std::make_tuple(outVector[i].MoveID, *it));
				}
			}
		}

		/// <summary>
		/// Find the required items in the requirement set and assign the items to available locations
		/// </summary>
		/// <param name="things"></param>
		void AccessibleThings::AddItems(LogicGroup::RequirementSet requirement)
		{
			std::vector<RandomizedObject> outVector;
			outVector = ItemLocations;
			std::shuffle(outVector.begin(), outVector.end(), std::default_random_engine(seed));
			for (int i = 0; i < requirement.RequiredItems.size(); i++)
			{
				if (requirement.RequiredItems[i] == "Note")
				{
					int collectableAmount = GetCollectableCount(requirement.RequiredItems[i]);

					for (int levelIndex = 0; levelIndex < GetLevels().size(); levelIndex++)
					{
						bool trebleUsed;
						int usedNotes = 0; //Number of normal note nests that have been used in this level
						if (collectableAmount > 0)
						{
							for (int noteIdx; noteIdx < SetItems.size(); i++)
							{
								int itemAmount = std::get<1>(SetItems[noteIdx]).ItemAmount;
								if (itemAmount == 20)
								{
									trebleUsed = true;
									continue;
								}
								else
								{
									usedNotes++;
								}
							}
						}
						if (trebleUsed == false)
						{
							collectableAmount += 20; //Always try a treble
							int sourceObjectID = FindObjectOfType(requirement.RequiredItems[i], 20);
							if (sourceObjectID != -1)
							{
								auto matchesLocationObjectID = [sourceObjectID](RandomizedObject object) {return object.ObjectID == sourceObjectID; };
								auto foundObject = std::find_if(objectsList.begin(), objectsList.end(), matchesLocationObjectID);

								SetItems.push_back(std::make_tuple(outVector[0].ObjectID, *foundObject));
								int objectID = outVector[0].ObjectID;
								outVector.erase(outVector.begin());
								auto matchesObjectID = [objectID](RandomizedObject object) {return object.ObjectID == objectID; };
								auto foundLocation = std::find_if(ItemLocations.begin(), ItemLocations.end(), matchesObjectID);
								ItemLocations.erase(foundLocation);
							}
						}
						//Keep trying to add notes until we reach the required value making sure the number of notes does not exceed the maximum notes in the level
						while (collectableAmount < requirement.RequiredItemsCount[i] && usedNotes < 16 && usedNotes + 1 < GetLocationsFromMap(GetLevels()[levelIndex]).size())
						{
							collectableAmount += 5;
							int sourceObjectID = FindObjectOfType(requirement.RequiredItems[i], 5);
							if (sourceObjectID != -1)
							{
								auto matchesLocationObjectID = [sourceObjectID](RandomizedObject object) {return object.ObjectID == sourceObjectID; };
								auto foundObject = std::find_if(objectsList.begin(), objectsList.end(), matchesLocationObjectID);

								SetItems.push_back(std::make_tuple(outVector[0].ObjectID, *foundObject));
								int objectID = outVector[0].ObjectID;
								outVector.erase(outVector.begin());
								auto matchesObjectID = [objectID](RandomizedObject object) {return object.ObjectID == objectID; };
								auto foundLocation = std::find_if(ItemLocations.begin(), ItemLocations.end(), matchesObjectID);
								ItemLocations.erase(foundLocation);
							}
							usedNotes++;
						}
					}
				}
			}

			for (int i = 0; i < requirement.RequiredItems.size(); i++)
			{
				if (requirement.RequiredItems[i] != "Note")
				{
					for (int j = GetCollectableCount(requirement.RequiredItems[i]); j < requirement.RequiredItemsCount[i]; j++)
					{
						int sourceObjectID = FindObjectOfType(requirement.RequiredItems[i], 1);
						if (sourceObjectID != -1)
						{
							auto matchesLocationObjectID = [sourceObjectID](RandomizedObject object) {return object.ObjectID == sourceObjectID; };
							auto foundObject = std::find_if(objectsList.begin(), objectsList.end(), matchesLocationObjectID);

							SetItems.push_back(std::make_tuple(outVector[0].ObjectID, *foundObject));
							int objectID = outVector[0].ObjectID;
							outVector.erase(outVector.begin());
							auto matchesObjectID = [objectID](RandomizedObject object) {return object.ObjectID == objectID; };
							auto foundLocation = std::find_if(ItemLocations.begin(), ItemLocations.end(), matchesObjectID);
							ItemLocations.erase(foundLocation);
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
			for (int i = 0; i < objectsList.size(); i++)
			{
				int locationObjectID = objectsList[i].ObjectID;
				auto matchesObject = [locationObjectID, amount](std::tuple<int, RandomizedObject> object) {return (std::get<1>(object).ObjectID == locationObjectID && std::get<1>(object).ItemAmount == amount); };
				auto it = std::find_if(SetItems.begin(), SetItems.end(), matchesObject);
				if (it == SetItems.end())
					unusedObjects.push_back(objectsList[i]);
			}
			auto matchesTag = [objectName](RandomizedObject move) {return move.ItemTag == objectName; };
			auto foundObject = std::find_if(unusedObjects.begin(), unusedObjects.end(), matchesTag);
			if (foundObject != unusedObjects.end())
				return (*foundObject).ObjectID;
			else
				return -1;
		}

		void AccessibleThings::UpdateCollectables()
		{
			ContainedItems.clear();
			for (int i = 0; i < SetItems.size(); i++)
			{
				AddCollectable(std::get<1>(SetItems[i]).ItemTag, std::get<1>(SetItems[i]).ItemAmount);
			}
		}

		bool AccessibleThings::CanFulfill(LogicGroup::RequirementSet requirement)
		{
			int missingAbilities = 0;
			for (int i = 0; i < requirement.RequiredAbilities.size(); i++)
			{
				int ability = requirement.RequiredAbilities[i];
				auto matchesAbility = [ability](std::tuple<int,MoveObject> move) {return std::get<1>(move).MoveID == ability; };
				auto it = std::find_if(SetAbilities.begin(), SetAbilities.end(), matchesAbility);
				if (it == SetAbilities.end()) //If the ability is not already found in the set abilities
				{
					missingAbilities++;
				}
			}
			if (AbilityLocations.size() < missingAbilities) //Check if the available abilty locations could allow for the requirement to be fulfilled
			{
				return false;
			}
			int neededSpots = 0;//Number of locations required to fulfill the requirement
			for (int j = 0; j < requirement.RequiredItems.size(); j++)
			{
				std::string collectableName = requirement.RequiredItems[j];
				
				int collectableAmount = 0;
				auto it = std::find_if(ContainedItems.begin(), ContainedItems.end(), [collectableName](std::tuple<std::string, int> Item) {return std::get<0>(Item) == collectableName; });
				if(it != ContainedItems.end())
					collectableAmount = std::get<1>(ContainedItems[it - ContainedItems.begin()]);
				if (requirement.RequiredItems[j] == "Note")
				{
					for (int levelIndex = 0; levelIndex < GetLevels().size(); levelIndex++)
					{
						collectableAmount += 20; //Always try a treble
						neededSpots++;
						int usedNotes = 0; //Number of normal note nests that are available to grab in the entire level
						//Keep trying to add notes until we reach the required value making sure the number of notes does not exceed the maximum notes in the level
						while (collectableAmount < requirement.RequiredItemsCount[j] && usedNotes < 16 && usedNotes + 1 < GetLocationsFromMap(GetLevels()[levelIndex]).size())
						{
							collectableAmount += 5;
							neededSpots++;
							usedNotes++;
						}
					}
					if (collectableAmount < requirement.RequiredItemsCount[j]) //If we cannot meet the quota we cannot meet the requirements
					{
						return false;

					}
				}
				else
				{
					//Make sure that we do not allocate more spots for items we already can afford
					if (collectableAmount < requirement.RequiredItemsCount[j])
						neededSpots += (requirement.RequiredItemsCount[j] - collectableAmount);
				}
			}
			if (neededSpots > ItemLocations.size())
			{
				return false;
			}
			if (!ContainsRequiredKeys(*this, requirement))
			{
				return false;
			}
			return true;
		}

		void AccessibleThings::AddCollectable(std::string collectableName, int value)
		{
			auto it = std::find_if(ContainedItems.begin(), ContainedItems.end(), [collectableName](std::tuple<std::string, int> Item) {return std::get<0>(Item) == collectableName; });
			if (it == ContainedItems.end())
			{
				ContainedItems.push_back(std::make_tuple(collectableName,value));
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
				if (ItemLocations[i].LevelIndex == LevelIndex)
				{
					objects.push_back(ItemLocations[i]);
				}
			}
			return objects;
		}

		//Get Locations that do not have any
		std::vector<RandomizedObject> AccessibleThings::GetNormalLocationsFromMap(int LevelIndex)
		{
			std::vector<RandomizedObject> objects;
			for (int i = 0; i < ItemLocations.size(); i++)
			{
				if (ItemLocations[i].LevelIndex == LevelIndex && ItemLocations[i].rewardObjectIndex != -1)
				{
					objects.push_back(ItemLocations[i]);
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
				auto it = std::find(levels.begin(), levels.end(), ItemLocations[i].LevelIndex);
				if (it == levels.end())
				{
					levels.push_back(ItemLocations[i].LevelIndex);
				}
			}
			return levels;
		}
	};
	static LogicHandler::AccessibleThings GetAllTotals(LogicGroup startingGroup, std::vector<LogicGroup> logicGroups, LogicHandler::AccessibleThings stateStart, std::vector<RandomizedObject>& objects, std::vector<MoveObject>& moves, std::vector<int>& seenLogicGroups, std::vector<int>& nextLogicGroups);
	static LogicHandler::AccessibleThings GetAccessibleRecursive(LogicGroup startingGroup, std::vector<LogicGroup> logicGroups, LogicHandler::AccessibleThings start, std::vector<RandomizedObject>& objects, std::vector<MoveObject>& moves, std::vector<int>& seenLogicGroups, std::vector<int>& nextLogicGroups);

	static bool FulfillsRequirements(LogicGroup groupToUnlock, LogicHandler::AccessibleThings state);
	static bool CanFulfillRequirements(LogicHandler::AccessibleThings accessibleSpots, LogicGroup groupToOpen);
	static bool ContainsRequiredKeys(LogicHandler::AccessibleThings state, LogicGroup::RequirementSet requirements);
	void TryRoute(LogicGroup startingGroup, std::vector<LogicGroup> logicGroups, std::vector<int> lookedAtLogicGroups, std::vector<int> nextLogicGroups, LogicHandler::AccessibleThings initialState, std::vector<int> viableLogicGroups, std::vector<RandomizedObject> objects, std::vector<MoveObject> moves);
};


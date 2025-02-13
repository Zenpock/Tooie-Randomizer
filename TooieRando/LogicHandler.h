#pragma once
#include "LogicGroup.h"
#include "MoveObject.h"
#include <tuple> 

class LogicHandler
{
private:

public:
	class AccessibleThings
	{
	public:
		~AccessibleThings() {
			OutputDebugString("Destroying AccessibleThings at \n");
		}
		std::vector<MoveObject> Abilities;
		std::vector<RandomizedObject> Items;

		std::vector<std::tuple<std::string,int>> ContainedItems;

		std::vector<std::string> Keys;

		/// <summary>
		/// Add all of the items from the given group to this one
		/// </summary>
		/// <param name="things"></param>
		void AccessibleThings::Add(AccessibleThings things)
		{
			for (int i = 0; i < things.Abilities.size(); i++)
			{
				Abilities.push_back(things.Abilities[i]);
			}
			for (int i = 0; i < things.Items.size(); i++)
			{
				Items.push_back(things.Items[i]);
			}
			for (int i = 0; i < things.Keys.size(); i++)
			{
				Keys.push_back(things.Keys[i]);
			}
			for (int i = 0; i < things.ContainedItems.size(); i++)
			{
				std::string name = std::get<0>(things.ContainedItems[i]);
				AddCollectable(name, std::get<1>(things.ContainedItems[i]));
			}
		}
		/// <summary>
		/// Add all of the items from the given group to this one
		/// </summary>
		/// <param name="group"></param>
		/// <param name="objects">The list of all of the objects that exist so additional data can be gathered from them</param>
		/// <param name="moves">The list of all of the moves that exist so additional data can be gathered from them</param>
		void AccessibleThings::Add(LogicGroup group,std::vector<RandomizedObject> objects, std::vector<MoveObject> moves)
		{
			for (int i = 0; i < group.objectIDsInGroup.size(); i++)
			{
				for (int j = 0; j < objects.size(); j++)
				{
					if (objects[j].ObjectID == group.objectIDsInGroup[i])
					{
						Items.push_back(objects[j]);
						AddCollectable(objects[j].ItemTag, objects[j].ItemAmount);
					}
				}
			}
			for (int j = 0; j < moves.size(); j++)
			{
				if (group.containedMove == moves[j].MoveID)
					Abilities.push_back(moves[j]);
			}
			if(!group.key.empty())
			Keys.push_back(group.key);
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
			for (int i = 0; i < Items.size(); i++)
			{
				if (Items[i].LevelIndex == LevelIndex)
				{
					objects.push_back(Items[i]);
				}
			}
			return objects;
		}

		//Get Locations that do not have any
		std::vector<RandomizedObject> AccessibleThings::GetNormalLocationsFromMap(int LevelIndex)
		{
			std::vector<RandomizedObject> objects;
			for (int i = 0; i < Items.size(); i++)
			{
				if (Items[i].LevelIndex == LevelIndex && Items[i].rewardObjectIndex != -1)
				{
					objects.push_back(Items[i]);
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
			for (int i = 0; i < Items.size(); i++)
			{
				auto it = std::find(levels.begin(), levels.end(), Items[i].LevelIndex);
				if (it == levels.end())
				{
					levels.push_back(Items[i].LevelIndex);
				}
			}
			return levels;
		}
	};
	static LogicHandler::AccessibleThings GetAllTotals(LogicGroup startingGroup, std::vector<LogicGroup> logicGroups, LogicHandler::AccessibleThings stateStart, std::vector<RandomizedObject> objects, std::vector<MoveObject> moves, std::vector<int>& seenLogicGroups, std::vector<int>& nextLogicGroups);
	static LogicHandler::AccessibleThings GetAccessibleRecursive(LogicGroup startingGroup, std::vector<LogicGroup> logicGroups, LogicHandler::AccessibleThings start, std::vector<RandomizedObject> objects, std::vector<MoveObject> moves, std::vector<int>& seenLogicGroups, std::vector<int>& nextLogicGroups);

	static bool FulfillsRequirements(LogicGroup groupToUnlock, LogicHandler::AccessibleThings state);
	static bool CanFulfillRequirements(LogicHandler::AccessibleThings accessibleSpots, LogicGroup groupToOpen);
	static bool ContainsRequiredKeys(LogicHandler::AccessibleThings state, LogicGroup::RequirementSet requirements);
	void TryRoute(LogicGroup startingGroup, std::vector<LogicGroup> logicGroups, std::vector<int> lookedAtLogicGroups, LogicHandler::AccessibleThings initialState, std::vector<int> viableLogicGroups, std::vector<RandomizedObject> objects, std::vector<MoveObject> moves);
};


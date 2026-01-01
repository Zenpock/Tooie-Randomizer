#pragma once

#include <string>
#include <vector>
#include "Props.h"

class RewardObject
{
public:
	bool shouldRandomize = true;
	bool hasFlag = false;
	//The rando object id associated with this reward object
	int associatedRandoObjectID;
	//This is the objectID associated with the object e.g. 0x1F6 for Jiggies
	int objectID = 0; 
	//This is the ItemType which is the collectable Item Type
	int itemType = 0; 
	//This is the flag from the object
	int itemId = 0; 
	//This should be the index in the main table
	std::vector<int> associatedScripts; 
	//If multiple objects are spawned by the same script this determines which edits this object needs
	int itemIndex = 0; 
	
	RewardObject(int newAssociatedRandoObjectID, int newObjectID, int newItemId)
	{
		this->objectID = newObjectID;
		this->itemType = GetItemType(newObjectID);
		this->associatedRandoObjectID = newAssociatedRandoObjectID;
		this->itemId = newItemId;
	}

	/// <summary>
	/// Get The Ingame flag associated with given reward object
	/// </summary>
	/// <returns></returns>
	int getRewardFlag(int rewardIndex)
	{
		switch (itemType)
		{
			case 8:
				return rewardIndex + 0x50F;
			default:
				return rewardIndex + 0x2D6;
			case -1:
				break;
		}
		return -1;
	}
	//Get the Item Type as used by the game e.g. 1 = Jiggy, 2 = Honeycomb
	static int GetItemType(int objectID)
	{
		switch (objectID)
		{
		case Prop_Jinjo: //Jinjo
			return 0;
		case Prop_Jiggy: //Jiggy
			return 1;
		case Prop_Honeycomb: //Honeycomb
			return 2;
		case Prop_Glowbo: //Glowbo
			return 3;
		case Prop_Ticket: //Ticket
			return 8;
		case Prop_Doubloon: //Doubloon
			return 7;
		case Prop_CheatoPage: //Cheato
			return 4;
		case Prop_CUSTOM_MOVE_ITEM: //Move Item
			return 9;
		default:
			return -1;
		}
		return -1;
	}
};

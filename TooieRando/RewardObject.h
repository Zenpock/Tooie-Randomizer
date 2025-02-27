#pragma once

#include <string>
#include <vector>

class RewardObject
{
public:
	bool shouldRandomize = true;
	bool hasFlag = false;
	int associatedRandoObjectID;//The rando object id associated with this reward object
	int objectID = 0; //This is the objectID associated with the object e.g. 0x1F6 for Jiggies
	int itemType = 0; //This is the ItemType which is the collectable Item Type
	int itemId = 0; //This is the flag from the object
	std::vector<int> associatedScripts; //This should be the index in the main table
	int itemIndex = 0; //If multiple objects are spawned by the same script this determines which edits this object needs
	
	RewardObject(int newAssociatedRandoObjectID, int newObjectID, int newItemId)
	{
		this->objectID = newObjectID;
		switch (newObjectID)
		{
		case 0x1f5: //Jinjo
			this->itemType = 0;
			break;
		case 0x1f6: //Jiggy
			this->itemType = 1;
			break;
		case 0x1f7: //Honeycomb
			this->itemType = 2;
			break;
		case 0x1f8: //Glowbo
			this->itemType = 3;
			break;
		case 0x4E6: //Ticket
			this->itemType = 8;
			break;
		case 0x29D: //Doubloon
			this->itemType = 7;
			break;
		case 0x201: //Cheato
			this->itemType = 4;
			break;
		default:
			this->itemType = -1;
			break;
		}
		this->associatedRandoObjectID = newAssociatedRandoObjectID;
		this->itemId = newItemId;
	}
};

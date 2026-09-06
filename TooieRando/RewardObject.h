#pragma once

#include <string>
#include <vector>
#include "Props.h"
#include "Collectables.h"

class RewardObject
{
public:
	bool shouldRandomize = true;
	bool hasFlag = false;
	//The rando object id associated with this reward object
	int associatedRandoObjectID;
	//This is the propID associated with the object e.g. 0x1F6 for Jiggies
	int propId = 0; 
	//This is the ItemType which is the collectable Item Type
	int itemType = 0; 
	//This is the flag from the object
	int itemId = 0; 
	//This should be the index in the main table
	std::vector<int> associatedScripts; 
	//If multiple objects are spawned by the same script this determines which edits this object needs
	int itemIndex = 0; 
	
	RewardObject(int newAssociatedRandoObjectID, int newPropId, int newItemId)
	{
		this->propId = newPropId;
		this->itemType = GetItemType(newPropId);
		this->associatedRandoObjectID = newAssociatedRandoObjectID;
		this->itemId = newItemId;
	}

	/// <summary>
	/// Get The Ingame flag associated with given reward object
	/// </summary>
	/// <returns></returns>
	static int getRewardFlag(int rewardIndex)
	{
		int rewardFlag = rewardIndex -1 +0x2D7;
		if ((rewardFlag) >= 0x315)
		{
			if ((rewardFlag) >= 0x510) //So we Dont Overlap just force it to be 0x50F
				rewardFlag = 0x50F;
			else
				rewardFlag = 0x509 + ((rewardFlag) - 0x315);
		}
		return rewardFlag;
	}

#define JinjoFlagStart 0x1F3
#define JiggyFlagStart 0x24A
#define HoneycombFlagStart 0x220
#define GlowboFlagStart 0x239
#define CheatoPageFlagStart 0x2A6
#define JadeTotemFlagStart 0x2BD
#define NoteFlagStart 0x44E
#define DoubloonFlagStart 0x13E
#define TicketFlagStart 0x50F
#define MoveFlagStart 0xED

	/// <summary>
	/// Get The Ingame flag associated with collecting the given object
	/// </summary>
	/// <returns></returns>
	int getCollectedFlag()
	{
		//Jinjo,Jiggy,Honeycomb,Glowbo,Cheato Page,Jade Totem,Note,Doubloon,Ticket,Move
		int itemStarts[] = {JinjoFlagStart,JiggyFlagStart,HoneycombFlagStart,GlowboFlagStart,CheatoPageFlagStart,JadeTotemFlagStart,NoteFlagStart,DoubloonFlagStart,TicketFlagStart,MoveFlagStart};
		return itemStarts[this->itemType] + this->itemId;
	}
};

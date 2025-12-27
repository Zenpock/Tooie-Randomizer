#pragma once
#include <string>
#include <vector>
#include "Prop.h"

class RandomizedObject
{
public:
	int rewardObjectIndex = -1; //The index of the reward object associated with this object if there is one 
	Prop Data; //This is the raw data regarding the object
	int fileIndex = 0; //This should be the index in the main table
	int associatedOffset = 0; //The offset from the start of the file this data is located;
	std::string LocationName = "";
	int LevelIndex = -1; //The index of the level this object is in
	int mapID = -1; //The mapId that this object is in
	int RandoObjectID = -1;//The id of this object used for referencing specific objects
	std::string ItemTag = "";//This is the name of associated Item but for key usage like Jiggy or A Unique Id for a switch
	int ItemAmount = 1;//This is the amount of the associated item this item counts for really only necessary for Notes due to Treble
	bool isSpawnLocation = false;
	bool randomized = true;

	int objectID; //The In Game ID used to place the object EX:0x1F6

	bool isReward()
	{
		return CanBeReward(ItemTag);
	}
	static bool CanBeReward(std::string itemTag)
	{
		if (itemTag == "Jinjo" ||
			itemTag == "Jiggy" ||
			itemTag == "Honeycomb" ||
			itemTag == "Glowbo" ||
			itemTag == "Ticket" ||
			itemTag == "Doubloon" ||
			itemTag == "Cheato Page"||
			itemTag == "Move Item")
            return true;      
        return false;
	}

	public:RandomizedObject()
	{
	}

	public:RandomizedObject(Prop newData, int newFileIndex, int newAssociatedOffset)
	{
		this->Data = newData;
		this->fileIndex = newFileIndex;
		this->associatedOffset = newAssociatedOffset;
	}
};



#pragma once
#include <string>
#include <vector>
#include "HelperFunctions.h"
#include "Prop.h"
#include "Collectables.h"
#include "Props.h"
class RandomizedObject
{

public:
	static constexpr int JinjoColors[] = {0x07,0x05,0x05,0x08,0x05,0x03,0x08,0x06,0x08,0x07,0x08,0x05,0x02,0x01,0x04,0x04,0x03,0x05,0x05,0x00,0x06,0x03,0x08,0x04,0x03,0x08,0x04,0x08,0x07,0x08,0x04,0x01,0x02,0x02,0x06,0x07,0x06,0x07,0x08,0x07,0x06,0x07,0x07,0x06,0x06};

	int RewardObjectIndex = -1; //The index of the reward object associated with this object if there is one 
	Prop Data; //This is the raw data regarding the object
	int FileIndex = 0; //This should be the index in the main table
	int AssociatedOffset = 0; //The offset from the start of the file this data is located;
	std::string LocationName = "";
	int LevelIndex = -1; //The index of the level this object is in
	int MapID = -1; //The mapId that this object is in
	int RandoObjectID = -1;//The id of this object used for referencing specific objects
	std::string ItemTag = "";//This is the name of associated Item but for key usage like Jiggy or A Unique Id for a switch
	CollectableId collectableId = (CollectableId)0;
	int ItemAmount = 1;//This is the amount of the associated item this item counts for really only necessary for Notes due to Treble
	bool IsSpawnLocation = false;
	bool Randomized = true;
	int ObjectID = -1; //The In Game ID used to place the object EX:0x1F6

	//This is all move data stuff
	std::vector<int> RestrictedMoves; //The hexadecimal values relating to the moves that should not be placed at this location due to the move being necessary to get to this location
	std::string DialogData = ""; //This contains all data relevant to the dialog changes for this object
	int Ability = -1; //The ability value used when setting abilities (used for most items)
	std::string MoveType = "Silo"; //How to retrieve and use the associated data so silos have their dialogue moved to new silos but individuals just have the ability number used
	std::string MoveName = "";
	int ScriptOffset=0;

	static bool CanBeReward(CollectableId itemId)
	{
		if (itemId == Collect_White_Jinjo ||
			itemId == Collect_Orange_Jinjo ||
			itemId == Collect_Yellow_Jinjo ||
			itemId == Collect_Brown_Jinjo ||
			itemId == Collect_Green_Jinjo ||
			itemId == Collect_Red_Jinjo ||
			itemId == Collect_Blue_Jinjo ||
			itemId == Collect_Purple_Jinjo ||
			itemId == Collect_Black_Jinjo ||
			itemId == Collect_Jiggy ||
			itemId == Collect_Honeycomb ||
			itemId == Collect_Glowbo ||
			itemId == Collect_Ticket ||
			itemId == Collect_Doubloon ||
			itemId == Collect_Cheato_Page||
			itemId == Collect_Move_Item)
            return true;      
        return false;
	}

	public:RandomizedObject()
	{
	}

	public:RandomizedObject(Prop newData, int newFileIndex, int newAssociatedOffset)
	{
		this->Data = newData;
		this->FileIndex = newFileIndex;
		this->AssociatedOffset = newAssociatedOffset;
	}

	int getItemType() const
	{
		switch (ObjectID)
		{
			case Prop_Jinjo: //Jinjo
				return 0;
			case Prop_Jiggy: //Jiggy
				return 1;
			case Prop_Honeycomb: //Honeycomb
				return 2;
			case Prop_Glowbo: //Glowbo
				return 3;
			case Prop_JadeTotem: //Jade Totem
				return 5;
			case Prop_Ticket: //Ticket
				return 8;
			case Prop_Doubloon: //Doubloon
				return 7;
			case Prop_Note: //Note
				return 6;
			case Prop_Treble_Clef: //Treble
				return 6;
			case Prop_CheatoPage: //Cheato
				return 4;
			case Prop_CUSTOM_MOVE_ITEM: //Move Item
				return 9;
		}
		
		return ObjectID;
	}

	int getJinjoColor() const
	{
		if (ItemTag == "Jinjo" || Data.FlagOrRotation < 45)
			return JinjoColors[Data.FlagOrRotation];
		else
			return -1;
	}

	bool thisCanBeReward() const
	{
		return CanBeReward(collectableId);
	}
	//Is this Location valid for the given item for the default settings
	bool isValidLocation(RandomizedObject& object)
	{
		if (IsSpawnLocation && object.thisCanBeReward())
		{
			return false;
		}
		if (object.getItemType() == 6 && object.LevelIndex != LevelIndex)
		{
			return false;
		}
		return true;
	}
	bool isVirtualObject () const
	{
		if (this->AssociatedOffset == -1)
			return true;
		else return false;
	}

	virtual void Deserialize(std::string rawdata)
	{
		char* endPtr = nullptr;

		std::string mapIDStr = GetStringAfterTag(rawdata, "MapID:", ",");
		MapID = !mapIDStr.empty() ? strtol(mapIDStr.c_str(), &endPtr, 16) : 1;

		std::string objectType = GetStringAfterTag(rawdata, "ObjectType:", ","); //Try and get the object type which should only be defined for virtual items this is the actual in game object id
		ObjectID = !objectType.empty() ? strtol(objectType.c_str(), &endPtr, 16) : -1;

		if (ObjectID != -1)
		{
			std::string AssociatedFlag = GetStringAfterTag(rawdata, "AssociatedFlag:", ",");
			int flag = strtol(AssociatedFlag.c_str(), &endPtr, 16);
			
			Data.position[0] = 0;
			Data.position[1] = 0;
			Data.position[2] = 0;
			//This is handled differently because editing the gccollect stuff to add a new item is a lot harder than just using unk6_7
			if (ObjectID == Prop_CUSTOM_MOVE_ITEM)
			{
				Data.unk6_7 = flag;
				Ability = flag;
				Data.FlagOrRotation = 0;
				Data.unkC_0 = 0x16;
			}
			else
			{
				Data.unk6_7 = 0x32;
				Data.FlagOrRotation = flag;
				Data.unkC_0 = 0x64;
			}
			Data.unk6_1 = 0x6;
			Data.unk6_0 = 0;
			Data.ItemID = ObjectID;

			Data.unkA = 0;

			Data.unk10 = 0x760;
			Data.unk12 = 0x100;
		}
		std::string levelOffset = GetStringAfterTag(rawdata, "ObjectOffset:", ",");
		AssociatedOffset = !levelOffset.empty() ? strtol(levelOffset.c_str(), &endPtr, 16) : -1;

		//Ported from the MoveObjects because Associated Scripts has too much baggage
		std::string scriptOffset = GetStringAfterTag(rawdata, "ScriptOffset:", ",");
		ScriptOffset = !scriptOffset.empty() ? strtol(scriptOffset.c_str(), &endPtr, 16) : -1;
		
		std::string randomize = GetStringAfterTag(rawdata, "Randomized:", ",");
		
		std::string randoObjectIDStr = GetStringAfterTag(rawdata, "ObjectId:", ",");//Get the id used as a unique identifier for a specific object in the randomization list
		RandoObjectID = !randoObjectIDStr.empty() ? strtol(randoObjectIDStr.c_str(), &endPtr, 16) : -1;
		ItemTag = GetStringAfterTag(rawdata, "ItemTag:\"", "\",");//The tag saying what this item type is
		std::string ItemAmountStr = GetStringAfterTag(rawdata, "ItemAmount:", ",");
		ItemAmount = !ItemAmountStr.empty() ? strtol(ItemAmountStr.c_str(), &endPtr, 10) : 1;
		
		LocationName = GetStringAfterTag(rawdata, "ObjectName:\"", "\",");

		if (randomize == "false")
		{
			Randomized = false;
		}
		
		std::string moveName = GetStringAfterTag(rawdata, "MoveName:\"", "\","); //The name of the move
		std::string moveType = GetStringAfterTag(rawdata, "MoveSource:", ","); //The type of move this is. if it's from a silo the dialogue needs to be moved to the other silo
		std::string moveRestrictions = GetStringAfterTag(rawdata, "RestrictedMoves:[", "],"); //Moves that should not be set a this location
		std::string dialogData = GetStringAfterTag(rawdata, "DialogData:{", "},"); //The data for the associated dialog changes

		std::vector<int> moveRestrictionsList = GetIntVectorFromString(moveRestrictions, ",");
		MoveName = moveName;
		MoveType = moveType;
		RestrictedMoves = moveRestrictionsList;
		DialogData = dialogData;

	}

	static int GetObjectIdFromAbility(int ability,std::vector<RandomizedObject>& objects)
	{
		for (auto& object : objects)
		{
			if (ability == object.Ability)
			{
				return object.RandoObjectID;
			}
		}
	}

};



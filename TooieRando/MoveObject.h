#pragma once

#include <string>

#include "HelperFunctions.h"
#include <vector>

class MoveObject
{
public:
	std::vector<int> restrictedMoves; //The hexadecimal values relating to the moves that should not be placed at this location due to the move being necessary to get to this location
	std::vector<unsigned char> Data; //This is the raw data regarding the silodata
	std::string scriptAddress = ""; //This should be the index in the main table
	int fileIndex = 0; //This should be the index in the main table
	std::string dialogData = ""; //This contains all data relevant to the dialog changes for this object
	int associatedOffset = 0; //The offset from the start of the file this data is located
	int Ability = 0; //The ability value used when setting abilities (used for most items)
	std::string MoveType = "Silo"; //How to retrieve and use the associated data so silos have their dialogue moved to new silos but individuals just have the ability number used
	std::string MoveName = "";
	/// <summary>
	/// The unique ID value used in the randomizer to represent this move location
	/// </summary>
	int MoveID = 0;
	bool randomized = true;
	MoveObject()
	{

	}
	MoveObject(std::vector<unsigned char> newData, int newFileIndex, int newAssociatedOffset)
	{
		this->Data = newData;
		this->fileIndex = newFileIndex;
		this->associatedOffset = newAssociatedOffset;
	}

	static MoveObject Deserialize(std::string rawdata)
	{

		char* endPtr = nullptr;
		MoveObject newObject = MoveObject();

		std::string scriptAddress = GetStringAfterTag(rawdata, "AssociatedScript:", ","); //The relative address from the start of the script asset table to the pointer to the start of the file
		std::string scriptOffset = GetStringAfterTag(rawdata, "ScriptOffset:", ","); //Offset from the start of the script where the data for the silo is held
		std::string MoveName = GetStringAfterTag(rawdata, "MoveName:\"", "\","); //The name of the move
		std::string Ability = GetStringAfterTag(rawdata, "AbilityValue:", ","); //Value used by the GiveAbility Function to give the ability to the player
		std::string MoveType = GetStringAfterTag(rawdata, "MoveSource:", ","); //The type of move this is. if it's from a silo the dialogue needs to be moved to the other silo
		std::string MoveRestrictions = GetStringAfterTag(rawdata, "RestrictedMoves:[", "],"); //Moves that should not be set a this location
		std::string DialogData = GetStringAfterTag(rawdata, "DialogData:{", "},"); //The data for the associated dialog changes
		std::string MoveIdStr = GetStringAfterTag(rawdata, "MoveID:", ","); //The move ID for reference in logic
		std::string randomize = GetStringAfterTag(rawdata, "Randomized:", ",");

		std::vector<int> moveRestrictions = GetIntVectorFromString(MoveRestrictions, ",");
		int MoveID = strtol(MoveIdStr.c_str(), &endPtr, 16);
		newObject.Ability = strtol(Ability.c_str(), &endPtr, 16);
		newObject.MoveName = MoveName;
		newObject.MoveType = MoveType;
		newObject.restrictedMoves = moveRestrictions;
		newObject.dialogData = DialogData;
		newObject.MoveID = MoveID;
		newObject.randomized = (randomize != "false");
		newObject.associatedOffset = scriptOffset.size() > 0 ? strtol(scriptOffset.c_str(), &endPtr, 16) : 0;
		newObject.scriptAddress = scriptAddress;
		return newObject;
	}

};

#pragma once

#include <string>
#include <vector>

class MoveObject
{
public:
	std::vector<int> restrictedMoves; //The hexadecimal values relating to the moves that should not be placed at this location due to the move being necessary to get to this location
	std::vector<unsigned char> Data; //This is the raw data regarding the silodata
	int fileIndex = 0; //This should be the index in the main table
	std::string dialogData = ""; //This contains all data relevant to the dialog changes for this object
	int associatedOffset = 0; //The offset from the start of the file this data is located
	int Ability = 0; //The ability value used when setting abilities (used for most items)
	std::string MoveType = "Silo"; //How to retrieve and use the associated data so silos have their dialogue moved to new silos but individuals just have the ability number used
	std::string MoveName = "";
	int MoveID = 0;

	MoveObject()
	{

	}
	MoveObject(std::vector<unsigned char> newData, int newFileIndex, int newAssociatedOffset)
	{
		this->Data = newData;
		this->fileIndex = newFileIndex;
		this->associatedOffset = newAssociatedOffset;
	}
};

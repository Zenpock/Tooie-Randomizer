#pragma once
#include <string>
#include <vector>
class RandomizedObject
{
public:
	int rewardObjectIndex = -1; //The index of the reward object associated with this object if there is one 
	std::vector<unsigned char> Data; //This is the raw data regarding the silodata
	int fileIndex = 0; //This should be the index in the main table
	int associatedOffset = 0; //The offset from the start of the file this data is located;
	std::string LocationName = "";

public:RandomizedObject(std::vector<unsigned char> newData, int newFileIndex, int newAssociatedOffset)
{
	this->Data = newData;
	this->fileIndex = newFileIndex;
	this->associatedOffset = newAssociatedOffset;
}
};

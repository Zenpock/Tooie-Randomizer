#pragma once
#include <string>
#include <vector>
#include "RandomizedObject.h"
#include "HelperFunctions.h"
#include <unordered_map>
#include <fstream>

class LogicGroup
{
	
private:
	static int lastGroupID;
public:
	class RequirementSet
	{
		public:
		std::string SetName = "Default";
		std::vector<int> RequiredAbilities;
		std::vector<std::string> RequiredItems;
		std::vector<int> RequiredItemsCount;
		std::vector<std::string> RequiredKeys;//Unique Identifiers used to indicate whether a switch or something has been set, used for mumbo and humba flags in levels
	};
	int DependentShuffleGroup = -1; //This refers to the shufflegroups for entrances which this logic group will find one of the entrances within and make it a dependent group
	int AssociatedWarp = -1; //The EntranceID that unlocks this group
	std::string GroupName = "Group";
	std::string SpecialTag = "";//This is a tag used to edit specific groups when randomizing. e.g. changing the note requirement for certain moves/adjusting jiggy requirements based on options
	std::vector<RequirementSet> Requirements;//Group distinct requirements together, so if you can achieve a goal with grenade eggs or bill drill you would make 2 different requirement sets
	std::vector<int> objectIDsInGroup; //Stores the object IDs in the group
	std::vector<int> dependentGroupIDs; //Stores the group IDs in the group
	int containedMove = -1; //The Move ID that is contained within this group. There aren't enough moves in the game for it to matter that you can only have 1 move in a group.
	std::string key; //The key rewarded after the group has fulfilled at least 1 requirement set
	int GroupID = -1;
	static LogicGroup GetLogicGroupFromGroupId(int groupID, std::unordered_map<int,LogicGroup>& logicGroups);

	LogicGroup();
	LogicGroup(int groupID);

	static LogicGroup getGroupFromString(std::string stringRepresentation)
	{
		int GroupID = 0;
		char* endPtr;
		std::string GroupIdStr = GetStringAfterTag(stringRepresentation, "GroupId:", ",");
		GroupID = !GroupIdStr.empty() ? strtol(GroupIdStr.c_str(), &endPtr, 16) : -1; //If there is a script reward index
		std::string GroupName = GetStringAfterTag(stringRepresentation, "GroupName:\"", "\",");
		std::string ObjectsInGroupStr = GetStringAfterTag(stringRepresentation, "ObjectsInGroup:[", "],");
		std::string DependentGroupStr = GetStringAfterTag(stringRepresentation, "DependentGroups:[", "],");

		std::string Requirements = GetStringAfterTag(stringRepresentation, "Requirements:[{", "}],");

		LogicGroup NewGroup = LogicGroup(GroupID);
		NewGroup.GroupName = GroupName;
		std::vector<std::string> RequirementsVector = GetVectorFromString(Requirements, "},{");
		for (int i = 0; i < RequirementsVector.size(); i++)
		{
			LogicGroup::RequirementSet requirementSet;
			requirementSet.SetName = GetStringAfterTag(RequirementsVector[i], "SetName:\"", "\",");
			std::string ItemCountStr = GetStringAfterTag(RequirementsVector[i], "RequiredItemCounts:[", "],");
			std::string ItemsStr = GetStringAfterTag(RequirementsVector[i], "RequiredItem:[", "],");
			std::string RequiredMoveStr = GetStringAfterTag(RequirementsVector[i], "RequiredMoves:[", "],");
			std::string RequiredKeysStr = GetStringAfterTag(RequirementsVector[i], "RequiredKeys:[", "],");

			requirementSet.RequiredItems = GetVectorFromString(ItemsStr, ",");
			requirementSet.RequiredKeys = GetVectorFromString(RequiredKeysStr, ",");
			requirementSet.RequiredItemsCount = GetIntVectorFromString(ItemCountStr, ",");
			requirementSet.RequiredAbilities = GetIntVectorFromString(RequiredMoveStr, ",");
			NewGroup.Requirements.push_back(requirementSet);
		}
		NewGroup.key = GetStringAfterTag(stringRepresentation, "RewardKey:\"", "\",");

		std::string MoveID = GetStringAfterTag(stringRepresentation, "ContainedMove:", ",");
		NewGroup.containedMove = !MoveID.empty() ? strtol(MoveID.c_str(), &endPtr, 16) : -1;

		std::string ShuffleGroupStr = GetStringAfterTag(stringRepresentation, "DependentShuffleGroup:", ",");
		NewGroup.DependentShuffleGroup = !ShuffleGroupStr.empty() ? strtol(ShuffleGroupStr.c_str(), &endPtr, 16) : -1;
		std::string AssociatedWarpStr = GetStringAfterTag(stringRepresentation, "AssociatedWarp:", ",");
		NewGroup.AssociatedWarp = !AssociatedWarpStr.empty() ? strtol(AssociatedWarpStr.c_str(), &endPtr, 16) : -1;
		std::string SpecialTagStr = GetStringAfterTag(stringRepresentation, "SpecialTag:", ",");
		NewGroup.SpecialTag = SpecialTagStr;
		NewGroup.objectIDsInGroup = GetIntVectorFromString(ObjectsInGroupStr, ",");
		NewGroup.dependentGroupIDs = GetIntVectorFromString(DependentGroupStr, ",");
		return NewGroup;
	}

	/// <summary>
	/// Load Either Unordered or OrderedMap
	/// </summary>
	/// <typeparam name="MapType"></typeparam>
	/// <param name="logicGroups"></param>
	/// <param name="fileName"></param>
	template <typename MapType>
	static void LoadLogicGroupsFromFile(MapType& logicGroups, CString fileName)
	{
		char message[256];
		std::ifstream myfile(fileName);
		std::string line;
		try {
			if (!myfile.is_open()) {
				sprintf(message, "Error: Could not open the logic file: %s\n", fileName);
				throw std::runtime_error(message);
			}
		}
		catch (const std::exception& ex) {
			::MessageBox(NULL, ex.what(), "Error", NULL);
			return;
		}
		myfile.clear();
		myfile.seekg(0);

		if (myfile.peek() == std::ifstream::traits_type::eof()) {
			::MessageBox(NULL, "Error: The file is empty.", "Error", NULL);
			return;
		}
		logicGroups.clear();
		myfile.clear();
		myfile.seekg(0);
		while (std::getline(myfile, line)) // Read each line from the file
		{
			LogicGroup group = getGroupFromString(line);
			logicGroups[group.GroupID] = group;
		}
	}

	static std::vector<std::tuple<std::string, std::string, int>> LoadLogicFileOptions()
	{
		std::vector<std::tuple<std::string, std::string, int>> LogicFilePaths;

		std::ifstream myfile("LogicFiles.txt");
		std::string line;
		try {
			if (!myfile.is_open()) {
				throw std::runtime_error("Error: Could not open the file 'LogicFiles.txt'.");
			}
		}
		catch (const std::exception& ex) {
			::MessageBox(NULL, ex.what(), "Error", NULL);
			return LogicFilePaths;
		}
		char message[1024];
		myfile.clear();
		myfile.seekg(0);
		if (myfile.peek() == std::ifstream::traits_type::eof()) {
			::MessageBox(NULL, "Error: The file is empty.", "Error", NULL);
			return LogicFilePaths;
		}

		myfile.clear();
		myfile.seekg(0);
		char* endPtr;

		while (std::getline(myfile, line)) // Read each line from the file
		{
			std::string LogicName = GetStringAfterTag(line, "LogicName:", ",");
			std::string FileName = GetStringAfterTag(line, "FileName:", ",");//File name looks for the file in the Logic Folder
			std::string startGroupStr = GetStringAfterTag(line, "StartGroup:", ",");//Get starting group based on the group index
			int startGroup = !startGroupStr.empty() ? strtol(startGroupStr.c_str(), &endPtr, 16) : -1;
			LogicFilePaths.push_back(std::make_tuple(LogicName, FileName, startGroup));
		}
		myfile.close();
		return LogicFilePaths;
	}
};


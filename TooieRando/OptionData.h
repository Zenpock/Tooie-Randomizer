#pragma once
#include <string>
#include "HelperFunctions.h"
#include <vector>
#include <algorithm>

class OptionData
{
public:
	/// <summary>
	/// This is the type of option this refers to e.g. Flags, Multiselect, Value
	/// Used to determine how to make the option work like setting flags to true
	/// </summary>
	CString OptionType = "";

	/// <summary>
	/// What name will appear in the UI to indicate what this option does
	/// </summary>
	CString optionName;

	/// <summary>
	/// The value used when the option is toggled off
	/// </summary>
	CString defaultValue = "";

	/// <summary>
	/// The value used when the option is toggled on
	/// </summary>
	CString currentValue = "";

	/// <summary>
	/// The offset within the file to the location of the edit
	/// </summary>
	CString optionFileOffset = ""; //The offset within a script/map to place the edit
	
	/// <summary>
	/// Used for the offset from the start of the script table/or the Map ID for mapedits
	/// </summary>
	CString optionFileIndex = ""; //The script/map's index to actually find the associated script/map to edit

	/// <summary>
	/// The list of all of the selections when using a multiselect type option
	/// </summary>
	std::vector<std::string> possibleSelections; //For Enum or multiselect options determines the list of selections in a list
	
	/// <summary>
	/// Whether the functionality for this option will occur
	/// </summary>
	bool active = false;

	/// <summary>
	/// Does this option change any files or is it just options for how the randomization should go
	/// </summary>
	bool randomizerExclusive = false;

	/// <summary>
	/// All Flags that will be set when the option is enabled
	/// </summary>
	std::vector<int> flags;

	/// <summary>
	/// Custom script that will be executed in the gcgame
	/// </summary>
	std::string customCommands = "";

	/// <summary>
	/// Custom script that will be executed in the gcgame
	/// </summary>
	std::string logicKey = "";

	/// <summary>
	/// Whether the option will be visible in the ui
	/// </summary>
	bool hidden = false;

	/// <summary>
	/// Whether the option can be toggled on and off
	/// </summary>
	bool alwaysTrue = false;

	/// <summary>
	/// Id used to reference special options within the randomizer not running in the rom
	/// </summary>
	std::string lookupId = "";
	OptionData(CString OptionName)
	{
		this->optionName = OptionName;
		this->active = false;
	}

	int GetDefaultValueInt()
	{
		char* endPtr;
		return strtol(defaultValue, &endPtr, 10);
	}
	int GetCurrentValueInt()
	{
		char* endPtr;
		return strtol(currentValue, &endPtr, 10);
	}
	void SetCurrentValueInt(int newValue)
	{
		CString str;
		str.Format("%d", newValue);
		currentValue = str;
	}

	static OptionData Deserialize(std::string rawdata)
	{
		char* endPtr = nullptr;
		int pos = 0;
		//Read all of the option variables from the line
		std::string OptionName = GetStringAfterTag(rawdata, "OptionName:\"", "\",");
		std::string active = GetStringAfterTag(rawdata, "Active:", ",");
		std::string hidden = GetStringAfterTag(rawdata, "Hidden:", ",");
		std::string commands = GetStringAfterTag(rawdata, "Commands:{", "},");
		std::string lookupID = GetStringAfterTag(rawdata, "LookupID:\"", "\",");
		std::string optionType = GetStringAfterTag(rawdata, "OptionType:\"", "\",");
		std::string defaultValue = GetStringAfterTag(rawdata, "DefaultValue:\"", "\",");
		std::string currentValue = GetStringAfterTag(rawdata, "CurrentValue:\"", "\",");
		std::string fileOffset = GetStringAfterTag(rawdata, "FileOffset:{", "},");
		std::string scriptAddress = GetStringAfterTag(rawdata, "ScriptAddress:{", "},");
		std::string mapID = GetStringAfterTag(rawdata, "MapID:{", "},");
		std::string possibleSelections = GetStringAfterTag(rawdata, "PossibleSelections:[", "],");
		std::string randomizerExlusive = GetStringAfterTag(rawdata, "RandomizerExclusive:", ",");
		std::string logicKey = GetStringAfterTag(rawdata, "LogicKey:\"", "\",");
		std::string bonusData = GetStringAfterTag(rawdata, "BonusData:[", "],");

		commands.erase(std::remove(commands.begin(), commands.end(), ' '), commands.end());

		std::vector<int> flagsVector;
		flagsVector = GetIntVectorFromString(GetStringAfterTag(rawdata, "Flags:[", "],").c_str(), ",");
		OptionData newOption = OptionData(OptionName.c_str());

		//Connect the optiondata to the values from the line
		if (active == "always")
		{
			newOption.active = true;
			newOption.alwaysTrue = true;
		}
		else
		{
			newOption.active = active == "true";
			newOption.alwaysTrue = false;
		}
		newOption.randomizerExclusive = randomizerExlusive == "true";
		newOption.hidden = hidden == "true";
		newOption.flags = flagsVector;
		newOption.customCommands = commands;
		newOption.lookupId = lookupID;
		newOption.OptionType = (optionType.size() > 0) ? optionType.c_str() : "flags";
		if (!scriptAddress.empty())
			newOption.optionFileIndex = scriptAddress.c_str();
		else
			newOption.optionFileIndex = mapID.c_str();
		newOption.optionFileOffset = fileOffset.c_str();
		newOption.defaultValue = defaultValue.c_str();
		newOption.logicKey = logicKey;
		if (currentValue.size() > 0)
			newOption.currentValue = currentValue.c_str();
		else
			newOption.currentValue = defaultValue.c_str();

		std::vector<std::string> stringVector;
		stringVector = GetVectorFromString(possibleSelections, ",");
		for (int i = 0; i < stringVector.size(); i++)
		{
			newOption.possibleSelections.push_back(stringVector[i]);
		}
		return newOption;
	}
	static std::string Serialize(OptionData option)
	{
		std::string output="";
		output += "OptionName:\"" + option.optionName+"\",";
		output += "OptionType:\"" + option.OptionType + "\",";
		if (option.alwaysTrue)
		{
			output += "Active:always,";
		}
		else
		{
			output += "Active:" + (std::string)(option.active ? "true," : "false,");
		}

		output += "Flags:["+ intVectorToString(option.flags, ",") +"],";
		if (!option.lookupId.empty())
		{
			output += "LookupID:\"" + option.lookupId + "\",";
		}
		if (!option.logicKey.empty())
		{
			output += "LogicKey:\"" + option.logicKey + "\",";
		}
		if (!option.defaultValue.IsEmpty())
		{
			output += "DefaultValue:\"" + option.defaultValue + "\",";
		}
		if (!option.currentValue.IsEmpty())
		{
			output += "CurrentValue:\"" + option.currentValue + "\",";
		}
		if (option.OptionType == "mapedits")
		{
			output += "MapID:{" + option.optionFileIndex + "},";
		}
		else
		{
			if (!option.optionFileIndex.IsEmpty())
			{
				output += "ScriptAddress:{" + option.optionFileIndex + "},";
			}
		}
		if (option.possibleSelections.size()>0)
		{
			output += "PossibleSelections:[" + stringVectorToString(option.possibleSelections,",") + "],";
		}
		if (option.randomizerExclusive)
		{
			output += "RandomizerExclusive:true,";
		}
		if (!option.customCommands.empty())
		{
			output += "Commands:{" + option.customCommands + "},";
		}
		if (!option.optionFileOffset.IsEmpty())
		{
			output += "FileOffset:{" + option.optionFileOffset + "},";
		}
		return output;
	}
	static OptionData GetOption(std::string lookupID, std::vector<OptionData> options);
	static bool CheckOptionActive(std::string lookupID, std::vector<OptionData> options);
};

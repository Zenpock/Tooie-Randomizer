#pragma once
#include <string>
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
	std::vector<CString> possibleSelections; //For Enum or multiselect options determines the list of selections in a list
	
	/// <summary>
	/// Whether the functionality for this option will occur
	/// </summary>
	bool active = false;

	/// <summary>
	/// All Flags that will be set when the option is enabled
	/// </summary>
	std::vector<int> flags;

	/// <summary>
	/// Custom script that will be executed in the gcgame
	/// </summary>
	CString customCommands = "";

	/// <summary>
	/// Whether the option will be visible in the ui
	/// </summary>
	bool hidden = false;

	/// <summary>
	/// Id used to reference special options within the randomizer not running in the rom
	/// </summary>
	CString lookupId = "";
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
};

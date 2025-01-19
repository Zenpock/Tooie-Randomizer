
#include <string>
#include <vector>
#include <algorithm>

class OptionData
{
public:
	CString OptionType = "";
	CString optionName;
	CString defaultValue = ""; //Set by the options list
	CString currentValue = ""; //Set by the User
	CString scriptOffset = ""; //The offset within a script to place the edit
	CString scriptAddress = ""; //The script's index to actually find the associated script to edit
	std::vector<CString> possibleSelections; //For Enum or multiselect options determines the list of selections in a list
	bool active = false;
	std::vector<int> flags;
	CString customCommands = "";
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
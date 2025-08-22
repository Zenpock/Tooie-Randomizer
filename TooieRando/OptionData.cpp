#include "stdafx.h"
#include "OptionData.h"
OptionData OptionData::GetOption(CString lookupID,std::vector<OptionData> options)
{
	for (int i = 0; i < options.size(); i++)
	{
		if (options[i].lookupId == lookupID)
		{
			return options[i];
		}
	}
	char message[256];
	sprintf(message, "Special Command %s could not be found returning false\n", lookupID);
	//OutputDebugString(message);
	return OptionData("");
}
/// <summary>
/// Checks if the option with the provided ID has been set as active
/// </summary>
/// <returns></returns>
bool OptionData::CheckOptionActive(CString lookupID, std::vector<OptionData> options)
{
	return GetOption(lookupID, options).active;
}

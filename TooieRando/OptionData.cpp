#include "stdafx.h"
#include "OptionData.h"
OptionData OptionData::GetOption(std::string lookupID,std::vector<OptionData> options)
{
	for (int i = 0; i < options.size(); i++)
	{
		if (options[i].lookupId == lookupID)
		{
			return options[i];
		}
	}
	::MessageBox(NULL, ("Special Command "+lookupID+" could not be found returning false").c_str(), "Missing Option", MB_OK);
	return OptionData("");
}
/// <summary>
/// Checks if the option with the provided ID has been set as active
/// </summary>
/// <returns></returns>
bool OptionData::CheckOptionActive(std::string lookupID, std::vector<OptionData> options)
{
	OptionData optionRetrieved = GetOption(lookupID, options);
	return optionRetrieved.active;
}

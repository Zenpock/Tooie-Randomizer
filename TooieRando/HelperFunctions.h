#pragma once
#include <string>
#include <vector>
#include <iostream>

static unsigned long Flip32Bit(unsigned long inLong)
{
	return (((inLong & 0xFF000000) >> 24) | ((inLong & 0x00FF0000) >> 8) | ((inLong & 0x0000FF00) << 8) | ((inLong & 0x000000FF) << 24));
}

static float CharArrayToFloat(unsigned char* currentSpot)
{
	unsigned long tempLong = (Flip32Bit(*reinterpret_cast<unsigned long*> (currentSpot)));
	return (*reinterpret_cast<float*> (&tempLong));
}

static unsigned long CharArrayToLong(unsigned char* currentSpot)
{
	return Flip32Bit(*reinterpret_cast<unsigned long*> (currentSpot));
}

static unsigned short Flip16Bit(unsigned short ShortValue)
{
	return ((ShortValue >> 8) | ((ShortValue << 8)));
}

static unsigned short CharArrayToShort(unsigned char* currentSpot)
{
	return Flip16Bit(*reinterpret_cast<unsigned short*> (currentSpot));
}

static int HexToInt(char inChar)
{
	switch (inChar)
	{
	case '0':
		return 0;
	case '1':
		return 1;
	case '2':
		return 2;
	case '3':
		return 3;
	case '4':
		return 4;
	case '5':
		return 5;
	case '6':
		return 6;
	case '7':
		return 7;
	case '8':
		return 8;
	case '9':
		return 9;
	case 'A':
		return 10;
	case 'a':
		return 10;
	case 'B':
		return 11;
	case 'b':
		return 11;
	case 'C':
		return 12;
	case 'c':
		return 12;
	case 'D':
		return 13;
	case 'd':
		return 13;
	case 'E':
		return 14;
	case 'e':
		return 14;
	case 'F':
		return 15;
	case 'f':
		return 15;
	default:
		return 0;
	}
}

static unsigned long StringHexToLong(CString inString)
{
	int tempA = inString.GetLength();
	if (inString.GetLength() < 8)
	{
		CString tempStr = inString;
		inString = "";
		for (int x = 0; x < (8 - tempStr.GetLength()); x++)
		{
			inString = inString + "0";
		}
		inString = inString + tempStr;
	}
	char* b;
	b = inString.GetBuffer(0);
	unsigned long tempLong = 0;
	for (int x = 0; x < 8; x++)
	{
		char tempChar = b[x];
		int hexInt = HexToInt(tempChar);
		tempLong = tempLong | hexInt << ((7 - x) * 4);
	}
	return tempLong;
}

static unsigned char StringToUnsignedChar(CString inString)
{
	int tempA = inString.GetLength();
	if (inString.GetLength() < 2)
	{
		CString tempStr = inString;
		inString = "";
		for (int x = 0; x < (2 - tempStr.GetLength()); x++)
		{
			inString = inString + "0";
		}
		inString = inString + tempStr;
	}
	char* b;
	b = inString.GetBuffer(0);
	unsigned long tempLong = 0;
	for (int x = 0; x < 2; x++)
	{
		char tempChar = b[x];
		int hexInt = HexToInt(tempChar);
		tempLong = tempLong | hexInt << ((1 - x) * 4);
	}
	return (unsigned char)tempLong;
}


/// <summary>
/// Split a string into a string vector based on delimiter
/// </summary>
/// <param name="line"></param>
/// <param name=""></param>
/// <param name=""></param>
static std::vector<std::string> GetVectorFromString(std::string vectorString, std::string delimiter)
{
	char message[1024];

	std::vector<std::string> vectorOutput;
	std::string currentString = vectorString;
	int foundIndex = currentString.find(delimiter);
	while (foundIndex != -1)
	{
		std::string substring;
		substring = currentString.substr(0, foundIndex);
		vectorOutput.push_back(substring);
		currentString.replace(0, delimiter.length() + substring.length(), "");
		foundIndex = currentString.find(delimiter);
	}
	if (!currentString.empty())
		vectorOutput.push_back(currentString);
	return vectorOutput;

}

static std::vector<int> GetIntVectorFromString(std::string vectorString, std::string delimiter)
{
	char* endPtr;
	std::vector<int> intVector;
	std::vector<std::string> stringVector = GetVectorFromString(vectorString, delimiter);
	for (int i = 0; i < stringVector.size(); i++)
	{
		int flag = 0;
		sscanf(stringVector[i].c_str(), "%X", &flag);
		intVector.push_back(flag);
	}
	return intVector;
}

/// <summary>
/// Get the string within the line parameter that follows the given tag terminating with a comma
/// </summary>
/// <param name="line"></param>
/// <param name=""></param>
/// <param name=""></param>
static std::string GetStringAfterTag(std::string line, std::string tag, std::string endTag)
{
	std::string FoundString = "";
	if (line.find(tag) != -1)
	{
		char message[1024];
		int tagPosition = line.find(tag);
		int startReadPosition = tagPosition + tag.length();
		int nextCommaPosition = line.find(endTag, tagPosition);

		FoundString = line.substr(startReadPosition, nextCommaPosition - startReadPosition);
	}
	return FoundString;
}

static int AddElementToListCntrl(CListCtrl& list, std::string itemName)
{
	LVITEM lv;
	lv.iItem = list.GetItemCount();
	lv.iSubItem = 0;
	lv.pszText = "optionName";
	lv.mask = LVIF_TEXT;

	int item = list.InsertItem(&lv);

	list.SetItemText(item, 0, itemName.c_str());
	return item;
}

static std::string intVectorToString(std::vector<int> intVector, std::string delimiter)
{
	std::string outputString = "";
	for (int i = 0; i < intVector.size(); i++)
	{
		char hex_string[20];
		sprintf(hex_string, "%X", intVector[i]);
		outputString.append(hex_string);
		if (i != intVector.size() - 1)
			outputString.append(delimiter);
	}
	return outputString;
}

static std::string stringVectorToString(std::vector<std::string> stringVector, std::string delimiter)
{
	std::string outputString = "";
	for (int i = 0; i < stringVector.size(); i++)
	{
		outputString.append(stringVector[i]);
		if (i != stringVector.size() - 1)
			outputString.append(delimiter);
	}
	return outputString;
}

static bool FileExists(const std::string& fileName)
{
	if (FILE* file = fopen(fileName.c_str(), "r")) 
	{
		fclose(file);
		return true;
	}
	else 
	{
		return false;
	}
}
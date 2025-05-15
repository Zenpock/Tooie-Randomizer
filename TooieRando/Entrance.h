#pragma once
#include <string>



/// <summary>
/// This Class Refers to data About Entrances that go between different maps
/// </summary>
class Entrance
{
public:

	/// <summary>
	/// The name of the specific warp
	/// </summary>
	std::string EntranceName;

	/// <summary>
	/// The unique ID used for an entrance
	/// </summary>
	int EntranceID;

	/// <summary>
	/// The Id used when warping e.g. B8 for MT
	/// </summary>
	int WarpId;

	/// <summary>
	/// The specific loading zone used to warp
	/// </summary>
	int LoadingZoneId;

	/// <summary>
	/// The Id used to refer to an invidual map for getting the file e.g. 0AB3
	/// </summary>
	CString MapID;

	/// <summary>
	/// The offset from the start of the file pointing towards the map to load
	/// </summary>
	int fileOffset;

	/// <summary>
	/// The offset from the start of the file pointing towards the entrance to enter from
	/// </summary>
	int entranceOffset;

	/// <summary>
	/// The group amongst which this specific warp should be shuffled into -1 is unshuffled so for 
	/// making world order rando the warp from the outside to the inside would be labeled as unshuffled but the entrances inside would labeled as the same group
	/// I'm sorry my terminology is bad
	/// </summary>
	int shuffleGroup;

	/// <summary>
	/// Stores the index of the map/script to edit in the asset list so we don't have to look for it later
	/// </summary>
	int EntranceIndex;

	/// <summary>
	/// The id of the other side of this warp
	/// </summary>
	int ExitId;

	Entrance();

};


#include <vector>
#include "stdafx.h"

class HintDialog {
	public:
		int DialogID;
		int Level;
		int LineLengthOffset;
		std::string Name;
};

const std::vector<HintDialog> HintLocations = {
	{0xABA0,1,0xC,"MT Sslumber"},
	{0xABA4,1,0x9,"MT (Targitzan Temple) 1"},
	{0xABA8,1,0x9,"MT (Targitzan Temple) 2"},
	{0xABAC,3,0xD,"WW (Madame Grunty)"},
	{0xABB0,0,0xD,"IOH (Pine Grove)"},
	{0xABB4,1,0x9,"MT (By Quicksand)"},
	{0xABB8,3,0x9,"WW (Big Al's)"},
	{0xABBC,3,0x9,"WW (Salty Joe's)"},
	{0xABC0,3,0x9,"WW (Outside Cave of Horrors)"},
	{0xABC4,7,0x9,"HFP (By Hot Water)"},
	{0xABC8,0,0x9,"IOH (Pine Grove) 1"},
	{0xABCC,0,0x9,"IOH (Pine Grove) 2"},
	{0xABD0,0,0x9,"IOH (Wasteland)"},
	{0xABD4,0,0x9,"IOH (BEHIND JIGGYWIGGY) 1"}, //Behind Jiggywiggy
	{0xABD8,0,0x9,"IOH (BEHIND JIGGYWIGGY) 2"}, //Behind Jiggywiggy
	{0xABDC,0,0x9,"IOH (BEHIND JIGGYWIGGY) 3"}, //Behind Jiggywiggy
	//{0xABE0,5,0x9}, //T-REX ROAR CODE DO NOT USE
	{0xABEC,0,0x9,"IOH Plateau"},
	{0xABF0,0,0x9,"IOH Behind Heggys"},
	{0xABF4,2,0x9,"GGM Gen Cavern"},
	{0xABF8,0,0x9,"IOH JRL Entrance"},
	{0xABFC,5,0x9,"TDL (by the caged jiggy)"},
	{0xAC00,6,0x9,"GI (Top of Elevator) 1"},
	{0xAC04,6,0x9,"GI (Top of Elevator) 2"},
	{0xAC08,7,0x9,"HFP Inside the volcano 1"},
	{0xAC0C,7,0x9,"HFP Inside the volcano 2"},
	{0xAC10,7,0x9,"HFP Inside the volcano 3"},
	{0xAC14,0,0x9,"JV (Behind Grey House)"},
	{0xAC18,1,0x9,"MT (Goliath)"},
	{0xAC1C,3,0x5,"WW (Cactus of Strength)"},
	{0xAC20,4,0x9,"JRL (Smuggler's Cavern)"},
	{0xAC24,1,0x9,"MT (Prison Compound pillar room)"},
	{0xAC28,1,0x9,"MT (Mumbo)"},
	{0xAC2C,2,0x9,"Chuffy (Wagon)"},
	{0xAC30,2,0x9,"GGM (Gloomy Cavern)"},
	{0xAC34,2,0x9,"GGM (Toxic Gas Cave)"},
	{0xAC38,3,0x9,"WW (Pump Room)"},
	{0xAC3C,3,0x9,"WW (Outside Dodgem Dome)"},
	{0xAC40,3,0x9,"WW (GOBI Cage)"},
	{0xAC44,4,0x9,"JRL (Hole Above Pool)"},
	{0xAC48,4,0x9,"JRL (Seaweed Sanctum)"},
	{0xAC4C,5,0x9,"TDL (Inside mountain high up)"},
	{0xAC50,5,0x9,"TDL (River Passage)"},
	{0xAC54,6,0x9,"GI (Workers Quarters)"},
	{0xAC58,8,0x9,"CCL Central Cavern (Next to Jamjars)"},
	{0xAC5C,8,0x9,"CCL Central Cavern (Next to Pool)"},
	{0xAC60,8,0x9,"CCL Behind Humba"},
	//{0xAC64,0,0x6}, //Jiggywiggy
	//{0xAC68,0,0x6}, //Jiggywiggy
	//{0xAC6C,0,0x6}, //Jiggywiggy
	//{0xAC70,0,0x6}, //Jiggywiggy
	//{0xAC74,0,0x6}, //Jiggywiggy
	//{0xAC78,0,0x6}, //Jiggywiggy
	//{0xAC7C,0,0x6}, //Jiggywiggy
	//{0xAC80,0,0x6}, //Jiggywiggy
	//{0xAC84,0,0x6}, //Jiggywiggy
	{0xAC88,4,0x9,"JRL (Blubber)"},
	{0xAC8C,6,0x9,"GI (Outside)"},
	{0x9A24,9,0x5,"Grunty Blaster Warn"}, //Blaster Warn Grunty
	{0x9A30,9,0x5,"Grunty Clockwork Warn"} //Clockwork Warn Grunty
};
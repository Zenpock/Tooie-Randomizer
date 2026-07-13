#pragma once
#include "Props.h"
#include <set>

//Get the Item Type as used by the game e.g. 1 = Jiggy, 2 = Honeycomb
static int GetItemType(int propId)
{
	switch (propId)
	{
	case Prop_Jinjo: //Jinjo
		return 0;
	case Prop_Jiggy: //Jiggy
		return 1;
	case Prop_Honeycomb: //Honeycomb
		return 2;
	case Prop_Glowbo: //Glowbo
		return 3;
	case Prop_JadeTotem: //Jade Totem
		return 5;
	case Prop_Ticket: //Ticket
		return 8;
	case Prop_Doubloon: //Doubloon
		return 7;
	case Prop_CheatoPage: //Cheato
		return 4;
	case Prop_CUSTOM_MOVE_ITEM: //Move Item
		return 9;
	default:
		return -1;
	}
	return -1;
}

typedef enum collectables_e {
	Collect_Jinjo = 0x0,
	Collect_Jiggy = 0x1,
	Collect_Honeycomb = 0x2,
	Collect_Glowbo = 0x3,
	Collect_Cheato_Page = 0x4,
	Collect_Jade_Totem = 0x5,
	Collect_Note = 0x6,
	Collect_Doubloon = 0x7,
	Collect_Ticket = 0x8,
	Collect_Move_Item = 0x9,
	Collect_White_Jinjo = 0x10,
	Collect_Orange_Jinjo = 0x11,
	Collect_Yellow_Jinjo = 0x12,
	Collect_Brown_Jinjo = 0x13,
	Collect_Green_Jinjo = 0x14,
	Collect_Red_Jinjo = 0x15,
	Collect_Blue_Jinjo = 0x16,
	Collect_Purple_Jinjo = 0x17,
	Collect_Black_Jinjo = 0x18,
	Collect_Boggy_Fish = 0x20,
	Collect_Mega_Glowbo = 0x21,
	Collect_Treble = 0x22,
	Collect_Dragon_Kazooie = 0x23,
	Collect_Heal = 0x24,
	Collect_Stony = 0x25,
	Collect_Summon = 0x26,
	Collect_Detonator = 0x27,
	Collect_Levitate = 0x28,
	Collect_Van = 0x29,
	Collect_Power = 0x30,
	Collect_Submarine = 0x31,
	Collect_Oxygenate = 0x32,
	Collect_TRex = 0x33,
	Collect_Enlarge = 0x34,
	Collect_Washing_Machine = 0x35,
	Collect_EMP = 0x36,
	Collect_Snowball = 0x37,
	Collect_Life_Force = 0x38,
	Collect_Bee = 0x39,
	Collect_Rain_Dance = 0x40
} CollectableId;

typedef struct Collectable {
	std::string Name;
	CollectableId Id;
	PropId SpawnerPropId;
	//RealPropId should be the same as the spawner on any non spawning objects
	PropId RealPropId = SpawnerPropId;
	int ItemType = GetItemType(SpawnerPropId);
	bool operator < (const Collectable& other) const { return Id < other.Id; }
	bool operator==(const Collectable& other) const
	{
		return Id == other.Id && Name == other.Name && SpawnerPropId == other.SpawnerPropId;
	}
};

const std::set<Collectable> Collectables = {
	{"Note",Collect_Note,Prop_Note},
	{"Jinjo",Collect_Jinjo,Prop_Jinjo,Prop_Jinjo_Real},
	{"White Jinjo",Collect_White_Jinjo,Prop_Jinjo,Prop_Jinjo_Real},
	{"Orange Jinjo",Collect_Orange_Jinjo,Prop_Jinjo,Prop_Jinjo_Real},
	{"Yellow Jinjo",Collect_Yellow_Jinjo,Prop_Jinjo,Prop_Jinjo_Real},
	{"Brown Jinjo",Collect_Brown_Jinjo,Prop_Jinjo,Prop_Jinjo_Real},
	{"Green Jinjo",Collect_Green_Jinjo,Prop_Jinjo,Prop_Jinjo_Real},
	{"Red Jinjo",Collect_Red_Jinjo,Prop_Jinjo,Prop_Jinjo_Real},
	{"Blue Jinjo",Collect_Blue_Jinjo,Prop_Jinjo,Prop_Jinjo_Real},
	{"Purple Jinjo",Collect_Purple_Jinjo,Prop_Jinjo,Prop_Jinjo_Real},
	{"Black Jinjo",Collect_Black_Jinjo,Prop_Jinjo,Prop_Jinjo_Real},
	{"Boggy Fish",Collect_Boggy_Fish,Prop_BoggyFish},
	{"Jiggy",Collect_Jiggy,Prop_Jiggy,Prop_Jiggy_Real},
	{"Honeycomb",Collect_Honeycomb,Prop_Honeycomb,Prop_Honeycomb_Real},
	{"Glowbo",Collect_Glowbo,Prop_Glowbo,Prop_MegaGlowbo},
	{"Mega Glowbo",Collect_Mega_Glowbo,Prop_MegaGlowbo},
	{"Cheato Page",Collect_Cheato_Page,Prop_CheatoPage,Prop_CheatoPage_Real},
	{"Jade Totem",Collect_Jade_Totem,Prop_JadeTotem},
	{"Note Nest",Collect_Note,Prop_Note},
	{"Treble Clef",Collect_Treble,Prop_Treble_Clef},
	{"Doubloon",Collect_Doubloon,Prop_Doubloon,Prop_Doubloon_Real},
	{"Ticket",Collect_Ticket,Prop_Ticket,Prop_Ticket_Real},
	{"Move Item",Collect_Move_Item,Prop_CUSTOM_MOVE_ITEM},
	//Magic
	{"Dragon Kazooie",Collect_Dragon_Kazooie,Prop_MegaGlowbo},
	{"Heal",Collect_Heal,Prop_Glowbo,Prop_MegaGlowbo},
	{"Stony",Collect_Stony,Prop_Glowbo,Prop_MegaGlowbo},
	{"Summon",Collect_Summon,Prop_Glowbo,Prop_MegaGlowbo},
	{"Detonator",Collect_Detonator,Prop_Glowbo,Prop_MegaGlowbo},
	{"Levitate",Collect_Levitate,Prop_Glowbo,Prop_MegaGlowbo},
	{"Van",Collect_Van,Prop_Glowbo,Prop_MegaGlowbo},
	{"Power",Collect_Power,Prop_Glowbo,Prop_MegaGlowbo},
	{"Submarine",Collect_Submarine,Prop_Glowbo,Prop_MegaGlowbo},
	{"Oxygenate",Collect_Oxygenate,Prop_Glowbo,Prop_MegaGlowbo},
	{"T-Rex",Collect_TRex,Prop_Glowbo,Prop_MegaGlowbo},
	{"Enlarge",Collect_Enlarge,Prop_Glowbo,Prop_MegaGlowbo},
	{"Washing Machine",Collect_Washing_Machine,Prop_Glowbo,Prop_MegaGlowbo},
	{"EMP",Collect_EMP,Prop_Glowbo,Prop_MegaGlowbo},
	{"Snowball",Collect_Snowball,Prop_Glowbo,Prop_MegaGlowbo},
	{"Life Force",Collect_Life_Force,Prop_Glowbo,Prop_MegaGlowbo},
	{"Bee",Collect_Bee,Prop_Glowbo,Prop_MegaGlowbo},
	{"Rain Dance",Collect_Rain_Dance,Prop_Glowbo,Prop_MegaGlowbo}
};

static const Collectable& GetCollectibleFromName(std::string name)
{
	for (auto& collectable : Collectables)
	{
		if (collectable.Name == name)
			return collectable;
	}
	return {"",(CollectableId) - 1,(PropId) - 1};
}

static const Collectable& GetCollectibleFromPropId(PropId propId)
{
	for (auto& collectable : Collectables)
	{
		if (collectable.SpawnerPropId == propId)
			return collectable;
	}
	return { "",(CollectableId)-1,(PropId)-1 };
}

static const Collectable& GetCollectibleFromCollectibleId(CollectableId collectId)
{
	auto found = Collectables.find({"",collectId,(PropId)0});
	return *found;
}


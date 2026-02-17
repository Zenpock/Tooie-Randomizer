#pragma once
#include "Props.h"
#include <set>

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
	Collect_Treble = 0x22

} CollectableId;

typedef struct Collectable {
	std::string Name;
	CollectableId Id;
	PropId ObjectId;
	bool operator < (const Collectable& other) const { return Id < other.Id; }
	bool operator==(const Collectable& other) const
	{
		return Id == other.Id && Name == other.Name && ObjectId == other.ObjectId;
	}
};

const std::set<Collectable> Collectables = {
	{"Note",Collect_Note,Prop_Note},
	{"Jinjo",Collect_Jinjo,Prop_Jinjo},
	{"White Jinjo",Collect_White_Jinjo,Prop_Jinjo},
	{"Orange Jinjo",Collect_Orange_Jinjo,Prop_Jinjo},
	{"Yellow Jinjo",Collect_Yellow_Jinjo,Prop_Jinjo},
	{"Brown Jinjo",Collect_Brown_Jinjo,Prop_Jinjo},
	{"Green Jinjo",Collect_Green_Jinjo,Prop_Jinjo},
	{"Red Jinjo",Collect_Red_Jinjo,Prop_Jinjo},
	{"Blue Jinjo",Collect_Blue_Jinjo,Prop_Jinjo},
	{"Purple Jinjo",Collect_Purple_Jinjo,Prop_Jinjo},
	{"Black Jinjo",Collect_Black_Jinjo,Prop_Jinjo},
	{"Boggy Fish",Collect_Boggy_Fish,Prop_BoggyFish},
	{"Jiggy",Collect_Jiggy,Prop_Jiggy},
	{"Honeycomb",Collect_Honeycomb,Prop_Honeycomb},
	{"Glowbo",Collect_Glowbo,Prop_Glowbo},
	{"Mega Glowbo",Collect_Mega_Glowbo,Prop_MegaGlowbo},
	{"Cheato Page",Collect_Cheato_Page,Prop_CheatoPage},
	{"Jade Totem",Collect_Jade_Totem,Prop_JadeTotem},
	{"Note Nest",Collect_Note,Prop_Note},
	{"Treble Clef",Collect_Treble,Prop_Treble_Clef},
	{"Doubloon",Collect_Doubloon,Prop_Doubloon},
	{"Ticket",Collect_Ticket,Prop_Ticket},
	{"Move Item",Collect_Move_Item,Prop_CUSTOM_MOVE_ITEM}
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

static const Collectable& GetCollectibleFromObjectId(PropId objectId)
{
	for (auto& collectable : Collectables)
	{
		if (collectable.ObjectId == objectId)
			return collectable;
	}
	return { "",(CollectableId)-1,(PropId)-1 };
}

static const Collectable& GetCollectibleFromCollectibleId(CollectableId collectId)
{
	auto found = Collectables.find({"",collectId,(PropId)0});
	return *found;
}
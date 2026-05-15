// PlannedItemsMenu.cpp : implementation file
//

#include "stdafx.h"
#include "TooieRando.h"
#include "afxdialogex.h"
#include "PlannedItemsMenu.h"
#include "TooieRandoDlg.h"
#include "Worlds.h"


// PlannedItemsMenu dialog

IMPLEMENT_DYNAMIC(PlannedItemsMenu, CDialogEx)

PlannedItemsMenu::PlannedItemsMenu(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PLANNED_ITEMS_MENU, pParent)
{

}

PlannedItemsMenu::~PlannedItemsMenu()
{
}

void PlannedItemsMenu::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLACEDITEM_SELECT, placedItemSelector);
	DDX_Control(pDX, IDC_LOCATION_SELECT, locationSelector);
	DDX_Control(pDX, IDC_WORLD_ENT_SELECT, worldEntSelector);
	DDX_Control(pDX, IDC_WORLD_EXIT_SELECT, worldExitSelector);
	DDX_Control(pDX, IDC_ITEM_HINT_SELECT, itemHintSelector);
	DDX_Control(pDX, IDC_HINT_LOCATION_SELECT, hintLocationSelector);
}


BEGIN_MESSAGE_MAP(PlannedItemsMenu, CDialogEx)
	ON_CBN_SELCHANGE(IDC_LOCATION_SELECT, &PlannedItemsMenu::OnCbnSelchangeLocationSelect)
	ON_CBN_SELCHANGE(IDC_PLACEDITEM_SELECT, &PlannedItemsMenu::OnCbnSelchangePlaceditemSelect)
	ON_CBN_SELCHANGE(IDC_WORLD_ENT_SELECT, &PlannedItemsMenu::OnCbnSelchangeWorldEntSelect)
	ON_CBN_SELCHANGE(IDC_WORLD_EXIT_SELECT, &PlannedItemsMenu::OnCbnSelchangeWorldExitSelect)
	ON_CBN_SELCHANGE(IDC_ITEM_HINT_SELECT, &PlannedItemsMenu::OnCbnSelchangeItemHintSelect)
	ON_CBN_SELCHANGE(IDC_HINT_LOCATION_SELECT, &PlannedItemsMenu::OnCbnSelchangeHintLocationSelect)

END_MESSAGE_MAP()


BOOL PlannedItemsMenu::OnInitDialog()
{

	CDialog::OnInitDialog();

	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	pParentDlg->LoadObjects(false);
	pParentDlg->LoadEntrances(false);
	pParentDlg->LoadPlando();
	placedItemSelector.AddString("Random");
	placedItemSelector.SetItemData(placedItemSelector.GetCount() - 1, -1);
	itemHintSelector.AddString("Random");
	itemHintSelector.SetItemData(hintLocationSelector.GetCount() - 1, -1);

	for (auto& object : pParentDlg->RandomizedObjects)
	{
		if (!object.Randomized)
			continue;
		locationSelector.AddString(object.LocationName.c_str());
		locationSelector.SetItemData(locationSelector.GetCount() - 1, object.RandoObjectID);
		placedItemSelector.AddString(((object.MoveName.empty() ? object.ItemTag : object.MoveName) + " " + object.LocationName).c_str());
		placedItemSelector.SetItemData(placedItemSelector.GetCount() - 1, object.RandoObjectID);
		itemHintSelector.AddString(((object.MoveName.empty() ? object.ItemTag : object.MoveName) + " " + object.LocationName).c_str());
		itemHintSelector.SetItemData(itemHintSelector.GetCount() - 1, object.RandoObjectID);
	}

	worldExitSelector.AddString("Random");
	worldExitSelector.SetItemData(worldExitSelector.GetCount() - 1, -1);

	for (auto& entrance : pParentDlg->Entrances)
	{
		for (auto& world : WorldData)
		{
			if (entrance.EntranceID == world.EntrancePair.first)
			{
				worldEntSelector.AddString(entrance.EntranceName.c_str());
				worldEntSelector.SetItemData(worldEntSelector.GetCount() - 1, entrance.EntranceID);
				break;
			}
			if (entrance.EntranceID == world.EntrancePair.second)
			{
				worldExitSelector.AddString(entrance.EntranceName.c_str());
				worldExitSelector.SetItemData(worldExitSelector.GetCount() - 1, entrance.EntranceID);
				break;
			}
		}
	}

	for (auto& hintLocation : HintLocations)
	{
		hintLocationSelector.AddString(hintLocation.Name.c_str());
		hintLocationSelector.SetItemData(hintLocationSelector.GetCount() - 1, hintLocation.DialogID);
	}


	return true;
}


void PlannedItemsMenu::OnCbnSelchangeLocationSelect()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	bool found = false;
	for (auto& plannedPlacement : pParentDlg->plannedItems)
	{
		if (plannedPlacement.first == locationSelector.GetItemData(locationSelector.GetCurSel()))
		{
			for (int itemIndex = 0; itemIndex < placedItemSelector.GetCount(); itemIndex++)
			{
				
				if (placedItemSelector.GetItemData(itemIndex) == plannedPlacement.second)
				{
					placedItemSelector.SetCurSel(itemIndex);
					found = true;
					break;
				}
			}
			
			break;
		}
	}
	if (!found)
		placedItemSelector.SetCurSel(-1);
}

void PlannedItemsMenu::OnCbnSelchangePlaceditemSelect()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	int foundIndex = -1;
	for (int i = 0; i < pParentDlg->plannedItems.size();i++)//auto& plannedPlacement : pParentDlg->plannedItems)
	{
		if (pParentDlg->plannedItems[i].first == locationSelector.GetItemData(locationSelector.GetCurSel()))
		{
			pParentDlg->plannedItems[i].second = placedItemSelector.GetItemData(placedItemSelector.GetCurSel());
			foundIndex = i;
			break;
		}
	}
	if (foundIndex != -1 && pParentDlg->plannedItems[foundIndex].second == -1)
	{
		pParentDlg->plannedItems.erase(pParentDlg->plannedItems.begin()+foundIndex);
	}

	if (foundIndex == -1 && locationSelector.GetCurSel() != -1 && placedItemSelector.GetCurSel() != -1)
		pParentDlg->plannedItems.push_back(std::make_pair(locationSelector.GetItemData(locationSelector.GetCurSel()), placedItemSelector.GetItemData(placedItemSelector.GetCurSel())));
	SerializePlannedSettings();
}

void PlannedItemsMenu::OnCbnSelchangeWorldEntSelect()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	bool found = false;
	for (auto& plannedPlacement : pParentDlg->plannedWarps)
	{
		if (plannedPlacement.first == worldEntSelector.GetItemData(worldEntSelector.GetCurSel()))
		{
			for (int itemIndex = 0; itemIndex < worldExitSelector.GetCount(); itemIndex++)
			{

				if (worldExitSelector.GetItemData(itemIndex) == plannedPlacement.second)
				{
					worldExitSelector.SetCurSel(itemIndex);
					found = true;
					break;
				}
			}

			break;
		}
	}
	if (!found)
		worldExitSelector.SetCurSel(-1);

}

void PlannedItemsMenu::OnCbnSelchangeWorldExitSelect()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	int foundIndex = -1;
	for (int i = 0; i < pParentDlg->plannedWarps.size(); i++)
	{
		if (pParentDlg->plannedWarps[i].first == worldEntSelector.GetItemData(worldEntSelector.GetCurSel()))
		{
			pParentDlg->plannedWarps[i].second = worldExitSelector.GetItemData(worldExitSelector.GetCurSel());
			foundIndex = i;
			break;
		}
	}
	if (foundIndex != -1 && pParentDlg->plannedWarps[foundIndex].second == -1)
	{
		pParentDlg->plannedWarps.erase(pParentDlg->plannedWarps.begin() + foundIndex);
	}
	if (foundIndex == -1 && worldEntSelector.GetCurSel() != -1 && worldExitSelector.GetCurSel() != -1)
		pParentDlg->plannedWarps.push_back(std::make_pair(worldEntSelector.GetItemData(worldEntSelector.GetCurSel()), worldExitSelector.GetItemData(worldExitSelector.GetCurSel())));
	SerializePlannedSettings();
}

void PlannedItemsMenu::OnCbnSelchangeItemHintSelect()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	int foundIndex = -1;
	for (int i = 0; i < pParentDlg->plannedHints.size(); i++)
	{
		if (pParentDlg->plannedHints[i].second == hintLocationSelector.GetItemData(hintLocationSelector.GetCurSel()))
		{
			pParentDlg->plannedHints[i].first = itemHintSelector.GetItemData(itemHintSelector.GetCurSel());
			foundIndex = i;
			break;
		}
	}

	if (foundIndex != -1 && pParentDlg->plannedHints[foundIndex].first == -1)
	{
		pParentDlg->plannedHints.erase(pParentDlg->plannedHints.begin() + foundIndex);
	}

	if (foundIndex == -1 && hintLocationSelector.GetCurSel() != -1 && itemHintSelector.GetCurSel() != -1)
		pParentDlg->plannedHints.push_back(std::make_pair(itemHintSelector.GetItemData(itemHintSelector.GetCurSel()), hintLocationSelector.GetItemData(hintLocationSelector.GetCurSel())));
	SerializePlannedSettings();
}

void PlannedItemsMenu::OnCbnSelchangeHintLocationSelect()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	bool found = false;
	for (auto& plannedPlacement : pParentDlg->plannedHints)
	{
		if (plannedPlacement.second == hintLocationSelector.GetItemData(hintLocationSelector.GetCurSel()))
		{
			for (int itemIndex = 0; itemIndex < itemHintSelector.GetCount(); itemIndex++)
			{
				if (itemHintSelector.GetItemData(itemIndex) == plannedPlacement.first)
				{
					itemHintSelector.SetCurSel(itemIndex);
					found = true;
					break;
				}
			}
		}
	}
	if (!found)
		itemHintSelector.SetCurSel(-1);
}

void PlannedItemsMenu::SerializePlannedSettings()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();

	FILE* outFile = fopen(PlandoSettingsFile, "wb");
	if (outFile == NULL)
	{
		MessageBox("Cannot open Plando Settings file");
		return;
	}

	for (auto const& plannedItem : pParentDlg->plannedItems)
	{
		std::string optionString = "Location:"+IntToHexString(plannedItem.first)+",Item:"+IntToHexString(plannedItem.second) +",";
		fwrite(optionString.c_str(), 1, strlen(optionString.c_str()), outFile);
		fwrite("\n", 1, 1, outFile);
	}
	for (auto const& plannedItem : pParentDlg->plannedWarps)
	{
		std::string optionString = "WarpEnt:" + IntToHexString(plannedItem.first) + ",WarpEx:" + IntToHexString(plannedItem.second) + ",";
		fwrite(optionString.c_str(), 1, strlen(optionString.c_str()), outFile);
		fwrite("\n", 1, 1, outFile);
	}
	for (auto const& plannedItem : pParentDlg->plannedHints)
	{
		std::string optionString = "HintedItem:" + IntToHexString(plannedItem.first) + ",HintedDialog:" + IntToHexString(plannedItem.second) + ",";
		fwrite(optionString.c_str(), 1, strlen(optionString.c_str()), outFile);
		fwrite("\n", 1, 1, outFile);
	}
	fclose(outFile);
}

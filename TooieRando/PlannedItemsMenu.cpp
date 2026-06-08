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

	ON_BN_CLICKED(IDC_RESET_PLANDO, &PlannedItemsMenu::OnBnClickedResetPlando)
	ON_BN_CLICKED(IDC_IMPORT_PLANNED, &PlannedItemsMenu::OnBnClickedImportPlanned)
	ON_BN_CLICKED(IDC_EXPORT_PLANNED, &PlannedItemsMenu::OnBnClickedExportPlanned)
END_MESSAGE_MAP()


BOOL PlannedItemsMenu::OnInitDialog()
{

	CDialog::OnInitDialog();

	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	if(pParentDlg->RandomizedObjects.empty())
		pParentDlg->LoadObjects(false);
	if (pParentDlg->Entrances.empty())
		pParentDlg->LoadEntrances(false);
	pParentDlg->LoadPlando();
	RecreateSelects();

	return true;
}


void PlannedItemsMenu::OnCbnSelchangeLocationSelect()
{
	RecreatePlaceableItems();

	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	placedItemSelector.SetCurSel(GetPlannedForLocation(locationSelector.GetItemData(locationSelector.GetCurSel()),placedItemSelector, pParentDlg->plannedItems));
}

int PlannedItemsMenu::GetPlannedForLocation(int location, CComboBox& selector, std::vector<std::pair<int, int>>& planned, bool indexByFirst)
{
	for (auto& plannedPlacement : planned)
	{
		if ((indexByFirst ? plannedPlacement.first : plannedPlacement.second) == location)
		{
			for (int itemIndex = 0; itemIndex < selector.GetCount(); itemIndex++)
			{
				if (selector.GetItemData(itemIndex) == (indexByFirst ? plannedPlacement.second:plannedPlacement.first))
				{
					return itemIndex;
				}
			}
			break;
		}
	}
	return 0;
}


void PlannedItemsMenu::OnCbnSelchangePlaceditemSelect()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	int foundIndex = -1;
	for (int i = 0; i < pParentDlg->plannedItems.size();i++)
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
	RecreateSelects();
}

void PlannedItemsMenu::OnCbnSelchangeWorldEntSelect()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	worldExitSelector.SetCurSel(GetPlannedForLocation(worldEntSelector.GetItemData(worldEntSelector.GetCurSel()), worldExitSelector, pParentDlg->plannedWarps));
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
	RecreateSelects();
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
	RecreateSelects();
}

void PlannedItemsMenu::OnCbnSelchangeHintLocationSelect()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	itemHintSelector.SetCurSel(GetPlannedForLocation(hintLocationSelector.GetItemData(hintLocationSelector.GetCurSel()), itemHintSelector, pParentDlg->plannedHints,false));
}

void PlannedItemsMenu::SerializePlannedSettings(std::string path)
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();

	FILE* outFile = fopen(path.c_str(), "wb");
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

void PlannedItemsMenu::OnBnClickedResetPlando()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();

	pParentDlg->plannedItems.clear();
	pParentDlg->plannedWarps.clear();
	pParentDlg->plannedHints.clear();

	placedItemSelector.SetCurSel(-1);
	worldExitSelector.SetCurSel(-1);
	itemHintSelector.SetCurSel(-1);

	SerializePlannedSettings();
	RecreateSelects();
}

void PlannedItemsMenu::OnBnClickedImportPlanned()
{
	CString fileOpen;
	CFileDialog m_ldFile(TRUE, NULL, "PlannedSetup.txt", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "PlannedSetup (*.txt)|*.txt|", this);
	int didRead = m_ldFile.DoModal();
	if ((didRead == IDCANCEL) || (m_ldFile.GetPathName() == ""))
		return;

	if (didRead == FALSE)
		return;
	fileOpen = m_ldFile.GetPathName();

	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	pParentDlg->LoadPlando(fileOpen.GetString());
	SerializePlannedSettings();

	locationSelector.SetCurSel(-1);
	placedItemSelector.SetCurSel(-1);
	worldEntSelector.SetCurSel(-1);
	worldExitSelector.SetCurSel(-1);
	itemHintSelector.SetCurSel(-1);
	hintLocationSelector.SetCurSel(-1);

	RecreateSelects();
}

void PlannedItemsMenu::OnBnClickedExportPlanned()
{
	CString fileOpen;

	CFileDialog m_svFile(FALSE, NULL, (RandomizerOptionsFile), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "OUT PlannedSetup (*.txt)|*.txt|", this);

	int isFileOpened2 = m_svFile.DoModal();
	if (isFileOpened2 == IDCANCEL || m_svFile.GetFileName() == "")
		return;
	fileOpen = m_svFile.GetPathName();
	SerializePlannedSettings(fileOpen.GetString());
}

void PlannedItemsMenu::RecreateSelects() 
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();

	//Save all of the selection indices
	int locInd, entInd, extInd, hintInd,hintItemInd;
	locInd = locationSelector.GetCurSel();
	entInd = worldEntSelector.GetCurSel();
	extInd = worldExitSelector.GetCurSel();
	hintInd = hintLocationSelector.GetCurSel();
	hintItemInd = itemHintSelector.GetCurSel();

	locationSelector.ResetContent();
	worldEntSelector.ResetContent();
	worldExitSelector.ResetContent();
	itemHintSelector.ResetContent();
	hintLocationSelector.ResetContent();

	placedItemSelector.AddString("Random");
	placedItemSelector.SetItemData(placedItemSelector.GetCount() - 1, -1);
	itemHintSelector.AddString("Random");
	itemHintSelector.SetItemData(hintLocationSelector.GetCount() - 1, -1);

	for (auto& object : pParentDlg->RandomizedObjects)
	{
		if (!object.Randomized)
			continue;
		bool usedItem = false;
		bool usedLocation = false;
		bool usedForHint = false;
		for (auto& plannedItem : pParentDlg->plannedItems)
		{
			if (object.RandoObjectID == plannedItem.first)
			{
				usedLocation = true;
				//break;
			}
		}

		for (auto& plannedHint : pParentDlg->plannedHints)
		{
			if (object.RandoObjectID == plannedHint.first)
			{
				usedForHint = true;
				break;
			}
		}

		locationSelector.AddString(((usedLocation ? "*" : "") + object.LocationName).c_str());
		locationSelector.SetItemData(locationSelector.GetCount() - 1, object.RandoObjectID);
		itemHintSelector.AddString(((usedForHint?"*":"")+(object.MoveName.empty() ? object.ItemTag : object.MoveName) + " " + object.LocationName).c_str());
		itemHintSelector.SetItemData(itemHintSelector.GetCount() - 1, object.RandoObjectID);
	}

	worldExitSelector.AddString("Random");
	worldExitSelector.SetItemData(worldExitSelector.GetCount() - 1, -1);

	for (auto& entrance : pParentDlg->Entrances)
	{
		for (auto& world : WorldData)
		{
			bool usedEnt=false,usedExt=false;
			for (auto& plannedWarp : pParentDlg->plannedWarps)
			{
				if (entrance.EntranceID == plannedWarp.first)
				{
					usedEnt = true;
					break;
				}
				if (entrance.EntranceID == plannedWarp.second)
				{
					usedExt = true;
					break;
				}
			}

			if (entrance.EntranceID == world.EntrancePair.first)
			{

				worldEntSelector.AddString(((usedEnt ? "*" : "")+entrance.EntranceName).c_str());
				worldEntSelector.SetItemData(worldEntSelector.GetCount() - 1, entrance.EntranceID);
				break;
			}
			if (entrance.EntranceID == world.EntrancePair.second)
			{
				worldExitSelector.AddString(((usedExt ? "*" : "") + entrance.EntranceName).c_str());
				worldExitSelector.SetItemData(worldExitSelector.GetCount() - 1, entrance.EntranceID);
				break;
			}
		}
	}

	for (auto& hintLocation : HintLocations)
	{
		bool usedLocation = false;
		for (auto& plannedHint : pParentDlg->plannedHints)
		{
			if (hintLocation.DialogID == plannedHint.second)
			{
				usedLocation = true;
				break;
			}
		}
		hintLocationSelector.AddString(((usedLocation ? "*" : "") + hintLocation.Name).c_str());
		hintLocationSelector.SetItemData(hintLocationSelector.GetCount() - 1, hintLocation.DialogID);
	}

	//Restore the selections
	locationSelector.SetCurSel(locInd);
	worldEntSelector.SetCurSel(entInd);
	worldExitSelector.SetCurSel(extInd);
	hintLocationSelector.SetCurSel(hintInd);
	itemHintSelector.SetCurSel(hintItemInd);

	RecreatePlaceableItems(true);
}

//This should restrict the visible placeable items
void PlannedItemsMenu::RecreatePlaceableItems(bool keepIndex)
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();

	int itemInd = -1;
	int currentLocation = locationSelector.GetCurSel() != -1 ? locationSelector.GetItemData(locationSelector.GetCurSel()) : -1;
	
	itemInd = placedItemSelector.GetCurSel();
	placedItemSelector.ResetContent();

	if (currentLocation == -1)
	{
		placedItemSelector.EnableWindow(false);
		return;
	}
	placedItemSelector.EnableWindow(true);

	RandomizedObject& locationObject = pParentDlg->RandomizedObjects[pParentDlg->GetObjectFromID(currentLocation)];
	
	placedItemSelector.AddString("Random");
	placedItemSelector.SetItemData(placedItemSelector.GetCount() - 1, -1);

	for (auto& object : pParentDlg->RandomizedObjects)
	{
		//Hide all non randomized items
		if (!object.Randomized)
			continue;
		//Hide all notes that are invalid for the current level
		if(object.collectableId==Collect_Note && object.LevelIndex != locationObject.LevelIndex)
			continue;
		//Hide all non spawnable items in spawning locations
		if (locationObject.IsSpawnLocation && !object.thisCanBeReward())
			continue;
		bool usedItem = false;
		bool usedLocation = false;
		bool usedForHint = false;
		for (auto& plannedItem : pParentDlg->plannedItems)
		{
			if (object.RandoObjectID == plannedItem.second)
			{
				usedItem = true;
			}
		}
		placedItemSelector.AddString(((usedItem ? "*" : "") + (object.MoveName.empty() ? object.ItemTag : object.MoveName) + " " + object.LocationName).c_str());
		placedItemSelector.SetItemData(placedItemSelector.GetCount() - 1, object.RandoObjectID);
	}
	if(keepIndex)
	placedItemSelector.SetCurSel(itemInd);
}

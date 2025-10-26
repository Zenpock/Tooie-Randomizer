#include "stdafx.h"
#include "TooieRando.h"
#include "afxdialogex.h"
#include "LogicTracker.h"
#include "LogicGroup.h"
#include <map>
#include "Moves.h"
#include "TooieRandoDlg.h"
std::unordered_map<int, LogicGroup> logicGroups;
std::vector<std::pair<int,int>> openedWorlds; //World Order, Actual World
std::vector<std::pair<int, int>> availableChecks; //Check Type, Check ID
std::vector<std::pair<int, int>> markedChecks; //Check Type, Check ID
std::vector<std::pair<std::pair<int, std::string>, bool>> obtainedMoves;
std::vector<std::tuple<std::string, std::string, int>> LogicFilePaths;
static std::unordered_map<int, int> entranceAssociations;
std::string worlds[] = { "Not Found","Mayahem Temple" ,"Glitter Gulch Mine","Witchyworld","Jolly Rogers Lagoon" ,"Terrydactyland","Grunty Industries","Hailfire Peaks","Cloud Cuckooland","Cauldron Keep" };
std::vector<std::pair<int, std::string>> WorldEntrances = {
	{1,"World 1"},{3,"World 2"},{5,"World 3"},{7,"World 4"},{11,"World 5"},{15,"World 6"},{9,"World 7"},{13,"World 8"},{ 17,"World 9" }
};
std::vector<std::pair<int, std::string>> Worlds=
{
	{ 2,"MT" }, { 4,"GGM" }, { 6,"WW" }, { 8,"JRL" }, { 12,"TDL" }, { 16,"GI" }, { 10,"HFP" }, { 14,"CCL" }, { 18,"CK" }
};

IMPLEMENT_DYNAMIC(LogicTracker, CDialogEx)

LogicTracker::LogicTracker(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGIC_TRACKER, pParent)
{

}

LogicTracker::~LogicTracker()
{
}

void LogicTracker::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WORLDSOPENED, openedWorldsList);
	DDX_Control(pDX, IDC_COLLECTED_MOVES, foundMovesList);
	DDX_Control(pDX, IDC_MARKED_CHECKS, markedChecksList);
	DDX_Control(pDX, IDC_AVAILABLE_CHECKS, availableChecksList);
	DDX_Control(pDX, IDC_LOGIC_FILE, logicSelector);
}

BOOL LogicTracker::OnInitDialog()
{
	CDialog::OnInitDialog();

	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	pParentDlg->LoadObjects(false);
	pParentDlg->LoadMoves(false);

	openedWorldsList.InsertColumn(0, "World", LVCFMT_LEFT, 70);
	openedWorldsList.InsertColumn(1, "Found", LVCFMT_LEFT, 130);
	for (int i = 0; i < WorldEntrances.size(); i++)
	{
		int item = AddElementToListCntrl(openedWorldsList, worlds[i]);
		openedWorldsList.SetItemText(item, 0, WorldEntrances[i].second.c_str());
		openedWorlds.push_back(std::make_pair(WorldEntrances[i].first, -1));
		openedWorldsList.SetItemText(item, 1, "Not Found");
	}

	openedWorldsList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	foundMovesList.InsertColumn(0, "Move", LVCFMT_LEFT, 130);
	foundMovesList.InsertColumn(1, "Found", LVCFMT_LEFT, 70);
	
	for (int i = 0; i < Moves.size(); i++)
	{
		int item = AddElementToListCntrl(foundMovesList, Moves[i].second);
		foundMovesList.SetItemText(item, 1, "NO");
		obtainedMoves.push_back(std::make_pair(Moves[i], false));
	}
	foundMovesList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	LogicFilePaths = LogicGroup::LoadLogicFileOptions();
	for (int i = 0; i < LogicFilePaths.size(); i++)
	{
		logicSelector.AddString(std::get<0>(LogicFilePaths[i]).c_str());
	}
	markedChecksList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	availableChecksList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	availableChecksList.InsertColumn(0, "Locations", LVCFMT_LEFT, 250);
	markedChecksList.InsertColumn(0, "Locations", LVCFMT_LEFT, 250);

	return 0;
}


BEGIN_MESSAGE_MAP(LogicTracker, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_WORLDSOPENED, &LogicTracker::OnDblclkWorldsopened)
	ON_NOTIFY(NM_DBLCLK, IDC_COLLECTED_MOVES, &LogicTracker::OnDblclkCollectedMoves)
	ON_NOTIFY(NM_DBLCLK, IDC_MARKED_CHECKS, &LogicTracker::OnDblclkMarkedChecks)
	ON_NOTIFY(NM_DBLCLK, IDC_AVAILABLE_CHECKS, &LogicTracker::OnDblclkAvailableChecks)
	ON_CBN_SELCHANGE(IDC_LOGIC_FILE, &LogicTracker::OnSelchangeLogicFile)
END_MESSAGE_MAP()

void LogicTracker::OnDblclkWorldsopened(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pNMItemActivate->iItem != -1)
	{
		logicGroups[entranceAssociations[openedWorlds[pNMItemActivate->iItem].first]].dependentGroupIDs.clear();
		openedWorlds[pNMItemActivate->iItem].second = 
			openedWorlds[pNMItemActivate->iItem].second+1< worlds->size()? openedWorlds[pNMItemActivate->iItem].second + 1 : -1;
		if(openedWorlds[pNMItemActivate->iItem].second == -1)
		{ 
			openedWorldsList.SetItemText(pNMItemActivate->iItem, 1, "Not Found");
		}
		else
		{
			openedWorldsList.SetItemText(pNMItemActivate->iItem, 1, Worlds[openedWorlds[pNMItemActivate->iItem].second].second.c_str());
			logicGroups[entranceAssociations[openedWorlds[pNMItemActivate->iItem].first]].dependentGroupIDs.clear();
			logicGroups[entranceAssociations[openedWorlds[pNMItemActivate->iItem].first]].dependentGroupIDs.push_back(entranceAssociations[Worlds[openedWorlds[pNMItemActivate->iItem].second].first]);
		}
	}
	UpdateLists();
	*pResult = 0;
}

void LogicTracker::OnDblclkCollectedMoves(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pNMItemActivate->iItem != -1)
	{
		obtainedMoves[pNMItemActivate->iItem].second = !obtainedMoves[pNMItemActivate->iItem].second;
		foundMovesList.SetItemText(pNMItemActivate->iItem, 1, obtainedMoves[pNMItemActivate->iItem].second ? "YES" : "NO");
		UpdateLists();
	}	
	*pResult = 0;
}

void LogicTracker::OnDblclkMarkedChecks(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pNMItemActivate->iItem != -1)
	{
		markedChecks.erase(markedChecks.begin() + pNMItemActivate->iItem);
		UpdateLists();
	}
	*pResult = 0;
}

void LogicTracker::OnDblclkAvailableChecks(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pNMItemActivate->iItem != -1)
	{
		markedChecks.push_back(std::make_pair(availableChecks[pNMItemActivate->iItem].first, availableChecks[pNMItemActivate->iItem].second));
		UpdateLists();
	}	
	*pResult = 0;
}

void LogicTracker::OnSelchangeLogicFile()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();

	availableChecksList.DeleteAllItems();
	markedChecksList.DeleteAllItems();
	logicGroups.clear();
	LogicGroup::LoadLogicGroupsFromFile(logicGroups, std::get<1>(LogicFilePaths[logicSelector.GetCurSel()]).c_str());

	for (const auto& obj : logicGroups)
	{
		if (obj.second.AssociatedWarp != -1)
			entranceAssociations[obj.second.AssociatedWarp] = obj.first;
	}

	UpdateLists();
}
bool  LogicTracker::AlreadyMarked(int itemType,int checkID)
{
	for (int i = 0; i < markedChecks.size(); i++)
	{
		if(itemType == markedChecks[i].first && checkID == markedChecks[i].second)
			return true;
	}
	return false;
}
void  LogicTracker::UpdateLists()
{
	availableChecksList.DeleteAllItems();
	markedChecksList.DeleteAllItems();

	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();

	std::vector<int> lookedAt, next, viable;

	int startingLogicGroup = std::get<2>(LogicFilePaths[logicSelector.GetCurSel()]);
	LogicHandler::AccessibleThings startingState;
	for (int i = 0; i < obtainedMoves.size(); i++)
	{
		if (obtainedMoves[i].second)
		{
			MoveObject temp;
			temp.Ability = obtainedMoves[i].first.first;
			startingState.SetAbilities.push_back(std::make_pair(0, temp));
		}
	}

	for (int i = 0; i < openedWorlds.size(); i++)
	{
		if(openedWorlds[i].second!=-1)
		startingState.SetWarps.push_back(std::make_pair(openedWorlds[i].first,Worlds[openedWorlds[i].second].first));
	}

	//Set the collectable values to absurd numbers to skip any inventory based requirements
	startingState.keepCollectables = true;
	startingState.AddCollectable("Note", 5000);
	startingState.AddCollectable("Jiggy", 5000);
	startingState.AddCollectable("Glowbo", 5000);
	startingState.AddCollectable("Jinjo", 5000);
	startingState.AddCollectable("Cheato Page", 5000);
	startingState.AddCollectable("Mega Glowbo", 5000);
	startingState.AddCollectable("Ticket", 5000);
	startingState.AddCollectable("Honeycomb", 5000);

	availableChecks.clear();
	
	LogicHandler::AccessibleThings newState = LogicHandler::GetAllTotals(logicGroups[startingLogicGroup], logicGroups, startingState, pParentDlg->RandomizedObjects, pParentDlg->MoveObjects, lookedAt, next, viable);
	
	//Look for any item locations not already marked and add them to the available checks
	for (int i = 0; i < newState.ItemLocations.size(); i++)
	{
		if (AlreadyMarked(0, newState.ItemLocations[i]) == false)
		{
			availableChecks.push_back(std::make_pair(0, newState.ItemLocations[i]));
		}
	}
	
	//Look for any ability locations not already marked and add them to the available checks
	for (int i = 0; i < newState.AbilityLocations.size(); i++)
	{
		if (AlreadyMarked(1, newState.AbilityLocations[i].MoveID) == false)
		{
			availableChecks.push_back(std::make_pair(1, newState.AbilityLocations[i].MoveID));
		}
	}
	
	//Populate Available Checks List
	for (int i = 0; i < availableChecks.size(); i++)
	{
		switch (availableChecks[i].first)
		{
		case 0:
			AddElementToListCntrl(availableChecksList, pParentDlg->RandomizedObjects[pParentDlg->GetObjectFromID(availableChecks[i].second)].LocationName);
			break;
		case 1:
			AddElementToListCntrl(availableChecksList, pParentDlg->MoveObjects[pParentDlg->GetMoveFromID(availableChecks[i].second)].MoveName);
			break;
		}
	}
	//Populate Marked Checks List
	for (int i = 0; i < markedChecks.size(); i++)
	{
		switch (markedChecks[i].first)
		{
		case 0:
			AddElementToListCntrl(markedChecksList, pParentDlg->RandomizedObjects[pParentDlg->GetObjectFromID(markedChecks[i].second)].LocationName);
			break;
		case 1:
			AddElementToListCntrl(markedChecksList, pParentDlg->MoveObjects[pParentDlg->GetMoveFromID(markedChecks[i].second)].MoveName);
			break;
		}
	}
}
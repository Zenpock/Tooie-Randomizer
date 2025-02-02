#include "stdafx.h"
#include "LogicCreator.h"
#include "TooieRando.h"
#include "TooieRandoDlg.h"
#include "LogicGroup.h"
#include "RandomizedObject.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <istream>

std::vector<LogicGroup> LogicGroups;
std::vector<RandomizedObject*> UngroupedObjects;

std::vector <int> LogicGroupIndices;//The indices of the associated logic group in the logic group vector as they show up in the control list
std::vector <int> DependentGroupIndices;//The indices of the associated logic group in the logic group vector as they show up in the dependent list

std::vector <RandomizedObject*> groupedObjects;

CString lastSavePath = "";
int selectedGroup = -1;
int selectedRequirementSet = -1;
LogicCreator::LogicCreator(CWnd* pParent) : CDialog(LogicCreator::IDD)
{

}

LogicCreator::~LogicCreator()
{
}

void LogicCreator::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGIC_GROUP_LIST, logicGroupsList);
	DDX_Control(pDX, IDC_OBJECT_IN_GROUP_LIST, objectsInGroupList);
	DDX_Control(pDX, IDC_UNGROUPED_LIST, ungroupedObjectsList);
	DDX_Control(pDX, IDC_DEPENDENT_GROUP_LIST, dependentGroupsList);
	DDX_Control(pDX, IDC_REQUIRED_MOVE_LIST, requiredMovesList);
	DDX_Control(pDX, IDC_REQUIRED_ITEM_LIST, requiredItemsList);

	DDX_Control(pDX, IDC_DEPENDENT_GROUP_SELECT, dependentGroupSelector);
	DDX_Control(pDX, IDC_REQUIRED_MOVE_SELECTION, requiredMoveSelector);
	DDX_Control(pDX, IDC_REQUIRED_ITEM_SELECTION, requiredItemSelector);

	DDX_Control(pDX, IDC_ADD_DEPENDENT, addDependentGroupButton);
	DDX_Control(pDX, IDC_REMOVE_DEPENDENT, removeDependentGroupButton);
	DDX_Control(pDX, IDC_ADD_REQUIRED_MOVE, addRequiredMoveButton);
	DDX_Control(pDX, IDC_REMOVE_REQUIRED_MOVE, removeRequiredMoveButton);
	DDX_Control(pDX, IDC_ADD_REQUIRED_ITEM, addRequiredItemButton);
	DDX_Control(pDX, IDC_REMOVE_REQUIRED_ITEM, removeRequiredItemButton);
	
	DDX_Control(pDX, IDC_GROUP_NAME_EDIT_BOX, groupNameBox);
	DDX_Control(pDX, IDC_CREATE_NEW_GROUP, newGroupButton);
	DDX_Control(pDX, IDC_DELETE_GROUP, removeGroupButton);
	DDX_Control(pDX, IDC_REQUIRED_ITEM_NUMBER, numRequiredItemsBox);

	DDX_Control(pDX, IDC_REQUIREMENT_SELECTOR, requirementSetSelector);
	DDX_Control(pDX, IDC_REQUIREMENT_SET_EDITBOX, requirementSetNameBox);
	DDX_Control(pDX, IDC_CREATE_NEW_REQUIREMENT_SET, addRequirementSetButton);
	DDX_Control(pDX, IDC_DELETE_REQUIREMENTSET, deleteRequirementSetButton);

	DDX_Control(pDX, IDC_REWARD_KEY, keyRewardBox);
	DDX_Control(pDX, IDC_REQUIRED_KEY_LIST, requiredKeysList);
	DDX_Control(pDX, IDC_REQUIRED_KEY_SELECTION, requiredKeySelector);
	DDX_Control(pDX, IDC_ADD_REQUIRED_KEY, addRequiredKeyButton);
	DDX_Control(pDX, IDC_REMOVE_REQUIRED_KEY, removeRequiredKeyButton);

	DDX_Control(pDX, IDC_SEARCH_GROUPS_BOX, groupSearchBox);
	DDX_Control(pDX, IDC_SEARCH_GROUPED_BOX, groupedSearchBox);
	DDX_Control(pDX, IDC_SEARCH_UNGROUPED_BOX, ungroupedSearchBox);

	
}
BEGIN_MESSAGE_MAP(LogicCreator, CDialog)
	ON_LBN_SELCHANGE(IDC_LOGIC_GROUP_LIST, &LogicCreator::OnLbnSelchangeLogicGroupList)
	ON_BN_CLICKED(IDC_ADD_REQUIRED_MOVE, &LogicCreator::OnBnClickedAddRequiredMove)
	ON_BN_CLICKED(IDC_REMOVE_REQUIRED_MOVE, &LogicCreator::OnBnClickedRemoveRequiredMove)
	ON_BN_CLICKED(IDC_ADD_REQUIRED_ITEM, &LogicCreator::OnBnClickedAddRequiredItem)
	ON_BN_CLICKED(IDC_REMOVE_REQUIRED_ITEM, &LogicCreator::OnBnClickedRemoveRequiredItem)
	ON_BN_CLICKED(IDC_REMOVE_DEPENDENT, &LogicCreator::OnBnClickedRemoveDependent)
	ON_BN_CLICKED(IDC_ADD_DEPENDENT, &LogicCreator::OnBnClickedAddDependent)
	ON_LBN_SELCHANGE(IDC_OBJECT_IN_GROUP_LIST, &LogicCreator::OnLbnSelchangeObjectInGroupList)
	ON_LBN_SELCHANGE(IDC_UNGROUPED_LIST, &LogicCreator::OnLbnSelchangeUngroupedList)
	ON_BN_CLICKED(IDC_CREATE_NEW_GROUP, &LogicCreator::OnBnClickedCreateNewGroup)
	ON_BN_CLICKED(IDC_DELETE_GROUP, &LogicCreator::OnBnClickedDeleteGroup)
	ON_NOTIFY(NM_DBLCLK, IDC_LOGIC_GROUP_LIST, &LogicCreator::OnDblclkLogicGroupList)
	ON_EN_CHANGE(IDC_GROUP_NAME_EDIT_BOX, &LogicCreator::OnEnChangeGroupNameEditBox)
	ON_NOTIFY(NM_DBLCLK, IDC_UNGROUPED_LIST, &LogicCreator::OnDblclkUngroupedList)
	ON_NOTIFY(NM_DBLCLK, IDC_OBJECT_IN_GROUP_LIST, &LogicCreator::OnDblclkObjectInGroupList)
	ON_BN_CLICKED(IDC_LOADLOGICFILEBUTTON, &LogicCreator::OnBnClickedLoadlogicfilebutton)
	ON_BN_CLICKED(IDC_SAVELOGICFILEBUTTON, &LogicCreator::OnBnClickedSavelogicfilebutton)
	ON_NOTIFY(NM_DBLCLK, IDC_DEPENDENT_GROUP_LIST, &LogicCreator::OnDblclkDependentGroupList)
	ON_BN_CLICKED(IDC_CREATE_NEW_REQUIREMENT_SET, &LogicCreator::OnBnClickedCreateNewRequirementSet)
	ON_BN_CLICKED(IDC_DELETE_REQUIREMENTSET, &LogicCreator::OnBnClickedDeleteRequirementset)
	ON_CBN_SELCHANGE(IDC_REQUIREMENT_SELECTOR, &LogicCreator::OnCbnSelchangeRequirementSelector)
	ON_CBN_DBLCLK(IDC_REQUIREMENT_SELECTOR, &LogicCreator::OnDblclkRequirementSelector)
	ON_EN_CHANGE(IDC_REQUIREMENT_SET_EDITBOX, &LogicCreator::OnEnChangeRequirementSetEditbox)
	ON_EN_CHANGE(IDC_REWARD_KEY, &LogicCreator::OnEnChangeRewardKey)
	ON_BN_CLICKED(IDC_ADD_REQUIRED_KEY, &LogicCreator::OnBnClickedAddRequiredKey)
	ON_BN_CLICKED(IDC_REMOVE_REQUIRED_KEY, &LogicCreator::OnBnClickedRemoveRequiredKey)
	ON_EN_CHANGE(IDC_SEARCH_GROUPS_BOX, &LogicCreator::OnEnChangeSearchGroupsBox)
	ON_EN_CHANGE(IDC_SEARCH_GROUPED_BOX, &LogicCreator::OnEnChangeSearchGroupedBox)
	ON_EN_CHANGE(IDC_SEARCH_UNGROUPED_BOX, &LogicCreator::OnEnChangeSearchUngroupedBox)
END_MESSAGE_MAP()
BOOL LogicCreator::OnInitDialog()
{
	
	CDialog::OnInitDialog();
	LogicGroups.clear();
	selectedGroup = -1;

	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	pParentDlg->LoadObjects();
	pParentDlg->LoadMoves();

	UpdateUngroupedItemsList();
	UpdateRequiredMovesSelector();
	UpdateRequiredItemSelector();

	ungroupedObjectsList.InsertColumn(0, "Item Location Names", LVCFMT_LEFT, 150);
	ungroupedObjectsList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	ungroupedObjectsList.SetExtendedStyle(LVS_EX_GRIDLINES);

	objectsInGroupList.InsertColumn(0, "Item Location Names", LVCFMT_LEFT, 250);
	objectsInGroupList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	objectsInGroupList.SetExtendedStyle(LVS_EX_GRIDLINES);


	logicGroupsList.InsertColumn(0, "Group Names", LVCFMT_LEFT, 250);
	logicGroupsList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	logicGroupsList.SetExtendedStyle(LVS_EX_GRIDLINES);
	logicGroupsList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	dependentGroupsList.InsertColumn(0, "Group Names", LVCFMT_LEFT, 250);
	dependentGroupsList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	dependentGroupsList.SetExtendedStyle(LVS_EX_GRIDLINES);

	requiredItemsList.InsertColumn(0, "Item", LVCFMT_LEFT, 75);
	requiredItemsList.InsertColumn(1, "Count", LVCFMT_LEFT, 75);
	requiredItemsList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	requiredItemsList.SetExtendedStyle(LVS_EX_GRIDLINES);

	requiredMovesList.InsertColumn(0, "Move Name", LVCFMT_LEFT, 150);
	requiredMovesList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	requiredMovesList.SetExtendedStyle(LVS_EX_GRIDLINES);

	requiredKeysList.InsertColumn(0, "Key Name", LVCFMT_LEFT, 150);
	requiredKeysList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	requiredKeysList.SetExtendedStyle(LVS_EX_GRIDLINES);

	return true;
}


void LogicCreator::OnLbnSelchangeLogicGroupList()
{
	// TODO: Add your control notification handler code here
}


void LogicCreator::OnBnClickedAddRequiredMove()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	int curSel = requiredMoveSelector.GetCurSel();
	int ability = pParentDlg->MoveObjects[curSel].Ability;
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size() && selectedRequirementSet != -1 && selectedRequirementSet < LogicGroups[selectedGroup].Requirements.size())
	{
		auto it = find(LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredAbilities.begin(), LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredAbilities.end(), ability);
		if (it == LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredAbilities.end())
			LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredAbilities.push_back(ability);
	}
	UpdateRequiredMovesList();
	Savelogicfile(lastSavePath);
	// TODO: Add your control notification handler code here
}


void LogicCreator::OnBnClickedRemoveRequiredMove()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	int curSel = requiredMoveSelector.GetCurSel();
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size() && selectedRequirementSet != -1 && selectedRequirementSet < LogicGroups[selectedGroup].Requirements.size())
	{
		int ability = pParentDlg->MoveObjects[curSel].Ability;
		auto it = find(LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredAbilities.begin(), LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredAbilities.end(), ability);
		if (it != LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredAbilities.end())
			LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredAbilities.erase(it);
	}
	UpdateRequiredMovesList();
	Savelogicfile(lastSavePath);
}


void LogicCreator::OnBnClickedAddRequiredItem()
{
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size() && selectedRequirementSet != -1 && selectedRequirementSet < LogicGroups[selectedGroup].Requirements.size())
	{

		CString value;
		requiredItemSelector.GetWindowTextA(value);

		auto it = find(LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredItems.begin(), LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredItems.end(), value.GetString());
		if (it == LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredItems.end())
		{
			LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredItems.push_back(value.GetString());

			CString inputText;
			numRequiredItemsBox.GetWindowText(inputText);
			char* p;
			int itemAmount = strtol(inputText.GetString(), &p, 10);
			LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredItemsCount.push_back(itemAmount);
		}
		else
		{
			CString inputText;
			numRequiredItemsBox.GetWindowText(inputText);
			char* p;
			int itemAmount = strtol(inputText.GetString(), &p, 16);
			LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredItemsCount[it - LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredItems.begin()] = itemAmount;
		}
	}
	UpdateRequiredItemsList();
	Savelogicfile(lastSavePath);
}


void LogicCreator::OnBnClickedRemoveRequiredItem()
{
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size() && selectedRequirementSet != -1 && selectedRequirementSet < LogicGroups[selectedGroup].Requirements.size())
	{


		CString value;
		requiredItemSelector.GetWindowTextA(value);

		auto it = find(LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredItems.begin(), LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredItems.end(), value.GetString());
		if (it == LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredItems.end())
		{
			
		}
		else
		{
			int foundIndex = it - LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredItems.begin();
			LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredItems.erase(it);
			LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredItemsCount.erase(LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredItemsCount.begin() + foundIndex);
		}
	}
	UpdateRequiredItemsList();
	Savelogicfile(lastSavePath);
}


void LogicCreator::OnBnClickedRemoveDependent()
{
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		int selectedGroupID = LogicGroups[dependentGroupSelector.GetCurSel()].GroupID;
		auto it = std::find(LogicGroups[selectedGroup].dependentGroupIDs.begin(), LogicGroups[selectedGroup].dependentGroupIDs.end(), selectedGroupID);
		if(it != LogicGroups[selectedGroup].dependentGroupIDs.end())
			LogicGroups[selectedGroup].dependentGroupIDs.erase(it);
	}
	UpdateDependentGroupList();
	Savelogicfile(lastSavePath);
}


void LogicCreator::OnBnClickedAddDependent()
{
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		LogicGroups[selectedGroup].dependentGroupIDs.push_back(LogicGroups[dependentGroupSelector.GetCurSel()].GroupID);
	}
	UpdateDependentGroupList();
	Savelogicfile(lastSavePath);
	
}


void LogicCreator::OnLbnSelchangeObjectInGroupList()
{
	// TODO: Add your control notification handler code here
}


void LogicCreator::OnLbnSelchangeUngroupedList()
{
	// TODO: Add your control notification handler code here
}


void LogicCreator::OnBnClickedCreateNewGroup()
{
	selectedGroup = -1;
	CString newGroupName;
	groupNameBox.GetWindowText(newGroupName);
	groupNameBox.SetWindowTextA("");

	LogicGroup group;
	group.GroupName = (newGroupName);
	LogicGroups.push_back(group);
	UpdateGroupList();
	Savelogicfile(lastSavePath);
}


void LogicCreator::OnBnClickedDeleteGroup()
{
	if (selectedGroup != -1 && selectedGroup<LogicGroups.size())
	{
		LogicGroups.erase(LogicGroups.begin() + selectedGroup);
		UpdateGroupList();
		selectedGroup = -1;
		UpdateRequirementSelector();
		UpdateUngroupedItemsList();
		UpdateGroupedItemsList();
		UpdateDependentGroupList();
		UpdateRequiredItemsList();
		UpdateRequiredMovesList();
		Savelogicfile(lastSavePath);
	}
}

void LogicCreator::UpdateGroupList()
{
	logicGroupsList.DeleteAllItems();
	LogicGroupIndices.clear();
	for (int i = 0; i < LogicGroups.size(); i++)
	{
		CString searchTerm;
		groupSearchBox.GetWindowTextA(searchTerm);
		if (searchTerm.IsEmpty()||LogicGroups[i].GroupName.find(searchTerm.GetString()) != std::string::npos)
		{
			LogicGroupIndices.push_back(i);
			AddElementToListCntrl(logicGroupsList, LogicGroups[i].GroupName);
		}
	}
	UpdateGroupSelector();
}

void LogicCreator::UpdateDependentGroupList()
{
	addDependentGroupButton.EnableWindow(selectedGroup != -1);
	removeDependentGroupButton.EnableWindow(selectedGroup != -1);
	dependentGroupsList.DeleteAllItems();
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		bool foundNull = false;
		std::vector<int> newDependentGroupIds;

		for (int i = 0; i < LogicGroups[selectedGroup].dependentGroupIDs.size(); i++)
		{
			LogicGroup* logicGroup = GetLogicGroupFromId(LogicGroups[selectedGroup].dependentGroupIDs[i]);
			if (logicGroup != nullptr)
			{
				newDependentGroupIds.push_back(LogicGroups[selectedGroup].dependentGroupIDs[i]);
				std::string groupName = GetLogicGroupFromId(LogicGroups[selectedGroup].dependentGroupIDs[i])->GroupName;
				AddElementToListCntrl(dependentGroupsList, groupName);
			}
			else
				foundNull = true;
		}
		if (foundNull)//If there isn't a group associated with a dependentgroupId set to a list of all of the ids that could be verified
		{
			LogicGroups[selectedGroup].dependentGroupIDs = newDependentGroupIds;
		}
	}
	Savelogicfile(lastSavePath);
}

void LogicCreator::UpdateGroupSelector()
{
	dependentGroupSelector.ResetContent();
	for (int i = 0; i < LogicGroups.size(); i++)
	{
		dependentGroupSelector.AddString(LogicGroups[i].GroupName.c_str());
	}
}

void LogicCreator::AddElementToListCntrl(CListCtrl &list, std::string itemName)
{
	LVITEM lv;
	lv.iItem = list.GetItemCount();
	lv.iSubItem = 0;
	lv.pszText = "optionName";
	lv.mask = LVIF_TEXT;

	int item = list.InsertItem(&lv);

	list.SetItemText(item, 0, itemName.c_str());
}

/// <summary>
/// Try and Select a logic group to edit
/// </summary>
/// <param name="pNMHDR"></param>
/// <param name="pResult"></param>
void LogicCreator::OnDblclkLogicGroupList(NMHDR* pNMHDR, LRESULT* pResult)
{
	selectedGroup = -1;
	if (LogicGroups.size() > 0)
	{
		LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
		SelectGroupByIndex(LogicGroupIndices[pNMItemActivate->iItem]);
	}
	
	
	*pResult = 0;
}


void LogicCreator::OnEnChangeGroupNameEditBox()
{
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		CString newGroupName;
		groupNameBox.GetWindowText(newGroupName);
		LogicGroups[selectedGroup].GroupName = newGroupName.GetString();
		UpdateGroupList();
		Savelogicfile(lastSavePath);
	}
}


void LogicCreator::OnDblclkUngroupedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int selectedObject = pNMItemActivate->iItem;
	if (UngroupedObjects.size() > selectedObject && selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		LogicGroups[selectedGroup].objectIDsInGroup.push_back(UngroupedObjects[selectedObject]->ObjectID);
		UpdateUngroupedItemsList();
		UpdateGroupedItemsList();
	}
	*pResult = 0;
	Savelogicfile(lastSavePath);
}

void LogicCreator::SelectGroupByPointer(LogicGroup* selectedLogicGroup)
{
	for (int i = 0; i < LogicGroups.size(); i++)
	{
		if (selectedLogicGroup == &LogicGroups[i])
		{
			SelectGroupByIndex(i);
		}
	}
}

void LogicCreator::SelectGroupByIndex(int newSelection)
{
	if(LogicGroups.size()>newSelection)
	{
		selectedGroup = newSelection;
	}
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size() && LogicGroups.size())
	{
		groupNameBox.SetWindowTextA(LogicGroups[selectedGroup].GroupName.c_str());
	}
	UpdateRewardKey();
	RequirementSetSelector(0);
	UpdateRequirementSelector();
	UpdateDependentGroupList();
	UpdateGroupedItemsList();
	UpdateRequiredItemsList();
	UpdateRequiredMovesList();
	UpdateRequiredKeyList();
}

void LogicCreator::OnDblclkObjectInGroupList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int selectedObject = pNMItemActivate->iItem;
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size() && selectedObject< groupedObjects.size())
	{
		LogicGroups[selectedGroup].objectIDsInGroup.erase(std::remove(LogicGroups[selectedGroup].objectIDsInGroup.begin(), LogicGroups[selectedGroup].objectIDsInGroup.end(), groupedObjects[selectedObject]->ObjectID), LogicGroups[selectedGroup].objectIDsInGroup.end());
		UpdateUngroupedItemsList();
		UpdateGroupedItemsList();
	}
	*pResult = 0;
	Savelogicfile(lastSavePath);
}

void LogicCreator::UpdateUngroupedItemsList()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	ungroupedObjectsList.DeleteAllItems();
	UngroupedObjects.clear();
	for(int i = 0; i< pParentDlg->RandomizedObjects.size();i++)
	{
		CString searchTerm;
		ungroupedSearchBox.GetWindowTextA(searchTerm);
		if (searchTerm.IsEmpty() || pParentDlg->RandomizedObjects[i].LocationName.find(searchTerm.GetString()) != std::string::npos)
		{
			OutputDebugString(_T("Randomized Object Found"));
			LogicGroup* foundGroup = GetLogicGroupContainingObjectId(pParentDlg->RandomizedObjects[i].ObjectID);
			if (foundGroup == nullptr)
			{
				UngroupedObjects.push_back(&pParentDlg->RandomizedObjects[i]);
			}
		}
	}

	for (int i = 0; i < UngroupedObjects.size(); i++)
	{
		AddElementToListCntrl(ungroupedObjectsList, UngroupedObjects[i]->LocationName.c_str());
	}
}

/// <summary>
/// Updates the ui list and the pointer vector pointing to the objects in the group
/// </summary>
void LogicCreator::UpdateGroupedItemsList()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	objectsInGroupList.DeleteAllItems();
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		LogicGroups[selectedGroup].objectsInGroup.clear();
		groupedObjects.clear();
		for (int i = 0; i < pParentDlg->RandomizedObjects.size(); i++)
		{
			OutputDebugString(_T("Randomized Object Found"));
			LogicGroup* foundGroup = GetLogicGroupContainingObjectId(pParentDlg->RandomizedObjects[i].ObjectID);
			if (foundGroup == &LogicGroups[selectedGroup])
			{
				LogicGroups[selectedGroup].objectsInGroup.push_back(&pParentDlg->RandomizedObjects[i]);
				CString searchTerm;
				groupedSearchBox.GetWindowTextA(searchTerm);
				if (searchTerm.IsEmpty() || pParentDlg->RandomizedObjects[i].LocationName.find(searchTerm.GetString()) != std::string::npos)
				{
					groupedObjects.push_back(&pParentDlg->RandomizedObjects[i]);
				}
			}
		}

		for (int i = 0; i < groupedObjects.size(); i++)
		{
			AddElementToListCntrl(objectsInGroupList, groupedObjects[i]->LocationName.c_str());
		}
	}
}

void LogicCreator::UpdateRequiredItemsList()
{
	addRequiredItemButton.EnableWindow(selectedGroup != -1);
	removeRequiredItemButton.EnableWindow(selectedGroup != -1);
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	requiredItemsList.DeleteAllItems();
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size() && selectedRequirementSet != -1 && selectedRequirementSet < LogicGroups[selectedGroup].Requirements.size())
	{
		for (int i = 0; i < LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredItems.size(); i++)
		{
			LVITEM lv;
			lv.iItem = requiredItemsList.GetItemCount();
			lv.iSubItem = 0;
			lv.pszText = "optionName";
			lv.mask = LVIF_TEXT;

			int item = requiredItemsList.InsertItem(&lv);

			requiredItemsList.SetItemText(item, 0, LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredItems[i].c_str());
			CString itemAmount;
			itemAmount.Format("%d", LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredItemsCount[i]);
			requiredItemsList.SetItemText(item, 1, itemAmount);
		}
	}
}

void LogicCreator::UpdateRequiredMovesList()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	addRequiredMoveButton.EnableWindow(selectedGroup != -1);
	removeRequiredMoveButton.EnableWindow(selectedGroup != -1);
	requiredMovesList.DeleteAllItems();
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size() && selectedRequirementSet != -1 && selectedRequirementSet < LogicGroups[selectedGroup].Requirements.size())
	{
		for (int i = 0; i < LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredAbilities.size(); i++)
		{
			CString str;
			str.Format("%X", LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredAbilities[i]);
			AddElementToListCntrl(requiredMovesList, str.GetString());
		}
	}
}

void LogicCreator::UpdateRequiredMovesSelector()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	requiredMoveSelector.ResetContent();
	for (int i = 0; i < pParentDlg->MoveObjects.size(); i++)
	{
		requiredMoveSelector.AddString(pParentDlg->MoveObjects[i].MoveName.c_str());
	}
}

void LogicCreator::UpdateRequiredItemSelector()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();

	requiredItemSelector.ResetContent();
	for (int i = 0; i < pParentDlg->RandomizedObjects.size(); i++)
	{
		int found = requiredItemSelector.FindString(0, pParentDlg->RandomizedObjects[i].ItemTag.c_str());
		if(found == -1)
			requiredItemSelector.AddString(pParentDlg->RandomizedObjects[i].ItemTag.c_str());
	}
}

LogicGroup* LogicCreator::GetLogicGroupFromId(int groupID)
{
	for (int i = 0; i < LogicGroups.size(); i++)
	{
		if (groupID == LogicGroups[i].GroupID)
			return &LogicGroups[i];
	}
	return nullptr;
}

void LogicCreator::OnBnClickedLoadlogicfilebutton()
{
	CString fileOpen;
	CFileDialog m_ldFile(TRUE, NULL, "LogicFile.txt", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Logic (*.txt)|*.txt|", this);
	int didRead = m_ldFile.DoModal();
	if ((didRead == IDCANCEL) || (m_ldFile.GetPathName() == ""))
		return;

	if (didRead == FALSE)
		return;

	fileOpen=m_ldFile.GetPathName();
	lastSavePath = m_ldFile.GetPathName();
	TooieRandoDlg::LoadLogicGroupsFromFile(&LogicGroups, fileOpen);

	selectedGroup = -1;
	requirementSetNameBox.SetWindowTextA("");
	UpdateGroupList();
	UpdateDependentGroupList();
	UpdateGroupedItemsList();
	UpdateUngroupedItemsList();
	UpdateRequiredItemsList();
	UpdateRequiredMovesList();
	UpdateRequirementSelector();

	UpdateGroupSelector();
	UpdateRequiredMovesSelector();
	UpdateRequiredItemSelector();

	UpdateRequiredKeySelector();
	UpdateRequiredKeyList();
	UpdateRewardKey();

}


void LogicCreator::OnBnClickedSavelogicfilebutton()
{
	CFileDialog m_svFile(FALSE, NULL, ("LogicFile.txt"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "OUT Logic (*.txt)|*.txt|", this);

	int isFileOpened2 = m_svFile.DoModal();

	if (isFileOpened2 == IDCANCEL)
		return;

	if (m_svFile.GetFileName() == "")
		return;
	lastSavePath = m_svFile.GetPathName();
	Savelogicfile(lastSavePath);
}


void LogicCreator::Savelogicfile(CString filepath)
{
	if (filepath.IsEmpty() == true)
		return;
	FILE* outFile = fopen(filepath, "wb");
	if (outFile == NULL)
	{
		MessageBox("Cannot open output file");
		return;
	}
	for (int i = 0; i < LogicGroups.size(); i++)
	{
		char str[1000];
		sprintf(str, "GroupId:%x,", LogicGroups[i].GroupID);
		fwrite(str, 1, strlen(str), outFile);
		std::string Requirements;
		Requirements.append("Requirements:[");
		for (int j = 0; j < LogicGroups[i].Requirements.size(); j++)
		{
			Requirements.append("{SetName:\"");
			Requirements.append(LogicGroups[i].Requirements[j].SetName);
			Requirements.append("\",");
			if (LogicGroups[i].Requirements[j].RequiredAbilities.size() > 0)
			{
				Requirements.append("RequiredMoves:[");
				Requirements.append(intVectorToString(LogicGroups[i].Requirements[j].RequiredAbilities, ","));
				Requirements.append("],");
			}
			if (LogicGroups[i].Requirements[j].RequiredItems.size() > 0)
			{
				Requirements.append("RequiredItem:[");
				Requirements.append(stringVectorToString(LogicGroups[i].Requirements[j].RequiredItems, ","));
				Requirements.append("],RequiredItemCounts:[");
				Requirements.append(intVectorToString(LogicGroups[i].Requirements[j].RequiredItemsCount, ","));
				Requirements.append("],");
			}
			if (LogicGroups[i].Requirements[j].RequiredKeys.size() > 0)
			{
				Requirements.append("RequiredKeys:[");
				Requirements.append(stringVectorToString(LogicGroups[i].Requirements[j].RequiredKeys, ","));
				Requirements.append("],");
			}
			Requirements.append("}");
			if(j != LogicGroups[i].Requirements.size()-1)
				Requirements.append(",");
		}
		Requirements.append("],");
		sprintf(str, "%s", Requirements.c_str());
		fwrite(str, 1, strlen(str), outFile);
		if (!LogicGroups[i].key.empty())
		{
			sprintf(str, "RewardKey:\"%s\",", LogicGroups[i].key.c_str());
			fwrite(str, 1, strlen(str), outFile);
		}
		if (LogicGroups[i].dependentGroupIDs.size() > 0)
		{
			sprintf(str, "DependentGroups:[%s],", intVectorToString(LogicGroups[i].dependentGroupIDs, ",").c_str());
			fwrite(str, 1, strlen(str), outFile);
		}
		sprintf(str, "GroupName:\"%s\",", LogicGroups[i].GroupName.c_str());
		fwrite(str, 1, strlen(str), outFile);
		if (LogicGroups[i].objectIDsInGroup.size()>0)
		{
			sprintf(str, "ObjectsInGroup:[%s],", intVectorToString(LogicGroups[i].objectIDsInGroup, ",").c_str());
			fwrite(str, 1, strlen(str), outFile);
		}
		fwrite("\n", 1, 1, outFile);
	}

	fclose(outFile);
}

/// <summary>
/// Used to edit the randomizer addresses file en mass not really used but I'm keeping it here in case I need to do it again
/// </summary>
void LogicCreator::SaveRandomizerObjectEdits()
{
	std::vector<std::string> fileLines;
	std::ifstream myfile("RandomizerAddresses.txt");
	std::string line;
	try {
		if (!myfile.is_open()) {
			throw std::runtime_error("Error: Could not open the file 'RandomizerAddresses.txt'.");
		}
	}
	catch (const std::exception& ex) {
		::MessageBox(NULL, ex.what(), "Error", NULL);
		return;
	}
	char message[256];
	myfile.clear();
	myfile.seekg(0);

	if (myfile.peek() == std::ifstream::traits_type::eof()) {
		::MessageBox(NULL, "Error: The file is empty.", "Error", NULL);
		return;
	}

	myfile.clear();
	myfile.seekg(0);
	bool isJustScript = false; //Whether the next line should be treated as a new object or another script to add
	while (std::getline(myfile, line)) // Read each line from the file
	{
		fileLines.push_back(line);
	}
	int objectID = 0;

	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();

	FILE* outFile = fopen("RandomizerAddresses1.txt", "wb");
	if (outFile == NULL)
	{
		MessageBox("Cannot open output file");
		return;
	}
	for (int i = 0; i < fileLines.size(); i++)
	{	
		std::string ObjectID = pParentDlg->GetStringAfterTag(fileLines[i], "ObjectId:", ",");
		if (fileLines[i][0] != '/'&& fileLines[i][0] != '*' && ObjectID.size() == 0)
		{
			char str[500];
			std::string dataOutput = "";
			char message[256];
			if (pParentDlg->RandomizedObjects[objectID].Data.size() > 0)
				for (size_t j = 0; j < 10; ++j) {
					char byteStr[4];

					sprintf(byteStr, "%02X", pParentDlg->RandomizedObjects[objectID].Data[j]);
					dataOutput += byteStr;

				}
			std::string ItemTag = "";
			int itemAmount = 1;
			if (dataOutput.find("01F6") != std::string::npos)
			{
				ItemTag = "Jiggy";
			}
			else if (dataOutput.find("01F5") != std::string::npos)
			{
				ItemTag = "Jinjo";
			}
			else if (dataOutput.find("01F8") != std::string::npos)
			{
				ItemTag = "Glowbo";
			}
			else if (dataOutput.find("021B") != std::string::npos)
			{
				ItemTag = "Mega Glowbo";
			}
			else if (dataOutput.find("029D") != std::string::npos)
			{
				ItemTag = "Doubloon";
			}
			else if (dataOutput.find("04E6") != std::string::npos)
			{
				ItemTag = "Ticket";
			}
			else if (dataOutput.find("01F7") != std::string::npos)
			{
				ItemTag = "Honeycomb";
			}
			else if (dataOutput.find("0201") != std::string::npos)
			{
				ItemTag = "Cheato Page";
			}
			else if (dataOutput.find("02B3") != std::string::npos)
			{
				ItemTag = "Jade Totem";
			}
			else if (dataOutput.find("04BA") != std::string::npos)
			{
				ItemTag = "Boggy Fish";
			}
			else if (dataOutput.find("01D7") != std::string::npos)
			{
				ItemTag = "Note Nest";
				itemAmount = 5;
			}
			else if (dataOutput.find("01D8") != std::string::npos)
			{
				ItemTag = "Treble Clef";
				itemAmount = 20;
			}

			sprintf(str, "%sObjectId:%d,ItemTag:\"%s\",ItemAmount:%d,", fileLines[i].c_str(), objectID, ItemTag.c_str(), itemAmount);
			objectID++;
			fileLines[i] = str;
		}
		fwrite(fileLines[i].c_str(), 1, strlen(fileLines[i].c_str()), outFile);
		fwrite("\n", 1, 1, outFile);
	}

	fclose(outFile);
}

std::string LogicCreator::intVectorToString(std::vector<int> intVector,std::string delimiter)
{
	std::string outputString = "";
	for (int i = 0; i < intVector.size(); i++)
	{
		char hex_string[20];
		sprintf(hex_string, "%X", intVector[i]);
		outputString.append(hex_string);
		if(i != intVector.size()-1)
		outputString.append(delimiter);
	}
	return outputString;
}

std::string LogicCreator::stringVectorToString(std::vector<std::string> stringVector, std::string delimiter)
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

LogicGroup* LogicCreator::GetLogicGroupContainingObjectId(int objectID)
{
	for (int i = 0; i < LogicGroups.size(); i++)
	{
		for (int j = 0; j < LogicGroups[i].objectIDsInGroup.size(); j++)
		{
			if (LogicGroups[i].objectIDsInGroup[j] == objectID)
				return &LogicGroups[i];
		}
	}
	return nullptr;
}


void LogicCreator::OnDblclkDependentGroupList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void LogicCreator::OnBnClickedCreateNewRequirementSet()
{
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		CString name;
		LogicGroup::RequirementSet requirement;
		requirementSetNameBox.GetWindowTextA(name);
		requirement.SetName = name.GetString();
		LogicGroups[selectedGroup].Requirements.push_back(requirement);
	}
	RequirementSetSelector(0);
	UpdateRequirementSelector();
	UpdateRequiredItemsList();
	UpdateRequiredMovesList();
	Savelogicfile(lastSavePath);
}


void LogicCreator::OnBnClickedDeleteRequirementset()
{
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		CString name;
		requirementSetSelector.GetWindowTextA(name);

		LogicGroups[selectedGroup].Requirements.erase(LogicGroups[selectedGroup].Requirements.begin() + requirementSetSelector.GetCurSel());

	}
	RequirementSetSelector(0);
	UpdateRequirementSelector();
	UpdateRequiredItemsList();
	UpdateRequiredMovesList();
	Savelogicfile(lastSavePath);
}

void LogicCreator::UpdateRequirementSelector()
{
	requirementSetSelector.EnableWindow(selectedGroup != -1);
	addRequirementSetButton.EnableWindow(selectedGroup != -1);
	deleteRequirementSetButton.EnableWindow(selectedGroup != -1);
	requirementSetNameBox.EnableWindow(selectedGroup != -1);

	requirementSetSelector.ResetContent();
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		for (int i = 0; i < LogicGroups[selectedGroup].Requirements.size(); i++)
		{
			requirementSetSelector.AddString(LogicGroups[selectedGroup].Requirements[i].SetName.c_str());
		}
		if (selectedRequirementSet != -1)
		{
			requirementSetSelector.SetCurSel(selectedRequirementSet);
		}
	}
}

void LogicCreator::OnCbnSelchangeRequirementSelector()
{
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size() && requirementSetSelector.GetCurSel() != -1)
	{
		RequirementSetSelector(requirementSetSelector.GetCurSel());
		UpdateRequiredItemsList();
		UpdateRequiredMovesList();
		UpdateRequiredKeyList();
	}
}


void LogicCreator::OnDblclkRequirementSelector()
{
	requirementSetNameBox.SetWindowTextA(LogicGroups[selectedGroup].Requirements[selectedRequirementSet].SetName.c_str());
}


void LogicCreator::OnEnChangeRequirementSetEditbox()
{
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size() && selectedRequirementSet != -1 && LogicGroups[selectedGroup].Requirements.size()>0)
	{
		CString name;
		requirementSetNameBox.GetWindowTextA(name);
		LogicGroups[selectedGroup].Requirements[selectedRequirementSet].SetName = name.GetString();
	}
	Savelogicfile(lastSavePath);
	UpdateRequirementSelector();
}

void LogicCreator::RequirementSetSelector(int setToSelect)
{
	requirementSetNameBox.EnableWindow(selectedGroup != -1);
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size() && setToSelect !=-1 && setToSelect < LogicGroups[selectedGroup].Requirements.size())
	{

		selectedRequirementSet = setToSelect;
		requirementSetNameBox.SetWindowTextA(LogicGroups[selectedGroup].Requirements[selectedRequirementSet].SetName.c_str());
	}
}

void LogicCreator::OnEnChangeRewardKey()
{
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		CString name;
		keyRewardBox.GetWindowTextA(name);
		LogicGroups[selectedGroup].key = name.GetString();
		Savelogicfile(lastSavePath);
	}
	UpdateRequiredKeySelector();
}
void LogicCreator::UpdateRequiredKeySelector()
{
	requiredKeySelector.ResetContent();
	for (int i = 0; i < LogicGroups.size(); i++)
	{
		int found = requiredKeySelector.FindString(0, LogicGroups[i].key.c_str());
		if (found == -1 && !LogicGroups[i].key.empty())
			requiredKeySelector.AddString(LogicGroups[i].key.c_str());
	}
}

void LogicCreator::UpdateRequiredKeyList()
{
	addRequiredKeyButton.EnableWindow(selectedGroup != -1);
	removeRequiredKeyButton.EnableWindow(selectedGroup != -1);
	requiredKeysList.DeleteAllItems();
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
		if (selectedGroup != -1 && selectedGroup < LogicGroups.size() && selectedRequirementSet != -1 && selectedRequirementSet < LogicGroups[selectedGroup].Requirements.size())
		{
			for (int i = 0; i < LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredKeys.size(); i++)
			{
				LVITEM lv;
				lv.iItem = requiredKeysList.GetItemCount();
				lv.iSubItem = 0;
				lv.pszText = "optionName";
				lv.mask = LVIF_TEXT;

				int item = requiredKeysList.InsertItem(&lv);

				requiredKeysList.SetItemText(item, 0, LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredKeys[i].c_str());
				CString itemAmount;
				itemAmount.Format("%d", LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredKeys[i]);
				requiredKeysList.SetItemText(item, 1, itemAmount);
			}
		}
	}
}

void LogicCreator::UpdateRewardKey()
{
	keyRewardBox.EnableWindow(selectedGroup != -1);

	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		keyRewardBox.SetWindowTextA(LogicGroups[selectedGroup].key.c_str());
	}
}


void LogicCreator::OnBnClickedAddRequiredKey()
{
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size() && selectedRequirementSet != -1 && selectedRequirementSet < LogicGroups[selectedGroup].Requirements.size())
	{
		CString value;
		requiredKeySelector.GetWindowTextA(value);

		auto it = find(LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredKeys.begin(), LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredKeys.end(), value.GetString());
		if (it == LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredKeys.end())
		{
			LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredKeys.push_back(value.GetString());
		}
	}
	UpdateRequiredKeyList();
	Savelogicfile(lastSavePath);
}


void LogicCreator::OnBnClickedRemoveRequiredKey()
{
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size() && selectedRequirementSet != -1 && selectedRequirementSet < LogicGroups[selectedGroup].Requirements.size())
	{
		CString value;
		requiredKeySelector.GetWindowTextA(value);

		auto it = find(LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredKeys.begin(), LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredKeys.end(), value.GetString());
		if (it != LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredKeys.end())
		{
			int foundIndex = it - LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredKeys.begin();
			LogicGroups[selectedGroup].Requirements[selectedRequirementSet].RequiredKeys.erase(it);
		}
	}
	UpdateRequiredKeyList();
	Savelogicfile(lastSavePath);
}

void LogicCreator::OnEnChangeSearchGroupsBox()
{
	UpdateGroupList();
}


void LogicCreator::OnEnChangeSearchGroupedBox()
{
	UpdateGroupedItemsList();
}


void LogicCreator::OnEnChangeSearchUngroupedBox()
{
	UpdateUngroupedItemsList();
}

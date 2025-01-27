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

int selectedGroup = -1;
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
	if(selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		auto it = find(LogicGroups[selectedGroup].RequiredAbilities.begin(), LogicGroups[selectedGroup].RequiredAbilities.end(), ability);
		if (it == LogicGroups[selectedGroup].RequiredAbilities.end())
			LogicGroups[selectedGroup].RequiredAbilities.push_back(ability);
	}
	UpdateRequiredMovesList();
	// TODO: Add your control notification handler code here
}


void LogicCreator::OnBnClickedRemoveRequiredMove()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	int curSel = requiredMoveSelector.GetCurSel();
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		int ability = pParentDlg->MoveObjects[curSel].Ability;
		auto it = find(LogicGroups[selectedGroup].RequiredAbilities.begin(), LogicGroups[selectedGroup].RequiredAbilities.end(), ability);
		if (it != LogicGroups[selectedGroup].RequiredAbilities.end())
			LogicGroups[selectedGroup].RequiredAbilities.erase(it);
	}
	UpdateRequiredMovesList();
}


void LogicCreator::OnBnClickedAddRequiredItem()
{
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{


		CString value;
		requiredItemSelector.GetWindowTextA(value);

		auto it = find(LogicGroups[selectedGroup].RequiredItems.begin(), LogicGroups[selectedGroup].RequiredItems.end(), value.GetString());
		if (it == LogicGroups[selectedGroup].RequiredItems.end())
		{
			LogicGroups[selectedGroup].RequiredItems.push_back(value.GetString());

			CString inputText;
			numRequiredItemsBox.GetWindowText(inputText);
			char* p;
			int itemAmount = strtol(inputText.GetString(), &p, 16);
			LogicGroups[selectedGroup].RequiredItemsCount.push_back(itemAmount);
		}
		else
		{
			CString inputText;
			numRequiredItemsBox.GetWindowText(inputText);
			char* p;
			int itemAmount = strtol(inputText.GetString(), &p, 16);
			LogicGroups[selectedGroup].RequiredItemsCount[it - LogicGroups[selectedGroup].RequiredItems.begin()] = itemAmount;
		}
	}
	UpdateRequiredItemsList();
}


void LogicCreator::OnBnClickedRemoveRequiredItem()
{
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{


		CString value;
		requiredItemSelector.GetWindowTextA(value);

		auto it = find(LogicGroups[selectedGroup].RequiredItems.begin(), LogicGroups[selectedGroup].RequiredItems.end(), value.GetString());
		if (it == LogicGroups[selectedGroup].RequiredItems.end())
		{
			
		}
		else
		{
			int foundIndex = it - LogicGroups[selectedGroup].RequiredItems.begin();
			LogicGroups[selectedGroup].RequiredItems.erase(it);
			LogicGroups[selectedGroup].RequiredItemsCount.erase(LogicGroups[selectedGroup].RequiredItemsCount.begin() + foundIndex);
		}
	}
	UpdateRequiredItemsList();
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
}


void LogicCreator::OnBnClickedAddDependent()
{
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		LogicGroups[selectedGroup].dependentGroupIDs.push_back(LogicGroups[dependentGroupSelector.GetCurSel()].GroupID);
	}
	UpdateDependentGroupList();
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
	CString newGroupName;
	groupNameBox.GetWindowText(newGroupName);
	groupNameBox.SetWindowTextA("");

	LogicGroup group;
	group.GroupName = (newGroupName);
	LogicGroups.push_back(group);
	UpdateGroupList();

}


void LogicCreator::OnBnClickedDeleteGroup()
{
	if (selectedGroup != -1 && selectedGroup<LogicGroups.size())
	{
		LogicGroups.erase(LogicGroups.begin() + selectedGroup);
		UpdateGroupList();
		selectedGroup = -1;
		UpdateUngroupedItemsList();
		UpdateGroupedItemsList();
		UpdateDependentGroupList();
		UpdateRequiredItemsList();
		UpdateRequiredMovesList();

	}
}

void LogicCreator::UpdateGroupList()
{
	logicGroupsList.DeleteAllItems();
	for (int i = 0; i < LogicGroups.size(); i++)
	{
		AddElementToListCntrl(logicGroupsList,LogicGroups[i].GroupName);
	}
	UpdateGroupSelector();
}

void LogicCreator::UpdateDependentGroupList()
{
	dependentGroupsList.DeleteAllItems();
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		for (int i = 0; i < LogicGroups[selectedGroup].dependentGroupIDs.size(); i++)
		{
			AddElementToListCntrl(dependentGroupsList, GetLogicGroupFromId(LogicGroups[selectedGroup].dependentGroupIDs[i])->GroupName);
		}
	}
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


void LogicCreator::OnDblclkLogicGroupList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	selectedGroup = pNMItemActivate->iItem;
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		groupNameBox.SetWindowTextA(LogicGroups[selectedGroup].GroupName.c_str());
	}
	UpdateDependentGroupList();
	UpdateGroupedItemsList();
	UpdateRequiredItemsList();
	UpdateRequiredMovesList();
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
}


void LogicCreator::OnDblclkObjectInGroupList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int selectedObject = pNMItemActivate->iItem;
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size() && selectedObject<LogicGroups[selectedGroup].objectsInGroup.size())
	{
		LogicGroups[selectedGroup].objectIDsInGroup.erase(std::remove(LogicGroups[selectedGroup].objectIDsInGroup.begin(), LogicGroups[selectedGroup].objectIDsInGroup.end(), LogicGroups[selectedGroup].objectIDsInGroup[selectedObject]), LogicGroups[selectedGroup].objectIDsInGroup.end());
		UpdateUngroupedItemsList();
		UpdateGroupedItemsList();
	}
	*pResult = 0;
}

void LogicCreator::UpdateUngroupedItemsList()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	ungroupedObjectsList.DeleteAllItems();
	UngroupedObjects.clear();
	for(int i = 0; i< pParentDlg->RandomizedObjects.size();i++)
	{
		OutputDebugString(_T("Randomized Object Found"));
		LogicGroup* foundGroup = GetLogicGroupContainingObjectId(pParentDlg->RandomizedObjects[i].ObjectID);
		if (foundGroup == nullptr)
		{
			UngroupedObjects.push_back(&pParentDlg->RandomizedObjects[i]);
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
	LogicGroups[selectedGroup].objectsInGroup.clear();
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		for (int i = 0; i < pParentDlg->RandomizedObjects.size(); i++)
		{
			OutputDebugString(_T("Randomized Object Found"));
			LogicGroup* foundGroup = GetLogicGroupContainingObjectId(pParentDlg->RandomizedObjects[i].ObjectID);
			if (foundGroup == &LogicGroups[selectedGroup])
			{
				LogicGroups[selectedGroup].objectsInGroup.push_back(&pParentDlg->RandomizedObjects[i]);
			}
		}

		for (int i = 0; i < LogicGroups[selectedGroup].objectsInGroup.size(); i++)
		{
			AddElementToListCntrl(objectsInGroupList, LogicGroups[selectedGroup].objectsInGroup[i]->LocationName.c_str());
		}
	}
}

void LogicCreator::UpdateRequiredItemsList()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	requiredItemsList.DeleteAllItems();
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		for (int i = 0; i < LogicGroups[selectedGroup].RequiredItems.size(); i++)
		{
			LVITEM lv;
			lv.iItem = requiredItemsList.GetItemCount();
			lv.iSubItem = 0;
			lv.pszText = "optionName";
			lv.mask = LVIF_TEXT;

			int item = requiredItemsList.InsertItem(&lv);

			requiredItemsList.SetItemText(item, 0, LogicGroups[selectedGroup].RequiredItems[i].c_str());
			CString itemAmount;
			itemAmount.Format("%d", LogicGroups[selectedGroup].RequiredItemsCount[i]);
			requiredItemsList.SetItemText(item, 1, itemAmount);
		}
	}
}

void LogicCreator::UpdateRequiredMovesList()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	requiredMovesList.DeleteAllItems();
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		for (int i = 0; i < LogicGroups[selectedGroup].RequiredAbilities.size(); i++)
		{
			CString str;
			str.Format("%X", LogicGroups[selectedGroup].RequiredAbilities[i]);
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

	std::ifstream myfile(fileOpen);
	std::string line;
	try {
		if (!myfile.is_open()) {
			throw std::runtime_error("Error: Could not open the file 'LogicFile.txt'.");
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
	LogicGroups.clear();
	myfile.clear();
	myfile.seekg(0);
	while (std::getline(myfile, line)) // Read each line from the file
	{
		int GroupID = 0;
		char* endPtr;
		std::string GroupIdStr = TooieRandoDlg::GetStringAfterTag(line, "GroupId:", ",");
		GroupID = !GroupIdStr.empty() ? strtol(GroupIdStr.c_str(), &endPtr, 16) : -1; //If there is a script reward index
		std::string GroupName = TooieRandoDlg::GetStringAfterTag(line, "GroupName:\"", "\",");
		std::string ItemCountStr = TooieRandoDlg::GetStringAfterTag(line, "RequiredItemCounts:[", "],");
		std::string ItemsStr = TooieRandoDlg::GetStringAfterTag(line, "RequiredItem:[", "],");
		std::string ObjectsInGroupStr = TooieRandoDlg::GetStringAfterTag(line, "ObjectsInGroup:[", "],");
		std::string RequiredMoveStr = TooieRandoDlg::GetStringAfterTag(line, "RequiredMoves:[", "],");
		std::string DependentGroupStr = TooieRandoDlg::GetStringAfterTag(line, "DependentGroups:[", "],");

		LogicGroup NewGroup = LogicGroup(GroupID);
		NewGroup.GroupName = GroupName;
		NewGroup.RequiredItems = TooieRandoDlg::GetVectorFromString(ItemsStr.c_str(), ",");
		NewGroup.RequiredItemsCount = TooieRandoDlg::GetIntVectorFromString(ItemCountStr.c_str(), ",");
		NewGroup.objectIDsInGroup = TooieRandoDlg::GetIntVectorFromString(ObjectsInGroupStr.c_str(), ",");
		NewGroup.RequiredAbilities = TooieRandoDlg::GetIntVectorFromString(RequiredMoveStr.c_str(), ",");
		NewGroup.dependentGroupIDs = TooieRandoDlg::GetIntVectorFromString(DependentGroupStr.c_str(), ",");
		//NewGroup.DependentGroups = ;
		LogicGroups.push_back(NewGroup);
		OutputDebugString(line.c_str());
	}

	selectedGroup = -1;
	UpdateGroupList();
	UpdateDependentGroupList();
	UpdateGroupedItemsList();
	UpdateUngroupedItemsList();
	UpdateRequiredItemsList();
	UpdateRequiredMovesList();

	UpdateGroupSelector();
	UpdateRequiredMovesSelector();
	UpdateRequiredItemSelector();
}


void LogicCreator::OnBnClickedSavelogicfilebutton()
{
	CFileDialog m_svFile(FALSE, NULL, ("LogicFile.txt"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "OUT Logic (*.txt)|*.txt|", this);

	int isFileOpened2 = m_svFile.DoModal();

	if (isFileOpened2 == IDCANCEL)
		return;

	if (m_svFile.GetFileName() == "")
		return;

	FILE* outFile = fopen(m_svFile.GetPathName(), "wb");
	if (outFile == NULL)
	{
		MessageBox("Cannot open output file");
		return;
	}
	for (int i = 0; i < LogicGroups.size(); i++)
	{
		char str[100];
		sprintf(str, "GroupId:%d,", LogicGroups[i].GroupID);
		fwrite(str, 1, strlen(str), outFile);
		sprintf(str, "RequiredMoves:[%s],", intVectorToString(LogicGroups[i].RequiredAbilities).c_str());
		fwrite(str, 1, strlen(str), outFile);
		sprintf(str, "RequiredItem:[%s],", stringVectorToString(LogicGroups[i].RequiredItems).c_str());
		fwrite(str, 1, strlen(str), outFile);
		sprintf(str, "RequiredItemCounts:[%s],", intVectorToString(LogicGroups[i].RequiredItemsCount).c_str());
		fwrite(str, 1, strlen(str), outFile);
		sprintf(str, "DependentGroups:[%s],", intVectorToString(LogicGroups[i].dependentGroupIDs).c_str());
		fwrite(str, 1, strlen(str), outFile);
		sprintf(str, "GroupName:\"%s\",", LogicGroups[i].GroupName.c_str());
		fwrite(str, 1, strlen(str), outFile);
		sprintf(str, "ObjectsInGroup:[%s],", intVectorToString(LogicGroups[i].objectIDsInGroup).c_str());
		fwrite(str, 1, strlen(str), outFile);
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

std::string LogicCreator::intVectorToString(std::vector<int> intVector)
{
	std::string outputString = "";
	for (int i = 0; i < intVector.size(); i++)
	{
		char hex_string[20];
		sprintf(hex_string, "%X", intVector[i]);
		outputString.append(hex_string);
		if(i != intVector.size()-1)
		outputString.append(",");
	}
	return outputString;
}

std::string LogicCreator::stringVectorToString(std::vector<std::string> stringVector)
{
	std::string outputString = "";
	for (int i = 0; i < stringVector.size(); i++)
	{
		outputString.append(stringVector[i]);
		if (i != stringVector.size() - 1)
			outputString.append(",");
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

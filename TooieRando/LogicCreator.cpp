#include "stdafx.h"
#include "LogicCreator.h"
#include "TooieRando.h"
#include "TooieRandoDlg.h"
#include "LogicGroup.h"
#include "RandomizedObject.h"

std::vector<LogicGroup> LogicGroups;
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
END_MESSAGE_MAP()
BOOL LogicCreator::OnInitDialog()
{
	

	CDialog::OnInitDialog();
	LogicGroups.clear();
	selectedGroup = -1;

	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	pParentDlg->LoadObjects();
	for each (RandomizedObject object in pParentDlg->RandomizedObjects)
	{
		OutputDebugString(_T("Randomized Object Found"));

		LVITEM lv;
		lv.iItem = logicGroupsList.GetItemCount();
		lv.iSubItem = 0;
		lv.pszText = "optionName";
		lv.mask = LVIF_TEXT;
		int item = ungroupedObjectsList.InsertItem(&lv);
		ungroupedObjectsList.SetItemText(item, 0, object.LocationName.c_str());
	}
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
	// TODO: Add your control notification handler code here
}


void LogicCreator::OnBnClickedRemoveRequiredMove()
{
	// TODO: Add your control notification handler code here
}


void LogicCreator::OnBnClickedAddRequiredItem()
{
	// TODO: Add your control notification handler code here
}


void LogicCreator::OnBnClickedRemoveRequiredItem()
{
	// TODO: Add your control notification handler code here
}


void LogicCreator::OnBnClickedRemoveDependent()
{
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		LogicGroup* pointerToDependentGroupFromSelector = &LogicGroups[dependentGroupSelector.GetCurSel()];
		auto it = std::find(LogicGroups[selectedGroup].DependentGroups.begin(), LogicGroups[selectedGroup].DependentGroups.end(), pointerToDependentGroupFromSelector);
		LogicGroups[selectedGroup].DependentGroups.erase(it);
	}
	UpdateDependentGroupList();
}


void LogicCreator::OnBnClickedAddDependent()
{
	if (selectedGroup != -1 && selectedGroup < LogicGroups.size())
	{
		LogicGroups[selectedGroup].DependentGroups.push_back(&LogicGroups[dependentGroupSelector.GetCurSel()]);
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
		for (int i = 0; i < LogicGroups[selectedGroup].DependentGroups.size(); i++)
		{
			AddElementToListCntrl(dependentGroupsList, LogicGroups[selectedGroup].DependentGroups[i]->GroupName);
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
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void LogicCreator::OnDblclkObjectInGroupList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void LogicCreator::UpdateUngroupedItemsList()
{
	
}
void LogicCreator::UpdateGroupedItemsList()
{

}
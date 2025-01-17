#include "stdafx.h"
#include "LogicCreator.h"
#include "TooieRando.h"
#include "TooieRandoDlg.h"

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
END_MESSAGE_MAP()
BOOL LogicCreator::OnInitDialog()
{
	CDialog::OnInitDialog();
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
	ungroupedObjectsList.InsertColumn(0, "Item Location Names", LVCFMT_LEFT, 700);
	ungroupedObjectsList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	ungroupedObjectsList.SetExtendedStyle(LVS_EX_GRIDLINES);

	logicGroupsList.InsertColumn(0, "Group Names", LVCFMT_LEFT, 700);
	logicGroupsList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	logicGroupsList.SetExtendedStyle(LVS_EX_GRIDLINES);
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
	// TODO: Add your control notification handler code here
}


void LogicCreator::OnBnClickedAddDependent()
{
	// TODO: Add your control notification handler code here
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
	LVITEM lv;
	lv.iItem = logicGroupsList.GetItemCount();
	lv.iSubItem = 0;
	lv.pszText = "optionName";
	lv.mask = LVIF_TEXT;
	int item = logicGroupsList.InsertItem(&lv);
	CString newGroupName;
	groupNameBox.GetWindowText(newGroupName);
	logicGroupsList.SetItemText(item, 0, newGroupName);
	// TODO: Add your control notification handler code here
}


void LogicCreator::OnBnClickedDeleteGroup()
{
	// TODO: Add your control notification handler code here
}

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "resource.h"
#include <string>
#include <vector>
#include "LogicGroup.h"
#include <unordered_map>
#include <map>

// LogicCreator dialog

class LogicCreator : public CDialog
{
	//DECLARE_DYNAMIC(LogicCreator)

public:
	LogicCreator(CWnd* pParent = nullptr);   // standard constructor
	virtual ~LogicCreator();
	enum { IDD = IDD_LOGIC_CREATOR };

// Dialog Data


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CEdit numRequiredItemsBox;
	CButton removeDependentGroupButton;
	CButton addDependentGroupButton;
	CButton removeRequiredMoveButton;
	CButton addRequiredMoveButton;
	CButton removeRequiredItemButton;
	CButton addRequiredItemButton;
	CListCtrl logicGroupsList;
	CListCtrl dependentGroupsList;
	CListCtrl objectsInGroupList;
	CListCtrl ungroupedObjectsList;
	CListCtrl requiredMovesList;
	CListCtrl requiredItemsList;
	CComboBox requiredMoveSelector;
	CComboBox requiredItemSelector;
	CComboBox dependentGroupSelector;
	CComboBox requirementSetSelector;

	CEdit requirementSetNameBox;
	CButton addRequirementSetButton;
	CButton deleteRequirementSetButton;
	CEdit groupNameBox;
	CButton newGroupButton;
	CButton removeGroupButton;

	CEdit keyRewardBox;
	CListCtrl requiredKeysList;
	CComboBox requiredKeySelector;
	CButton addRequiredKeyButton;
	CButton removeRequiredKeyButton;

	CEdit groupSearchBox;
	CEdit groupedSearchBox;
	CEdit ungroupedSearchBox;

	CEdit shuffleGroupBox;
	CEdit associatedWarpBox;
	CEdit specialTagBox;


	CComboBox moveLocationSelector;

	afx_msg void OnCbnSelchangeCombo2();
	afx_msg void OnLbnSelchangeLogicGroupList();
	afx_msg void OnBnClickedAddRequiredMove();
	afx_msg void OnBnClickedRemoveRequiredMove();
	afx_msg void OnBnClickedAddRequiredItem();
	afx_msg void OnBnClickedRemoveRequiredItem();
	afx_msg void OnBnClickedRemoveDependent();
	afx_msg void OnBnClickedAddDependent();
	afx_msg void OnLbnSelchangeObjectInGroupList();
	afx_msg void OnLbnSelchangeUngroupedList();
	afx_msg void OnBnClickedCreateNewGroup();
	afx_msg void OnBnClickedDeleteGroup();
	void UpdateGroupList();
	void UpdateDependentGroupList();
	void UpdateGroupSelector();
	afx_msg void OnDblclkLogicGroupList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnChangeGroupNameEditBox();
	afx_msg void OnDblclkUngroupedList(NMHDR* pNMHDR, LRESULT* pResult);
	void SelectGroupByPointer(LogicGroup* selectedLogicGroup);
	void SelectGroupByIndex(int newSelection);
	afx_msg void OnDblclkObjectInGroupList(NMHDR* pNMHDR, LRESULT* pResult);
	void UpdateUngroupedItemsList();
	void UpdateGroupedItemsList();
	void UpdateRequiredMovesList();
	void UpdateRequiredItemsList();
	void UpdateRequiredMovesSelector();
	void UpdateRequiredItemSelector();
	LogicGroup GetLogicGroupFromId(int groupID);
	afx_msg void OnBnClickedLoadlogicfilebutton();
	afx_msg void OnBnClickedSavelogicfilebutton();
	void Savelogicfile(CString filepath);
	static LogicGroup GetLogicGroupContainingObjectId(int objectID, std::map<int, LogicGroup>& logicGroups);
	static LogicGroup GetLogicGroupContainingMoveId(int moveID, std::map<int, LogicGroup>& logicGroups);
	void SaveRandomizerObjectEdits();
	afx_msg void OnDblclkDependentGroupList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedCreateNewRequirementSet();
	afx_msg void OnBnClickedDeleteRequirementset();
	void UpdateRequirementSelector();
	afx_msg void OnCbnSelchangeRequirementSelector();
	afx_msg void OnDblclkRequirementSelector();
	afx_msg void OnEnChangeRequirementSetEditbox();
	void RequirementSetSelector(int setToSelect);
	afx_msg void OnEnChangeRewardKey();
	void UpdateRequiredKeyList();
	void UpdateRequiredKeySelector();
	void UpdateRewardKey();
	void UpdateAssociatedWarp();
	void UpdateShuffleGroup();
	void UpdateSpecialTag();
	afx_msg
	void OnBnClickedAddRequiredKey();
	afx_msg void OnBnClickedRemoveRequiredKey();
	afx_msg void OnEnChangeSearchGroupsBox();
	afx_msg void OnEnChangeSearchGroupedBox();
	afx_msg void OnEnChangeSearchUngroupedBox();
	void UpdateMoveLocationList();
	afx_msg void OnCbnSelchangeMoveLocationsList();
	afx_msg void OnEnChangeAssociatedwarp();
	afx_msg void OnEnChangeDependentShufflegroup();
	afx_msg void OnEnChangeSpecialTag();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEditchangeDependentGroupSelect();
	afx_msg void OnEditchangeRequiredMoveSelection();
	afx_msg void OnEditchangeRequiredKeySelection();
};
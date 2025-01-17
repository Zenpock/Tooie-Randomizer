#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "resource.h"

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

	CEdit groupNameBox;
	CButton newGroupButton;
	CButton removeGroupButton;

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
};

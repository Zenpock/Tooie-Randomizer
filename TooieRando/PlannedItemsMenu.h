#pragma once
#include "afxdialogex.h"


// PlannedItemsMenu dialog

class PlannedItemsMenu : public CDialogEx
{
	DECLARE_DYNAMIC(PlannedItemsMenu)

public:
	PlannedItemsMenu(CWnd* pParent = nullptr);   // standard constructor
	virtual ~PlannedItemsMenu();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PLANNED_ITEMS_MENU };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg
		BOOL OnInitDialog();
	void OnCbnSelchangeLocationSelect();
	afx_msg void OnCbnSelchangePlaceditemSelect();
	afx_msg void OnCbnSelchangeWorldEntSelect();
	afx_msg void OnCbnSelchangeWorldExitSelect();
	afx_msg void OnCbnSelchangeItemHintSelect();
	afx_msg void OnCbnSelchangeHintLocationSelect();
	void SerializePlannedSettings();

	CComboBox placedItemSelector;
	CComboBox locationSelector;
	CComboBox worldEntSelector;
	CComboBox worldExitSelector;
	CComboBox itemHintSelector;
	CComboBox hintLocationSelector;
};

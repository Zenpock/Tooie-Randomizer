#pragma once
#include "afxdialogex.h"
#include <string>
#include <vector>

#include "DataPaths.h"


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
	int GetPlannedForLocation(int location, CComboBox& selector, std::vector<std::pair<int, int>>& planned, bool indexByFirst = true);
	afx_msg void OnCbnSelchangePlaceditemSelect();
	afx_msg void OnCbnSelchangeWorldEntSelect();
	afx_msg void OnCbnSelchangeWorldExitSelect();
	afx_msg void OnCbnSelchangeItemHintSelect();
	afx_msg void OnCbnSelchangeHintLocationSelect();
	void SerializePlannedSettings(std::string path = PlandoSettingsFile);

	CComboBox placedItemSelector;
	CComboBox locationSelector;
	CComboBox worldEntSelector;
	CComboBox worldExitSelector;
	CComboBox itemHintSelector;
	CComboBox hintLocationSelector;
	afx_msg void OnBnClickedResetPlando();
	afx_msg void OnBnClickedImportPlanned();
	afx_msg void OnBnClickedExportPlanned();
	void RecreateSelects();
	void RecreatePlaceableItems(bool keepIndex = true);
};

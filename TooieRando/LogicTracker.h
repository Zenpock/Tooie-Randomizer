#pragma once
#include "afxdialogex.h"
#include "LogicHandler.h"


// LogicTracker dialog

class LogicTracker : public CDialogEx
{
	DECLARE_DYNAMIC(LogicTracker)

public:
	LogicTracker(CWnd* pParent = nullptr);   // standard constructor
	virtual ~LogicTracker();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGIC_TRACKER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:

	CListCtrl openedWorldsList;
	CListCtrl foundMovesList;
	CListCtrl markedChecksList;
	CListCtrl availableChecksList;
	CComboBox logicSelector;

	afx_msg void OnItemdblclickWorldsopened(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkWorldsopened(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkCollectedMoves(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkMarkedChecks(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkAvailableChecks(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeLogicFile();
	bool AlreadyMarked(int itemType, int checkID);
	void UpdateLists();

};

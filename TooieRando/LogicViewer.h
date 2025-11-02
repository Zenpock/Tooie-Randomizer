#pragma once
#include "afxdialogex.h"


// LogicViewer dialog

class LogicViewer : public CDialogEx
{
	DECLARE_DYNAMIC(LogicViewer)

public:
	LogicViewer(CWnd* pParent = nullptr);   // standard constructor
	virtual ~LogicViewer();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGIC_VIEWER };
#endif

private:
	std::unordered_map<int, LogicGroup> logicGroups;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CComboBox logicSelector;

	afx_msg void OnBnClickedDrawDisplayButton();
	afx_msg void OnCbnSelchangeLogicSelect();
	void DrawLogicBox(int x, int y, LogicGroup* group, CClientDC* pDC);
	void DrawLogicBoxColor(int x, int y, LogicGroup* group, CBrush* brush, CClientDC* pDC);
	void DrawLine(CPoint, CPoint, CClientDC*);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void Reset();
	void Redraw();
	void AttemptPlaceGroup(LogicGroup* group, LogicGroup* parent, int depth, int index);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	void TraverseGroups(int depth, int indexOfPrevious, LogicGroup* group, LogicGroup* parent);
};

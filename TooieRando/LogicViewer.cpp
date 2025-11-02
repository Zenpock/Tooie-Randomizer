
#include "stdafx.h"
#include "LogicGroup.h"
#include "TooieRando.h"

#include "TooieRando.h"
#include "afxdialogex.h"
#include "LogicViewer.h"
#include "TooieRandoDlg.h"
#include <map>

int zoom{5};
int minZoom{ 5 };
int maxZoom{ 500 };
CPoint panOffset{0,0};
bool isMoving{ false };
CPoint lastClickLocation{ 0,0 };

struct LinkedObjects
{
	int column;
	int row;
};

std::map<int, LinkedObjects> GroupsDisplay;//GroupID, Row, Column
std::vector<int> depthSizes;
IMPLEMENT_DYNAMIC(LogicViewer, CDialogEx)

LogicViewer::LogicViewer(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGIC_VIEWER, pParent)
{

}

LogicViewer::~LogicViewer()
{
}

void LogicViewer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGIC_SELECT, logicSelector);

}

BOOL LogicViewer::OnInitDialog()
{
	CDialog::OnInitDialog();
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	pParentDlg->LogicFilePaths = LogicGroup::LoadLogicFileOptions();
	for (int i = 0; i < pParentDlg->LogicFilePaths.size(); i++)
	{
		logicSelector.AddString(std::get<0>(pParentDlg->LogicFilePaths[i]).c_str());
	}
	Redraw();
	return 0;
}

BEGIN_MESSAGE_MAP(LogicViewer, CDialogEx)
	ON_BN_CLICKED(IDC_DRAW_DISPLAY_BUTTON, &LogicViewer::OnBnClickedDrawDisplayButton)
	ON_CBN_SELCHANGE(IDC_LOGIC_SELECT, &LogicViewer::OnCbnSelchangeLogicSelect)
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// LogicViewer message handlers

void LogicViewer::OnBnClickedDrawDisplayButton()
{
	Reset();
	Redraw();
}

void LogicViewer::OnCbnSelchangeLogicSelect()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();

	logicGroups.clear();
	LogicGroup::LoadLogicGroupsFromFile(logicGroups, std::get<1>(pParentDlg->LogicFilePaths[logicSelector.GetCurSel()]).c_str());
	LogicHandler::AccessibleThings state{};
	state.SetWarps.push_back(std::make_pair(0x1, 0x2));
	state.SetWarps.push_back(std::make_pair(0x3, 0x4));
	state.SetWarps.push_back(std::make_pair(0x5, 0x6));
	state.SetWarps.push_back(std::make_pair(0x7, 0x8));
	state.SetWarps.push_back(std::make_pair(0x9, 0xA));
	state.SetWarps.push_back(std::make_pair(0xB, 0xC));
	state.SetWarps.push_back(std::make_pair(0xD, 0xE));
	state.SetWarps.push_back(std::make_pair(0xF, 0x10));
	state.SetWarps.push_back(std::make_pair(0x11, 0x12));

	LogicHandler newLogicHandler = LogicHandler();
	std::unordered_map<int, RandomizedObject> objectMap;
	if (!newLogicHandler.alreadySetup)
	{
		for (const auto& obj : logicGroups)
		{
			if (obj.second.AssociatedWarp != -1)
				newLogicHandler.entranceAssociations[obj.second.AssociatedWarp] = obj.first;
		}
	}

	for (auto i : logicGroups)
	{
		LogicHandler::HandleSpecialTags(&i.second, &state);
	}

	Redraw();
}

void LogicViewer::DrawLogicBox(int x, int y, LogicGroup* group, CClientDC* pDC)
{
	DrawLogicBoxColor(x, y, group, &CBrush(RGB(200, 0, 0)),pDC);
}

void LogicViewer::DrawLogicBoxColor(int x, int y, LogicGroup* group, CBrush* brush,CClientDC* pDC)
{
	CStatic* pbox = (CStatic*)GetDlgItem(IDC_DISPLAY_BOX);
	CRect rect = CRect(0, 0 , 4* zoom, 2* zoom)+CPoint(x* zoom,y* zoom)+ panOffset;
	pDC->FillRect(rect, brush);
	pDC->DrawText(group->GroupName.c_str(), rect, DT_END_ELLIPSIS);
}

void LogicViewer::DrawLine(CPoint point1, CPoint point2, CClientDC* pDC)
{
	pDC->MoveTo(CPoint(point1.x * zoom, point1.y * zoom) + panOffset);
	pDC->LineTo(CPoint(point2.x * zoom, point2.y * zoom) + panOffset);
}


BOOL LogicViewer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int positive = zDelta > 0?1:-1;
	if(zoom + positive < minZoom)
		zoom = minZoom;
	else if (zoom + positive > maxZoom)
		zoom = maxZoom;
	else
		zoom += positive;

	Redraw();
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}

void LogicViewer::OnMouseMove(UINT nFlags, CPoint point)
{
	if (isMoving)
	{
		panOffset = point-lastClickLocation;
		Redraw();

	}

	CDialogEx::OnMouseMove(nFlags, point);
}


void LogicViewer::Reset()
{
	GroupsDisplay.clear();
	depthSizes.clear();
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();

	int startingLogicGroup = std::get<2>(pParentDlg->LogicFilePaths[logicSelector.GetCurSel()]);
	LogicGroup* start = &logicGroups[startingLogicGroup];
	TraverseGroups(0,0, start,NULL);



	zoom = minZoom;
	panOffset = { 0,0 };
	lastClickLocation = { 0,0 };
	isMoving = false;
}
void LogicViewer::Redraw()
{
	CStatic* pbox = (CStatic*)GetDlgItem(IDC_DISPLAY_BOX);
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	HFONT font = CreateFont(zoom/2, 0, 0, 0, FW_DONTCARE, FALSE, TRUE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH, TEXT("Arial"));

	CRect rect;
	pbox->GetClientRect(&rect);
	CClientDC pDC(pbox);
	pDC.IntersectClipRect(rect);
	SelectObject(pDC, font);
	pDC.FillRect(rect, &CBrush(RGB( 256,  158, 158)));

	for (auto group : GroupsDisplay)
	{
		//pDC.MoveTo(CPoint(i * 5 * zoom, j*5 * zoom) + panOffset);
		//pDC.LineTo(CPoint(50 * zoom, 50 * zoom)+panOffset);
		DrawLogicBox(group.second.column*5, group.second.row * 5, &logicGroups[group.first], &pDC);

		for (int index = 0; index < logicGroups[group.first].dependentGroupIDs.size(); index++)
		{
			LinkedObjects* object = &GroupsDisplay[logicGroups[group.first].dependentGroupIDs[index]];
			DrawLine(CPoint(group.second.column * 5+4, group.second.row * 5+1), CPoint(object->column * 5, object->row * 5), &pDC);
		}
		//DrawLogicBoxColor(i * 5, height, GroupsDisplay[i][j][k], &CBrush(RGB((k*120)%256, 100 + (i * 21) % 158, 100 + (j * 14) % 158)), &pDC);
		//DrawLine(CPoint(i * 5, k * 5), CPoint(i * 5, j * 5), &pDC);			
	}

	/*
	
	DrawLogicBoxColor(2, 5, start, &CBrush(RGB(0, 250, 0)), &pDC);
	for (int i = 0; i < start->dependentGroupIDs.size(); i++)
	{
		DrawLogicBox(10, 5 * i, &logicGroups[start->dependentGroupIDs[i]], &pDC);
	}
	*/

}

void LogicViewer::AttemptPlaceGroup(LogicGroup* group, LogicGroup* parent,int depth,int index)
{
	if (GroupsDisplay.find(group->GroupID) == GroupsDisplay.end())
	{
		while (depthSizes.size() <= depth)
		{
			depthSizes.push_back(0);
		}
		GroupsDisplay[group->GroupID].column = depth;
		GroupsDisplay[group->GroupID].row = depthSizes[depth];
		depthSizes[depth]++;
	}

}

void LogicViewer::OnLButtonDown(UINT nFlags, CPoint point)
{
	lastClickLocation = point - panOffset;
	isMoving = true;
	CDialogEx::OnLButtonDown(nFlags, point);
}

void LogicViewer::OnLButtonUp(UINT nFlags, CPoint point)
{
	isMoving = false;
	CDialogEx::OnLButtonUp(nFlags, point);
}

void LogicViewer::TraverseGroups(int depth,int indexOfPrevious,LogicGroup* group, LogicGroup* parent)
{
	AttemptPlaceGroup(group,parent, depth, indexOfPrevious);
	for (int i = 0; i < group->dependentGroupIDs.size();i++)
	{
		TraverseGroups(depth + 1,i, &logicGroups[group->dependentGroupIDs[i]], group);
	}
}

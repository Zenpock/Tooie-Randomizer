// CChangeLength dialogs
#pragma once
#include "TooieRandoDlg.h"
#include "resource.h"
class CChangeLength : public CDialog
{
	int startTableAddress = 0;//Address of the table element that defines the start of the asset
	int startAddress = 0;//Start of the asset in the rom
	int nextStartAddress = 0;//Start of the asset after the chosen asset
	int diff = 0;//Difference between the original end of an asset and the new defined size
	int offsetsStart = 0;//The location in the file where the offsets start
	int numOffsets = 0; //The number of offsets to try and set
	int offsetsTarget = 0; //The relative location for the offset to redirect to
	int offsetsLocation = 0; //The location of the offset to change
	int offsetsLocationLUI = 0; //The location of the first part of the LUI ADDIU pair
	int offsetXOR = 0; //The XOR Value for the offset
	int offsetType = 0; //The type of offset currently shown
	int selectedIndex = 0; //The Index of the selected item
	int codeStart = 0; //The address within the file where the code actually starts
public:
	CChangeLength();
	CChangeLength(CWnd* pParent /*=nullptr*/, int start)
		: CDialog(IDD_CHANGE_LENGTH, pParent)
	{
		char* endPtr;
		TooieRandoDlg* pParentDlg = (TooieRandoDlg*)pParent;
		startTableAddress = strtol(pParentDlg->m_list.GetItemText(start, 0), &endPtr, 16);
		selectedIndex = start;
	}
	// Dialog Data
	enum { IDD = IDD_CHANGE_LENGTH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedConfirmextension();
	afx_msg void OnEnChangeStartAssetAddress();
	CEdit start_Address_Box;
	CEdit end_Address_Box;
	CEdit num_Offset_Box;
	CEdit offset_Index_Box;
	CEdit offset_Location_Box;
	CEdit offset_Target_Box;
	CEdit offset_ShiftStart_Box;
	CEdit offset_ShiftAmount_Box;

	CComboBox offset_TypeList;
	CButton offsetApplyButton;
	CButton offsetShiftButton;

	virtual BOOL OnInitDialog();
	afx_msg void OnClickedConfirmextension();
	void CChangeLength::UpdateSyscallTable(int start, int end, int diff);
	void CChangeLength::UpdateOffsetAtIndex(int index, CString fileLocation);
	void UpdateAssetTable(int start, int end, int diff);
	bool CChangeLength::ShiftAssets(int startAssetAddress, int endAllowedSpace, int diff, int endTable);
	void CChangeLength::UpdateBasedOnLocation(int location, CString fileLocation);

	afx_msg void OnEnChangeOffsetIndex();
	afx_msg void OnBnClickedApplyOffset();
	afx_msg void OnEnChangeOffsetLocation();
	void UpdateRelativeShifts(int startAddress, int amount);
	afx_msg void OnBnClickedOffShiftButton();
	void UpdateNumOffsetInRom();
	afx_msg void OnBnClickedRemoveOffset();
	afx_msg void OnEnChangeNumoffset();
};

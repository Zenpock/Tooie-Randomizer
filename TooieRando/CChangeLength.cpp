#include "stdafx.h"
#include "CChangeLength.h"
#include "resource.h"
CChangeLength::CChangeLength() : CDialog(CChangeLength::IDD)
{
}
void CChangeLength::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_START_ASSET_ADDRESS, start_Address_Box);
	DDX_Control(pDX, IDC_END_ASSET_ADDRESS, end_Address_Box);
	DDX_Control(pDX, IDC_NUMOFFSET, num_Offset_Box);
	DDX_Control(pDX, IDC_OFFSET_INDEX, offset_Index_Box);
	DDX_Control(pDX, IDC_OFFSET_LOCATION, offset_Location_Box);
	DDX_Control(pDX, IDC_OFFSET_TARGET, offset_Target_Box);
	DDX_Control(pDX, IDC_OFFSET_TYPELIST, offset_TypeList);
	DDX_Control(pDX, IDC_APPLY_OFFSET, offsetApplyButton);
	DDX_Control(pDX, IDC_OFF_SHIFT_AMOUNT, offset_ShiftAmount_Box);
	DDX_Control(pDX, IDC_OFF_SHIFT_START, offset_ShiftStart_Box);
	DDX_Control(pDX, IDC_OFF_SHIFT_BUTTON, offsetShiftButton);
}

BEGIN_MESSAGE_MAP(CChangeLength, CDialog)
	ON_BN_CLICKED(IDC_ConfirmExtension, &CChangeLength::OnClickedConfirmextension)
	ON_EN_CHANGE(IDC_OFFSET_INDEX, &CChangeLength::OnEnChangeOffsetIndex)
	ON_BN_CLICKED(IDC_APPLY_OFFSET, &CChangeLength::OnBnClickedApplyOffset)
	ON_EN_CHANGE(IDC_OFFSET_LOCATION, &CChangeLength::OnEnChangeOffsetLocation)
	ON_BN_CLICKED(IDC_OFF_SHIFT_BUTTON, &CChangeLength::OnBnClickedOffShiftButton)
	ON_BN_CLICKED(IDC_REMOVE_OFFSET, &CChangeLength::OnBnClickedRemoveOffset)
	ON_EN_CHANGE(IDC_NUMOFFSET, &CChangeLength::OnEnChangeNumoffset)
END_MESSAGE_MAP()

void CChangeLength::OnBnClickedOffShiftButton()
{
	CString offShiftStartStr;
	CString offShiftAmountStr;
	offset_ShiftStart_Box.GetWindowText(offShiftStartStr);
	offset_ShiftAmount_Box.GetWindowText(offShiftAmountStr);

	char* p;
	int offShiftStart = strtol(offShiftStartStr.GetString(), &p, 16);
	int offShiftAmount = strtol(offShiftAmountStr.GetString(), &p, 16);

	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	CString originalFileLocation = pParentDlg->m_list.GetItemText(selectedIndex, 4);
	if (offShiftAmount > 0)
	{
		std::vector<unsigned char> zeroBuffer(offShiftAmount, 0);

		//Add the space before offsetting
		pParentDlg->ReplaceFileDataAtAddressResize(codeStart + offShiftStart, originalFileLocation, 0,offShiftAmount, zeroBuffer.data());
	}

	int i = 0;
	for (int i = 0; i < numOffsets; i++)
	{
		CString index;
		index.Format("%X", i);
		offset_Index_Box.SetWindowTextA(index);
		if (offsetsLocation >= offShiftStart) //Update target before offset
		{
			CString newLocation;
			newLocation.Format("%X", offShiftAmount + offsetsLocation);
			char message[256];
			sprintf(message, "Offset Location 0x%X Shift Amount 0x%X index %X\n", offsetsLocation, offShiftAmount, index);
			OutputDebugString(message);
			offset_Location_Box.SetWindowTextA(newLocation);
			OnBnClickedApplyOffset();
		}
		if (offsetsTarget >= offShiftStart)
		{
			CString newLocation;
			newLocation.Format("%X", offShiftAmount + offsetsTarget);
			char message[256];
			sprintf(message, "Offset Target 0x%X Shift Amount 0x%X  index %X\n", offsetsTarget, offShiftAmount, index);
			OutputDebugString(message);
			offset_Target_Box.SetWindowTextA(newLocation);
			OnBnClickedApplyOffset();
		}
	}
	UpdateRelativeShifts(offShiftStart,offShiftAmount);

	//Increase the amount of executable space if we add more
	int executableSize = pParentDlg->GetIntFromROM(pParentDlg->syscallTableStart + startAddress + 0xA, 2);
	unsigned char* RomFromParent = pParentDlg->ROM;
	executableSize += offShiftAmount;
	RomFromParent[pParentDlg->syscallTableStart + startAddress] = executableSize >> 8;
	RomFromParent[pParentDlg->syscallTableStart + startAddress+1] = executableSize;
	//TODO:Shift the entry points before the file


	char message[256];
	sprintf(message, "Offsets shifted by 0x%X\n", offShiftAmount);
	MessageBox(message);
}
void CChangeLength::OnClickedConfirmextension()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();

	CString newNextAddressStr;
	end_Address_Box.GetWindowTextA(newNextAddressStr);
	char* endPtr;
	int newNextStartAddress = strtol(newNextAddressStr, &endPtr, 16);
	char message[256];
	if (startTableAddress > pParentDlg->syscallTableStart) //Scripts
	{
		unsigned char* ROMFromParent = pParentDlg->ROM;

		int assetAddress = pParentDlg->GetIntFromROM(pParentDlg->syscallTableStart + 0xDD0, 0x4);
		//char message[256];
		//sprintf(message, "Syscall Table Start Address 0x%X Asset Address 0x%X\n", pParentDlg->syscallTableStart, assetAddress);
		//MessageBox(message);
		bool shiftSuccess = ShiftAssets(pParentDlg->syscallTableStart + nextStartAddress, pParentDlg->ROMSize, newNextStartAddress - nextStartAddress, assetAddress + pParentDlg->syscallTableStart);
		if (shiftSuccess)
			UpdateSyscallTable(startTableAddress + 4, pParentDlg->syscallTableStart + 0xDD0, newNextStartAddress - nextStartAddress);
	}
	else if (startTableAddress == pParentDlg->core1Start || startTableAddress == pParentDlg->core2Start || startTableAddress == pParentDlg->core3Start || startTableAddress == pParentDlg->core4Start)
	{
		char message[256];
		sprintf(message, "Attempting to change core size Address 0x%X\n", startTableAddress);
		MessageBox(message);
	}
	else //Assets
	{
		unsigned char* ROMFromParent = pParentDlg->ROM;

		char hexString[9];
		snprintf(hexString, sizeof(hexString), "%08X", pParentDlg->core3Start);
		int index = pParentDlg->FindItemInListCtrl(pParentDlg->m_list, hexString, 0);
		if (index == -1)
			return;
		CString originalFileLocation = pParentDlg->m_list.GetItemText(index, 4);
		unsigned char buffer[4];
		pParentDlg->GetFileDataAtAddress(0x67D92, originalFileLocation, 4, buffer);
		int soundStartAddress = buffer[0] << 8 | buffer[1];
		pParentDlg->GetFileDataAtAddress(0x67DA2, originalFileLocation, 4, buffer);
		soundStartAddress = soundStartAddress << 16 | buffer[0] << 8 | buffer[1];


		int assetAddress = pParentDlg->GetIntFromROM(0x12B20, 0x3);

		bool shiftSuccess = ShiftAssets(nextStartAddress, soundStartAddress, newNextStartAddress - nextStartAddress, assetAddress * 4 + 0x12B24);
		if (shiftSuccess)
			UpdateAssetTable(startTableAddress + 4, 0x12B20, newNextStartAddress - nextStartAddress);
	}
	pParentDlg->OnBnClickedButtonsaverom();
	EndDialog(0);
}


void CChangeLength::UpdateSyscallTable(int start, int end, int diff)
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();

	if (pParentDlg != nullptr)
	{
		unsigned char* ROMFromParent = pParentDlg->ROM;
		CString folderPath;

		unsigned int assetAddress = 0;//Address of the asset
		int currentAddress = start;//Address within the table
		while (currentAddress <= end)
		{
			assetAddress = pParentDlg->GetIntFromROM(currentAddress, 4);
			assetAddress += diff;
			unsigned char hexString[4];
			ROMFromParent[currentAddress] = assetAddress >> 24;
			ROMFromParent[currentAddress + 1] = assetAddress >> 16 & 0xff;
			ROMFromParent[currentAddress + 2] = assetAddress >> 8 & 0xff;
			ROMFromParent[currentAddress + 3] = assetAddress & 0xff;
			currentAddress += 4;
		}
	}
}

void CChangeLength::UpdateAssetTable(int start, int end, int diff)
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();

	if (pParentDlg != nullptr)
	{
		unsigned char* ROMFromParent = pParentDlg->ROM;
		CString folderPath;

		unsigned int assetAddress = 0;//Address of the asset
		int currentAddress = start;//Address within the table
		char message[256];
		while (currentAddress <= end)
		{
			assetAddress = pParentDlg->GetIntFromROM(currentAddress, 3);
			assetAddress += diff / 4;
			unsigned char hexString[4];
			ROMFromParent[currentAddress] = assetAddress >> 16;
			ROMFromParent[currentAddress + 1] = assetAddress >> 8 & 0xff;
			ROMFromParent[currentAddress + 2] = assetAddress & 0xff;
			currentAddress += 4;
		}
	}
}

/// <summary>
/// Shift the assets after from the start of the given address
/// </summary>
/// <param name="startAssetAddress"></param>
/// <param name="endAllowedSpace"></param>
/// <param name="diff"></param>
/// <param name="endTable"></param>
/// <returns>Whether the shift could be performed</returns>
bool CChangeLength::ShiftAssets(int startAssetAddress, int endAllowedSpace, int diff, int endMoveRegion)
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();

	if (pParentDlg != nullptr)
	{
		unsigned char* ROMFromParent = pParentDlg->ROM;

		//int originalSize = endAssetAddress - startAssetAddress;

		// Number of positions to shift
		int shiftAmount = diff;

		// Position where to start shifting
		int position = startAssetAddress;
		// Value to fill the shifted area
		unsigned char fillValue = 0xAA;
		char message[256];
		sprintf(message, "Position 0x%X Shift Amount 0x%X End Allowed: 0x%X\n", position, shiftAmount, endAllowedSpace);
		MessageBox(message);
		// Ensure there's enough room after the shift
		if (endMoveRegion + shiftAmount <= endAllowedSpace) {
			// Shift elements to the right (keeping size same)
			std::memmove(&ROMFromParent[position + shiftAmount], &ROMFromParent[position], (endMoveRegion - position) * sizeof(unsigned char));

			// Fill the gap with the desired value
			for (int i = position; i < position + shiftAmount; ++i) {
				ROMFromParent[i] = fillValue;
			}
			return true;
		}
		else
		{
			sprintf(message, "Shift would move outside of allowed Area Calculated End 0x%X End Allowed: 0x%X\n", endMoveRegion + shiftAmount, endAllowedSpace);
			MessageBox(message);
			return false;
		}
	}
}
/// <summary>
/// Given the offset index get the offset data from the code start that the index is pointing to in the offsets list
/// </summary>
/// <param name="offset"></param>
/// <returns></returns>
void CChangeLength::UpdateOffsetAtIndex(int index, CString fileLocation)
{

	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	unsigned char buffer[2];
	pParentDlg->GetFileDataAtAddress(offsetsStart + index * 2, fileLocation, 2, &buffer[0]);
	offsetsLocation = (buffer[0] << 8 | buffer[1]) ^ offsetXOR;
	if (offsetsLocation == offsetXOR)
	{
		offset_Location_Box.SetWindowTextA(0);
		offsetType = offsetsLocation & 0x3;
		offsetsTarget = 0;
		return;
	}
	offsetType = offsetsLocation & 0x3;
	offsetsLocation = offsetsLocation ^ offsetType; //Remove the offset type
	offsetsTarget = 0;

	UpdateBasedOnLocation(offsetsLocation, fileLocation);

	char message[256];
	sprintf(message, "Find new offset at Index %X location %X type %X target %X", index, offsetsLocation, offsetType, offsetsTarget);
	//MessageBox(_T(message));
	CString offLocString;
	offLocString.Format("%X", offsetsLocation);

	offset_Location_Box.SetWindowTextA(offLocString);


}
void CChangeLength::UpdateBasedOnLocation(int location, CString fileLocation)
{

	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();

	switch (offsetType)
	{
	case 0:
		//Change a relative offset to a full 4 byte rom address EX: 0x000001F4 would become 0x8019786
		offsetsTarget = pParentDlg->GetIntAtAddress(codeStart + location, fileLocation, 4);
		offset_TypeList.SelectString(0, "FullWord");

		break;
	case 1:
		//JAL / J EX:0C00007F would become 0x80191B50 Jals address are divided by 4 so 7F*0x4 is the place to jump to from the code start
		offsetsTarget = pParentDlg->GetIntAtAddress(codeStart + location, fileLocation, 4);
		offsetsTarget = offsetsTarget - (offsetsTarget & 0xFC000000);
		offsetsTarget = offsetsTarget * 4;
		offset_TypeList.SelectString(0, "JAL");
		break;
	case 2:
		//Used to indicate the LUI of an LUI and addiu pair EX: for 0x8019D250 would be 0x8019
		offsetsLocationLUI = offsetsLocation;
		offset_TypeList.SelectString(0, "LUI Init");
		break;
	case 3:
		//Follows a case 2 and contains the data for the target location
		offsetsTarget = pParentDlg->GetIntAtAddress(codeStart + location + 2, fileLocation, 2);
		//The location of the Addiu EX: for 0x8019D250 would be 0xD250
		offset_TypeList.SelectString(0, "LUI End");
		break;
	}
	CString offTarString;
	offTarString.Format("%X", offsetsTarget);
	offset_Target_Box.SetWindowTextA(offTarString);
	if (offsetType == 2)
		offset_Target_Box.SetWindowTextA("N/A");
}


void CChangeLength::OnBnClickedApplyOffset()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	CString originalFileLocation = pParentDlg->m_list.GetItemText(selectedIndex, 4);

	//std::string buf = "0000";
	CString offIndStr;
	offset_Index_Box.GetWindowText(offIndStr);

	CString numOffStr;
	num_Offset_Box.GetWindowText(numOffStr);

	CString offLocStr;
	offset_Location_Box.GetWindowText(offLocStr);

	CString offTarStr;
	offset_Target_Box.GetWindowText(offTarStr);

	char* p;
	int numOffNew = strtol(numOffStr.GetString(), &p, 16);
	int offLocNew = strtol(offLocStr.GetString(), &p, 16);
	int offTypeNew = offset_TypeList.GetCurSel();
	int offTarNew = strtol(offTarStr.GetString(), &p, 16);
	int offInd = strtol(offIndStr.GetString(), &p, 16);
	
	if (numOffNew > numOffsets)
	{
		unsigned char* RomFromParent = pParentDlg->ROM;
		RomFromParent[pParentDlg->syscallTableStart + startAddress + 0xA] = numOffNew >> 8;
		RomFromParent[pParentDlg->syscallTableStart + startAddress + 0xA + 0x1] = numOffNew;
		
		int newCodeStart = offsetsStart + (numOffNew << 1) + 0x3; //800820DC 800820E0 800820E8
		newCodeStart = newCodeStart - (newCodeStart & 0x3); //800820EC-F0
		newCodeStart = newCodeStart + (pParentDlg->GetIntFromROM(pParentDlg->syscallTableStart + startAddress + 0xC, 2) << 2);
		newCodeStart += 0xF;//80082100
		newCodeStart = newCodeStart - (newCodeStart & 0xF);
		

		int sizeToADD = newCodeStart-codeStart;
		std::vector<unsigned char> buffer(sizeToADD, 0);
		pParentDlg->ReplaceFileDataAtAddressResize(codeStart, originalFileLocation, 0, sizeToADD, &buffer[0]);
		codeStart = newCodeStart;
		numOffsets = numOffNew;
	}
	else if (numOffNew < numOffsets)
	{
		UpdateNumOffsetInRom();
		int newCodeStart = offsetsStart + (numOffNew << 1) + 0x3; //800820DC 800820E0 800820E8
		newCodeStart = newCodeStart - (newCodeStart & 0x3); //800820EC-F0
		newCodeStart = newCodeStart + (pParentDlg->GetIntFromROM(pParentDlg->syscallTableStart + startAddress + 0xC, 2) << 2);
		newCodeStart += 0xF;//80082100
		newCodeStart = newCodeStart - (newCodeStart & 0xF);
		int sizeToRemove = codeStart-newCodeStart;
		std::vector<unsigned char> buffer(sizeToRemove, 0);
		pParentDlg->ReplaceFileDataAtAddressResize(newCodeStart, originalFileLocation, sizeToRemove, 0, &buffer[0]);
		codeStart = newCodeStart;
		numOffsets = numOffNew;
	}
	//Write a new offset to the offset table
	if (offsetsLocation != offLocNew || offTypeNew != offsetType)
	{
		std::vector<unsigned char> buffer(2, 0);
		pParentDlg->WriteIntToBuffer(buffer.data(), 0, (offLocNew + offTypeNew) ^ offsetXOR, 2);
		pParentDlg->ReplaceFileDataAtAddress(offsetsStart + offInd * 2, originalFileLocation, 2, &buffer[0]);
		offsetType = offTypeNew;
		offsetsLocation = offLocNew;
	}
	if (offTarStr.GetString() != "N/A" && offsetsTarget != offTarNew)
	{
		if (offsetType == 0)//FULLWORD
		{
			std::vector<unsigned char> buffer(4, 0);
			pParentDlg->WriteIntToBuffer(buffer.data(), 0, offTarNew, 4);
			pParentDlg->ReplaceFileDataAtAddress(codeStart + offsetsLocation, originalFileLocation, 4, &buffer[0]);
			offsetsTarget = offTarNew;
		}
		else if (offsetType == 1) //JAL
		{
			std::vector<unsigned char> intBuffer(4, 0);
			std::vector<unsigned char> buffer(4, 0);
			pParentDlg->GetFileDataAtAddress(codeStart + offsetsLocation, originalFileLocation, 4, &buffer[0]);
			pParentDlg->WriteIntToBuffer(intBuffer.data(), 0, offTarNew / 4, 4);
			intBuffer[0] = intBuffer[0] + (buffer[0] & 0xFC);
			pParentDlg->ReplaceFileDataAtAddress(codeStart + offsetsLocation, originalFileLocation, 4, &intBuffer[0]);
			offsetsTarget = offTarNew;
		}
		else if (offsetType == 3)//LUI END
		{
			std::vector<unsigned char> buffer(2, 0);
			pParentDlg->WriteIntToBuffer(buffer.data(), 0, offTarNew, 2);
			pParentDlg->ReplaceFileDataAtAddress(codeStart + offsetsLocation + 2, originalFileLocation, 2, &buffer[0]);
			offsetsTarget = offTarNew;
		}
	}
}

void CChangeLength::OnEnChangeOffsetIndex()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	CString originalFileLocation = pParentDlg->m_list.GetItemText(selectedIndex, 4);
	CString inputText;
	offset_Index_Box.GetWindowText(inputText);
	char* p;
	int index = strtol(inputText.GetString(), &p, 16);
	if (index < numOffsets)
	{
		UpdateOffsetAtIndex(index, originalFileLocation);
	}
	else
	{
		offset_Location_Box.SetWindowTextA("N/A");
		offset_Target_Box.SetWindowTextA("N/A");
	}
}

void CChangeLength::OnEnChangeOffsetLocation()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	CString originalFileLocation = pParentDlg->m_list.GetItemText(selectedIndex, 4);
	CString inputText;
	offset_Location_Box.GetWindowText(inputText);
	char* p;
	int location = strtol(inputText.GetString(), &p, 16);
	if (offsetType != 2)
	{
		UpdateBasedOnLocation(location, originalFileLocation);
	}
	else
	{
		offset_Target_Box.SetWindowTextA("N/A");
	}
}

/// <summary>
/// Update all of the branches within the file that cross the shifted region
/// </summary>
/// <param name="startAddress"></param>
/// <param name="amount"></param>
void CChangeLength::UpdateRelativeShifts(int startAddress, int amount)
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	CString originalFileLocation = pParentDlg->m_list.GetItemText(selectedIndex, 4);
	long fileSize = TooieRandoDlg::GetSizeFile(originalFileLocation);
	for (int i = codeStart; i < fileSize; i = i + 4)
	{
		unsigned char buffer[4];
		pParentDlg->GetFileDataAtAddress(i, originalFileLocation, 4, &buffer[0]);
		char message[256];
		int wholeCommand = buffer[0] << 24 | buffer[1]<<16| buffer[2] << 8 | buffer[3];
		CString firstByte;
		firstByte.Format("Branches Byte %X at address %X\n", wholeCommand, i);
		if (wholeCommand>>28 == 0x1||wholeCommand >> 28 == 0x5)
		{
			OutputDebugString(firstByte);
			signed short relativeOffset = wholeCommand & 0xFFFF;
			int targetAddress = relativeOffset*4+i+4;
			signed short newTarget = -1;
			int realStart = startAddress + codeStart;//Address from start of file
			if (i < realStart && realStart <= targetAddress && targetAddress < fileSize) //Branch jumps ahead of the shifted region
			{
				newTarget = relativeOffset + amount/4;
			}
			else if (i > realStart && realStart >= targetAddress && targetAddress > 0) //Branch jumps back over the shifted region
			{
				newTarget = relativeOffset - amount/4;
			}
			if (newTarget != -1)
			{
				int newCommand = (wholeCommand & 0xFFFF0000)| (newTarget & 0xFFFF);
				std::vector<unsigned char> buffer(4, 0);
				pParentDlg->WriteIntToBuffer(buffer.data(), 0, (newCommand), 4);
				pParentDlg->ReplaceFileDataAtAddress(i, originalFileLocation, 4, &buffer[0]);
			}
			firstByte.Format("Relative Offset %d Target Address 0x%X\n", relativeOffset, targetAddress);
			OutputDebugString(firstByte);

		}
	}


}

BOOL CChangeLength::OnInitDialog()
{
	CDialog::OnInitDialog();

	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	offset_TypeList.AddString("FullWord");
	offset_TypeList.AddString("JAL");
	offset_TypeList.AddString("LUI Init");
	offset_TypeList.AddString("LUI End");
	if (pParentDlg != nullptr)
	{
		unsigned char* RomFromParent = pParentDlg->ROM;
		char message[256];

		CString startString;
		CString endString;
		CString numOffString;

		//Script Assets
		if (startTableAddress >= pParentDlg->syscallTableStart)
		{
			startAddress = pParentDlg->GetIntFromROM(startTableAddress, 4);
			nextStartAddress = pParentDlg->GetIntFromROM(startTableAddress + 4, 4);
			numOffsets = pParentDlg->GetIntFromROM(pParentDlg->syscallTableStart + startAddress + 0xA, 2);
			startString.Format("%X", startAddress);
			endString.Format("%X", nextStartAddress);
			numOffString.Format("%X", numOffsets);
			offsetsStart = pParentDlg->GetIntFromROM(pParentDlg->syscallTableStart + startAddress + 0xE, 1);
			offsetsStart += pParentDlg->GetIntFromROM(pParentDlg->syscallTableStart + startAddress + 0x8, 2) << 2;
			offsetsStart += 0x3 + 0x38;
			offsetsStart = offsetsStart - (offsetsStart & 0x3);
			offsetsStart -= 0x10; //Subtract by 10 since the calculated offset includes the script header

			int offsetXORAddress = startTableAddress - pParentDlg->syscallTableStart + 0x40 + 0x4;
			offsetXOR = pParentDlg->GetIntFromROM(offsetXORAddress + 2, 2);
			codeStart = offsetsStart + (numOffsets << 1) + 0x3; //800820DC 800820E0 800820E8
			codeStart = codeStart - (codeStart & 0x3); //800820EC-F0
			codeStart = codeStart + (pParentDlg->GetIntFromROM(pParentDlg->syscallTableStart + startAddress + 0xC, 2) << 2);
			codeStart += 0xF;//80082100
			codeStart = codeStart - (codeStart & 0xF); //80082104 80082108
			CString originalFileLocation = pParentDlg->m_list.GetItemText(selectedIndex, 4);
			UpdateOffsetAtIndex(0, originalFileLocation);

		}
		else
		{
			startAddress = pParentDlg->GetIntFromROM(startTableAddress, 3);
			nextStartAddress = pParentDlg->GetIntFromROM(startTableAddress + 4, 3);
			startAddress = startAddress * 4 + 0x12B24;
			nextStartAddress = nextStartAddress * 4 + 0x12B24;
			startString.Format("%X", startAddress);
			endString.Format("%X", nextStartAddress);
			num_Offset_Box.EnableWindow(FALSE);
			offset_Index_Box.EnableWindow(FALSE);
			offset_Location_Box.EnableWindow(FALSE);
			offset_Target_Box.EnableWindow(FALSE);
			offset_Index_Box.EnableWindow(FALSE);
			offsetApplyButton.EnableWindow(FALSE);
			offset_TypeList.EnableWindow(FALSE);
		}
		start_Address_Box.SetWindowTextA(startString);
		end_Address_Box.SetWindowTextA(endString);
		num_Offset_Box.SetWindowTextA(numOffString);
		offset_Index_Box.SetWindowTextA("0");
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE

}

void CChangeLength::UpdateNumOffsetInRom()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	unsigned char* RomFromParent = pParentDlg->ROM;
	RomFromParent[pParentDlg->syscallTableStart + startAddress + 0xA] = numOffsets >> 8;
	RomFromParent[pParentDlg->syscallTableStart + startAddress + 0xA + 0x1] = numOffsets;
}

void CChangeLength::OnBnClickedRemoveOffset()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();
	CString offIndStr;

	char* p;

	offset_Index_Box.GetWindowText(offIndStr);
	numOffsets--;
	UpdateNumOffsetInRom();
	int offInd = strtol(offIndStr.GetString(), &p, 16);
	int newCodeStart = offsetsStart + (numOffsets << 1) + 0x3; //800820DC 800820E0 800820E8
	newCodeStart = newCodeStart - (newCodeStart & 0x3); //800820EC-F0
	newCodeStart = newCodeStart + (pParentDlg->GetIntFromROM(pParentDlg->syscallTableStart + startAddress + 0xC, 2) << 2);
	newCodeStart += 0xF;//80082100
	newCodeStart = newCodeStart - (newCodeStart & 0xF);
	CString originalFileLocation = pParentDlg->m_list.GetItemText(selectedIndex, 4);

	if (pParentDlg != nullptr)
	{
		std::vector<unsigned char> buffer(4, 0);
		pParentDlg->WriteIntToBuffer(buffer.data(), 0, 0, 2);
		pParentDlg->ReplaceFileDataAtAddressResize(offsetsStart + offInd * 2, originalFileLocation, 2,0, &buffer[0]);
		if (newCodeStart == codeStart)
		{
			pParentDlg->WriteIntToBuffer(buffer.data(), 0, 0, 2);
			pParentDlg->ReplaceFileDataAtAddressResize(offsetsStart + (numOffsets) * 2, originalFileLocation, 0, 2, &buffer[0]);
		}
		else
		{
			int sizeToRemove = codeStart-(offsetsStart + (numOffsets+1) * 2);
			std::vector<unsigned char> removeBuffer(sizeToRemove, 0);
			pParentDlg->ReplaceFileDataAtAddressResize(offsetsStart + (numOffsets) * 2, originalFileLocation, sizeToRemove, 0, &removeBuffer[0]);
			
		}
		codeStart = newCodeStart;
	}
	UpdateOffsetAtIndex(offInd, originalFileLocation);

	// TODO: Add your control notification handler code here
}

void CChangeLength::OnEnChangeNumoffset()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

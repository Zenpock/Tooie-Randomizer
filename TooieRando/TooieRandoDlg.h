// GEDecompressorDlg.h : header file
//

#pragma once
#include "GECompression.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "rnc_deco.h"
#include "erzdec.h"
#include "n643docompression.h"
#include "n64_cic_nus_6105.h"
#include "NintendoEncoder.h"
#include <string>
#include <algorithm>

#define UPDATE_LIST (WM_APP + 1)
#define UPDATE_PROGRESS_BAR (WM_APP + 2)

struct ListUpdateStruct
{
	unsigned long address;
	int fileSizeUncompressed;
	int fileSizeCompressed;
	int fileSizeOriginal;
	CString type;
	CString internalName;
	CString tempLocation;
};

class RandomizedObject
{
public:
	int rewardObjectIndex = -1; //The index of the reward object associated with this object if there is one 
	std::vector<unsigned char> Data; //This is the raw data regarding the silodata
	int fileIndex = 0; //This should be the index in the main table
	int associatedOffset = 0; //The offset from the start of the file this data is located;
public:RandomizedObject(std::vector<unsigned char> newData, int newFileIndex, int newAssociatedOffset)
{
	this->Data = newData;
	this->fileIndex = newFileIndex;
	this->associatedOffset = newAssociatedOffset;
}
};

class RewardObject
{
public:
	bool shouldRandomize = true;
	bool hasFlag = false;
	int associatedObjectIndex;//Index of the associated Object
	int objectID = 0; //This is the objectID associated with the object
	int itemType = 0; //This is the ItemType which is the collectable Item Type
	int itemId = 0; //This is the flag from the object
	std::vector<int> associatedScripts; //This should be the index in the main table
	int itemIndex = 0; //If multiple objects are spawned by the same script this determines which edits this object needs
	RewardObject(int newAssociateObjectIndex, int newObjectID, int newItemId)
	{
		this->objectID = newObjectID;
		switch (newObjectID)
		{
		case 0x1f5: //Jinjo
			this->itemType = 0;
			break;
		case 0x1f6: //Jiggy
			this->itemType = 1;
			break;
		case 0x1f7: //Honeycomb
			this->itemType = 2;
			break;
		case 0x1f8: //Glowbo
			this->itemType = 3;
			break;
		case 0x4E6: //Ticket
			this->itemType = 8;
			break;
		case 0x29D: //Doubloon
			this->itemType = 7;
			break;
		case 0x201: //Cheato
			this->itemType = 4;
			break;
		default:
			this->itemType = -1;
			break;
		}
		this->associatedObjectIndex = newAssociateObjectIndex;
		this->itemId = newItemId;
	}
};

class MoveObject
{
public:
	std::vector<int> restrictedMoves; //The hexadecimal values relating to the moves that should not be placed at this location due to the move being necessary to get to this location
	std::vector<unsigned char> Data; //This is the raw data regarding the silodata
	int fileIndex = 0; //This should be the index in the main table
	int associatedOffset = 0; //The offset from the start of the file this data is located
	int Ability = 0; //The ability value used when setting abilities (used for most items)
	std::string MoveType = "Silo"; //How to retrieve and use the associated data so silos have their dialogue moved to new silos but individuals just have the ability number used
	std::string MoveName = "";
	MoveObject()
	{

	}
	MoveObject(std::vector<unsigned char> newData, int newFileIndex, int newAssociatedOffset)
	{
		this->Data = newData;
		this->fileIndex = newFileIndex;
		this->associatedOffset = newAssociatedOffset;
	}
};

class ScriptEdit
{
public:
	int scriptIndex = -1; //The index of the associated script in the main table
	int editType = 0; //What to replace the data with 1 = Type, 2 = Flag, 3 = ObjectID
	int associatedOffset = 0; //The offset from the start of the file this data is located;
	int rewardIndex = 0; //Which reward the edit applies to
public:ScriptEdit(int newScriptIndex, int newEditType, int newAssociatedOffset, int newRewardIndex)
{
	this->scriptIndex = newScriptIndex;
	this->editType = newEditType;
	this->associatedOffset = newAssociatedOffset;
	this->rewardIndex = newRewardIndex;
}
};

class OptionData
{
public:
	CString OptionType = "";
	CString optionName;
	CString defaultValue = ""; //Set by the options list
	CString currentValue = ""; //Set by the User
	CString scriptOffset = ""; //The offset within a script to place the edit
	CString scriptAddress = ""; //The script's index to actually find the associated script to edit
	std::vector<CString> possibleSelections; //For Enum or multiselect options determines the list of selections in a list
	bool active = false;
	std::vector<int> flags;
	CString customCommands = "";
	/// <summary>
	/// Id used to reference special options within the randomizer not running in the rom
	/// </summary>
	CString lookupId = "";
	OptionData(CString OptionName)
	{
		this->optionName = OptionName;
		this->active = false;
	}

	int GetDefaultValueInt()
	{
		char* endPtr;
		return strtol(defaultValue, &endPtr, 10);
	}
	int GetCurrentValueInt()
	{
		char* endPtr;
		return strtol(currentValue, &endPtr, 10);
	}
	void SetCurrentValueInt(int newValue)
	{
		CString str;
		str.Format("%d", newValue);
		currentValue = str;
	}
};

// CGEDecompressorDlg dialog
class TooieRandoDlg : public CDialog
{
// Construction
public:
	TooieRandoDlg(CWnd* pParent = NULL);	// standard constructor
// Dialog Data
	enum { IDD = IDD_GEDECOMPRESSOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	void TooieRandoDlg::AddOption(CString optionName, bool active, std::vector<int> flags);
	void TooieRandoDlg::AddOption(CString optionName, bool active, CString customCommands);
	void TooieRandoDlg::AddSpecialOption(CString optionName, bool active, int optionID);
	BOOL TooieRandoDlg::CheckOptionActive(CString lookupID);
	OptionData TooieRandoDlg::GetOption(CString lookupID);
	void TooieRandoDlg::AddOption(OptionData option);
	void TooieRandoDlg::SaveSeedToFile();
	void SetupOptions();
	void SetDefaultFlag(bool active, int flag, int commandsUsed);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT AddListEntry(WPARAM, LPARAM);
	afx_msg LRESULT UpdateProgressBar(WPARAM, LPARAM);
	afx_msg void OnBnClickedButton1();
	CComboBox m_gameselection;
	afx_msg void OnBnClickedCompressfilebutton();
	afx_msg void OnBnClickedDecompressgame();
	static UINT DecompressGameThread( LPVOID pParam );
	LRESULT TooieRandoDlg::OnThreadComplete(WPARAM wParam, LPARAM lParam);
	static void ToUpdateProgressBar(TooieRandoDlg* dlg, int curSpot, int romSize);
	static int GetZLibGameName(CString gameNameStr);
	static bool ReadROM(CString gameNameStr, CString fileLocation, unsigned char*& GameBuffer, int& romSize, CString& folderPath);
	static bool AllocateInput(int offset, unsigned char*& Buffer, unsigned char* GameBuffer, int& endSize, int romSize);
	static void WriteASCIIFile(CString filename, unsigned char* outputDecompressed, int fileSize, bool appendFile);
	static void WriteResult(bool genTextFile, CString filename, unsigned char* outputDecompressed, int fileSize, bool appendFile);
	static int DecompressZLibSpot(GECompression* compressed, bool genText, int offset, unsigned char* GameBuffer, int romSize, int GAME, CString folderPath, CString internalName, int expectedSize, CString& tempLocation, int& fileSizeCompressed, CString& type, unsigned long printedAddress, bool printBank, unsigned printbankAddress);
	static int DecompressRNCSpot(RncDecoder* compressed, bool genText, int offset, unsigned char* GameBuffer, int romSize, int GAME, CString folderPath, CString internalName, int expectedSize, CString& tempLocation, int& fileSizeCompressed, CString& type, bool appendFile, unsigned long appendROMLocation, CString appendInternalFileName);
	static int DecompressERZSpot(ERZ* compressed, bool genText, int offset, unsigned char* GameBuffer, int romSize, int GAME, CString folderPath, CString internalName, int expectedSize, CString& tempLocation, int& fileSizeCompressed, CString& type);
	static void WriteBinaryFile(CString filename, unsigned char* outputDecompressed, int fileSize, bool appendFile);
	void WriteLongToBuffer(unsigned char* Buffer, unsigned long address, unsigned long data);
	static void DecompressZLibAtPosition(CString gameNameStr, TooieRandoDlg* dlg, CString filein, unsigned long start,int GAME);
	static void DecompressZLibFromTable(CString gameNameStr, TooieRandoDlg* dlg, CString filein, unsigned long start, unsigned long end, int step, int GAME, unsigned long tblOffset, int shift, int multiplier, int offset);
	static void DecompressConkerFromTable(TooieRandoDlg* dlg, CString filein, unsigned char* input, int size, unsigned long start, unsigned long end, int GAME, bool writeFileNumberInstead, int bankNumber);
	static void DecryptBTFile(int fileNumber, unsigned char* input, unsigned char* output, int size);
	static void ReceivedNewROM(TooieRandoDlg* dlg, CString fileLocation, unsigned char* GameBuffer, int romSize);
	static unsigned long CharArrayToLong(unsigned char* currentSpot);
	static float CharArrayToFloat(unsigned char* currentSpot);
	static unsigned long Flip32Bit(unsigned long inLong);
	static unsigned short CharArrayToShort(unsigned char* currentSpot);
	static unsigned short Flip16Bit(unsigned short ShortValue);
	void ReplaceObject(int sourceIndex, int insertIndex);
	void ReplaceFileDataAtAddress(int address, CString filepath,int size, unsigned char* buffer);
	void InjectFile(CString filePath,int index);
	static unsigned long StringHexToLong(CString inString);
	static int HexToInt(char inChar);
	unsigned char StringToUnsignedChar(CString inString);
	static unsigned long GetSizeFile(CString filename);
	afx_msg void OnBnClickedButton3();
	CButton m_genTextFiles;
	CListCtrl m_list;
	CListCtrl option_list;
	int fileCount;
	static void AddRowData(TooieRandoDlg* dlg, unsigned long address,  int fileSizeCompressed, int fileSizeUncompressed, CString internalName, CString tempLocation, CString type);
	CProgressCtrl m_progressBar;
	afx_msg void OnLvnItemchangedListdecompressedfiles(NMHDR *pNMHDR, LRESULT *pResult);
	unsigned char ReturnASCIIChar(unsigned char asciiByte);
	CString strROMPath;
	CString gameNameStr;
	int assetTableStart;
	int syscallTableStart;
	bool genText;
	CString directory;
	CWinThread* decompressGamethread;
	bool killThread;
	afx_msg void OnBnClickedButtoncancelload();
	void KillDecompressGameThread();
	void GetFileDataAtAddress(int address, CString filepath,int size, unsigned char* buffer);
	CButton m_cancelLoad;
	CButton m_injectButton;
	unsigned char* ROM;
	int ROMSize;
	CString romName;
	~TooieRandoDlg(void);
	CButton m_saveROMButton;
	afx_msg void OnBnClickedButtonsaverom();
	afx_msg void OnClose();
	BOOL hiddenExec (PTSTR pCmdLine, CString currentDirectory);
	BOOL hiddenExec (PTSTR pCmdLine, CString currentDirectory, int& errorCode);
	BOOL hiddenExec (PTSTR pCmdLine, CString currentDirectory, int& errorCode, int waitTime);
	CEdit m_hexView;
	CEdit mSearch;
	afx_msg void OnBnClickedButtonsearch();
	int lastSearchSpot;
	CEdit mFile;
	CButton mCompressEncryptedButton;
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedCompressfilebuttonencrypted();
	CEdit mEncryptedFileNumber;
	unsigned short StringToUnsignedShort(CString inString);
	CStatic mFileNumberStatic;
	CButton mCompressFileButton;
	CButton mDecompressFileButton;
	int FindItemInListCtrl(CListCtrl& listCtrl, const CString& searchText, int columnIndex);
    CEdit SeedEntry;
	CEdit VariableEdit;
	CListBox SelectionList;
	CButton SelectionListAdd;
	CButton SelectionListRemove;
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton4();
	std::vector<std::string> GetVectorFromString(CString vectorString, char* delimiter);
	std::vector<int> TooieRandoDlg::GetIntVectorFromString(CString vectorString, char* delimiter);
	void TooieRandoDlg::LoadMoves();
    void TooieRandoDlg::RandomizeMoves();
    void TooieRandoDlg::RandomizeMove(int source, int target);
	int TooieRandoDlg::FindUnusedMove(std::vector<int> objects, std::vector<int> restrictedMoves);
    void TooieRandoDlg::ClearReward(int itemType,int itemFlag);
    int TooieRandoDlg::FindRewardFlagOffset(int itemType, int itemFlag);
    void TooieRandoDlg::ClearRewards();
    bool TooieRandoDlg::CanBeReward(int itemType);
    int TooieRandoDlg::GetReward(int itemType, int itemFlag);
	int TooieRandoDlg::GetScriptIndex(CString scriptId);
	void TooieRandoDlg::LoadOptions();
	std::string GetStringAfterTag(std::string line, std::string tag, std::string endTag);
    void TooieRandoDlg::LoadObjects();
    void TooieRandoDlg::RandomizeObjects();
    int TooieRandoDlg::PlaceObjectsIntoLevelGroup(std::string mapID);
	int TooieRandoDlg::FindUnusedRewardObject(std::vector<int> objects);
	int TooieRandoDlg::GetLevelIndexFromMapId(std::string MapID);
	int TooieRandoDlg::FindFreeLocationInLevel(std::vector<int> locations, int levelIndex);
    void TooieRandoDlg::SetReward(int itemType, int itemFlag, int value);
    void TooieRandoDlg::SetRewardScript(int reward, int itemType, int itemFlag, int objectId);
    void TooieRandoDlg::LoadScriptEdits();
    afx_msg void OnEnChangeSeedEntry();
	afx_msg void OnDblclkListdecompressedfiles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickListdecompressedfiles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedDecompressgame2();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnItemdblclickOptionList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkOptionList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnChangeIdPreviewClose();
	afx_msg void OnEnChangeVariableEdit();
	afx_msg void OnBnClickedSelectAdd();
	afx_msg void OnBnClickedSelectRemove();
};

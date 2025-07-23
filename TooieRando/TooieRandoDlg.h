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
#include "RandomizedObject.h"
#include "RewardObject.h"
#include "MoveObject.h"
#include "ScriptEdit.h"
#include "OptionData.h"
#include "resource.h"
#include "LogicGroup.h"
#include "LogicHandler.h"
#include <map>


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
	bool TooieRandoDlg::CheckOptionActive(CString lookupID);
	OptionData TooieRandoDlg::GetOption(CString lookupID);
	void TooieRandoDlg::AddOption(OptionData option);
	void TooieRandoDlg::SaveSeedToFile();
	void ClearSpoilers();
	void AddSpoilerToLog(std::string spoiler);
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
	void WriteIntToBuffer(unsigned char* Buffer, int address, int data, int size);
	static void DecompressZLibAtPosition(CString gameNameStr, TooieRandoDlg* dlg, CString filein, unsigned long start,int GAME);
	static void DecompressZLibAtPosition(CString gameNameStr, TooieRandoDlg* dlg, CString filein, unsigned long start, int GAME, int& compressedSize);
	static void DecompressZLibFromTable(CString gameNameStr, TooieRandoDlg* dlg, CString filein, unsigned long start, unsigned long end, int step, int GAME, unsigned long tblOffset, int shift, int multiplier, int offset);
	static void DecompressConkerFromTable(TooieRandoDlg* dlg, CString filein, unsigned char* input, int size, unsigned long start, unsigned long end, int GAME, bool writeFileNumberInstead, int bankNumber);
	static void DecryptBTFile(int fileNumber, unsigned char* input, unsigned char* output, int size);
	static void ReceivedNewROM(TooieRandoDlg* dlg, CString fileLocation, unsigned char* GameBuffer, int romSize);
	static unsigned long CharArrayToLong(unsigned char* currentSpot);
	static float CharArrayToFloat(unsigned char* currentSpot);
	static unsigned long Flip32Bit(unsigned long inLong);
	static unsigned short CharArrayToShort(unsigned char* currentSpot);
	static unsigned short Flip16Bit(unsigned short ShortValue);
	int GetIntFromROM(int address, int length);
	void ReplaceObject(int sourceIndex, int insertIndex);
	int GetObjectFromID(int objectID);
	int GetMoveFromID(int moveID);

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
	int core1Start;
	int core2Start;
	int core3Start;
	int core4Start;
	bool genText;
	CString directory;
	CWinThread* decompressGamethread;
	bool killThread;
	afx_msg void OnBnClickedButtoncancelload();
	void KillDecompressGameThread();
	void GetFileDataAtAddress(int address, CString filepath,int size, unsigned char* buffer);
	int GetIntAtAddress(int address, CString filepath, int size);
	void ReplaceFileDataAtAddressResize(int address, CString filepath, int oldsize, int newsize, unsigned char* buffer);
	std::vector<RandomizedObject> RandomizedObjects;
	std::vector<MoveObject> MoveObjects;
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
	bool stopNow = false;
	int GetMoveIndexFromAbility(int ability);
	int FindItemInListCtrl(CListCtrl& listCtrl, const CString& searchText, int columnIndex);
    CEdit SeedEntry;
	CEdit VariableEdit;
	CListBox SelectionList;
	CButton SelectionListAdd;
	CButton SelectionListRemove;
	CComboBox LogicSelector;
	std::vector<std::tuple<std::string, std::string,int>> LogicFilePaths;
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton4();
	std::vector<int> TooieRandoDlg::GetIdsFromNameSelection(std::vector<std::string> names);

	static std::vector<std::string> GetVectorFromString(std::string vectorString, std::string delimiter);
	static std::vector<int> TooieRandoDlg::GetIntVectorFromString(std::string vectorString, std::string delimiter);
	std::unordered_map<int,LogicGroup> LogicGroups;
	void TooieRandoDlg::LoadMoves();
    void TooieRandoDlg::RandomizeMoves(LogicHandler::AccessibleThings state);
	void TooieRandoDlg::LoadEntrances();
	void TooieRandoDlg::ConnectWarp(int entrance, int exit);
	void TooieRandoDlg::RandomizeWarps(LogicHandler::AccessibleThings& state);
	void TooieRandoDlg::SetMovePrice(int source, int price);
	int TooieRandoDlg::GetEntranceByID(int entranceID);
	std::vector<int> GetAllEntrancesInShuffleGroup(int shuffleGroup);
	void TooieRandoDlg::RandomizeMove(int source, int target);
	void TooieRandoDlg::CreateTempFile(CString filePath);
	static CString GetTempFileString(CString filePath);
	int TooieRandoDlg::FindUnusedMove(std::vector<int> objects, std::vector<int> restrictedMoves);
    void TooieRandoDlg::ClearReward(int itemType,int itemFlag);
    int TooieRandoDlg::FindRewardFlagOffset(int itemType, int itemFlag);
    void TooieRandoDlg::ClearRewards();
    bool TooieRandoDlg::CanBeReward(int itemType);
    int TooieRandoDlg::GetReward(int itemType, int itemFlag);
	int TooieRandoDlg::GetScriptIndex(CString scriptId);
	int TooieRandoDlg::GetAssetIndex(CString assetAddress);
	void TooieRandoDlg::LoadOptions();
	static std::string GetStringAfterTag(std::string line, std::string tag, std::string endTag);
    void TooieRandoDlg::LoadObjects();
    void TooieRandoDlg::RandomizeObjects(LogicHandler::AccessibleThings state);
    void TooieRandoDlg::PlaceObjectIntoLevelGroup(int mapID,RandomizedObject& object);
	int TooieRandoDlg::FindUnusedRewardObject(std::vector<int> objects);
	int TooieRandoDlg::GetLevelIndexFromMapId(int MapID);
	int TooieRandoDlg::FindFreeLocationInLevel(std::vector<int> locations, int levelIndex);
	std::vector<int> FindFreeLocationsInLevel(std::vector<int> locations, int levelIndex);
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
	afx_msg void OnBnClickedLogicEditorButton();
	void LoadLogicFileOptions();
	void UpdateLogicSelector();
	static void LoadLogicGroupsFromFile(std::unordered_map<int,LogicGroup>& logicGroups, CString fileName);
	afx_msg void OnBnClickedLogicCheck();
};
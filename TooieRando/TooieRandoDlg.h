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
	int fileCount;
	static void AddRowData(TooieRandoDlg* dlg, unsigned long address,  int fileSizeCompressed, int fileSizeUncompressed, CString internalName, CString tempLocation, CString type);
	CProgressCtrl m_progressBar;
	afx_msg void OnLvnItemchangedListdecompressedfiles(NMHDR *pNMHDR, LRESULT *pResult);
	unsigned char ReturnASCIIChar(unsigned char asciiByte);
	CString strROMPath;
	CString gameNameStr;
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
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton4();
    void TooieRandoDlg::LoadMoves();
    void TooieRandoDlg::RandomizeMoves();
    void TooieRandoDlg::RandomizeMove(int source, int target);
    void TooieRandoDlg::ClearReward(int itemType,int itemFlag);
    int TooieRandoDlg::FindRewardFlagOffset(int itemType, int itemFlag);
    void TooieRandoDlg::ClearRewards();
    bool TooieRandoDlg::CanBeReward(int itemType);
    int TooieRandoDlg::GetReward(int itemType, int itemFlag);
	int TooieRandoDlg::GetScriptIndex(char* scriptId);
    void TooieRandoDlg::LoadObjects();
    void TooieRandoDlg::RandomizeObjects();
    int TooieRandoDlg::PlaceObjectsIntoLevelGroup(char* mapID);
    void TooieRandoDlg::SetReward(int itemType, int itemFlag, int value);
    void TooieRandoDlg::SetRewardScript(int reward, int itemType, int itemFlag, int objectId);
    void TooieRandoDlg::LoadScriptEdits();
    afx_msg void OnEnChangeSeedEntry();
	afx_msg void OnDblclkListdecompressedfiles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickListdecompressedfiles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedDecompressgame2();
	afx_msg void OnLbnSelchangeList1();
};

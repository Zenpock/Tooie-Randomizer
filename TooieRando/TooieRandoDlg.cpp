// GEDecompressorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TooieRando.h"
#include "TooieRandoDlg.h"
#include ".\TooieRandodlg.h"

#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <istream>
#include <string>
#include <cstring>
#include <vector>
#include <map>
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <random>

#define PI           3.14159265358979323846  /* pi */
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CChangeLength dialogs

class CChangeLength : public CDialog
{
	int startTableAddress = 0;//Address of the table element that defines the start of the asset
	int startAddress = 0;//Start of the asset in the rom
	int nextStartAddress = 0;//Start of the asset after the chosen asset
	int diff = 0;//Difference between the original end of an asset and the new defined size
public:
	CChangeLength();
	CChangeLength(CWnd* pParent /*=nullptr*/, int start)
		: CDialog(IDD_CHANGE_LENGTH, pParent)
	{
		startTableAddress = start;
		
	}
	// Dialog Data
	enum { IDD = IDD_CHANGE_LENGTH};

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
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedConfirmextension();
	void CChangeLength::UpdateSyscallTable(int start, int end,int diff);
	void CChangeLength::ShiftAssets(int assetAddress, int diff);

};

CChangeLength::CChangeLength() : CDialog(CChangeLength::IDD)
{
}
void CChangeLength::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_START_ASSET_ADDRESS, start_Address_Box);
	DDX_Control(pDX, IDC_END_ASSET_ADDRESS, end_Address_Box);
}

BEGIN_MESSAGE_MAP(CChangeLength, CDialog)
ON_BN_CLICKED(IDC_ConfirmExtension, &CChangeLength::OnClickedConfirmextension)
END_MESSAGE_MAP()

void CChangeLength::OnClickedConfirmextension()
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();

	CString newNextAddressStr;
	end_Address_Box.GetWindowTextA(newNextAddressStr);
	char* endPtr;
	int newNextStartAddress = strtol(newNextAddressStr, &endPtr, 10);
	char message[256];
	sprintf(message, "0x%X - 0x%X Diff: 0x%X\n", newNextStartAddress, nextStartAddress, newNextStartAddress - nextStartAddress);
	MessageBox(message);
	UpdateSyscallTable(startTableAddress + 4, pParentDlg->syscallTableStart+0xDD0, newNextStartAddress - nextStartAddress);
	ShiftAssets(pParentDlg->syscallTableStart+nextStartAddress, newNextStartAddress - nextStartAddress);
	pParentDlg->OnBnClickedButtonsaverom();
	EndDialog(0);
}


void CChangeLength::UpdateSyscallTable(int start,int end,int diff)
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
			assetAddress = ROMFromParent[currentAddress] << 24 | ROMFromParent[currentAddress + 1] << 16 | ROMFromParent[currentAddress + 2] << 8 | ROMFromParent[currentAddress + 3];
			assetAddress += diff;
			unsigned char hexString[4];
			ROMFromParent[currentAddress] = assetAddress >> 24;
			ROMFromParent[currentAddress+1] = assetAddress >> 16 & 0xff;
			ROMFromParent[currentAddress+2] = assetAddress >> 8 & 0xff;
			ROMFromParent[currentAddress+3] = assetAddress &0xff;
			currentAddress += 4;
		}
	}
}

void CChangeLength::ShiftAssets(int assetAddress, int diff)
{
	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();

	if (pParentDlg != nullptr)
	{
		unsigned char* ROMFromParent = pParentDlg->ROM;

		int originalSize = pParentDlg->ROMSize;

		// Number of positions to shift
		int shiftAmount = diff;

		// Position where to start shifting
		int position = assetAddress;

		// Value to fill the shifted area
		unsigned char fillValue = 0xAA;

		// Ensure there's enough room after the shift
		if (position + shiftAmount <= originalSize) {
			// Shift elements to the right (keeping size same)
			std::memmove(&ROMFromParent[position + shiftAmount], &ROMFromParent[position], (originalSize - position - shiftAmount) * sizeof(unsigned char));

			// Fill the gap with the desired value
			for (int i = position; i < position + shiftAmount; ++i) {
				ROMFromParent[i] = fillValue;
			}
		}
	}
}

BOOL CChangeLength::OnInitDialog()
{
	CDialog::OnInitDialog();

	TooieRandoDlg* pParentDlg = (TooieRandoDlg*)GetParent();

	if (pParentDlg != nullptr)
	{
		unsigned char* RomFromParent = pParentDlg->ROM;

		CString startString;
		CString endString;

		if (startTableAddress >= pParentDlg->syscallTableStart)
		{
			startAddress = RomFromParent[startTableAddress] << 24 | RomFromParent[startTableAddress + 1] << 16 | RomFromParent[startTableAddress + 2] << 8 | RomFromParent[startTableAddress + 3];
			nextStartAddress = RomFromParent[startTableAddress + 4] << 24 | RomFromParent[startTableAddress + 5] << 16 | RomFromParent[startTableAddress + 6] << 8 | RomFromParent[startTableAddress + 7];
			startString.Format("%d",startAddress);
			endString.Format("%d", nextStartAddress);
		}
		else
		{
			startString.Format("%d", startTableAddress);
		}
		start_Address_Box.SetWindowTextA(startString);
		end_Address_Box.SetWindowTextA(endString);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE

}

// CGEDecompressorDlg dialog

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
    int itemType=0; //This is the ItemType which is the collectable Item Type
    int itemId=0; //This is the flag from the object
    std::vector<int> associatedScripts; //This should be the index in the main table
    int itemIndex = 0; //If multiple objects are spawned by the same script this determines which edits this object needs
    RewardObject(int newAssociateObjectIndex, int newObjectID,int newItemId)
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
            this->itemType=-1;
            break;
        }
        this->associatedObjectIndex = newAssociateObjectIndex;
        this->itemId = newItemId;
    }
};

class SiloObject
{
public:
    std::vector<unsigned char> Data; //This is the raw data regarding the silodata
    int fileIndex = 0; //This should be the index in the main table
    int associatedOffset = 0; //The offset from the start of the file this data is located
    SiloObject(std::vector<unsigned char> newData, int newFileIndex, int newAssociatedOffset)
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


CChangeLength* m_pChangeLength;  // Pointer to the dialog for extending allocated asset
std::vector<RewardObject> RewardObjects; //Stores the object indexes that are originally reward objects
std::vector<RandomizedObject> RandomizedObjects; //Stores the object indexes and offsets for collectable items
std::vector<SiloObject> SiloObjects; //Stores the object data for silo objects
std::vector<ScriptEdit> ScriptEdits; //The edits to make to reward object spawning scripts
int seed = 0;
std::vector<std::string> MapIDs; //Array of MapIds associated with the object data
std::vector< std::vector<int>> levelObjects(9); //Contains the indices from ObjectData which objects are in what level with storage being [LevelIndex][]
typedef std::vector<std::string> MapIDGroup;
MapIDGroup IOH = {"0AA4","0AA5","0AA6","0AA7","0AA8","0AA9","0AAA","0AAB","0AAC","0AAF","0AB0","0AB1","0A03","0A04","0A96","0AC8","0A97","0A98","0A99","0A9A"};
MapIDGroup MT = { "0A0B","0A0C","0A0D","0A0E","0A0F","0A10","0A11","0A19","0A1A","0A1B","0A1D","0A1E","0ACC","0ACD","0ACE","0ACF"};
MapIDGroup GGM = {"0A1C","0A1F","0A20","0A21","0A22","0A23","0A24","0A25","0A26","0A27","0A28","0A29","0A2C","0A2D","0A2E","0A2F","0A30","0A31","0A3E","0A3","0A7B","0AB8","0AB9","0AC6"};
MapIDGroup HFP = {"0A7C","0A7D","0A7E","0A7F","0A80","0A81","0A82","0A83","0A84","0A85","0A86","0A87","0A88","0A89","0A8A" };
MapIDGroup TDL = {"0A67","0A68","0A69","0A6A","0A6B","0A6C","0A6D","0A6E","0A6F","0A70","0A73","0A77","0A78" };
MapIDGroup CCL = {"0A8B","0A8C","0A8D","0A8E","0A8F","0A92","0A93","0A94","0A95" };
MapIDGroup GI = {"0A55","0A56","0A57","0A58","0A59","0A5A","0A5B","0A5C","0A5D","0A5E","0A5F","0A60","0A61","0A62","0A63","0A64","0A65","0A66","0A74","0A7A","0AB7","0AC7","0ADC" };
MapIDGroup WW = {"0A2A","0A2B","0A32","0A33","0A34","0A35","0A36","0A37","0A38","0A39","0A3A","0A3B","0A3C","0A3F","0A40","0A41","0A4E","0A79","0ACB" };
MapIDGroup JRL = {"0AFB","0A42","0A43","0A44","0A46" ,"0A49" ,"0A4B" ,"0A4C","0A4D","0A4F","0A51","0A54","0AD6","0A75","0AFC","0AFD","0AFE" };

std::vector<MapIDGroup> mapIDGroups{IOH,MT,GGM,HFP,TDL,CCL,GI,WW,JRL};



TooieRandoDlg::TooieRandoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TooieRandoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void TooieRandoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GENTXT, m_genTextFiles);
	DDX_Control(pDX, IDC_LISTDECOMPRESSEDFILES, m_list);
	DDX_Control(pDX, IDC_PROGRESS, m_progressBar);
	DDX_Control(pDX, IDC_BUTTONCANCELLOAD, m_cancelLoad);
	DDX_Control(pDX, IDC_BUTTON3, m_injectButton);
	DDX_Control(pDX, IDC_BUTTONSAVEROM, m_saveROMButton);
	DDX_Control(pDX, IDC_COMPRESSFILEBUTTONENCRYPTED, mCompressEncryptedButton);
	DDX_Control(pDX, IDC_EDITENCRYPTED, mEncryptedFileNumber);
	DDX_Control(pDX, IDC_FILENUMBERLABEL, mFileNumberStatic);
	DDX_Control(pDX, IDC_COMPRESSFILEBUTTON, mCompressFileButton);
	DDX_Control(pDX, IDC_BUTTON1, mDecompressFileButton);
	DDX_Control(pDX, IDC_OriginalObject, OriginalObjectList);
	DDX_Control(pDX, IDC_ReplaceObject, NewObjectList);
    DDX_Control(pDX, IDC_SEED_ENTRY, SeedEntry);

}

BEGIN_MESSAGE_MAP(TooieRandoDlg, CDialog)
	ON_MESSAGE(WM_USER + 1, &TooieRandoDlg::OnThreadComplete)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_COMPRESSFILEBUTTON, OnBnClickedCompressfilebutton)
	ON_BN_CLICKED(IDC_DECOMPRESSGAME, OnBnClickedDecompressgame)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTDECOMPRESSEDFILES, &TooieRandoDlg::OnLvnItemchangedListdecompressedfiles)
	ON_MESSAGE(UPDATE_LIST, AddListEntry)
	ON_MESSAGE(UPDATE_PROGRESS_BAR, UpdateProgressBar)
	ON_BN_CLICKED(IDC_BUTTONCANCELLOAD, &TooieRandoDlg::OnBnClickedButtoncancelload)
	ON_BN_CLICKED(IDC_BUTTONSAVEROM, &TooieRandoDlg::OnBnClickedButtonsaverom)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTONSEARCH, &TooieRandoDlg::OnBnClickedButtonsearch)
	ON_BN_CLICKED(IDC_COMPRESSFILEBUTTONENCRYPTED, &TooieRandoDlg::OnBnClickedCompressfilebuttonencrypted)
	ON_BN_CLICKED(IDC_ApplyObjectChange, &TooieRandoDlg::OnBnClickedApplyobjectchange)
	ON_CBN_SELCHANGE(IDC_OriginalObject, &TooieRandoDlg::OnCbnSelchangeOriginalobject)
	ON_CBN_SELCHANGE(IDC_ReplaceObject, &TooieRandoDlg::OnCbnSelchangeReplaceobject)
	ON_BN_CLICKED(IDC_BUTTON5, &TooieRandoDlg::OnBnClickedButton5)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(NM_RCLICK, IDC_LISTDECOMPRESSEDFILES, &TooieRandoDlg::OnRclickListdecompressedfiles)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTDECOMPRESSEDFILES, &TooieRandoDlg::OnDblclkListdecompressedfiles)
	ON_EN_CHANGE(IDC_SEED_ENTRY, &TooieRandoDlg::OnEnChangeSeedEntry)
    ON_BN_CLICKED(IDC_BUTTON4, &TooieRandoDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_DECOMPRESSGAME2, &TooieRandoDlg::OnBnClickedDecompressgame2)
END_MESSAGE_MAP()


// CGEDecompressorDlg message handlers

int GetSizeFile(CString filename)
{
	FILE* inFile = fopen(filename, "rb");
	
	if (inFile == 0)
		return 0;

	fseek(inFile, 0, SEEK_END);
	int fileSize = ftell(inFile);
	fclose(inFile);
	return fileSize;
}

struct FoundDLImageData
{
	bool found[7];
	int widths[7];
	int heights[7];
	int uls[7];
	int ult[7];
	int lrs[7];
	int lrt[7];
	CString files[7];
	unsigned long addresses[7];

	FoundDLImageData()
	{
		for (int x = 0; x < 7; x++)
			found[x] = false;
	}
};

BOOL TooieRandoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	fileCount = 0;
	killThread = false;
	lastSearchSpot = -1;

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	
	// TODO: Add extra initialization here
	char tempFolder[8000];
	::GetCurrentDirectory(8000, tempFolder);
	directory.Format("%s\\", tempFolder);

	decompressGamethread = NULL;

	ROM = NULL;
	romName = "Banjo Tooie";
	ROMSize = 0;

	return TRUE;  // return TRUE  unless you set the focus to a control
}


TooieRandoDlg::~TooieRandoDlg(void)
{
	
}

LRESULT TooieRandoDlg::AddListEntry(WPARAM wp, LPARAM lp)
{    	
	ListUpdateStruct* listUpdate = ((ListUpdateStruct*)lp);
	CString addressStr;
	addressStr.Format("%08X", listUpdate->address);
    LVITEM lv;
    lv.iItem = fileCount;
    lv.iSubItem = 0;
	lv.pszText = (LPSTR) addressStr.GetBuffer();
    lv.mask = LVIF_TEXT;
    m_list.InsertItem(&lv);
	m_list.SetItemData(fileCount, (DWORD_PTR)listUpdate);
	
	CString fileSizeUncompressedStr;
	fileSizeUncompressedStr.Format("%06X", listUpdate->fileSizeUncompressed);

    LVITEM lv2;
    lv2.iItem = fileCount;
    lv2.iSubItem = 1;
    lv2.pszText = (LPSTR) fileSizeUncompressedStr.GetBuffer();
    lv2.mask = LVIF_TEXT;
	m_list.SetItem(&lv2);   

	CString fileSizeCompressedStr;
	fileSizeCompressedStr.Format("%06X", listUpdate->fileSizeCompressed);

	LVITEM lv3;
    lv3.iItem = fileCount;
    lv3.iSubItem = 2;
    lv3.pszText = (LPSTR) fileSizeCompressedStr.GetBuffer();
    lv3.mask = LVIF_TEXT;
	m_list.SetItem(&lv3);

	LVITEM lv4;
    lv4.iItem = fileCount;
    lv4.iSubItem = 3;
	lv4.pszText = (LPSTR) listUpdate->type.GetBuffer();
    lv4.mask = LVIF_TEXT;
	m_list.SetItem(&lv4);

	LVITEM lv5;
    lv5.iItem = fileCount;
    lv5.iSubItem = 4;
    lv5.pszText = (LPSTR) listUpdate->tempLocation.GetBuffer();
    lv5.mask = LVIF_TEXT;
	m_list.SetItem(&lv5);

	LVITEM lv6;
    lv6.iItem = fileCount;
    lv6.iSubItem = 5;
    lv6.pszText = (LPSTR) listUpdate->internalName.GetBuffer();
    lv6.mask = LVIF_TEXT;
	m_list.SetItem(&lv6);

	fileCount++;

	return 0;
}

LRESULT TooieRandoDlg::UpdateProgressBar(WPARAM wp, LPARAM lp)
{    	
	int progress = (int)lp;
	m_progressBar.SetPos(progress);

	if (progress == 100)
	{
		m_progressBar.ShowWindow(SW_HIDE);
		m_cancelLoad.ShowWindow(SW_HIDE);
	}
	return 0;
}

void TooieRandoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void TooieRandoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR TooieRandoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//Tables type 1
unsigned short bt1Table1;

//Tables type 1 and 2


void TooieRandoDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here

	
	CString fileOpen;
	CFileDialog m_ldFile(TRUE, "bin", "uploadfile.bin", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Bin File of Codes (*.bin)|*.bin|");
	int didRead = m_ldFile.DoModal();
	if ((didRead == IDCANCEL) || (m_ldFile.GetPathName() == ""))
		return;

	if (didRead == FALSE)
		return;

	CFileDialog m_svFile(FALSE, "bin", (m_ldFile.GetFileName() + ".bin"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Bin File of Codes (*.*)|*.*|");
	didRead = m_svFile.DoModal();
	
	if ((didRead == IDCANCEL) || (m_ldFile.GetPathName() == ""))
		return;

	if (didRead == FALSE)
		return;

	CString gameNameStr;
	//m_gameselection.GetWindowText(gameNameStr);
	
	int size = GetSizeFile(m_ldFile.GetPathName());
	if (size > 0)
	{
		FILE* inFile = fopen(m_ldFile.GetPathName(),"rb");
		if (inFile != NULL)
		{
			unsigned char* Buffer;
			Buffer = new unsigned char[size];
			fread(Buffer, 1, size, inFile);
			

			unsigned char* outputDecompressed = NULL;
			int fileSize = 0;
			try
			{
				if (GetZLibGameName(gameNameStr) != -1)
				{
					GECompression compressed;
					compressed.SetPath(directory);
					compressed.SetGame(GetZLibGameName(gameNameStr));
					
					compressed.SetCompressedBuffer(Buffer, size);
					int compressedSize = 0;
					unsigned char* outputDecompressed = compressed.OutputDecompressedBuffer(fileSize, compressedSize);
				}

				if ((outputDecompressed != NULL))
				{
					if (fileSize > 0)
					{
						FILE* outFile = fopen(m_svFile.GetPathName(), "wb");
						for (int x = 0; x < fileSize; x++)
						{
							fwrite(&outputDecompressed[x], 1, 1, outFile);
						}
						fclose(outFile);
					}

					delete [] outputDecompressed;
				}
			}
			catch (...)
			{
				MessageBox("Error decompressing");
			}

			delete [] Buffer;


			fclose(inFile);
		}
		else
		{
			MessageBox("Cannot open input file");
			return;
		}
	}
}

int TooieRandoDlg::HexToInt(char inChar)
{
	switch(inChar)
	{
	case '0':
		return 0;
	case '1':
		return 1;
	case '2':
		return 2;
	case '3':
		return 3;
	case '4':
		return 4;
	case '5':
		return 5;
	case '6':
		return 6;
	case '7':
		return 7;
	case '8':
		return 8;
	case '9':
		return 9;
	case 'A':
		return 10;
	case 'a':
		return 10;
	case 'B':
		return 11;
	case 'b':
		return 11;
	case 'C':
		return 12;
	case 'c':
		return 12;
	case 'D':
		return 13;
	case 'd':
		return 13;
	case 'E':
		return 14;
	case 'e':
		return 14;
	case 'F':
		return 15;
	case 'f':
		return 15;
	default:
		return 0;
	}
}

unsigned long TooieRandoDlg::StringHexToLong(CString inString)
{
	int tempA = inString.GetLength();
	if (inString.GetLength() < 8)
	{
		CString tempStr = inString;
		inString = "";
		for (int x = 0; x < (8-tempStr.GetLength()); x++)
		{
			inString = inString + "0";
		}
		inString = inString + tempStr;
	}
	char* b;
	b = inString.GetBuffer(0);
	unsigned long tempLong = 0;
	for (int x = 0;x < 8; x++)
	{
		char tempChar = b[x];
		int hexInt = HexToInt(tempChar);
		tempLong = tempLong | hexInt<<((7-x)*4);
	}
	return tempLong;
}

unsigned long TooieRandoDlg::GetSizeFile(CString filename)
{
	FILE* inFile = fopen(filename, "rb");
	
	if (inFile == 0)
		return 0;

	fseek(inFile, 0, SEEK_END);
	unsigned long fileSize = ftell(inFile);
	fclose(inFile);
	return fileSize;
}

void TooieRandoDlg::OnBnClickedCompressfilebutton()
{
	// TODO: Add your control notification handler code here
	CString fileOpen;
	CFileDialog m_ldFile(TRUE, NULL, "compressedfile.bin", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Bin File of Codes (*.*)|*.*|");
	int didRead = m_ldFile.DoModal();
	if ((didRead == IDCANCEL) || (m_ldFile.GetPathName() == ""))
		return;

	if (didRead == FALSE)
		return;

	CString gameNameStr;
	//m_gameselection.GetWindowText(gameNameStr);

	if (GetZLibGameName(gameNameStr) != -1)
	{
		GECompression compressed;
		compressed.SetPath(directory);
		compressed.SetGame(GetZLibGameName(gameNameStr));

		CFileDialog m_svFile(FALSE, "bin", (m_ldFile.GetFileName() + ".bin"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Bin File of Codes (*.*)|*.*|");
		didRead = m_svFile.DoModal();
		if ((didRead == IDCANCEL) || (m_svFile.GetPathName() == ""))
			return;

		if (didRead == FALSE)
			return;

		compressed.CompressGZipFile(m_ldFile.GetPathName(), m_svFile.GetPathName(), false);
	}
	else
	{
		MessageBox("Unsupported game, cannot compress");
		return;
	}
}

float TooieRandoDlg::CharArrayToFloat(unsigned char* currentSpot)
{
	unsigned long tempLong = (Flip32Bit(*reinterpret_cast<unsigned long*> (currentSpot)));
	return (*reinterpret_cast<float*> (&tempLong));
}

unsigned long TooieRandoDlg::CharArrayToLong(unsigned char* currentSpot)
{
	return Flip32Bit(*reinterpret_cast<unsigned long*> (currentSpot));
}

unsigned long TooieRandoDlg::Flip32Bit(unsigned long inLong)
{
	return (((inLong & 0xFF000000) >> 24) | ((inLong & 0x00FF0000) >> 8) | ((inLong & 0x0000FF00) << 8) | ((inLong & 0x000000FF) << 24));
}

unsigned short TooieRandoDlg::CharArrayToShort(unsigned char* currentSpot)
{
	return Flip16Bit(*reinterpret_cast<unsigned short*> (currentSpot));
}

unsigned short TooieRandoDlg::Flip16Bit(unsigned short ShortValue)
{
	return ((ShortValue >> 8) | ((ShortValue << 8)));
}

void TooieRandoDlg::ReceivedNewROM(TooieRandoDlg* dlg, CString fileLocation, unsigned char* GameBuffer, int romSize)
{
	if (romSize > 0)
	{
		dlg->romName = fileLocation;
		dlg->ROMSize = romSize;
		if (dlg->ROM != NULL)
		{
			delete [] dlg->ROM;
			dlg->ROM = NULL;
		}
		dlg->ROM = new unsigned char[romSize];
		memcpy(dlg->ROM, GameBuffer, romSize);
	}
}

void TooieRandoDlg::DecryptBTFile(int fileNumber, unsigned char* input, unsigned char* output, int size)
{
	char rsp[0x20]; 

	unsigned char inputKey[0x10];

	signed short key = (fileNumber - 0x955);

	signed long A2 = 0x10;
	for (signed long V1 = 0; V1 < 0xE; V1+=2)
	{

		signed long T6 = key >> V1;
		signed long T7 = A2 - V1;
		signed long T8 = key << T7;

		signed long T9 = T6 | T8;

		inputKey[V1] = ((T9) & 0xFF);
		inputKey[V1+1] = ((T9 & 0xFF00) & 0xFF);
	}

	inputKey[0x0E] = 0x00;
	inputKey[0x0F] = 0x02;

	char nibbleKeyVersion[0x20];
	for (int x = 0; x < 0x20; x++)
	{
		if ((x % 2) == 0)
			nibbleKeyVersion[x] = ((inputKey[x/2] >> 4) & 0xF);
		else
			nibbleKeyVersion[x] = ((inputKey[x/2]) & 0xF);
	}

	n64_cic_nus_6105(nibbleKeyVersion, rsp, CHL_LEN - 2);
	rsp[CHL_LEN - 2] = rsp[CHL_LEN - 1] = 0;

	unsigned char cicValue[0x10];
	for (int x = 0; x < 0x20; x+=2)
	{
		cicValue[x/2] = (rsp[x] << 4) | rsp[x+1];
	}

	for (int x = 0; x < size; x++)
	{
		unsigned char value = input[x];
		output[x] = value ^ cicValue[x % 0xE];
	}
}

void TooieRandoDlg::DecompressZLibAtPosition(CString gameNameStr, TooieRandoDlg* dlg, CString filein, unsigned long start, int GAME)
{

	CString folderPath = filein;
	folderPath = folderPath.Mid(0, (folderPath.ReverseFind('\\') + 1));

	GECompression compressed;
	compressed.SetPath(dlg->directory);
	compressed.SetGame(GAME);

	unsigned char* input;
	int size;
	if (ReadROM(gameNameStr, filein, input, size, folderPath))
	{
		ReceivedNewROM(dlg, filein, input, size);
		unsigned long address;
		CString type = "";
		CString fileNameStr = "";
		address = start;

		CString tempLocation;
		int fileSizeCompressed = -1;
		
			int fileSizeUncompressed = DecompressZLibSpot(&compressed, dlg->genText, address, input, size, GAME, folderPath, fileNameStr, -1, tempLocation, fileSizeCompressed, type, address, false, 0);
				
			AddRowData(dlg, address, fileSizeCompressed, fileSizeUncompressed, fileNameStr, tempLocation, type);	
	}
}
void TooieRandoDlg::DecompressZLibFromTable(CString gameNameStr, TooieRandoDlg* dlg, CString filein, unsigned long start, unsigned long end, int step, int GAME, unsigned long tblOffset, int shift, int multiplier, int offset)
{
	CString folderPath = filein;
	folderPath = folderPath.Mid(0, (folderPath.ReverseFind('\\') + 1));

	GECompression compressed;
	compressed.SetPath(dlg->directory);
	compressed.SetGame(GAME);

	unsigned char* input;
	int size;
	if (ReadROM(gameNameStr, filein, input, size, folderPath))
	{
		ReceivedNewROM(dlg, filein, input, size);
		for (unsigned long x = start; x < end; x+=step)
		{
			if (dlg->killThread)
				break;
			ToUpdateProgressBar(dlg, (x - start), (end - x));
			unsigned long address;
			CString type = "";
			CString fileNameStr = "";

			if (GAME == BANJOTOOIE)
			{
				address = ((CharArrayToLong(&input[x]) >> shift) * multiplier) + tblOffset + offset;
				unsigned char btType = input[x+3];
				if(address<=0x1E8A794) //Don't exclude files with an 0x04 in the second table because they are not empty
					if (btType == 0x04) // empty
						continue;

				type.Format("%02X", btType);
				if (btType == 0x1A)
					type += " Encrypted";
			}
			else 
				address = CharArrayToLong(&input[x]) + tblOffset;
			
			CString tempLocation;
			int fileSizeCompressed = -1;
			

			if ((GAME == BANJOTOOIE) && (input[x+3] == 0x1A)) // Encrypted
			{
				unsigned long nextFile = ((CharArrayToLong(&input[x+4]) >> shift) * multiplier) + tblOffset + offset;
				fileSizeCompressed = (nextFile - address);
				unsigned char* compressedCopy = new unsigned char[fileSizeCompressed];
				memcpy(compressedCopy, &input[address], fileSizeCompressed);
				int fileNumber = ((x - 0x5188) / 4);
				fileNameStr.Format("%04X", fileNumber);
				DecryptBTFile(fileNumber, compressedCopy, compressedCopy, fileSizeCompressed);

				int fileSizeUncompressed = DecompressZLibSpot(&compressed, dlg->genText, 0, compressedCopy, fileSizeCompressed, GAME, folderPath, fileNameStr, -1, tempLocation, fileSizeCompressed, type, address, false, 0);
				if (fileSizeUncompressed > 0)
				{
					fileSizeCompressed = (nextFile - address);
					AddRowData(dlg, x, fileSizeCompressed, fileSizeUncompressed, fileNameStr, tempLocation, type);
				}
			}
			else
			{
				int fileSizeUncompressed = DecompressZLibSpot(&compressed, dlg->genText, address, input, size, GAME, folderPath, fileNameStr, -1, tempLocation, fileSizeCompressed, type, address, false, 0);
				if (fileSizeUncompressed > 0)
				{
					AddRowData(dlg, x, fileSizeCompressed, fileSizeUncompressed, fileNameStr, tempLocation, type);
				}
			}
			
		}

		delete [] input;
	}
}

bool TooieRandoDlg::ReadROM(CString gameNameStr, CString fileLocation, unsigned char*& GameBuffer, int& romSize, CString& folderPath)
{
	folderPath = fileLocation;
	folderPath = folderPath.Mid(0, (folderPath.ReverseFind('\\') + 1));

	
	romSize = GetSizeFile(fileLocation);
	FILE* inFile = fopen(fileLocation,"rb");
	if (inFile == NULL)
	{
		::MessageBox(NULL, "Invalid ROM", "Error", NULL);
		return false;
	}
	
	
	if (romSize == 0)
	{
		fclose(inFile);
		return false;
	}

	fseek(inFile,0,SEEK_END);
	romSize = ftell(inFile);
	rewind(inFile);

	GameBuffer = new unsigned char[romSize+1];
	int readAmt = fread(GameBuffer, 1, romSize, inFile);
	fclose(inFile);

	
	bool byteFlippedROM = false;
	if ((gameNameStr != "Star Fox Adventures (GC) ZLB"))
	{
		if ((GameBuffer[0] == 0x37) || (GameBuffer[0] == 0x08))
			byteFlippedROM = true;
		else if (((GameBuffer[0] == 0x80) || (GameBuffer[0] == 0x3C)) || (CharArrayToLong(&GameBuffer[0]) == 0xE848D316))
			byteFlippedROM = false;
		else
		{
			int iResults = ::MessageBox(NULL, "Invalid ROM, do you want to try and continue?", "Wrong ROM Header", MB_YESNO);
			if (iResults == IDNO)
			{
				delete [] GameBuffer;
				return false;
			}
		}
	}

	if (byteFlippedROM == true)
	{
		if ((romSize % 2) == 1)
		{
			GameBuffer[romSize] = 0;
			romSize++;
		}

		for (int x = 0; x < romSize; x=x+2)
		{
			unsigned char tempSpot = GameBuffer[x];
			GameBuffer[x] = GameBuffer[x+1];
			GameBuffer[x+1] = tempSpot;
		}
	}	
	return true;
}

bool TooieRandoDlg::AllocateInput(int offset, unsigned char*& Buffer, unsigned char* GameBuffer, int& endSize, int romSize)
{
	endSize = maxByteSize;
	if ((offset + maxByteSize) > romSize)
	{
		endSize = (romSize - offset);
	}

	Buffer = new unsigned char[endSize];
	for (int y = 0; y < endSize; y++)
	{
		Buffer[y] = GameBuffer[offset+y];
	}
	return true;
}

int TooieRandoDlg::DecompressZLibSpot(GECompression* compressed, bool genText, int offset, unsigned char* GameBuffer, int romSize, int GAME, CString folderPath, CString internalName, int expectedSize, CString& tempLocation, int& fileSizeCompressed, CString& type, unsigned long printedAddress, bool printBank, unsigned printbankAddress)
{
	int returnSize = 0;
	unsigned char* Buffer = NULL;
	unsigned char* outputDecompressed = NULL;

	try
	{
		int endSize = 0;
		AllocateInput(offset, Buffer, GameBuffer, endSize, romSize);
		
		compressed->SetCompressedBuffer(Buffer, endSize);
		int fileSize = 0;
		outputDecompressed = compressed->OutputDecompressedBuffer(fileSize, fileSizeCompressed);

		if ((outputDecompressed != NULL))
		{
			if (fileSize > 0)
			{
				if ((expectedSize == -1) || (fileSize == expectedSize))
				{
					if (printBank)
						tempLocation.Format("%s%06X_%06X.bin", folderPath, printedAddress, printbankAddress);
					else if (internalName == "")
						tempLocation.Format("%s%06X.bin", folderPath, printedAddress);
					else
						tempLocation.Format("%s%06X_%s.bin", folderPath, printedAddress, internalName);

					WriteResult(genText, tempLocation, outputDecompressed, fileSize, false);

					returnSize = fileSize;
				}
			}

			delete [] outputDecompressed;
		}

		delete [] Buffer;
	}
	catch (...)
	{
		if (Buffer != NULL)
		{
			delete [] Buffer;
			Buffer = NULL;
		}
		if (outputDecompressed != NULL)
		{
			delete [] outputDecompressed;
			outputDecompressed = NULL;
		}
	}
	return returnSize;
}

int TooieRandoDlg::GetZLibGameName(CString gameNameStr)
{
	return BANJOTOOIE;
}



void TooieRandoDlg::AddRowData(TooieRandoDlg* dlg, unsigned long address, int fileSizeCompressed,  int fileSizeUncompressed, CString internalName, CString tempLocation, CString type)
{
	ListUpdateStruct* listUpdate = new ListUpdateStruct();
	listUpdate->address = address;
	listUpdate->fileSizeUncompressed = fileSizeUncompressed;
	listUpdate->fileSizeCompressed = fileSizeCompressed;
	listUpdate->fileSizeOriginal = fileSizeCompressed;
	listUpdate->internalName = static_cast<LPCTSTR>(internalName);
	listUpdate->tempLocation = static_cast<LPCTSTR>(tempLocation);
	listUpdate->type = type;
	::SendMessage(dlg->m_hWnd, UPDATE_LIST, 0, (LPARAM)listUpdate);
}

void TooieRandoDlg::ToUpdateProgressBar(TooieRandoDlg* dlg, int curSpot,  int romSize)
{
	int progress = (((float)curSpot / (float)romSize) * 100);
	if ((curSpot % 0x10000) == 0)
	{
		if (progress != 100)
			::SendMessage(dlg->m_hWnd, UPDATE_PROGRESS_BAR, 0, (LPARAM)progress);
	}
	if ((progress == 100) && (romSize == curSpot))
		::SendMessage(dlg->m_hWnd, UPDATE_PROGRESS_BAR, 0, (LPARAM)progress);
}

UINT TooieRandoDlg::DecompressGameThread( LPVOID pParam )
{
	TooieRandoDlg* dlg = (TooieRandoDlg*)(pParam);
	CString gameNameStr = "Banjo Tooie";
	CString strROMPath = dlg->strROMPath;
	bool genText = dlg->genText;
	GECompression compressed;
	compressed.SetPath(dlg->directory);

	unsigned char* GameBuffer = NULL;
	try
	{
		if (gameNameStr == "Banjo Tooie")
		{
			int romSize = 0;
			unsigned char* GameBuffer = NULL;
			CString folderPath;
			if (ReadROM(gameNameStr, strROMPath, GameBuffer, romSize, folderPath))
			{
				ReceivedNewROM(dlg, strROMPath, GameBuffer, romSize);
			}
			int region = GameBuffer[0x3E];
			
			dlg->syscallTableStart = int(GameBuffer[0x50E0])<<24 | int(GameBuffer[0x50E1])<<16| int(GameBuffer[0x50E2])<<8| int(GameBuffer[0x50E3]);
			delete [] GameBuffer;

			int game = GetZLibGameName(gameNameStr);
			compressed.SetGame(game);
			if (region == 0x4A) // (J)
			{
				DecompressZLibFromTable(gameNameStr, dlg, strROMPath, 0x00005148, 0x00012AE4 , 4, BANJOTOOIE, 0x00012AE4, 8, 4, 0);
				DecompressZLibFromTable(gameNameStr, dlg, strROMPath, 0x01E308D0, 0x01E316A0 , 4, BANJOTOOIE, 0x01E308D0, 0, 1, 0x10);
			}
			else if (region == 0x55) // (A)
			{
				DecompressZLibFromTable(gameNameStr, dlg, strROMPath, 0x00005148, 0x00012AE4 , 4, BANJOTOOIE, 0x00012AE4, 8, 4, 0);
				DecompressZLibFromTable(gameNameStr, dlg, strROMPath, 0x01E18A80, 0x01E1984C , 4, BANJOTOOIE, 0x01E18A80, 0, 1, 0x10);
			}
			else if (region == 0x50) // (E)
			{
				DecompressZLibFromTable(gameNameStr, dlg, strROMPath, 0x00005148, 0x00012AE4 , 4, BANJOTOOIE, 0x00012AE4, 8, 4, 0);
				DecompressZLibFromTable(gameNameStr, dlg, strROMPath, 0x01E8DA70, 0x01E8E840 , 4, BANJOTOOIE, 0x01E8DA70, 0, 1, 0x10);
			}
			else if (region == 0x45) // (U)
			{


				DecompressZLibFromTable(gameNameStr, dlg, strROMPath, 0x7958, 0x7E58 , 4, BANJOTOOIE, 0x12B24, 8, 4, 0);
				DecompressZLibAtPosition(gameNameStr, dlg, strROMPath, 0x1E29B60,BANJOTOOIE);
				DecompressZLibAtPosition(gameNameStr, dlg, strROMPath, 0x1E3F718,BANJOTOOIE);
				DecompressZLibAtPosition(gameNameStr, dlg, strROMPath, 0x1E42550,BANJOTOOIE);
				DecompressZLibAtPosition(gameNameStr, dlg, strROMPath, 0x1E86C76,BANJOTOOIE);
				DecompressZLibFromTable(gameNameStr, dlg, strROMPath, dlg->syscallTableStart, dlg->syscallTableStart +0xDCC, 4, BANJOTOOIE, dlg->syscallTableStart, 0, 1, 0x10);
				
			}
			::MessageBox(NULL, "NO", "Error", NULL);
			TooieRandoDlg* pDlg = (TooieRandoDlg*)pParam;
			pDlg->PostMessage(WM_USER + 1, 0, 0);
			return 0;
		}
	}
	catch (...)
	{
		if (GameBuffer != NULL)
		{
			delete [] GameBuffer;
			GameBuffer = NULL;
		}
	}
	ToUpdateProgressBar(dlg, 100, 100);
	dlg->decompressGamethread = NULL;
	return 0;
}

void TooieRandoDlg::OnBnClickedDecompressgame()
{
	KillDecompressGameThread();
	// TODO: Add your control notification handler code here
	CString fileOpen;
	CFileDialog m_ldFile(TRUE, NULL, "Game.rom", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "ROM (*.v64;*.z64;*.rom;*.bin;*.zlb)|*.v64;*.z64;*.rom;*.bin;*.zlb|", this);
	int didRead = m_ldFile.DoModal();
	if ((didRead == IDCANCEL) || (m_ldFile.GetPathName() == ""))
		return;

	if (didRead == FALSE)
		return;

	fileCount = 0;

	for (int x = 0; x < m_list.GetItemCount(); x++)
	{
		delete ((ListUpdateStruct*)m_list.GetItemData(x));
	}

	m_list.DeleteAllItems();
	for ( int i = 5; i >= 0; i--)
     m_list.DeleteColumn (i);
	m_list.InsertColumn(0, "Address",LVCFMT_LEFT, 200);
	m_list.InsertColumn(1, "Uncompressed Size",LVCFMT_LEFT, 100);
	m_list.InsertColumn(2, "Compressed Size",LVCFMT_LEFT, 100);
	m_list.InsertColumn(3, "Type",LVCFMT_LEFT, 60);
	m_list.InsertColumn(4, "Path",LVCFMT_LEFT, 300);
	m_list.InsertColumn(5, "Internal Name",LVCFMT_LEFT, 100);
	

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	CString gameNameStr = "Banjo-Tooie";

	CString filePathString = m_ldFile.GetPathName();

	this->strROMPath = filePathString;
	this->gameNameStr = gameNameStr;
	this->genText = m_genTextFiles.GetCheck();

	int size = GetSizeFile(this->strROMPath);
	m_progressBar.SetRange(0, 100);
	m_progressBar.SetPos(0);
	m_progressBar.ShowWindow(SW_SHOW);
	killThread = false;
	m_cancelLoad.ShowWindow(SW_SHOW);
	lastSearchSpot = -1;
	decompressGamethread = AfxBeginThread(&TooieRandoDlg::DecompressGameThread, this);
 
	mDecompressFileButton.ShowWindow(SW_SHOW);


	int zlibGame = GetZLibGameName(gameNameStr);
	mCompressFileButton.ShowWindow(SW_SHOW);
	m_injectButton.ShowWindow(SW_SHOW);
	m_saveROMButton.ShowWindow(SW_SHOW);


}

void TooieRandoDlg::WriteASCIIFile(CString filename, unsigned char* outputDecompressed, int fileSize, bool appendFile)
{
	FILE* outFile;
	if (!appendFile)
		outFile = fopen(filename, "w");
	else
		outFile = fopen(filename, "a");
	if (outFile != NULL)
	{
		for (int y = 0; y < fileSize; y++)
		{
			if (outputDecompressed != NULL)
				fprintf(outFile, "%02X", outputDecompressed[y]);
		}
		fprintf(outFile, "\n\n\n");
		for (int y = 0; y < fileSize; y++)
		{
			if (outputDecompressed != NULL)
				fprintf(outFile, "%c", outputDecompressed[y]);
		}
		fprintf(outFile, "\n\n\n");
	}
	fflush(outFile);
	fclose(outFile);
}

void TooieRandoDlg::WriteBinaryFile(CString filename, unsigned char* outputDecompressed, int fileSize, bool appendFile)
{
	FILE* outFile;
	if (!appendFile)
		outFile = fopen(filename, "wb");
	else
		outFile = fopen(filename, "ab");
	if (outFile != NULL)
	{
		for (int y = 0; y < fileSize; y++)
		{
			if (outputDecompressed != NULL)
				fwrite(&outputDecompressed[y], 1, 1, outFile);
		}
		fflush(outFile);
		fclose(outFile);
	}
}

void TooieRandoDlg::WriteResult(bool genTextFile, CString filename, unsigned char* outputDecompressed, int fileSize, bool appendFile)
{
	if (fileSize > 0)
	{
		WriteBinaryFile(filename, outputDecompressed, fileSize, appendFile);
		if (genTextFile)
		{
			WriteASCIIFile(filename + ".txt", outputDecompressed, fileSize, appendFile);
		}
	}
}

void TooieRandoDlg::WriteLongToBuffer(unsigned char* Buffer, unsigned long address, unsigned long data)
{
	Buffer[address] = ((data >> 24) & 0xFF);
	Buffer[address+1] = ((data >> 16) & 0xFF);
	Buffer[address+2] = ((data >> 8) & 0xFF);
	Buffer[address+3] = ((data) & 0xFF);
}
void TooieRandoDlg::InjectFile(CString filePath, int index)
{
	ListUpdateStruct* listUpdateStruct = ((ListUpdateStruct*)m_list.GetItemData(index));

	unsigned long diff = 0xF000000;

	if (listUpdateStruct->fileSizeCompressed > 0)
		diff = listUpdateStruct->fileSizeOriginal;
	else if (index < (m_list.GetItemCount() - 1))
	{
		ListUpdateStruct* listUpdateStructNext = ((ListUpdateStruct*)m_list.GetItemData(index+1));
		diff = listUpdateStructNext->address - listUpdateStruct->address;
	}

	unsigned long address = listUpdateStruct->address;


	CString gameNameStr;
	//m_gameselection.GetWindowText(gameNameStr);

	CString tempAddrStr;
	tempAddrStr.Format("%08X_MODIFIED.bin", address);

	int zlibGame = GetZLibGameName(gameNameStr);
	if (zlibGame != -1)
	{
		if (listUpdateStruct->type == "Uncompressed")
		{
			unsigned long outSize = GetSizeFile(filePath);
			if (outSize > 0)
			{
				if (outSize > diff)
				{
					CString sizeTempStr;
					sizeTempStr.Format("%08X is larger than %08X (next item), are you sure you want to replace?", outSize, diff);
					//int iResults = MessageBox(sizeTempStr, "Are you sure?", MB_YESNO | MB_ICONINFORMATION);
					//if (iResults == IDNO)
						//return;
				}

				FILE* inNew = fopen(filePath, "rb");
				if (inNew == NULL)
				{
					MessageBox("Error opening temporary file");
					return;
				}

				fread(&ROM[address], 1, outSize, inNew);
				
				fclose(inNew);
				
				listUpdateStruct->fileSizeCompressed = outSize;
				listUpdateStruct->tempLocation = static_cast<LPCTSTR>(directory + tempAddrStr);
			}
		}
		else if (listUpdateStruct->type == "MP3")
		{
			unsigned long outSize = GetSizeFile(filePath);
			if (outSize > 0)
			{
				if (outSize > diff)
				{
					CString sizeTempStr;
					sizeTempStr.Format("%08X is larger than %08X (next item), are you sure you want to replace?", outSize, diff);
					//int iResults = MessageBox(sizeTempStr, "Are you sure?", MB_YESNO | MB_ICONINFORMATION);
					//if (iResults == IDNO)
					//	return;
				}

				FILE* inNew = fopen(filePath, "rb");
				if (inNew == NULL)
				{
					MessageBox("Error opening temporary file");
					return;
				}

				fread(&ROM[address], 1, outSize, inNew);
				
				fclose(inNew);
				
				listUpdateStruct->fileSizeCompressed = outSize;
				listUpdateStruct->tempLocation = static_cast<LPCTSTR>(directory + tempAddrStr);
			}
		}
		else
		{
			GECompression compressed;
			compressed.SetPath(directory);
			compressed.SetGame(zlibGame);

			CString zlibInput = filePath;
			compressed.CompressGZipFile(zlibInput, (directory + tempAddrStr), false);

			unsigned long outSize = GetSizeFile((directory + tempAddrStr));
			if (outSize > 0)
			{
				if (outSize > diff)
				{
					CString sizeTempStr;
					sizeTempStr.Format("%08X is larger than %08X (next item), are you sure you want to replace?", outSize, diff);
					//int iResults = MessageBox(sizeTempStr, "Are you sure?", MB_YESNO | MB_ICONINFORMATION);
					//if (iResults == IDNO)
					//	return;
				}

				if (zlibGame == BANJOTOOIE)
				{
					if (listUpdateStruct->type.Find("Encrypted") != -1)
					{
						FILE* inFile = fopen((directory + tempAddrStr), "rb");
						if (inFile == NULL)
						{
							MessageBox("Cannot open BT file", "Error");
							return;
						}

						unsigned char* tempInputBuffer = new unsigned char[outSize];
						fread(tempInputBuffer, 1, outSize, inFile);
						fclose(inFile);

						DecryptBTFile(((listUpdateStruct->address - 0x5188) / 4), tempInputBuffer, tempInputBuffer, outSize);

						FILE* outFile = fopen((directory + tempAddrStr), "wb");
						if (outFile == NULL)
						{
							MessageBox("Cannot open encrypted BT file", "Error");
							return;
						}
						fwrite(tempInputBuffer, 1, outSize, outFile);
						fclose(outFile);

						delete [] tempInputBuffer;
					}

					int extraAAs = 0;
					if (outSize < diff)
					{
						extraAAs = (diff - outSize);
					}

					FILE* appendFile = fopen((directory + tempAddrStr), "ab");
					if (appendFile == NULL)
					{
						MessageBox("Cannot open BT file", "Error");
						return;
					}

					unsigned char aa = 0xAA;
					for (int x = outSize; x < diff; x++)
					{
						fwrite(&aa, 1, 1, appendFile);
					}
					fclose(appendFile);
				}

				FILE* inNew = fopen((directory + tempAddrStr), "rb");
				if (inNew == NULL)
				{
					MessageBox("Error opening temporary file");
					return;
				}

				if ((zlibGame == BANJOTOOIE) && (address >= 0x5188)  && (address < 0x11A24))
				{
					address = ((CharArrayToLong(&ROM[address]) >> 8) * 4) + 0x12B24;
					if(diff>outSize)
						fread(&ROM[address], 1, diff, inNew);
					else
						fread(&ROM[address], 1, outSize, inNew);
				}
				if ((zlibGame == BANJOTOOIE) && (address >= 0x01E899B0)) 
				{
					//char message[256];
					//sprintf(message, "Original address: 0x%X\n", address);
					//MessageBox(message);
			        
					long value = CharArrayToLong(&ROM[address]);
					//sprintf(message, "Value from ROM: 0x%08lx\n", value);
					//MessageBox(message);

					address = (value) + syscallTableStart+0x10;
					//sprintf(message, "Transformed address: 0x%08lx\n", address);
					//MessageBox(message);
					if(diff>outSize)
						fread(&ROM[address], 1, diff, inNew);
					else
						fread(&ROM[address], 1, outSize, inNew);
				}
				else
				{
					fread(&ROM[address], 1, outSize, inNew);
				}
				fclose(inNew);
				
				listUpdateStruct->fileSizeCompressed = outSize;
				listUpdateStruct->tempLocation = static_cast<LPCTSTR>(directory + tempAddrStr);
			}
		}
	}
	else
	{
		MessageBox("Unsupported game, cannot compress");
		return;
	}

}
void TooieRandoDlg::OnBnClickedButton3()
{

	CString fileOpen;
	
	CFileDialog m_ldFile(TRUE, NULL, "binary.bin", NULL, "IN Binary to Inject(*.bin)|*.*|", this);
	int didRead = m_ldFile.DoModal();
	if ((didRead == IDCANCEL) || (m_ldFile.GetPathName() == ""))
		return;
	if (didRead == FALSE)
		return;

	int rowSel = m_list.GetSelectionMark();
	if (rowSel == -1)
		return;
	InjectFile(m_ldFile.GetPathName(),rowSel);
}

unsigned char TooieRandoDlg::ReturnASCIIChar(unsigned char asciiByte)
{
	if ((asciiByte >= 0x21) && (asciiByte < 0x7F))
		return asciiByte;
	else
		return 0x20;
}

void TooieRandoDlg::OnLvnItemchangedListdecompressedfiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	// only when done
	if (decompressGamethread == NULL)
	{
		LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
		int item = pNMLV->iItem;
		ListUpdateStruct* listUpdateStruct = ((ListUpdateStruct*)m_list.GetItemData(item));

		int size = GetSizeFile(listUpdateStruct->tempLocation);
		CString locationAddressStr;
		mFile.SetWindowText(listUpdateStruct->tempLocation);
		FILE* inFile = fopen(listUpdateStruct->tempLocation, "rb");
		
		if (inFile == NULL)
		{
			m_hexView.SetWindowText("!!!GEDECOMPRESSOR ERROR:File not found or error opening!!!");
			return;
		}
		unsigned char* buffer = new unsigned char[size];
		fread(buffer, 1, size, inFile);
		fclose(inFile);

		CString totalStr = "";
		if (size >= 0x10)
		{
			for (int x = 0; x < size; x+=0x10)
			{
				CString tempAddStr;
				tempAddStr.Format("%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\r\n", buffer[x], buffer[x+1], buffer[x+2], buffer[x+3], buffer[x+4], buffer[x+5], buffer[x+6], buffer[x+7], buffer[x+8], buffer[x+9], buffer[x+0xA], buffer[x+0xB], buffer[x+0xC], buffer[x+0xD], buffer[x+0xE], buffer[x+0xF], ReturnASCIIChar(buffer[x]), ReturnASCIIChar(buffer[x+1]), ReturnASCIIChar(buffer[x+2]), ReturnASCIIChar(buffer[x+3]), ReturnASCIIChar(buffer[x+4]), ReturnASCIIChar(buffer[x+5]), ReturnASCIIChar(buffer[x+6]), ReturnASCIIChar(buffer[x+7]), ReturnASCIIChar(buffer[x+8]), ReturnASCIIChar(buffer[x+9]), ReturnASCIIChar(buffer[x+0xA]), ReturnASCIIChar(buffer[x+0xB]), ReturnASCIIChar(buffer[x+0xC]), ReturnASCIIChar(buffer[x+0xD]), ReturnASCIIChar(buffer[x+0xE]), ReturnASCIIChar(buffer[x+0xF]));
				totalStr = totalStr + tempAddStr;
			}
		}

		int strL = totalStr.GetLength();
		if ((size % 0x10) != 0)
		{
			for (int x = (size - (size % 0x10)); x < size; x++)
			{
				CString tempAddStr;
				tempAddStr.Format("%02X", buffer[x]);
				totalStr = totalStr + tempAddStr;
			}
			totalStr += " ";
			for (int x = (size - (size % 0x10)); x < size; x++)
			{
				CString tempAddStr;
				tempAddStr.Format("%c", buffer[x]);
				totalStr = totalStr + tempAddStr;
			}
		}

		m_hexView.SetWindowText(totalStr);

		delete [] buffer;
	}
}



void TooieRandoDlg::KillDecompressGameThread()
{

	if (decompressGamethread != NULL)
	{
		try
		{
			killThread = true;
			/*Sleep(1000);
			if (decompressGamethread != NULL)
			{
				TerminateThread(decompressGamethread, 0);
			}*/
		}
		catch (...)
		{

		}
		m_progressBar.ShowWindow(SW_HIDE);
		decompressGamethread = NULL;
		m_cancelLoad.ShowWindow(SW_HIDE);
	}
}

void TooieRandoDlg::OnBnClickedButtoncancelload()
{
	KillDecompressGameThread();
}

void TooieRandoDlg::OnBnClickedButtonsaverom()
{
	if ((ROMSize > 0) && (ROM != NULL))
	{
		CFileDialog m_svFile(FALSE, NULL, (romName + ".rom"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "OUT ROM(*.v64;*.z64;*.rom;*.bin)|*.v64;*.z64;*.rom;*.bin|", this);

		int isFileOpened2 = m_svFile.DoModal();

		if (isFileOpened2 == IDCANCEL)
			return;

		if (m_svFile.GetFileName() == "")
			return;

		FILE* outFile = fopen(m_svFile.GetPathName(), "wb");
		if (outFile == NULL)
		{
			MessageBox("Cannot open output file");
			return;
		}
		fwrite(ROM, 1, ROMSize, outFile);

		fclose(outFile);

		CString RN64dir = m_svFile.GetPathName().Mid(0, (m_svFile.GetPathName().ReverseFind('\\')+1));
		CString RN64name = m_svFile.GetPathName().Mid((m_svFile.GetPathName().ReverseFind('\\')+1), (m_svFile.GetPathName().GetLength() - (m_svFile.GetPathName().ReverseFind('\\')+1)));

		if (RN64dir != directory)
			::CopyFile((directory + "rn64crc.exe"), (RN64dir + "rn64crc.exe"), false);

		::SetCurrentDirectory(RN64dir);

		CString tempStr;
		tempStr.Format("rn64crc.exe -u \"%s\"", RN64name);
		hiddenExec(_T(tempStr.GetBuffer()), RN64dir);
	}
}

BOOL TooieRandoDlg::hiddenExec (PTSTR pCmdLine, CString currentDirectory)
{
	int errorCode;
	return hiddenExec(pCmdLine, currentDirectory, errorCode);
};

BOOL TooieRandoDlg::hiddenExec (PTSTR pCmdLine, CString currentDirectory, int& errorCode)
{
	return hiddenExec(pCmdLine, currentDirectory, errorCode, 20000);
}

BOOL TooieRandoDlg::hiddenExec (PTSTR pCmdLine, CString currentDirectory, int& errorCode, int waitTime)
{
	errorCode = 0;
	::SetCurrentDirectory(currentDirectory);
   STARTUPINFO si;
   PROCESS_INFORMATION processInfo;
   ZeroMemory(&si, sizeof(si));
   si.cb           = sizeof(si);
   si.dwFlags      = STARTF_USESHOWWINDOW;
   si.wShowWindow  = SW_HIDE;
   ZeroMemory(&processInfo, sizeof(processInfo));


   if (currentDirectory.ReverseFind('\\') == (currentDirectory.GetLength()-1))
   {
		currentDirectory = currentDirectory.Mid(0, (currentDirectory.GetLength()-1));
   }

   /*return */CreateProcess(0, pCmdLine, 0, 0, FALSE, 0, 0, currentDirectory, &si, &processInfo);
   WaitForSingleObject(processInfo.hProcess, waitTime);
   DWORD exitCode;
   if (GetExitCodeProcess(processInfo.hProcess, &exitCode))
   {
	   errorCode = exitCode;
		if (exitCode == STILL_ACTIVE)
		{
			MessageBox("For some reason Process Failed", "Error");
			TerminateProcess(processInfo.hProcess, exitCode);
			return false;
		}
   }   
   return true;
}

void TooieRandoDlg::OnClose()
{
	if (ROM != NULL)
	{
		delete [] ROM;
		ROM = NULL;
	}

	for (int x = 0; x < m_list.GetItemCount(); x++)
	{
		delete ((ListUpdateStruct*)m_list.GetItemData(x));
	}

	CDialog::OnClose();
}

unsigned char TooieRandoDlg::StringToUnsignedChar(CString inString)
{
	int tempA = inString.GetLength();
	if (inString.GetLength() < 2)
	{
		CString tempStr = inString;
		inString = "";
		for (int x = 0; x < (2-tempStr.GetLength()); x++)
		{
			inString = inString + "0";
		}
		inString = inString + tempStr;
	}
	char* b;
	b = inString.GetBuffer(0);
	unsigned long tempLong = 0;
	for (int x = 0;x < 2; x++)
	{
		char tempChar = b[x];
		int hexInt = HexToInt(tempChar);
		tempLong = tempLong | hexInt<<((1-x)*4);
	}
	return (unsigned char) tempLong;
}

void TooieRandoDlg::OnBnClickedButtonsearch()
{
	int start = m_list.GetSelectionMark();
	if (m_list.GetSelectionMark() == -1)
	{
		if (m_list.GetItemCount() > 0)
			start = 0;
		else
			return;
	}

	if (lastSearchSpot == start)
	{
		start = lastSearchSpot + 1;
		if (lastSearchSpot >= m_list.GetItemCount())
			start = 0;
	}

	CString searchStr;
	mSearch.GetWindowText(searchStr);

	int searchLength = searchStr.GetLength();
	if (((searchLength % 2) != 0) && (searchLength > 0))
	{
		MessageBox("Must be multiple of 2 and > 0");
		return;
	}

	searchLength = searchLength / 2;

	unsigned char* search = new unsigned char[searchLength];
	for (int x = 0; x < searchLength; x++)
	{
		search[x] = StringToUnsignedChar(searchStr.Mid((x * 2), 2));
	}

	
	for (int x = start; x < m_list.GetItemCount(); x++)
	{
		ListUpdateStruct* list = ((ListUpdateStruct*)m_list.GetItemData(x));
		int size = GetSizeFile(list->tempLocation);
		if (searchLength >= size)
			continue;

		FILE* inFile = fopen(list->tempLocation, "rb");
		unsigned char* buffer = new unsigned char[size];
		fread(buffer, 1, size, inFile);
		fclose(inFile);

		for (int y = 0; y < (size - searchLength); y++)
		{
			bool match = true;
			for (int z = y; z < (y + searchLength); z++)
			{
				if (search[z - y] != buffer[z])
				{
					match = false;
					break;
				}

			}
			if (match)
			{
				m_list.SetItemState(x, LVIS_SELECTED, LVIS_SELECTED);
				m_list.SetItemState(x, LVIS_FOCUSED, LVIS_FOCUSED);
				CString tempStr;
				mFile.SetWindowText(list->tempLocation);
				delete [] buffer;
				delete [] search;
				lastSearchSpot = x;
				return;
			}
		}
		delete [] buffer;
	}

	delete [] search;
}

void TooieRandoDlg::OnCbnSelchangeCombo1()
{
	CString tempStr;
	//m_gameselection.GetWindowText(tempStr);

	if (tempStr.Find("Banjo Tooie") != -1)
	{
		mCompressEncryptedButton.ShowWindow(SW_SHOW);
		mEncryptedFileNumber.ShowWindow(SW_SHOW);
		mFileNumberStatic.ShowWindow(SW_SHOW);
	}
	else
	{
		mCompressEncryptedButton.ShowWindow(SW_HIDE);
		mEncryptedFileNumber.ShowWindow(SW_HIDE);
		mFileNumberStatic.ShowWindow(SW_HIDE);
	}
}

unsigned short TooieRandoDlg::StringToUnsignedShort(CString inString)
{
	int tempA = inString.GetLength();
	if (inString.GetLength() < 4)
	{
		CString tempStr = inString;
		inString = "";
		for (int x = 0; x < (4-tempStr.GetLength()); x++)
		{
			inString = inString + "0";
		}
		inString = inString + tempStr;
	}
	char* b;
	b = inString.GetBuffer(0);
	unsigned long tempLong = 0;
	for (int x = 0;x < 4; x++)
	{
		char tempChar = b[x];
		int hexInt = HexToInt(tempChar);
		tempLong = tempLong | hexInt<<((3-x)*4);
	}
	return tempLong;
}


void TooieRandoDlg::OnBnClickedCompressfilebuttonencrypted()
{
	// TODO: Add your control notification handler code here
	CString fileOpen;
	CFileDialog m_ldFile(TRUE, NULL, "compressedfile.bin", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Bin File of Codes (*.*)|*.*|");
	int didRead = m_ldFile.DoModal();
	if ((didRead == IDCANCEL) || (m_ldFile.GetPathName() == ""))
		return;

	if (didRead == FALSE)
		return;

	CString gameNameStr;
	//m_gameselection.GetWindowText(gameNameStr);

	if (GetZLibGameName(gameNameStr) != -1)
	{
		GECompression compressed;
		compressed.SetPath(directory);
		compressed.SetGame(GetZLibGameName(gameNameStr));

		CFileDialog m_svFile(FALSE, "bin", (m_ldFile.GetFileName() + ".bin"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Bin File of Codes (*.*)|*.*|");
		didRead = m_svFile.DoModal();
		if ((didRead == IDCANCEL) || (m_svFile.GetPathName() == ""))
			return;

		if (didRead == FALSE)
			return;

		compressed.CompressGZipFile(m_ldFile.GetPathName(), m_svFile.GetPathName(), false);

		FILE* inFile = fopen(m_svFile.GetPathName(), "rb");
		if (inFile == NULL)
		{
			MessageBox("Cannot open BT file", "Error");
			return;
		}

		unsigned long outSize = GetSizeFile(m_svFile.GetPathName());
		if (outSize > 0)
		{
			unsigned char* tempInputBuffer = new unsigned char[outSize];
			fread(tempInputBuffer, 1, outSize, inFile);

			fclose(inFile);

			CString tempStr;
			mEncryptedFileNumber.GetWindowText(tempStr);
			int fileNumber = StringToUnsignedShort(tempStr);

			DecryptBTFile(fileNumber, tempInputBuffer, tempInputBuffer, outSize);

			FILE* outFile = fopen(m_svFile.GetPathName(), "wb");
			if (outFile == NULL)
			{
				MessageBox("Cannot open encrypted BT file", "Error");
				return;
			}
			fwrite(tempInputBuffer, 1, outSize, outFile);
			fclose(outFile);

			delete [] tempInputBuffer;
		}
	}	
}

void TooieRandoDlg::OnBnClickedApplyobjectchange()
{
	ReplaceObject();
	// TODO: Add your control notification handler code here
}

void TooieRandoDlg::OnCbnSelchangeOriginalobject()
{
	// TODO: Add your control notification handler code here
}

void TooieRandoDlg::OnCbnSelchangeReplaceobject()
{
	// TODO: Add your control notification handler code here
}
void TooieRandoDlg::ReplaceFileDataAtAddress(int address, CString filepath,int size, unsigned char* buffer)
{
    char message[256];

	long fileSize = GetSizeFile(filepath);
	FILE* inFile = fopen(filepath,"rb+");
	if (inFile == NULL)
	{
        sprintf(message, "Invalid File: %s\n", filepath.GetString());
		::MessageBox(NULL, message, "Error", NULL);
		return;
	}
	if (fileSize == 0)
	{
		fclose(inFile);
		return;
	}
	fseek(inFile,address,SEEK_SET);
	int readAmt = fwrite(buffer, 1, size, inFile);
	fclose(inFile);
	
	std::string dataOutput;
    for (size_t i = 0; i < size; ++i) {
        char byteStr[4];
        sprintf(byteStr, "%02X ", buffer[i]);
        dataOutput += byteStr;
    }
    sprintf(message, "Data written to file: %s $%s\n", dataOutput.c_str() , filepath.GetString());
	OutputDebugString(_T(message));
}

void TooieRandoDlg::GetFileDataAtAddress(int address, CString filepath,int size, unsigned char* buffer)
{
	long fileSize = GetSizeFile(filepath);
	FILE* inFile = fopen(filepath,"rb");
	if (inFile == NULL)
	{
		::MessageBox(NULL, "Invalid File", "Error", NULL);
		return;
	}
	if (fileSize == 0)
	{
		fclose(inFile);
		return;
	}
	fseek(inFile,address,SEEK_SET);

	int readAmt = fread(buffer, 1, size, inFile);
	fclose(inFile);
	
	std::string dataOutput;
    for (size_t i = 0; i < size; ++i) {
        char byteStr[4];
        sprintf(byteStr, "%02X ", buffer[i]);
        dataOutput += byteStr;
    }
	char message[256];
    sprintf(message, "Data read from file: %s\n", dataOutput.c_str());
	//::MessageBox(NULL, message, "Rom", NULL); //Print out data at address

}

void TooieRandoDlg::ReplaceObject(int sourceIndex, int insertIndex)
{
	char message[256];
	sprintf(message, "Read address: 0x%s\n", m_list.GetItemText(RandomizedObjects[sourceIndex].fileIndex,4));
	//::MessageBox(NULL, message, "Rom", NULL); //Show file path for file with mapId
	std::string dataOutput;

	CString newFileLocation = m_list.GetItemText(RandomizedObjects[insertIndex].fileIndex,4);

	ReplaceFileDataAtAddress(RandomizedObjects[insertIndex].associatedOffset+6,newFileLocation,10,&(RandomizedObjects[sourceIndex].Data[0]));
	InjectFile(newFileLocation, RandomizedObjects[insertIndex].fileIndex);
}
void TooieRandoDlg::ReplaceObject()
{
	ReplaceObject(OriginalObjectList.GetCurSel(),NewObjectList.GetCurSel());
}

int TooieRandoDlg::FindItemInListCtrl(CListCtrl& listCtrl, const CString& searchText, int columnIndex) {
    int itemCount = listCtrl.GetItemCount();

    for (int i = 0; i < itemCount; ++i) {
        CString itemText = listCtrl.GetItemText(i, columnIndex); // Get text from the first column
        if (itemText.CompareNoCase(searchText) == 0) { // Compare case-insensitive
            return i; // Item found, return index
        }
    }
    return -1; // Item not found
}

void TooieRandoDlg::OnBnClickedButton5()
{
    //LoadMoves();
    LoadScriptEdits();
	OriginalObjectList.ResetContent();
	NewObjectList.ResetContent();
    LoadObjects();
}

int TooieRandoDlg::GetScriptIndex(char* scriptId)//Used for retreiving index of a script in the main table based on the position in the syscall table
{
	char* endPtr;
	int scriptAddress = strtol(scriptId, &endPtr, 16) + syscallTableStart;
	char hexString[9];
	snprintf(hexString, sizeof(hexString), "%08X", scriptAddress);
	int index = FindItemInListCtrl(m_list, hexString, 0); //Get the asset index for the script address
	return index;
}

void TooieRandoDlg::LoadObjects()
{
    RandomizedObjects.clear();
    RewardObjects.clear();
    std::ifstream myfile("RandomizerAddresses.txt");
    std::string line;
    try {
        myfile.open("RandomizerAddresses.txt");
        if (!myfile.is_open()) {
            throw std::runtime_error("Error: Could not open the file 'RandomizerAddresses.txt'.");
        }
    }
    catch (const std::exception& ex) {
        ::MessageBox(NULL, ex.what(), "Error", NULL);
        return;
    }
    char message[256];
    myfile.clear();
    myfile.seekg(0);

    if (myfile.peek() == std::ifstream::traits_type::eof()) {
        ::MessageBox(NULL, "Error: The file is empty.", "Error", NULL);
        return;
    }

    myfile.clear();
    myfile.seekg(0);
    bool isJustScript = false; //Whether the next line should be treated as a new object or another script to add
    while (std::getline(myfile, line)) // Read each line from the file
    {
        char* endPtr;
        char mapID[5] = { 0 };
        char Offset[11] = { 0 };
        int readOffset = 0;
        bool shouldRandomize = true;

        if (line[0] == '*')//Used if this line only contains a script associated with the previous object
        {
            isJustScript = true;
            char scriptId[9] = { 0 };
            strncpy(scriptId, line.c_str() + 1, 8);
			char* endPtr;
			int index = GetScriptIndex(scriptId); //Get the asset index for the script address
            if (index != -1)
            {
                RewardObjects.back().associatedScripts.push_back(index);
            }
            continue;
        }

        else if (line[0] == '/') //Used to disregard a line entirely
            continue;
        else if (line[0] == '?')//Used to indicate an object exists in script only
        {
            readOffset = 1;
            char scriptId[9] = { 0 };
            char ObjectId[5] = { 0 };
            char Flag[5] = { 0 };
            int offset = -1;
            strncpy(mapID, line.c_str()+readOffset, 4);//Use a map within the desired level
            readOffset += 5;
            strncpy(scriptId, line.c_str()+readOffset, 8);//Script to use
            readOffset += 9;
            strncpy(ObjectId, line.c_str() + readOffset, 4);//Use a map within the desired level
            readOffset += 5;
            strncpy(Flag, line.c_str()+readOffset, 4);//Use a map within the desired level
            readOffset += 5;
            int index = FindItemInListCtrl(m_list, mapID, 5); //Get the asset index for the mapID
            if (index == -1)
            {
                return;
            }
			int objectID = strtol(ObjectId, &endPtr, 16);
			int flag = strtol(Flag, &endPtr, 16);
			int shiftedFlag = flag << 23;
            std::vector<unsigned char> tempVector;
			tempVector.push_back(0x19);
			tempVector.push_back(0x0C);
			tempVector.push_back(objectID>>8);
			tempVector.push_back(objectID & 0xFF);
			tempVector.push_back(0x00);
			tempVector.push_back(0x00);
			tempVector.push_back(shiftedFlag>>24);
			tempVector.push_back(shiftedFlag>>16&0xFF);
			tempVector.push_back(shiftedFlag>>8&0xFF);
			tempVector.push_back(0x64);
            RandomizedObject thisObject = RandomizedObject(tempVector, index, offset);
            RandomizedObjects.push_back(thisObject);


            RewardObject reward = RewardObject(RandomizedObjects.size() - 1, objectID, flag);
			char* endPtr;
			index = GetScriptIndex(scriptId);
            if (index != -1)
            {
                char scriptRewardIndex[3] = { 0 }; //Which edits are associated with the object
                strncpy(scriptRewardIndex, line.c_str() + readOffset, 2);
                reward.itemIndex = strtol(scriptRewardIndex, &endPtr, 16);

                CString newFileLocation = m_list.GetItemText(index, 4);
                //sprintf(message, "Found Script %s %d\n", newFileLocation, reward.itemIndex);
                //::MessageBox(NULL, message, "Rom", NULL); //Print out data at address
                reward.associatedScripts.push_back(index);
            }
            else
                return;
            reward.hasFlag = GetReward(reward.itemType, reward.itemId) != 0x0;
            RewardObjects.push_back(reward);
            RandomizedObjects.back().rewardObjectIndex = RewardObjects.size() - 1;
            MapIDs.push_back(std::string(mapID));
            PlaceObjectsIntoLevelGroup(mapID);
            continue;
        }
        else if (line[0] == '!')//Used to indicate that a reward object is not to be randomized but still updated to handle changes to flags. non reward objects can just be commented out
        {
            readOffset = 1;
            shouldRandomize = false;
        }
        strncpy(mapID, line.c_str()+readOffset, 4);
        readOffset += 5;
        strncpy(Offset, line.c_str() + readOffset, 10);
        readOffset += 11;
        int index = FindItemInListCtrl(m_list, mapID, 5); //Get the asset index for the mapID
        if (index == -1)
            return;
        CString originalFileLocation = m_list.GetItemText(index, 4);
        int offset = strtol(Offset, &endPtr, 16);
        unsigned char buffer[10];
        GetFileDataAtAddress(offset + 6, originalFileLocation, 10, buffer);
        std::vector<unsigned char> tempVector;
        for (int i = 0; i < 10;i++)
        {
            tempVector.push_back(buffer[i]);
        }
        RandomizedObject thisObject = RandomizedObject(tempVector, index, offset);
        RandomizedObjects.push_back(thisObject);
        int objectID = (buffer[2] << 8) | buffer[3];
        if (CanBeReward(objectID))//Whether the object matches one of the potential reward Objects
        {
            //sprintf(message, "Data %s\n", buffer);
            //::MessageBox(NULL, message, "Rom", NULL); //Print out data at address

            int result = 0;
            for (int i = 0;i < 6;i++)
            {
                result = result << 8 | buffer[i + 4];
            }
            result = result >> 23;
            RewardObject reward = RewardObject(RandomizedObjects.size() - 1, objectID, result);
            sprintf(message, "Type %d Flag %d Has Flag %d\n", reward.itemType, reward.itemId, GetReward(reward.itemType, reward.itemId) != 0x0);
            //::MessageBox(NULL, message, "Rom", NULL); //Print out data at address
            reward.hasFlag = GetReward(reward.itemType, reward.itemId) != 0x0;
            reward.shouldRandomize = shouldRandomize;
            char scriptTableOffset[9] = { 0 };
            strncpy(scriptTableOffset, line.c_str()+readOffset, 8);
            readOffset += 9;
            sprintf(message, "Debug %s\n", scriptTableOffset);
			//OutputDebugString(_T(message));
			index = GetScriptIndex(scriptTableOffset);
            if (index != -1)
            {
                char scriptRewardIndex[3] = { 0 }; //Which edits are associated with the object
                strncpy(scriptRewardIndex, line.c_str() + readOffset, 2);
                readOffset += 3;
                reward.itemIndex = strtol(scriptRewardIndex, &endPtr, 16);

                CString newFileLocation = m_list.GetItemText(index, 4);
                sprintf(message, "Found Script %s %d\n", newFileLocation, reward.itemIndex);
				OutputDebugString(_T(message));                
				reward.associatedScripts.push_back(index);
            }
            RewardObjects.push_back(reward);
            RandomizedObjects.back().rewardObjectIndex = RewardObjects.size() - 1;
        }
        MapIDs.push_back(std::string(mapID));
        if (shouldRandomize)
            PlaceObjectsIntoLevelGroup(mapID);

        OriginalObjectList.AddString(line.c_str());
        NewObjectList.AddString(line.c_str());
    }
    myfile.close();
}

int TooieRandoDlg::PlaceObjectsIntoLevelGroup(char* mapID)
{
    char message[256];
    int found = 0;
    for (int levelIndex = 0;levelIndex < mapIDGroups.size();levelIndex++)//Group elements by level
    {
        if (std::find(mapIDGroups[levelIndex].begin(), mapIDGroups[levelIndex].end(), std::string(mapID)) != mapIDGroups[levelIndex].end())
        {
            levelObjects[levelIndex].push_back(RandomizedObjects.size() - 1);
            char message[256];
            sprintf(message, "Put data %d into group: %d\n", RandomizedObjects.size() - 1, levelIndex);
            //::MessageBox(NULL, message, "Rom", NULL); //Print out data at address
            found = 1;
            break;
        }
    }
    if (found == 0)
    {
        sprintf(message, "Could not find associated Level %s\n", mapID);
        ::MessageBox(NULL, message, "Rom", NULL); //Print out data at address
    }
    return found;
}

void TooieRandoDlg::RandomizeObjects()
{
	char message[256];

    int size = RandomizedObjects.size();
    std::vector<int> source, target;

    for (int i = 0; i < size; ++i) {
        source.push_back(i);
        target.push_back(i);
    }
    int rewardIndex = 1;


    for (int i = 0; i < size; ++i) {
        if (RandomizedObjects[i].rewardObjectIndex != -1 && !RewardObjects[RandomizedObjects[i].rewardObjectIndex].shouldRandomize)
        {
			sprintf(message, "Removed %d from targets and source\n", i);
			OutputDebugString(_T(message));
            if (RewardObjects[RandomizedObjects[i].rewardObjectIndex].hasFlag)
            {
                SetReward(RewardObjects[RandomizedObjects[i].rewardObjectIndex].itemType, RewardObjects[RandomizedObjects[i].rewardObjectIndex].itemId, rewardIndex);
                rewardIndex++;
            }
            auto sourceit = std::find(source.begin(), source.end(), i);
            auto targetit = std::find(target.begin(), target.end(), i);
            source.erase(sourceit);
            target.erase(targetit);
            continue;
        }
        auto targetit = std::find(target.begin(), target.end(), i);
        if (targetit == target.end()) //Check if this object has already been randomized
            continue;

        sprintf(message, "RewardObjectIndex %d\n", RandomizedObjects[i].rewardObjectIndex);
        //::MessageBox(NULL, message, "Rom", NULL); //Print out data at address
        bool alreadyRandomized = false;

        if (RandomizedObjects[i].rewardObjectIndex != -1 && RewardObjects[RandomizedObjects[i].rewardObjectIndex].associatedScripts.size()!=0) //Replace reward objects with ones that can be spawned
        {
			


            std::mt19937                        generator(seed);
            std::uniform_int_distribution<int>  distr(0, RewardObjects.size() - 1);
            for (int find = 0; find < RewardObjects.size();find++)
            {
                int replacementIndex = (distr(generator) + find) % RewardObjects.size();
                auto newSourceit = std::find(source.begin(), source.end(), RewardObjects[replacementIndex].associatedObjectIndex);
                if (newSourceit != source.end())
                {
                    if(RandomizedObjects[i].associatedOffset!=-1)
						ReplaceObject(RewardObjects[replacementIndex].associatedObjectIndex, i);
                    if (RewardObjects[replacementIndex].objectID != 0x4E6)
                    {
						if (RewardObjects[RandomizedObjects[i].rewardObjectIndex].hasFlag)
						{
							SetReward(RewardObjects[replacementIndex].itemType, RewardObjects[replacementIndex].itemId, rewardIndex);
							rewardIndex++;
						}
                        SetRewardScript(RandomizedObjects[i].rewardObjectIndex, RewardObjects[replacementIndex].itemType, RewardObjects[replacementIndex].itemId, RewardObjects[replacementIndex].objectID);
                    }
                    else
                    {
                        if (RewardObjects[RandomizedObjects[i].rewardObjectIndex].hasFlag)
                            SetReward(RewardObjects[replacementIndex].itemType, RewardObjects[replacementIndex].itemId, RewardObjects[replacementIndex].itemId);
                        SetRewardScript(RandomizedObjects[i].rewardObjectIndex, RewardObjects[replacementIndex].itemType, RewardObjects[replacementIndex].itemId, RewardObjects[replacementIndex].objectID);
                    }
                    sprintf(message, "Removed %d from replacement Removed %d from source\n", i, source[newSourceit - source.begin()]);
                    OutputDebugString(_T(message));
                    source.erase(newSourceit);
                    auto replacementit = std::find(target.begin(), target.end(), i);
                    sprintf(message, "Original Reward Item Type %x Flag %x Value %d\n", RewardObjects[RandomizedObjects[i].rewardObjectIndex].itemType, RewardObjects[RandomizedObjects[i].rewardObjectIndex].itemId, rewardIndex);
                    OutputDebugString(_T(message));
                    sprintf(message, "Replaced Reward Item Type %x Flag %x Value %d\n", RewardObjects[replacementIndex].itemType, RewardObjects[replacementIndex].itemId, rewardIndex);
                    OutputDebugString(_T(message));
                    target.erase(replacementit);
                    alreadyRandomized = true;
                    break;
                }
            }
            if (alreadyRandomized == true)
                continue;
        }
    }
	
    for (int i = 0; i < size; ++i) {
        auto sourceit = std::find(source.begin(), source.end(), i);

        if (sourceit == source.end()) //Check if this object has already been randomized
            continue;

        std::string dataOutput = "";
        char message[256];
        if(RandomizedObjects[i].Data.size()>0)
        for (size_t j = 0; j < 10; ++j) {
            char byteStr[4];

            sprintf(byteStr, "%02X", RandomizedObjects[i].Data[j]);
            dataOutput += byteStr;

        }

        bool alreadyRandomized = false;
        //Id/String combos in this vector are kept in their original level
        vector<std::string> NoteLabels{ "218C01D8","1A0C01D7","1A8C01D7","198C01D7","1B0C01D7","1B8C01D7","1C0C01D7","1C8C01D7","1D0C01D7","1D8C01D7","1E0C01D7","1E8C01D7","1F0C01D7","1F8C01D7","200C01D7","208C01D7","210C01D7" };
        
        for (int j = 0; j < NoteLabels.size();j++)
        {
            if (dataOutput.find(NoteLabels[j].c_str()) != std::string::npos)//Check if the current data ouput being read is a note 
            {
                char message[256];
                sprintf(message, "Note Type Found %s", NoteLabels[j].c_str());
                //::MessageBox(NULL, message, "Rom", NULL); //Print out data at address

                for (int levelIndex = 0;levelIndex < mapIDGroups.size();levelIndex++) //find the level associated with the map id that the note has
                {
                    if (std::find(mapIDGroups[levelIndex].begin(), mapIDGroups[levelIndex].end(), MapIDs[i]) != mapIDGroups[levelIndex].end())//Check through each level and see if the map is within one of these
                    {
                        std::mt19937                        generator(seed);
                        std::uniform_int_distribution<int>  distr(0, levelObjects[levelIndex].size()-1);
                        for (int find = 0; find < levelObjects[levelIndex].size();find++)
                        {
                            int replacementIndex = (distr(generator) + find) % levelObjects[levelIndex].size();
                            auto replacementit = std::find(target.begin(), target.end(), levelObjects[levelIndex][replacementIndex]);
                            if (replacementit != target.end())
                            {
                                
                                ReplaceObject(i, levelObjects[levelIndex][replacementIndex]);

                                sprintf(message, "Removed %d from source Removed %d from replacement\n", i, target[replacementit - target.begin()]);
                                OutputDebugString(_T(message));
                                source.erase(sourceit);
                                target.erase(replacementit);
                                alreadyRandomized = true;
                                break;
                            }
                        }
                        if (alreadyRandomized == true)
                            break;
                    }

                }
                sprintf(message, "Found Notes In %s Map %s\n", dataOutput.c_str(), MapIDs[i].c_str());
                //::MessageBox(NULL, message, "Rom", NULL); //Print out data at address
            }
            if (alreadyRandomized)
                break;
        }
        if (alreadyRandomized)
            continue;
        
    }

    
    if (source.size() == target.size())
    {

        for (int i = 0; i < source.size(); ++i) {
            char message[256];
            sprintf(message, "Target: %d Source: %d\n", target[i], source[i]);
            OutputDebugString(_T(message));
        }


        std::shuffle(source.begin(), source.end(), default_random_engine(seed));
        std::shuffle(target.begin(), target.end(),default_random_engine(seed));
        for (int i = 0; i < source.size(); ++i) {
            char message[256];
            sprintf(message, "Replace %d with %d", target[i], source[i]);
            //::MessageBox(NULL, message, "Rom", NULL); //Print out data at address
            ReplaceObject(source[i], target[i]);
        }
    }
    else
    {
        char message[256];
        sprintf(message, "Source and Replacement uneven sized");
       ::MessageBox(NULL, message, "Rom", NULL); //Print out data at address
    }
}

void TooieRandoDlg::OnBnClickedButton4()
{
    ClearRewards();
    RandomizeMoves();
    RandomizeObjects();
}

void TooieRandoDlg::LoadMoves()
{
    SiloObjects.clear();
    std::vector<CString> SiloFiles{"01E8A004","01E8A008","01E8A058","01E8A0BC","01E8A0C0","01E8A0EC","01E8A0F0","01E8A0F4","01E8A100" }; //The adresses from the file table that are associated with the Silo scripts

    std::ifstream myfile("SiloRandomizerAddresses.txt");
    std::string line;
    try {
        myfile.open("SiloRandomizerAddresses.txt");
        if (!myfile.is_open()) {
            throw std::runtime_error("Error: Could not open the file 'SiloRandomizerAddresses.txt'.");
        }
    }
    catch (const std::exception& ex) {
        ::MessageBox(NULL, ex.what(), "Error", NULL);
        return;
    }
    char message[256];
    myfile.clear();
    myfile.seekg(0);

    if (myfile.peek() == std::ifstream::traits_type::eof()) {
        ::MessageBox(NULL, "Error: The file is empty.", "Error", NULL);
        return;
    }

    myfile.clear();
    myfile.seekg(0);
    while (std::getline(myfile, line)) // Read each line from the file
    {
        if (line[0] == '/')
            continue;
		char* endPtr;
        char scriptId[9] = { 0 };
        char Offset[4] = { 0 };
        strncpy(scriptId, line.c_str(), 8);
        strncpy(Offset, line.c_str() + 9, 3);
        int index = GetScriptIndex(scriptId); //Get the asset index for the script address
        if (index == -1)
            return;
        CString originalFileLocation = m_list.GetItemText(index, 4);
        int offset = strtol(Offset, &endPtr, 16);
        unsigned char buffer[0x10];
        GetFileDataAtAddress(offset, originalFileLocation, 0x10, buffer);
        std::vector<unsigned char> tempVector;
        for (int i = 0; i < 0x10;i++)
        {
            tempVector.push_back(buffer[i]);
        }

        sprintf(message, "Value from Move: %s %s\n", scriptId, Offset);
        //MessageBox(message);
        SiloObject siloObject = SiloObject(tempVector, index, offset);
        SiloObjects.push_back(siloObject);
        //SiloData.push_back(tempVector);

        //std::vector<int> offsetVector({ index, offset });
        //SiloOffset.push_back(offsetVector);
    }
    myfile.close();
}

void TooieRandoDlg::LoadScriptEdits()
{
    ScriptEdits.clear();
    std::ifstream myfile("RewardScriptEditAddresses.txt");
    std::string line;
    try {
        myfile.open("RewardScriptEditAddresses.txt");
        if (!myfile.is_open()) {
            throw std::runtime_error("Error: Could not open the file 'RewardScriptEditAddresses.txt'.");
        }
    }
    catch (const std::exception& ex) {
        ::MessageBox(NULL, ex.what(), "Error", NULL);
        return;
    }
    char message[256];
    myfile.clear();
    myfile.seekg(0);

    if (myfile.peek() == std::ifstream::traits_type::eof()) {
        ::MessageBox(NULL, "Error: The file is empty.", "Error", NULL);
        return;
    }

    myfile.clear();
    myfile.seekg(0);
    while (std::getline(myfile, line)) // Read each line from the file
    {
        if (line[0] == '/')
            continue;
        char scriptId[9] = { 0 };
        char Offset[5] = { 0 };
        char EditType[3] = { 0 };
        char RewardIndex[3] = { 0 };
		int readOffset = 0;
		strncpy(scriptId, line.c_str() + readOffset, 8);
		readOffset += 9;
        strncpy(Offset, line.c_str() + readOffset, 4);
		readOffset += 5;
        strncpy(EditType, line.c_str() + readOffset, 2);
		readOffset += 3;
        strncpy(RewardIndex, line.c_str() + readOffset, 2);
		readOffset += 3;
		char* endPtr;
        int index = GetScriptIndex(scriptId); //Get the asset index for the script address
        if (index == -1)
            return;
        int offset = strtol(Offset, &endPtr, 16);
        int editType = strtol(EditType, &endPtr, 16);
        int rewardIndex = strtol(RewardIndex, &endPtr, 16);

        sprintf(message, "Script Edit: %s %s %s\n", scriptId, Offset, EditType);
        //MessageBox(message);
        ScriptEdit scriptEdit = ScriptEdit(index, editType, offset, rewardIndex);
        ScriptEdits.push_back(scriptEdit);

    }
    myfile.close();
}

void TooieRandoDlg::RandomizeMove(int source, int target)
{
        //CString newFileLocation = m_list.GetItemText(SiloOffset[target][0], 4);
        CString newFileLocation = m_list.GetItemText(SiloObjects[target].fileIndex, 4);
        std::vector<unsigned char> MainData;
        std::vector<unsigned char> TitleData;

        for (int i = 0;i < 0x10;i++)
        {
            if (i >= 4 && i < 12)
                MainData.push_back(SiloObjects[source].Data[i]);
                //MainData.push_back(SiloData[source][i]);
            if(i>=14)
                TitleData.push_back(SiloObjects[source].Data[i]);
                //TitleData.push_back(SiloData[source][i]);

        }
        ReplaceFileDataAtAddress(SiloObjects[target].associatedOffset+4, newFileLocation, 0x8, &(MainData[0]));
        ReplaceFileDataAtAddress(SiloObjects[target].associatedOffset+14, newFileLocation, 0x2, &(TitleData[0]));

        InjectFile(newFileLocation, SiloObjects[target].fileIndex);
}

void TooieRandoDlg::RandomizeMoves()
{
    std::vector<int> source, replacement;
    for (int i = 0; i < SiloObjects.size(); ++i) {
        source.push_back(i);
        replacement.push_back(i);
    }
    std::random_shuffle(source.begin(), source.end());
    std::random_shuffle(replacement.begin(), replacement.end());

    for (int i = 0; i < SiloObjects.size(); ++i) {
        RandomizeMove(source[i], replacement[i]);
    }
}

int TooieRandoDlg::FindRewardFlagOffset(int itemType, int itemFlag)
{
    int offset = 0;
    switch (itemType)
    {

    case 0: //Jinjo
        offset = itemFlag << 2;
            offset -= itemFlag;
            offset += 0x2EDF;
        break;

    case 1: //Jiggy
        offset = itemFlag << 1;
        offset += 0x2F67;
        break;

    case 2: //Honeycomb
        offset = itemFlag << 1;
        offset += 0x301B;
        break;

    case 3: //Glowbo
        offset = itemFlag << 1;
        offset += 0x304F;
        break;

    case 7: //Doubloon
        offset = itemFlag + 0x31DB;
        break;

    case 8: //Ticket
        offset = itemFlag +0x31FB;
        break;

    case 4: //Cheato
        offset = itemFlag << 1;
        offset += 0x3073;
        break;

    default:
        break;
    }
    return offset;

}
int TooieRandoDlg::GetReward(int itemType, int itemFlag)
{
    int index = FindItemInListCtrl(m_list, "01E86C76", 0);
    CString newFileLocation = m_list.GetItemText(index, 4);
    unsigned char* buffer;
    buffer = new unsigned char[1];
    char message[256];
    GetFileDataAtAddress(FindRewardFlagOffset(itemType, itemFlag), newFileLocation, 0x1, buffer);
    sprintf_s(message, "Found data 0x%02X at %s\n", buffer, newFileLocation);
	OutputDebugString(_T(message));
	int result = int(buffer[0]);
	delete[] buffer;
	return result;
}

void TooieRandoDlg::SetReward(int itemType, int itemFlag, int value)
{
    int index = FindItemInListCtrl(m_list, "01E86C76", 0);
    CString newFileLocation = m_list.GetItemText(index, 4);
    std::vector<unsigned char> buffer;
    buffer.push_back((unsigned char)value);
	char message[256];
	sprintf(message, "Set Reward %X %X to %X at %X %c \n", itemType,itemFlag,value, FindRewardFlagOffset(itemType, itemFlag), (unsigned char)value);
	OutputDebugString(_T(message));

    ReplaceFileDataAtAddress(FindRewardFlagOffset(itemType, itemFlag), newFileLocation, 0x1, &buffer[0]);
    InjectFile(newFileLocation, index);
}

void TooieRandoDlg::SetRewardScript(int reward,int itemType, int itemFlag, int objectID)
{
    for (int j = 0;j < RewardObjects[reward].associatedScripts.size();j++)
    {
        CString newFileLocation = m_list.GetItemText(RewardObjects[reward].associatedScripts[j], 4);
		char message[256];
		sprintf(message, "Script %s\n", newFileLocation);
		OutputDebugString(_T(message));
        for (int i = 0; i < ScriptEdits.size();i++)
        {
            char message[256];
            sprintf(message, "Script Edit Type: %d\n", ScriptEdits[i].editType);
            //::MessageBox(NULL, message, "Error", NULL);

            if (ScriptEdits[i].scriptIndex == RewardObjects[reward].associatedScripts[j] && RewardObjects[reward].itemIndex == ScriptEdits[i].rewardIndex)
            {
                std::vector<unsigned char> buffer;
                if (ScriptEdits[i].editType == 0x1)
                {
                    buffer.push_back((unsigned char)itemType);
                    ReplaceFileDataAtAddress(ScriptEdits[i].associatedOffset, newFileLocation, 0x1, &buffer[0]);
                }
                else if (ScriptEdits[i].editType == 0x2)
                {
                    buffer.push_back((unsigned char)itemFlag);
                    ReplaceFileDataAtAddress(ScriptEdits[i].associatedOffset, newFileLocation, 0x1, &buffer[0]);
                }
                else if (ScriptEdits[i].editType == 0x3)
                {
                    sprintf(message, "Script Edit 3: %d\n", ScriptEdits[i].editType);
                    //::MessageBox(NULL, message, "Error", NULL);

                    buffer.push_back((unsigned char)((objectID >> 8) & 0xFF)); // Push the high byte (0x01)
                    buffer.push_back((unsigned char)(objectID & 0xFF));
                    ReplaceFileDataAtAddress(ScriptEdits[i].associatedOffset, newFileLocation, 0x2, &buffer[0]);
                }
            }
        }
        InjectFile(newFileLocation, RewardObjects[reward].associatedScripts[j]);
    }
}

void TooieRandoDlg::ClearReward(int itemType, int itemFlag)
{
    SetReward(itemType, itemFlag, 0);
}
void TooieRandoDlg::ClearRewards()
{
    for (int i = 0;i < RewardObjects.size();i++)
    {
        ClearReward(RewardObjects[i].itemType, RewardObjects[i].itemId);
    }
}

bool TooieRandoDlg::CanBeReward(int itemType)
{
    switch (itemType)
    {
    case 0x1f5: //Jinjo
        break;
    case 0x1f6: //Jiggy
        break;
    case 0x1f7: //Honeycomb
        break;
    case 0x1f8: //Glowbo
        break;
    case 0x4E6: //Ticket
        break;
    case 0x29D: //Doubloon
        break;
    case 0x201: //Cheato
        break;
    default:
        return false;
        break;
    }
    return true;
}


void TooieRandoDlg::OnEnChangeSeedEntry()
{
	CString inputText;
    SeedEntry.GetWindowText(inputText);
    std::hash<std::string> hash_fn;
	size_t hash_value = hash_fn(inputText.GetString());
	seed = static_cast<int>(hash_value);
}

void TooieRandoDlg::OnDblclkListdecompressedfiles(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CString newFileLocation = m_list.GetItemText(pNMItemActivate->iItem, 4);
	ShellExecute(NULL, "open", newFileLocation, NULL, NULL, SW_SHOWNORMAL);
	*pResult = 0;
}


void TooieRandoDlg::OnRclickListdecompressedfiles(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	//*pResult = 0;
	this->SetForegroundWindow();    // Bring the first dialog to the front
	char* endPtr;
	int startAddress = strtol(m_list.GetItemText(pNMItemActivate->iItem, 0), &endPtr,16);
	CChangeLength cChangeLengthDialog(this,startAddress);
	cChangeLengthDialog.DoModal();
}

void TooieRandoDlg::OnBnClickedDecompressgame2()
{
	KillDecompressGameThread();
	// TODO: Add your control notification handler code here
	CString editedRom = "output\\edited_rom.rom";
	CString fileOpen;
	CFileDialog m_ldFile(TRUE, NULL, "Game.rom", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "ROM (*.v64;*.z64;*.rom;*.bin;*.zlb)|*.v64;*.z64;*.rom;*.bin;*.zlb|", this);
	int didRead = m_ldFile.DoModal();
	if ((didRead == IDCANCEL) || (m_ldFile.GetPathName() == ""))
		return;

	if (didRead == FALSE)
		return;
	char buffer[MAX_PATH];        // Buffer for the directory path

	GetCurrentDirectory(MAX_PATH, buffer);
	CString currentDirectory = buffer;

	fileCount = 0;
	CString command;
	command.Format("/c xdelta\\xdelta.exe -f -d -s \"%s\" \"patch\\randomizer_edits.xdelta\" \"%s\"", m_ldFile.GetPathName(), editedRom);
	hiddenExec(_T(command.GetBuffer()), currentDirectory);

	ShellExecute(0, "open", "cmd", command, 0, SW_SHOW);
	OutputDebugString(_T(command));

	for (int x = 0; x < m_list.GetItemCount(); x++)
	{
		delete ((ListUpdateStruct*)m_list.GetItemData(x));
	}

	m_list.DeleteAllItems();
	for (int i = 5; i >= 0; i--)
		m_list.DeleteColumn(i);
	m_list.InsertColumn(0, "Address", LVCFMT_LEFT, 200);
	m_list.InsertColumn(1, "Uncompressed Size", LVCFMT_LEFT, 100);
	m_list.InsertColumn(2, "Compressed Size", LVCFMT_LEFT, 100);
	m_list.InsertColumn(3, "Type", LVCFMT_LEFT, 60);
	m_list.InsertColumn(4, "Path", LVCFMT_LEFT, 300);
	m_list.InsertColumn(5, "Internal Name", LVCFMT_LEFT, 100);


	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	CString gameNameStr = "Banjo-Tooie";

	CString filePathString = buffer;
	filePathString.Append("\\"+editedRom);

	this->strROMPath = filePathString;
	this->gameNameStr = gameNameStr;
	this->genText = m_genTextFiles.GetCheck();

	int size = GetSizeFile(this->strROMPath);
	m_progressBar.SetRange(0, 100);
	m_progressBar.SetPos(0);
	m_progressBar.ShowWindow(SW_SHOW);
	killThread = false;
	m_cancelLoad.ShowWindow(SW_SHOW);
	lastSearchSpot = -1;
	decompressGamethread = AfxBeginThread(&TooieRandoDlg::DecompressGameThread, this);

	// Get the thread handle (use CWinThread's m_hThread)
	HANDLE hThread = decompressGamethread->m_hThread;

	// Wait for the thread to finish (with a timeout or INFINITE)
	//WaitForSingleObject(hThread, INFINITE);

	mDecompressFileButton.ShowWindow(SW_SHOW);


	int zlibGame = GetZLibGameName(gameNameStr);
	mCompressFileButton.ShowWindow(SW_SHOW);
	m_injectButton.ShowWindow(SW_SHOW);
	m_saveROMButton.ShowWindow(SW_SHOW);

	
}

LRESULT TooieRandoDlg::OnThreadComplete(WPARAM wParam, LPARAM lParam)
{
	OnBnClickedButton5();
	OnBnClickedButton4();
	AfxMessageBox(_T("Randomization Complete!"));
	OnBnClickedButtonsaverom();
	return 0;
}
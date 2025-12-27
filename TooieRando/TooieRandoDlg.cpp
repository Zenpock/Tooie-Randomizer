// GEDecompressorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TooieRando.h"
#include "LogicCreator.h"
#include "LogicTracker.h"

#include "TooieRandoDlg.h"
#include ".\TooieRandodlg.h"

#include "Props.h"

#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <istream>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include "CChangeLength.h"

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




// CGEDecompressorDlg dialog
bool Randomize = false; //Used to determine whether the decompression should trigger an event after it finishes
std::map<CString, std::pair<int,CString>> files; //ID, file path
default_random_engine generator;
CChangeLength* m_pChangeLength;  // Pointer to the dialog for extending allocated asset
std::vector<OptionData> OptionObjects; //Stores the object data for options
int selectedOption = -1;
std::vector<RewardObject> RewardObjects; //Stores the object indexes that are originally reward objects
std::vector<ScriptEdit> ScriptEdits; //The edits to make to reward object spawning scripts
std::vector<Entrance> Entrances; //The Entrances/warps that exist around the map
int seed = 0;
std::vector< std::vector<int>> levelObjects(9); //Contains the indices from ObjectData which objects are in what level with storage being [LevelIndex][]
bool TooieRandoDlg::genText = false;
typedef std::vector<int> MapIDGroup;

MapIDGroup IOH = {0x0AA4,0x0AA5,0x0AA6,0x0AA7,0x0AA8,0x0AA9,0x0AAA,0x0AAB,0x0AAC,0x0AAF,0x0AB0,0x0AB1,0x0A96,0x0AC8,0x0A97,0x0A98,0x0A99,0x0A9A};
MapIDGroup MT = {0x0A0B,0x0A0C,0x0A0D,0x0A0E,0x0A0F,0x0A10,0x0A11,0x0A19,0x0A1A,0x0A1B,0x0A1D,0x0A1E,0x0ACC,0x0ACD,0x0ACE,0x0ACF,0x0A03,0x0A04}; //SM counts as MT
MapIDGroup GGM = {0x0A1C,0x0A1F,0x0A20,0x0A21,0x0A22,0x0A23,0x0A24,0x0A25,0x0A26,0x0A27,0x0A28,0x0A29,0x0A2C,0x0A2D,0x0A2E,0x0A2F,0x0A30,0x0A31,0x0A3E,0x0A3,0x0A7B,0x0AB8,0x0AB9,0x0AC6};
MapIDGroup HFP = {0x0A7C,0x0A7D,0x0A7E,0x0A7F,0x0A80,0x0A81,0x0A82,0x0A83,0x0A84,0x0A85,0x0A86,0x0A87,0x0A88,0x0A89,0x0A8A};
MapIDGroup TDL = {0x0A67,0x0A68,0x0A69,0x0A6A,0x0A6B,0x0A6C,0x0A6D,0x0A6E,0x0A6F,0x0A70,0x0A73,0x0A77,0x0A78};
MapIDGroup CCL = {0x0A8B,0x0A8C,0x0A8D,0x0A8E,0x0A8F,0x0A92,0x0A93,0x0A94,0x0A95};
MapIDGroup GI = {0x0A55,0x0A56,0x0A57,0x0A58,0x0A59,0x0A5A,0x0A5B,0x0A5C,0x0A5D,0x0A5E,0x0A5F,0x0A60,0x0A61,0x0A62,0x0A63,0x0A64,0x0A65,0x0A66,0x0A74,0x0A7A,0x0AB7,0x0AC7,0x0ADC};
MapIDGroup WW = {0x0A2A,0x0A2B,0x0A32,0x0A33,0x0A34,0x0A35,0x0A36,0x0A37,0x0A38,0x0A39,0x0A3A,0x0A3B,0x0A3C,0x0A3F,0x0A40,0x0A41,0x0A4E,0x0A79,0x0ACB};
MapIDGroup JRL = {0x0AFB,0x0A42,0x0A43,0x0A44,0x0A46,0x0A49,0x0A4B,0x0A4C,0x0A4D,0x0A4F,0x0A51,0x0A54,0x0AD6,0x0A75,0x0AFC,0x0AFD,0x0AFE};
std::vector<MapIDGroup> mapIDGroups{IOH,MT,GGM,HFP,TDL,CCL,GI,WW,JRL};

vector<std::string> Notes{ "218C01D8","1A0C01D7","1A8C01D7","198C01D7","1B0C01D7","1B8C01D7","1C0C01D7","1C8C01D7","1D0C01D7","1D8C01D7","1E0C01D7","1E8C01D7","1F0C01D7","1F8C01D7","200C01D7","208C01D7","210C01D7" };

TooieRandoDlg::TooieRandoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TooieRandoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void TooieRandoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEVMODE, m_devMode);
	DDX_Control(pDX, IDC_LISTDECOMPRESSEDFILES, m_list);
	DDX_Control(pDX, IDC_OPTION_LIST, option_list);
	DDX_Control(pDX, IDC_PROGRESS, m_progressBar);
	DDX_Control(pDX, IDC_BUTTONCANCELLOAD, m_cancelLoad);
	DDX_Control(pDX, IDC_BUTTON3, m_injectButton);
	DDX_Control(pDX, IDC_BUTTONSAVEROM, m_saveROMButton);
	DDX_Control(pDX, IDC_COMPRESSFILEBUTTONENCRYPTED, mCompressEncryptedButton);
	DDX_Control(pDX, IDC_EDITENCRYPTED, mEncryptedFileNumber);
	DDX_Control(pDX, IDC_FILENUMBERLABEL, mFileNumberStatic);
	DDX_Control(pDX, IDC_COMPRESSFILEBUTTON, mCompressFileButton);
	DDX_Control(pDX, IDC_BUTTON1, mDecompressFileButton);
	DDX_Control(pDX, IDC_DECOMPRESSGAME, m_loadEditedRomButton);
    DDX_Control(pDX, IDC_SEED_ENTRY, SeedEntry);
	DDX_Control(pDX, IDC_VARIABLE_EDIT, VariableEdit);
	DDX_Control(pDX, IDC_SELECT_LIST, SelectionList);
	DDX_Control(pDX, IDC_SELECT_ADD, SelectionListAdd);
	DDX_Control(pDX, IDC_SELECT_REMOVE, SelectionListRemove);
	DDX_Control(pDX, IDC_LOGICSELECTOR, LogicSelector);
	DDX_Control(pDX, IDC_LOGIC_EDITOR_BUTTON, m_logicEditorButton);
	DDX_Control(pDX, IDC_LOGIC_CHECK, m_logicCheckButton);
	DDX_Control(pDX, IDC_BUTTON4, m_reRandomizeButton);

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
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(NM_RCLICK, IDC_LISTDECOMPRESSEDFILES, &TooieRandoDlg::OnRclickListdecompressedfiles)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTDECOMPRESSEDFILES, &TooieRandoDlg::OnDblclkListdecompressedfiles)
	ON_EN_CHANGE(IDC_SEED_ENTRY, &TooieRandoDlg::OnEnChangeSeedEntry)
    ON_BN_CLICKED(IDC_BUTTON4, &TooieRandoDlg::RandomizeElements)
	ON_BN_CLICKED(IDC_DECOMPRESSGAME2, &TooieRandoDlg::OnBnClickedDecompressgame2)
	ON_NOTIFY(NM_DBLCLK, IDC_OPTION_LIST, &TooieRandoDlg::OnDblclkOptionList)
	ON_EN_CHANGE(IDC_VARIABLE_EDIT, &TooieRandoDlg::OnEnChangeVariableEdit)
	ON_BN_CLICKED(IDC_SELECT_ADD, &TooieRandoDlg::OnBnClickedSelectAdd)
	ON_BN_CLICKED(IDC_SELECT_REMOVE, &TooieRandoDlg::OnBnClickedSelectRemove)
	ON_BN_CLICKED(IDC_LOGIC_EDITOR_BUTTON, &TooieRandoDlg::OnBnClickedLogicEditorButton)
	ON_BN_CLICKED(IDC_LOGIC_CHECK, &TooieRandoDlg::OnBnClickedLogicCheck)
	ON_BN_CLICKED(IDC_DEVMODE, &TooieRandoDlg::OnClickedDevmode)
	ON_BN_CLICKED(IDC_LOGIC_TRACKER_BUTTON, &TooieRandoDlg::OnBnClickedLogicTrackerButton)
	ON_BN_CLICKED(IDC_EXPORT_SETTINGS_BUTTON, &TooieRandoDlg::OnBnClickedExportSettingsButton)
	ON_BN_CLICKED(IDC_IMPORT_SETTINGS_BUTTON, &TooieRandoDlg::OnBnClickedImportSettingsButton)
	ON_NOTIFY(NM_CLICK, IDC_OPTION_LIST, &TooieRandoDlg::OnItemclickOptionList)
	ON_COMMAND(IDOK, &TooieRandoDlg::OnIdok)
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

	//Setup Option List
	option_list.InsertColumn(0, "Active", LVCFMT_LEFT, 70);
	option_list.InsertColumn(1, "Options", LVCFMT_LEFT, 200);
	option_list.InsertColumn(2, "Variables", LVCFMT_LEFT, 200);
	//option_list.InsertColumn(3, "IndexData", LVCFMT_LEFT, 70);

	
	LoadOptions("RandomizerOptions.txt");
	srand(time(NULL));
	seed = std::rand();
	CString seedStr;
	seedStr.Format("%d", seed);
	SeedEntry.SetWindowTextA(seedStr);
	option_list.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	LoadLogicFileOptions();


	return TRUE;  // return TRUE  unless you set the focus to a control
}
/// <summary>
/// Checks if the option with the provided ID has been set as active
/// </summary>
/// <returns></returns>
bool TooieRandoDlg::CheckOptionActive(std::string lookupID)
{
	return OptionData::CheckOptionActive(lookupID,OptionObjects);
}
/// <summary>
/// Checks if the option with the provided ID has been set as active
/// </summary>
/// <returns></returns>
OptionData TooieRandoDlg::GetOption(std::string lookupID)
{
	return OptionData::GetOption(lookupID,OptionObjects);
}

void TooieRandoDlg::AddOption(OptionData option)
{
	OptionObjects.push_back(option);
	if (option.hidden)
		return;
	LVITEM lv;
	lv.iItem = option_list.GetItemCount();
	lv.iSubItem = 0;
	lv.pszText = "optionName";
	lv.mask = LVIF_TEXT;
	int item = option_list.InsertItem(&lv);
	if (option.alwaysTrue)
	{
		option_list.SetItemText(item, 0, "Always");
	}
	else
	{
		if (option.active)
			option_list.SetItemText(item, 0, "Y");
		else
			option_list.SetItemText(item, 0, "N");
	}
	option_list.SetItemData(lv.iItem, OptionObjects.size()-1);
	option_list.SetItemText(item, 1, option.optionName);
	if (option.OptionType == "commands")
	{
		option_list.SetItemText(item, 2, option.customCommands.c_str());
	}
	else if (option.lookupId != "" && option.currentValue=="")
	{
		CString str;
		str.Format("%d", option.lookupId.c_str());
		option_list.SetItemText(item, 2, str);
	}
	else if(option.OptionType == "flags")
	{ 
		CString flagStr;
		for (int i = 0; i < option.flags.size(); i++)
		{
			CString tempFlagStr;
			tempFlagStr.Format("%X ", option.flags[i]);
			flagStr += tempFlagStr;
		}
		option_list.SetItemText(item, 2, flagStr);
	}
	else
	{
		option_list.SetItemText(item, 2, option.currentValue);
	}
	//option_list.SetItemText(item, 3, std::to_string(OptionObjects.size() - 1).c_str());
}

/// <summary>
/// According to the option object list setup the options according to the option type designated.
/// Places the majority of the setup for these options in the gcgame script
/// </summary>
void TooieRandoDlg::SetupOptions()
{
	char* endPtr;
	if (files.find("gcgame") == files.end())
	{
		return;
	}
	CString gameStartFileLocation = files["gcgame"].second;
	CreateTempFile(gameStartFileLocation);
	CString editableFile = TooieRandoDlg::GetTempFileString(gameStartFileLocation);
	char message[256];
	int commandsUsed = 0;
	std::vector<int> trueFlags;
	std::vector<int> falseFlags;

	for (int i = 0; i < OptionObjects.size(); i++)
	{
		for (int j = 0; j < OptionObjects[i].flags.size(); j++)
		{
			//TODO: Add something other than lookupid to check if we set the flags because I will probably want to be able to tell if a certain flag switch is done
			if (OptionObjects[i].randomizerExclusive)
				continue;
			if (OptionObjects[i].active)
			{
				trueFlags.push_back(OptionObjects[i].flags[j]);
			}
			else
			{
				//Unused for the moment
				falseFlags.push_back(OptionObjects[i].flags[j]);
			}
		}
	}
	
	//Setup The File edits needed for specific options
	for (int i = 0; i < OptionObjects.size(); i++)
	{	
		if (OptionObjects[i].randomizerExclusive||(OptionObjects[i].optionFileIndex.IsEmpty() && OptionObjects[i].OptionType != "commands"))
			continue;
		if (OptionObjects[i].active)
		{
			if (OptionObjects[i].OptionType == "commands")
			{
				std::vector<unsigned char> buffer;
				for (int j = 0; j < OptionObjects[i].customCommands.length(); j += 2)
				{
					char* endptr;
					buffer.push_back(strtol(CString(OptionObjects[i].customCommands[j]) + (OptionObjects[i].customCommands[j + 1]), &endptr, 16));
				}
				ReplaceFileDataAtAddress(0x1C0 + commandsUsed * 4, editableFile, OptionObjects[i].customCommands.length() / 2, &buffer[0]);
				commandsUsed += OptionObjects[i].customCommands.length() / 8;
			}
			else if (OptionObjects[i].OptionType == "value")
			{
				if (OptionObjects[i].optionFileIndex.IsEmpty()||files.find(OptionObjects[i].optionFileIndex) == files.end())
				{
					return;
				}
				CString originalFileLocation = files[OptionObjects[i].optionFileIndex].second;
				std::vector<unsigned char> buffer(1, 0);
				int value = OptionObjects[i].GetCurrentValueInt();
				WriteIntToBuffer(buffer.data(), 0, value, 1);
				char* endptr;
				int offset = strtol(OptionObjects[i].optionFileOffset, &endptr, 16);
				ReplaceFileDataAtAddress(offset, originalFileLocation, 1, &buffer[0]);
				InjectFile(originalFileLocation, files[OptionObjects[i].optionFileIndex].first);
			}
			else if (OptionObjects[i].OptionType == "mapedits")
			{
				if (files.find(OptionObjects[i].optionFileIndex) == files.end())
				{
					return;
				}
				CString originalFileLocation = files[OptionObjects[i].optionFileIndex].second;
				std::vector<unsigned char> buffer;

				if (OptionObjects[i].currentValue.GetLength() % 2 == 0)
				{
					for (int j = 0; j < OptionObjects[i].currentValue.GetLength(); j += 2)
					{
						CString byteString = OptionObjects[i].currentValue.Mid(j, 2); // Extract two characters
						int byte = std::strtol(byteString, nullptr, 16); // Convert hex to integer
						buffer.push_back(static_cast<unsigned char>(byte));
					}
				}

				char* endptr;
				int offset = strtol(OptionObjects[i].optionFileOffset, &endptr, 16);
				ReplaceFileDataAtAddress(offset, originalFileLocation, buffer.size(), &buffer[0]);
				InjectFile(originalFileLocation, files[OptionObjects[i].optionFileIndex].first);
			}
		}
		else
		{
			if (OptionObjects[i].OptionType == "value")
			{
				if (files.find(OptionObjects[i].optionFileIndex) == files.end())
				{
					return;
				}
				CString originalFileLocation = files[OptionObjects[i].optionFileIndex].second;
				std::vector<unsigned char> buffer(2,0);
				int value = OptionObjects[i].GetDefaultValueInt();
				WriteIntToBuffer(buffer.data(), 0, value, 2);
				char* endptr;
				int offset = strtol(OptionObjects[i].optionFileOffset, &endptr, 16);
				ReplaceFileDataAtAddress(offset, originalFileLocation, 2, &buffer[0]);
				InjectFile(originalFileLocation, files[OptionObjects[i].optionFileIndex].first);
			}
			else if (OptionObjects[i].OptionType == "mapedits")
			{
				if(files.find(OptionObjects[i].optionFileIndex) == files.end())
				{
					return;
				}
				CString originalFileLocation = files[OptionObjects[i].optionFileIndex].second;
				std::vector<unsigned char> buffer;

				if (OptionObjects[i].currentValue.GetLength() % 2 == 0)
				{
					for (int j = 0; j < OptionObjects[i].defaultValue.GetLength(); j += 2)
					{
						CString byteString = OptionObjects[i].currentValue.Mid(j, 2); // Extract two characters
						int byte = std::strtol(byteString, nullptr, 16); // Convert hex to integer
						buffer.push_back(static_cast<unsigned char>(byte));
					}
				}

				char* endptr;
				int offset = strtol(OptionObjects[i].optionFileOffset, &endptr, 16);
				ReplaceFileDataAtAddress(offset, originalFileLocation, buffer.size(), &buffer[0]);
				InjectFile(originalFileLocation, files[OptionObjects[i].optionFileIndex].first);
			}
		}
	}

	std::vector<unsigned char> buffer(4,0);
	//This is the load return address then return
	//8FBF0024 8FB00018 8FB1001C 8FB20020 03E00008 27BD0028
	
	WriteIntToBuffer(buffer.data(), 0, 0x8FBF0024, 4);
	ReplaceFileDataAtAddress(0x1C0 + commandsUsed * 4, editableFile, 4, &buffer[0]);
	buffer.assign(4, 0);
	commandsUsed++;
	WriteIntToBuffer(buffer.data(), 0, 0x8FB00018, 4);
	ReplaceFileDataAtAddress(0x1C0 + commandsUsed * 4, editableFile, 4, &buffer[0]);
	buffer.assign(4,0);
	commandsUsed++;
	WriteIntToBuffer(buffer.data(), 0, 0x8FB1001C, 4);
	ReplaceFileDataAtAddress(0x1C0 + commandsUsed * 4, editableFile, 4, &buffer[0]);
	buffer.assign(4, 0);
	commandsUsed++;
	WriteIntToBuffer(buffer.data(), 0, 0x8FB20020, 4);
	ReplaceFileDataAtAddress(0x1C0 + commandsUsed * 4, editableFile, 4, &buffer[0]);
	buffer.assign(4, 0);
	commandsUsed++;
	WriteIntToBuffer(buffer.data(), 0, 0x03E00008, 4);
	ReplaceFileDataAtAddress(0x1C0 + commandsUsed * 4, editableFile, 4, &buffer[0]);
	buffer.assign(4, 0);
	commandsUsed++;
	WriteIntToBuffer(buffer.data(), 0, 0x27BD0028, 4);
	ReplaceFileDataAtAddress(0x1C0 + commandsUsed * 4, editableFile, 4, &buffer[0]);
	buffer.assign(4, 0);
	commandsUsed++;

	//Keep all the flags at the end of the file for space
	int startOfFlags = 0x1C0 + (commandsUsed+1) * 4;
	for (int flag : trueFlags)
	{
		WriteIntToBuffer(buffer.data(), 0, 0x10000+ flag, 4);
		ReplaceFileDataAtAddress(0x1C0 + commandsUsed * 4, editableFile, 4, &buffer[0]);
		buffer.assign(4, 0);
		commandsUsed++;
	}
	//Replace the pointer to the start of the flags array
	buffer.assign(2, 0);
	WriteIntToBuffer(buffer.data(), 0, startOfFlags - 0x44, 2);
	ReplaceFileDataAtAddress(0x182, editableFile, 2, &buffer[0]);

	//Replace the number of flags to set
	buffer.assign(2, 0);
	WriteIntToBuffer(buffer.data(), 0, trueFlags.size(), 2);
	ReplaceFileDataAtAddress(0x186, editableFile, 2, &buffer[0]);

	InjectFile(editableFile, files["gcgame"].first);
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

	files[strlen(listUpdate->internalName) < 1 ? addressStr : listUpdate->internalName] = std::make_pair(fileCount ,  listUpdate->tempLocation);

	fileCount++;

	return 0;
}

LRESULT TooieRandoDlg::UpdateProgressBar(WPARAM wp, LPARAM lp)
{    	
	int progress = (int)lp;
	//m_progressBar.SetPos(progress);

	if (progress == 100)
	{
		//m_progressBar.ShowWindow(SW_HIDE);
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

/// <summary>
/// Get the integer at the current rom up to 4 bytes
/// </summary>
/// <param name="address"></param>
/// <param name="length"></param>
/// <returns></returns>
int TooieRandoDlg::GetIntFromROM(int address,int length)
{
	int output=0;
	if (length > 4)
		return -1;
	for (int i = 0; i < length; i++)
	{
		output = output | ROM[address+i] << ((length-i-1) * 8);
	}

	return output;
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
	int size = 0;
	DecompressZLibAtPosition(gameNameStr, dlg, filein, start, GAME, size);
}
void TooieRandoDlg::DecompressZLibAtPosition(CString gameNameStr, TooieRandoDlg* dlg, CString filein, unsigned long start, int GAME, int& compressedSize)
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
			compressedSize = fileSizeCompressed;
			AddRowData(dlg, address, fileSizeCompressed, fileSizeUncompressed, fileNameStr, tempLocation, type);	
	}
}
void TooieRandoDlg::DecompressZLibFromTable(CString gameNameStr, TooieRandoDlg* dlg, CString filein, unsigned long start, unsigned long end, int step, int GAME, unsigned long tblOffset, int shift, int multiplier, int offset,bool isScripts = false)
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
		//This should try and determine whether the headers have been changed to be readable or are in their preC
		bool HeadersFixed = true;
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
				if(!isScripts) //Don't exclude files with an 0x04 in the second table because they are not empty
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
				int offsetToFileName = -1;
				int fileNameLength = -1;
				
				if(isScripts && HeadersFixed) //If we are trying to decompress scripts get the name data from the header so we can append it to the filename
				{ 
					int firstHalf = input[address - 0x10 + 0x8];
					int secondHalf = input[address - 0x10 + 0x8+1];
					int merged = firstHalf << 8 | secondHalf;
					if (merged > 0x1000)
					{
						HeadersFixed = false;
					}
					else
					{
						offsetToFileName = merged * 4 + 0x28;
						fileNameLength = input[address - 0x10 + 0xE];
					}
				}
				int fileSizeUncompressed = DecompressZLibSpot(&compressed, dlg->genText, address, input, size, GAME, folderPath, fileNameStr, -1, tempLocation, fileSizeCompressed, type, address, false, 0, offsetToFileName, fileNameLength);
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
int TooieRandoDlg::DecompressZLibSpot(GECompression* compressed, bool genText, int offset, unsigned char* GameBuffer, int romSize, int GAME, CString folderPath, CString &internalName, int expectedSize, CString& tempLocation, int& fileSizeCompressed, CString& type, unsigned long printedAddress, bool printBank, unsigned printbankAddress)
{
	return DecompressZLibSpot(compressed, genText, offset, GameBuffer, romSize, GAME, folderPath, internalName, expectedSize, tempLocation, fileSizeCompressed, type, printedAddress, printBank, printbankAddress,-1,-1);
}
int TooieRandoDlg::DecompressZLibSpot(GECompression* compressed, bool genText, int offset, unsigned char* GameBuffer, int romSize, int GAME, CString folderPath, CString &internalName, int expectedSize, CString& tempLocation, int& fileSizeCompressed, CString& type, unsigned long printedAddress, bool printBank, unsigned printbankAddress, int offsetToName,int nameSize)
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
					if (nameSize > 0)
					{
						std::string str(outputDecompressed + offsetToName, outputDecompressed + offsetToName + nameSize);
						internalName = str.c_str();
					}
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
			dlg->assetTableStart = 0x5188;
			dlg->syscallTableStart = dlg->GetIntFromROM(0x50E0, 4); 
			dlg->core1Start = int(GameBuffer[0x1076]) << 24 | (int(GameBuffer[0x1077]) - 1 << 16) | dlg->GetIntFromROM(0x107E, 2);
			dlg->core3Start = (dlg->GetIntFromROM(0x1276, 2)<<16) | dlg->GetIntFromROM(0x1282, 2);

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
				//I should make a version for fully decompressing for modding tools then one for all the stuff I actually edit to decrease the build time

				DecompressZLibFromTable(gameNameStr, dlg, strROMPath, 0x7958, 0x7E58 , 4, BANJOTOOIE, 0x12B24, 8, 4, 0);
				dlg->m_progressBar.SetPos(15);
				DecompressZLibFromTable(gameNameStr, dlg, strROMPath, 0x8350, 0xC568, 4, BANJOTOOIE, 0x12B24, 8, 4, 0);
				dlg->m_progressBar.SetPos(25);
				int coreSize = 0;
				DecompressZLibAtPosition(gameNameStr, dlg, strROMPath, dlg->core1Start,BANJOTOOIE,coreSize);
				dlg->core2Start = dlg->core1Start + coreSize;
				DecompressZLibAtPosition(gameNameStr, dlg, strROMPath, dlg->core2Start,BANJOTOOIE);
				DecompressZLibAtPosition(gameNameStr, dlg, strROMPath, dlg->core3Start,BANJOTOOIE, coreSize);
				dlg->core4Start = dlg->core3Start + coreSize;
				DecompressZLibAtPosition(gameNameStr, dlg, strROMPath, dlg->core4Start,BANJOTOOIE);
				dlg->m_progressBar.SetPos(30);
				DecompressZLibFromTable(gameNameStr, dlg, strROMPath, dlg->syscallTableStart, dlg->syscallTableStart +0xDCC, 4, BANJOTOOIE, dlg->syscallTableStart, 0, 1, 0x10,true);
				dlg->m_progressBar.SetPos(35);

			}
			if (Randomize)
			{
				TooieRandoDlg* pDlg = (TooieRandoDlg*)pParam;
				pDlg->PostMessage(WM_USER + 1, 0, 0);
			}
			else
			{
				dlg->m_progressBar.SetPos(100);
			}

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
	Randomize = false;
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

/// <summary>
/// Write the int of size into the given buffer starting at address
/// </summary>
/// <param name="Buffer"></param>
/// <param name="address"></param>
/// <param name="data"></param>
/// <param name="size"></param>
void TooieRandoDlg::WriteIntToBuffer(unsigned char* Buffer, int address, int data,int size)
{
	for (int i = 0; i < size; i++)
	{
		int value = data >> ((size - i - 1) * 8);
		Buffer[address + i] = value;
	}
}

void TooieRandoDlg::InjectFile(CString filePath, int index)
{
	ListUpdateStruct* listUpdateStruct = ((ListUpdateStruct*)m_list.GetItemData(index));

	unsigned long diff = 0xF000000;
	unsigned long nextTableAddress;
	if (listUpdateStruct->fileSizeCompressed > 0)
		diff = listUpdateStruct->fileSizeOriginal;
	if (index < (m_list.GetItemCount() - 1))
	{
		ListUpdateStruct* listUpdateStructNext = ((ListUpdateStruct*)m_list.GetItemData(index+1));
		//diff = listUpdateStructNext->address - listUpdateStruct->address;
		nextTableAddress = listUpdateStructNext->address;
	}

	unsigned long address = listUpdateStruct->address;

	CString gameNameStr;
	//m_gameselection.GetWindowText(gameNameStr);

	CString tempAddrStr;
	tempAddrStr.Format("/modified/%08X_MODIFIED.bin", address);

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
				int thisAddress = 0;
				int nextAddress = 0;
				if (index < (m_list.GetItemCount() - 1))
				{
					if ((zlibGame == BANJOTOOIE) && (address >= 0x5188) && (address < 0x11A24))
					{
						thisAddress = ((CharArrayToLong(&ROM[address]) >> 8) * 4) + 0x12B24;
						nextAddress = ((CharArrayToLong(&ROM[nextTableAddress]) >> 8) * 4) + 0x12B24;
					}
					if ((zlibGame == BANJOTOOIE) && (address >= syscallTableStart))
					{

						long value = CharArrayToLong(&ROM[address]);

						thisAddress = (value)+syscallTableStart + 0x10;
							nextAddress = CharArrayToLong(&ROM[nextTableAddress]) +syscallTableStart;
					}
					if (outSize > (nextAddress-thisAddress) && (nextAddress != 0 && thisAddress!=0))
					{
						CString sizeTempStr;
						sizeTempStr.Format("Injecting a file of %08X at %08X would intersect with %08X (next item), are you sure you want to replace? This file %s Next file %s", outSize, thisAddress , nextAddress, m_list.GetItemText(index,4), m_list.GetItemText(index+1, 4));
						int iResults = MessageBox(sizeTempStr, "Are you sure?", MB_YESNO | MB_ICONINFORMATION);
						if (iResults == IDNO)
							return;
					}
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
				if ((zlibGame == BANJOTOOIE) && (address >= syscallTableStart)) 
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
		//m_progressBar.ShowWindow(SW_HIDE);
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
		CFileDialog m_svFile(FALSE, NULL, (romName + ".z64"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "OUT ROM(*.v64;*.z64;*.rom;*.bin)|*.v64;*.z64;*.rom;*.bin|", this);

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

		CString originalDir;
		TCHAR buffer[MAX_PATH];
		::GetCurrentDirectory(MAX_PATH, buffer);
		originalDir = buffer;

		::SetCurrentDirectory(RN64dir);

		CString tempStr;
		tempStr.Format("rn64crc.exe -u \"%s\"", RN64name);
		hiddenExec(_T(tempStr.GetBuffer()), RN64dir);

		::SetCurrentDirectory(originalDir);
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

void TooieRandoDlg::ReplaceFileDataAtAddress(int address, CString filepath,int size, unsigned char* buffer)
{
    char message[256];

	long fileSize = GetSizeFile(filepath);
	FILE* inFile = fopen(filepath,"rb+");
	if (inFile == NULL)
	{
        sprintf(message, "Invalid File Replace File Data at Address: %s\n", filepath.GetString());
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
	
	/*std::string dataOutput;
    for (size_t i = 0; i < size; ++i) {
        char byteStr[4];
        sprintf(byteStr, "%02X ", buffer[i]);
        dataOutput += byteStr;
    }*/
    //sprintf(message, "Data written to file: %s $%s at %X\n", dataOutput.c_str() , filepath.GetString(),address);
	//////OutputDebugString(_T(message));
}
int TooieRandoDlg::GetIntAtAddress(int address, CString filepath, int size)
{
	int output = 0;
	unsigned char *buffer = new unsigned char[size];
	GetFileDataAtAddress(address, filepath, size, buffer);
	for (int i = 0; i < size; i++)
	{
		output = output | buffer[i] << ((size - i - 1) * 8);
	}
	return output;
}
void TooieRandoDlg::GetFileDataAtAddress(int address, CString filepath,int size, unsigned char* buffer)
{
	CString filePath = strdup(filepath);
	long fileSize = GetSizeFile(filePath);
	FILE* inFile = fopen(filePath,"rb");
	if (inFile == NULL)
	{
		char message[256];
		sprintf(message, "Invalid File Get File At Address %X filepath %s\n", address, filePath);
		::MessageBox(NULL, message, "Error", NULL);
		return;
	}
	if (address > fileSize)
	{
		char message[256];
		sprintf(message, "Trying to get data outside of file size At Address %X filepath %s\n", address, filePath);
		::MessageBox(NULL, message, "Error", NULL);
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


/// <summary>
/// Replace the data starting at the given address in the file with the buffer
/// </summary>
/// <param name="address"></param>
/// <param name="filepath"></param>
/// <param name="oldsize">The size of data to remove</param>
/// <param name="newsize">The size of data to add</param>
/// <param name="buffer"></param>
void TooieRandoDlg::ReplaceFileDataAtAddressResize(int address, CString filepath,int oldsize, int newsize,unsigned char* buffer)
{
	char message[256];
	std::ifstream inputFile(filepath.GetString(), std::ios::binary);
	
	std::string FilePath = filepath.GetString();
	if (!inputFile.is_open()) {
		std::cerr << "Error opening file for readinf: " << filepath << std::endl;
		::MessageBox(NULL, ("Error opening file for readinf: " + FilePath).c_str(), "Error", NULL);
		return;
	}


	// Create a temporary file to write the modified data
    std::string tempFilePath = FilePath + ".tmp";
    std::ofstream tempFile(tempFilePath, std::ios::binary);
    if (!tempFile.is_open()) {
		::MessageBox(NULL, ("Error opening temporary file for writing: " + tempFilePath).c_str(), "Error", NULL);
		inputFile.close();
        return;
    }

	inputFile.seekg(0, SEEK_SET);
	if (address < 0) {
		::MessageBox(NULL, "Invalid address value!", "Error", NULL);
		return;
	}
	std::vector<char> oldbuffer(address);
	inputFile.read(oldbuffer.data(), address);
	tempFile.write(oldbuffer.data(), address);
	//Write the new data into place
	if(newsize != 0)
	tempFile.write((const char*)buffer, newsize); //Add the new data

	inputFile.seekg(address + oldsize, std::ios::beg);
	const size_t bufferSize = 4096; // Define a chunk size
	char chunk[bufferSize];

	// Read and write the remaining data
	while (inputFile.read(chunk, bufferSize) || inputFile.gcount() > 0) {
		tempFile.write(chunk, inputFile.gcount());
	}
	inputFile.close();
	tempFile.close();
	if (std::remove(filepath.GetString()) != 0) {
		::MessageBox(NULL, ("Error deleting the original file: " + std::string(filepath.GetString())).c_str(), "Error", NULL);
		return;
	}

	if (std::rename(tempFilePath.c_str(), filepath.GetString()) != 0) {
		::MessageBox(NULL,("Error renaming temporary file: " + tempFilePath + " to " + filepath.GetString()).c_str(),"Error",NULL);
	}
	//sprintf(message, "Data written to file: %s $%s at %X\n", dataOutput.c_str(), filepath.GetString(), address);
	//////OutputDebugString(_T(message));
}

void TooieRandoDlg::ReplaceObject(int sourceObjectId, int targetObjectId)
{
	int sourceIndex = GetObjectFromID(sourceObjectId);
	int targetIndex = GetObjectFromID(targetObjectId);
	


	if (sourceIndex != -1 && targetIndex != -1)
	{
		RandomizedObject& sourceObject = RandomizedObjects[sourceIndex];

		RandomizedObject& targetObject = RandomizedObjects[targetIndex];

		char message[256];
		sprintf(message, "Object at %s Replaced with %s\n", targetObject.LocationName.c_str(), sourceObject.LocationName.c_str());
		AddSpoilerToLog((std::string)(message));
		CString newFileLocation = m_list.GetItemText(targetObject.fileIndex, 4);
		if (targetObject.associatedOffset != -1)
		{
			Prop newProp(sourceObject.Data);

			newProp.position[0] = targetObject.Data.position[0];
			newProp.position[1] = targetObject.Data.position[1];
			newProp.position[2] = targetObject.Data.position[2];
			newProp.unk10 = targetObject.Data.unk10;
			newProp.unk12 = targetObject.Data.unk12;
			std::vector<unsigned char> buffer(newProp.OutputData());

			ReplaceFileDataAtAddress(targetObject.associatedOffset, newFileLocation, 0x14, &(buffer[0]));
			InjectFile(newFileLocation, targetObject.fileIndex);
		}
	}
}

//Returns the index in MoveObject that points to the Randomized Object
int TooieRandoDlg::GetObjectFromID(int objectID)
{
	auto findObject = [objectID](RandomizedObject& object) {return object.RandoObjectID == objectID; };
	auto it = std::find_if(RandomizedObjects.begin(), RandomizedObjects.end(), findObject);
	if (it != RandomizedObjects.end())
		return it - RandomizedObjects.begin();
	else
		return -1;
}

//Returns the index in MoveObject that points to the Move Object
int TooieRandoDlg::GetMoveFromID(int moveID)
{
	auto findObject = [moveID](MoveObject& object) {return object.MoveID == moveID; };
	auto it = std::find_if(MoveObjects.begin(), MoveObjects.end(), findObject);
	if (it != MoveObjects.end())
		return it - MoveObjects.begin();
	else
		return -1;
}

/// <summary>
/// Get the move index associated with the provided ability
/// </summary>
/// <param name="ability"></param>
/// <returns></returns>
int TooieRandoDlg::GetMoveIndexFromAbility(int ability)
{
	auto findObject = [ability](MoveObject& object) {return object.Ability == ability; };
	auto it = std::find_if(MoveObjects.begin(), MoveObjects.end(), findObject);
	if (it != MoveObjects.end())
		return it - MoveObjects.begin();
	else
		return -1;
}

int TooieRandoDlg::FindItemInListCtrl(CListCtrl& listCtrl, const CString& searchText, int columnIndex) {
    int itemCount = listCtrl.GetItemCount();

	char message[256];
	sprintf(message, "Item Count: %X Search Text %s column index %X\n", itemCount, searchText, columnIndex);
	////OutputDebugString(_T(message));

    for (int i = 0; i < itemCount; ++i) {
        CString itemText = listCtrl.GetItemText(i, columnIndex); // Get text from the first column
        if (itemText.CompareNoCase(searchText) == 0) { // Compare case-insensitive
			sprintf(message, "Found Item at index %X\n", i);
			////OutputDebugString(_T(message));
			return i; // Item found, return index
        }
    }
	sprintf(message, "Item not found\n");
	OutputDebugString(_T(message));
    return -1; // Item not found
}

void TooieRandoDlg::LoadElements()
{
    LoadMoves(true);
	m_progressBar.SetPos(40);

    LoadScriptEdits();
	m_progressBar.SetPos(44);

    LoadObjects(true);
	m_progressBar.SetPos(46);

	LoadEntrances();
	m_progressBar.SetPos(48);

}

int TooieRandoDlg::GetScriptIndex(CString scriptId)//Used for retreiving index of a script in the main table based on the position in the syscall table
{
	if (files.find(scriptId) == files.end())
	{
		return -1;
	}
	
	return files[scriptId].first;
}

CString TooieRandoDlg::GetScriptString(CString scriptId)//Used for retreiving index of a script in the main table based on the position in the syscall table
{
	char* endPtr;
	int scriptAddress = strtol(scriptId, &endPtr, 16) + syscallTableStart;
	char hexString[9];
	snprintf(hexString, sizeof(hexString), "%08X", scriptAddress);
	return hexString;
}

int TooieRandoDlg::GetAssetIndex(CString assetAddress)//Used for retreiving index of an asset in the main table based on the Address in the left column
{
	if (files.find(assetAddress) == files.end())
	{
		return -1;
	}

	return files[assetAddress].first;
}

void TooieRandoDlg::LoadObjects(bool extractFromFiles)
{
    RandomizedObjects.clear();
    RewardObjects.clear();
    std::ifstream myfile("RandomizerAddresses.txt");
    std::string line;
    try {
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
        bool shouldRandomize = true;
		std::string mapIDStr = GetStringAfterTag(line, "MapID:", ",");
		int mapID = !mapIDStr.empty() ? strtol(mapIDStr.c_str(), &endPtr, 16) : 1;
		
		std::string ObjectType = GetStringAfterTag(line, "ObjectType:", ","); //Try and get the object type which should only be defined for virtual items this is the actual in game object id
		std::string AssociatedScript = GetStringAfterTag(line, "AssociatedScript:", ",");
		int scriptIndex = AssociatedScript.size()>0 ? GetScriptIndex(AssociatedScript.c_str()) : -1; //If the associated script is actually defined as something find the index otherwise set to -1 meaning does not have any associated script
		
		std::string ScriptRewardIndex = GetStringAfterTag(line, "ScriptRewardIndex:", ",");
		int scriptRewardIndex = !ScriptRewardIndex.empty() ? strtol(ScriptRewardIndex.c_str(), &endPtr, 16) : -1; //If there is a script reward index
		
		std::string randomize = GetStringAfterTag(line, "Randomized:", ",");
		std::string randoObjectIDStr = GetStringAfterTag(line, "ObjectId:", ",");//Get the id used as a unique identifier for a specific object in the randomization list
		int randoObjectID = !randoObjectIDStr.empty()? strtol(randoObjectIDStr.c_str(), &endPtr, 16) : -1; 
		std::string ItemTag = GetStringAfterTag(line, "ItemTag:\"", "\",");//The tag saying what this item type is
		std::string ItemAmountStr = GetStringAfterTag(line, "ItemAmount:", ",");
		int ItemAmount = !ItemAmountStr.empty() ? strtol(ItemAmountStr.c_str(), &endPtr, 10) : 1;

		if (randomize == "false")
			shouldRandomize = false;
		
		if (line[0] == '*')//Used if this line only contains a script associated with the previous object
        {
            isJustScript = true;
			std::string AdditionalScript = GetStringAfterTag(line, "AdditionalScript:", ",");
			char* endPtr;
			if (extractFromFiles)
			{
				int addScriptIndex = AdditionalScript.size() > 0 ? GetScriptIndex(AdditionalScript.c_str()) : -1;
				if (addScriptIndex != -1)
				{
					RewardObjects.back().associatedScripts.push_back(addScriptIndex);
				}
			}
			continue;
			
        }

        else if (line[0] == '/') //Used to disregard a line entirely
            continue;

		std::string levelOffset = GetStringAfterTag(line, "ObjectOffset:", ",");
		RandomizedObject thisObject;
		int objectType = -1;
		int flag = -1;
		if (extractFromFiles)
		{
			if (files.find(mapIDStr.c_str()) == files.end())
			{
				return;
			}
			CString originalFileLocation = files[mapIDStr.c_str()].second;
			int offset = strtol(levelOffset.c_str(), &endPtr, 16);
			Prop tempProp{};
			bool virtualObject = false; //Is the object only spawned by script
			

			if (ObjectType.size() > 0) //If the object is only spawned by scripts and has no object in map file
			{
				virtualObject = true;
				std::string AssociatedFlag = GetStringAfterTag(line, "AssociatedFlag:", ",");
				offset = -1;

				objectType = strtol(ObjectType.c_str(), &endPtr, 16);
				flag = strtol(AssociatedFlag.c_str(), &endPtr, 16);
				
				tempProp.position[0] = 0;
				tempProp.position[1] = 0;
				tempProp.position[2] = 0;
				//This is handled differently because editing the gccollect stuff to add a new item is a lot harder than just using unk6_7
				if (objectType == Prop_CUSTOM_MOVE_ITEM)
				{
					tempProp.unk6_7 = flag;
					tempProp.FlagOrRotation = 0;
					tempProp.unkC_0 = 0x16;
				}
				else
				{
					tempProp.unk6_7 = 0x32;
					tempProp.FlagOrRotation = flag;
					tempProp.unkC_0 = 0x64;
				}
				tempProp.unk6_1 = 0x6;
				tempProp.unk6_0 = 0;
				tempProp.ItemID = objectType;
				
				tempProp.unkA = 0;
				
				tempProp.unk10 = 0x760;
				tempProp.unk12 = 0x100;

			}
			else //If the object physically exists in the map file
			{
				std::vector<unsigned char> buffer(0x14, 0);

				GetFileDataAtAddress(offset, originalFileLocation, 0x14, &buffer[0]);
				tempProp.InputData(buffer);
				objectType = tempProp.ItemID;
				flag = tempProp.FlagOrRotation;
			}
			thisObject = RandomizedObject(tempProp, files[mapIDStr.c_str()].first, offset);
			thisObject.objectID = objectType;

		}
		else
		{
			thisObject = RandomizedObject();
		}

		thisObject.LocationName = GetStringAfterTag(line, "ObjectName:\"", "\",");
		thisObject.RandoObjectID = randoObjectID;
		thisObject.mapID = mapID;
		thisObject.LevelIndex = GetLevelIndexFromMapId(mapID);
		thisObject.ItemTag = ItemTag;
		thisObject.ItemAmount = ItemAmount;
		thisObject.randomized = shouldRandomize;
        RandomizedObjects.push_back(thisObject);
			
		if (extractFromFiles && CanBeReward(objectType))//Whether the object matches one of the potential reward Objects
		{
			RewardObject reward = RewardObject(randoObjectID, objectType, flag);
			int spawnFlag = GetReward(reward.itemType, reward.itemId);
			sprintf(message, "Type %d Flag %d Has Flag %d\n", reward.itemType, reward.itemId, spawnFlag != 0x0);
			//::MessageBox(NULL, message, "Rom", NULL); //Print out data at address
			
			reward.shouldRandomize = shouldRandomize;
			if (scriptIndex != -1)
			{
				reward.itemIndex = scriptRewardIndex;

				CString newFileLocation = m_list.GetItemText(scriptIndex, 4);
				sprintf(message, "Found Script %s %d\n", newFileLocation, reward.itemIndex);
				////OutputDebugString(_T(message));
				reward.associatedScripts.push_back(scriptIndex);
			}
			if (spawnFlag != 0x0)
			{
				reward.hasFlag = true;
			}
			if(reward.associatedScripts.size() > 0)
			{
				RandomizedObjects.back().isSpawnLocation = true;
			}

			RewardObjects.push_back(reward);
			RandomizedObjects.back().rewardObjectIndex = RewardObjects.size() - 1;
		}
        if (shouldRandomize)
            PlaceObjectIntoLevelGroup(mapID, RandomizedObjects.back());
    }
    myfile.close();
	return;
}

void TooieRandoDlg::PlaceObjectIntoLevelGroup(int mapID, RandomizedObject& object)
{
    char message[256];
    int levelIndex = GetLevelIndexFromMapId(mapID);
	if (levelIndex != -1)
	{
		levelObjects[levelIndex].push_back(object.RandoObjectID);
	}
	else
	{
        sprintf(message, "Could not find associated Level %s\n", mapID);
        ::MessageBox(NULL, message, "Rom", NULL);
    }
}


void TooieRandoDlg::RandomizeObjects(LogicHandler::AccessibleThings state)
{
	char message[256];
	int rewardIndex = 1;
	//Store the location id and the flag associated with it
	std::map<int, int> rewardAssociations;

    int size = RandomizedObjects.size();
    std::vector<int> source, target;

	

    for (int i = 0; i < RandomizedObjects.size(); ++i) {
        source.push_back(RandomizedObjects[i].RandoObjectID);
        target.push_back(RandomizedObjects[i].RandoObjectID);
    }

	for (int i = 0; i < state.SetItems.size(); i++)
	{
		int sourceObject = std::get<1>(state.SetItems[i]);
		int location = std::get<0>(state.SetItems[i]);

		int sourceIndex = GetObjectFromID(sourceObject);
		int locationIndex = GetObjectFromID(location);

		
		ReplaceObject(sourceObject, location);
		if (RandomizedObjects[locationIndex].rewardObjectIndex != -1 && RewardObjects[RandomizedObjects[locationIndex].rewardObjectIndex].associatedScripts.size() != 0) //Replace reward objects with ones that can be spawned
		{
			int sourceRewardIndex = RandomizedObjects[sourceIndex].rewardObjectIndex;//The index of the reward object associated with the source object
			int locationRewardIndex = RandomizedObjects[locationIndex].rewardObjectIndex;//The index of the reward object associated with the location object
			int rewardFlagIndex;
			
			if (RewardObjects[RandomizedObjects[locationIndex].rewardObjectIndex].hasFlag)
			{
				switch (RewardObjects[sourceRewardIndex].objectID)
				{
				case Prop_Ticket:
					rewardFlagIndex = RewardObjects[sourceRewardIndex].itemId;
					break;
				default:
					rewardFlagIndex = rewardIndex;

					rewardIndex++;
					break;
				}
				rewardAssociations[RandomizedObjects[locationIndex].RandoObjectID] = RewardObjects[sourceRewardIndex].getRewardFlag(rewardFlagIndex);

				SetReward(RewardObjects[sourceRewardIndex].itemType, RewardObjects[sourceRewardIndex].itemId, rewardFlagIndex);
			}
			SetRewardScript(locationRewardIndex, RewardObjects[sourceRewardIndex].itemType, RewardObjects[sourceRewardIndex].itemId, RewardObjects[sourceRewardIndex].objectID);
		}
		auto sourceit = std::find(source.begin(), source.end(), sourceObject);
		auto targetit = std::find(target.begin(), target.end(), location);
		source.erase(sourceit);
		target.erase(targetit);
	}//Setup all objects assigned during logic calculation

	std::vector<int> levels = state.GetLevels();
	for (int i = 0; i < levels.size(); i++)
	{
		int levelInt = levels[i];

		////OutputDebugString(("Level " + std::to_string(levelInt) + " Unused Normal Count " + std::to_string(state.GetUnusedNormalGlobalLocationsFromLevel(levelInt).size()) + "\n").c_str());
		////OutputDebugString(("Free locations " + std::to_string(FindFreeLocationsInLevel(target, levelInt).size()) + "\n").c_str());
		
	}



	AddSpoilerToLog("End Logic Items\n");
	AddSpoilerToLog("Objects Not Randomized Items\n");

	//All non randomized objects encountered in the logic should be setup correctly in the logic handler
	//This is supposed to handle all of the non randomized objects before the level object and final shuffle
	bool doNotRandomize = false;
	vector<int> NoRandoObjectIds = GetIdsFromNameSelection(GetVectorFromString(GetOption("ObjectsNotRandomized").currentValue.GetString(), ","));
	bool notRandomizeOption = CheckOptionActive("ObjectsNotRandomized");
	
	for (int i = 0; i < size; ++i) 
	{

		auto targetit = std::find(target.begin(), target.end(), RandomizedObjects[i].RandoObjectID);
		if (targetit == target.end()) //Check if this object has already been randomized
		{
			sprintf(message, "Location already Randomized %i\n", i);
			////OutputDebugString(_T(message));
			continue;
		}
		bool doNotRandomize = !RandomizedObjects[i].randomized; //Used to say whether this object should be randomized
		if (notRandomizeOption)
		{
			auto foundNoRando = std::find(NoRandoObjectIds.begin(), NoRandoObjectIds.end(), RandomizedObjects[i].objectID);
			if (foundNoRando != NoRandoObjectIds.end())
			{
				doNotRandomize = true;
			}
		}
		if (doNotRandomize)
		{
			if (RandomizedObjects[i].rewardObjectIndex != -1 && RewardObjects[RandomizedObjects[i].rewardObjectIndex].hasFlag)
			{
				int rewardFlagIndex;
				switch (RewardObjects[RandomizedObjects[i].rewardObjectIndex].objectID)
				{
				case Prop_Ticket:
					rewardFlagIndex = RewardObjects[RandomizedObjects[i].rewardObjectIndex].itemId;
					break;
				default:
					rewardFlagIndex = rewardIndex;
					rewardIndex++;
					break;
				}
				rewardAssociations[RandomizedObjects[i].RandoObjectID] = RewardObjects[RandomizedObjects[i].rewardObjectIndex].getRewardFlag(rewardFlagIndex);
				SetReward(RewardObjects[RandomizedObjects[i].rewardObjectIndex].itemType, RewardObjects[RandomizedObjects[i].rewardObjectIndex].itemId, rewardFlagIndex);
			}
			AddSpoilerToLog(RandomizedObjects[i].LocationName + " Not Randomized\n");
			auto sourceit = std::find(source.begin(), source.end(), RandomizedObjects[i].RandoObjectID);
			source.erase(sourceit);
			target.erase(targetit);
		}
	}
	

	AddSpoilerToLog("Reward Objects Shuffle\n");

	//When randomizing rewards we use a location first approach of looking for suitable objects to place at the reward location
	for (int i = 0; i < size; ++i) {
        auto targetit = std::find(target.begin(), target.end(), RandomizedObjects[i].RandoObjectID);
		if (targetit == target.end()) //Check if this object has already been randomized
		{
			sprintf(message, "Location already Randomized %i\n", i);
			////OutputDebugString(_T(message));
			continue;
		}

        bool alreadyRandomized = false;
		//sprintf(message, "Reward object index %d associated script size %i\n", RandomizedObjects[i].rewardObjectIndex, RewardObjects[RandomizedObjects[i].rewardObjectIndex].associatedScripts.size());
		//////OutputDebugString(_T(message));

        if (RandomizedObjects[i].rewardObjectIndex != -1 && RewardObjects[RandomizedObjects[i].rewardObjectIndex].associatedScripts.size()!=0) //Replace reward objects with ones that can be spawned
        {
			sprintf(message, "Reward object index %d\n", RandomizedObjects[i].rewardObjectIndex);
			////OutputDebugString(_T(message));
			int replacementIndex = FindUnusedRewardObject(source);
			auto newSourceit = std::find(source.begin(), source.end(), RewardObjects[replacementIndex].associatedRandoObjectID);
            if (replacementIndex != -1)
            {
                if(RandomizedObjects[i].associatedOffset!=-1)
					ReplaceObject(RewardObjects[replacementIndex].associatedRandoObjectID, RandomizedObjects[i].RandoObjectID);
				else
					AddSpoilerToLog("Virtual Object at "+ RandomizedObjects[i].LocationName +" Replaced with "+ RandomizedObjects[GetObjectFromID(RewardObjects[replacementIndex].associatedRandoObjectID)].LocationName +" \n");
				int rewardFlagIndex;
				
				if (RewardObjects[RandomizedObjects[i].rewardObjectIndex].hasFlag)
				{
					switch (RewardObjects[replacementIndex].objectID)
					{
					case Prop_Ticket:
						rewardFlagIndex = RewardObjects[replacementIndex].itemId;
						break;
					default:
						rewardFlagIndex = rewardIndex;
						rewardIndex++;
						break;
					}
					rewardAssociations[RandomizedObjects[i].RandoObjectID] = RewardObjects[replacementIndex].getRewardFlag(rewardFlagIndex);

					SetReward(RewardObjects[replacementIndex].itemType, RewardObjects[replacementIndex].itemId, rewardFlagIndex);
				}
				SetRewardScript(RandomizedObjects[i].rewardObjectIndex, RewardObjects[replacementIndex].itemType, RewardObjects[replacementIndex].itemId, RewardObjects[replacementIndex].objectID);
				
                sprintf(message, "Removed %d from replacement Removed %d from source\n", i, source[newSourceit - source.begin()]);
                ////OutputDebugString(_T(message));
                source.erase(newSourceit);
                auto replacementit = std::find(target.begin(), target.end(), RandomizedObjects[i].RandoObjectID);
                target.erase(replacementit);
                alreadyRandomized = true;
                continue;
            }
            if (alreadyRandomized == true)
                continue;
        }
    }
	AddSpoilerToLog("Level Objects Shuffle\n");

	//Starting from a level object look for locations in that level to randomize to
    for (int i = 0; i < size; ++i) {
        auto sourceit = std::find(source.begin(), source.end(), RandomizedObjects[i].RandoObjectID);

        if (sourceit == source.end()) //Check if this object has already been randomized
            continue;

        /*std::string dataOutput = "";
        char message[256];
        if(RandomizedObjects[i].Data.size()>0)
        for (size_t j = 0; j < 10; ++j) {
            char byteStr[4];

            sprintf(byteStr, "%02X", RandomizedObjects[i].Data[j]);
            dataOutput += byteStr;

        }*/
		//TODO: Reimplement Level Objects on the logic charting side disabled everything but the notes
		vector<int> LevelObjectIds; //= GetIdsFromNameSelection(GetVectorFromString(GetOption("ObjectsKeptInLevel").currentValue.GetString(), ","));
		bool keepInLevel = true;//= CheckOptionActive("ObjectsKeptInLevel");
		LevelObjectIds.push_back(Prop_Note); //Always add notes
		LevelObjectIds.push_back(Prop_Treble_Clef);
        bool alreadyRandomized = false;
		//Level Objects are randomized by looking for a location within their original level that is valid this occurs after the rewards so that objects that cannot be rewards cannot find a reward location as they have already been used
        //Id/String combos in this vector are kept in their original level
		if(keepInLevel)
		{ 
			auto it = std::find(LevelObjectIds.begin(), LevelObjectIds.end(), RandomizedObjects[i].objectID);
			if (it != LevelObjectIds.end())
			{
				char message[256];
				sprintf(message, "Level Object Found %X\n", (*it));
				////OutputDebugString(_T(message));
				int levelIndex = RandomizedObjects[i].LevelIndex;
				int locationId = FindFreeLocationInLevel(target, levelIndex);
				ReplaceObject(RandomizedObjects[i].RandoObjectID, locationId);
				auto replacementit = std::find(target.begin(), target.end(), locationId);
				sprintf(message, "Note Removed 0x%X from source Removed 0x%X from replacement\n", RandomizedObjects[i].RandoObjectID, locationId);
				////OutputDebugString(_T(message));
				source.erase(sourceit);
				target.erase(replacementit);
				alreadyRandomized = true;
			}
		}
        if (alreadyRandomized)
            continue;
		//sprintf(message, "Not a Level Object %s\n", dataOutput.c_str());
		////OutputDebugString(_T(message));
    }




	AddSpoilerToLog("Final Leftover Shuffle\n");

    
    if (source.size() == target.size())
	{
        std::shuffle(source.begin(), source.end(), generator);
        std::shuffle(target.begin(), target.end(),generator);
        for (int i = 0; i < source.size(); ++i) {
			ReplaceObject(source[i], target[i]);
        }
    }
    else
    {
        char message[256];
        sprintf(message, "Source and Replacement uneven sized");
       ::MessageBox(NULL, message, "Rom", NULL); //Print out data at address
    }
	//TODO: Inject GOAL FLAGS into sujiggy
	//"00000D40"
	std::vector<int> goalFlags = {};

	OptionData endGoalsOption = GetOption("EndGoals");
	if (endGoalsOption.flags.size() != endGoalsOption.possibleSelections.size())
	{
		::MessageBox(NULL, "Option Error End Goals does not have corresponding goal ints setup", "Error", NULL);
		return;
	}
	for (int i = 0; i < endGoalsOption.flags.size(); i++)
	{
		std::string currentOptionValue(endGoalsOption.currentValue);
		if (currentOptionValue.find(endGoalsOption.possibleSelections[i]) == std::string::npos)
		{
			continue;
		}

		//If the int is positive treat it as a normal flag
		if (endGoalsOption.flags[i] > 0)
		{
			goalFlags.push_back(endGoalsOption.flags[i]);
		}
		else //Treat it as an object id
		{
			int objInd = GetObjectFromID(-endGoalsOption.flags[i]);
			if (objInd == -1 || RandomizedObjects[objInd].rewardObjectIndex == -1 || rewardAssociations.count(-endGoalsOption.flags[i]) == 0)
			{
				::MessageBox(NULL, "Option Error Attempted to Find a Goal Object that does not exist or is invalid", "Error", NULL);
				return;
			}
			else
			{
				goalFlags.push_back(rewardAssociations[-endGoalsOption.flags[i]]);
			}
		}
	}

	OptionData option = GetOption("MinimumGoals");
	int MinimumGoals = 1;
	if(option.optionName != "")
		MinimumGoals = option.active ? option.GetCurrentValueInt() : option.GetDefaultValueInt();
	if (MinimumGoals > goalFlags.size())
	{
		MinimumGoals = goalFlags.size();
	}
	else if (MinimumGoals == 0)
	{
		MinimumGoals = 1;
	}

	if (goalFlags.size() == 0) //If we dont have any goal flags fall back on hag1jiggy
	{
		goalFlags.push_back(0x39B);;
	}

	//Get the path to the sujiggy script
	CString gameStartFileLocation = files["sujiggy"].second;
	CreateTempFile(gameStartFileLocation);
	CString editableFile = TooieRandoDlg::GetTempFileString(gameStartFileLocation);
	std::vector<unsigned char> buffer;
	buffer.push_back(MinimumGoals);
	ReplaceFileDataAtAddress(0x32B, editableFile, 1, &buffer[0]);
	//A70 is the address of the goalflags array relative to the code start so if the sujiggy size changes this will break
	int flagSize = 0x0A70 + goalFlags.size() * 2;
	buffer.clear();

	buffer.push_back(flagSize >> 8);
	buffer.push_back(flagSize);
	ReplaceFileDataAtAddress(0x34E, editableFile, 2, &buffer[0]);

	for (int i = 0; i < goalFlags.size() && i<20; i++)
	{
		buffer.clear();
		buffer.push_back(goalFlags[i] >> 8);
		buffer.push_back(goalFlags[i]);
		ReplaceFileDataAtAddress(0xB80 + i*2 , editableFile, 2, &buffer[0]);
	}
	InjectFile(editableFile, files["sujiggy"].first);
}

std::vector<int> TooieRandoDlg::GetIdsFromNameSelection(std::vector<std::string> names)
{
	std::vector<int> returnIds;
	for (int ObjectTypeIndex = 0; ObjectTypeIndex < names.size(); ObjectTypeIndex++) //There is a possiblity that using just the object id could cause an issue if an object ends up having one of these within its data somewhere but I'm just gonna hope it wont and go from there
	{
		if (names[ObjectTypeIndex] == ("Note"))
		{
			returnIds.push_back(Prop_Note);
			returnIds.push_back(Prop_Treble_Clef);
		}
		if (names[ObjectTypeIndex] == ("Jiggy"))
		{
			returnIds.push_back(Prop_Jiggy);
		}
		if (names[ObjectTypeIndex] == ("Doubloon"))
		{
			returnIds.push_back(Prop_Doubloon);
		}
		if (names[ObjectTypeIndex] == ("Jinjo"))
		{
			returnIds.push_back(Prop_Jinjo);
		}
		if (names[ObjectTypeIndex] == ("Glowbo"))
		{
			returnIds.push_back(Prop_Glowbo);
			returnIds.push_back(Prop_MegaGlowbo);
		}
		if (names[ObjectTypeIndex] == ("Honeycomb"))
		{
			returnIds.push_back(Prop_Honeycomb);
		}
		if (names[ObjectTypeIndex] == ("Cheato Page"))
		{
			returnIds.push_back(Prop_CheatoPage);
		}
		if (names[ObjectTypeIndex] == ("Jade Totem"))
		{
			returnIds.push_back(Prop_JadeTotem);
		}
		if (names[ObjectTypeIndex] == ("Ticket"))
		{
			returnIds.push_back(Prop_Ticket);
		}
		if (names[ObjectTypeIndex] == ("Feather/Egg Nest"))
		{
			returnIds.push_back(Prop_FeatherNest);
			returnIds.push_back(Prop_EggNest);
		}
		if (names[ObjectTypeIndex] == ("Misc")) //Stuff like the fish
		{
			returnIds.push_back(Prop_BoggyFish);
		}
	}
	return returnIds;
}

void TooieRandoDlg::RandomizeElements()
{
	SetupOptions();

	//Create the Temp File for any Move Items that may become spawnable
	CreateTempFile(files["chjigsawdance"].second);

	ClearRewards();
	SaveSeedToFile();
	m_progressBar.SetPos(60);

	LogicGroup::LoadLogicGroupsFromFile(LogicGroups, std::get<1>(LogicFilePaths[LogicSelector.GetItemData(LogicSelector.GetCurSel())]).c_str());

	int startingLogicGroup = std::get<2>(LogicFilePaths[LogicSelector.GetCurSel()]);

	/// <summary>
	/// All of the LogicGroups that have already been recognized as accessible
	/// </summary>
	std::vector<int> lookedAtLogicGroups;

	/// <summary>
	/// All of the dependents of added logic groups that did not fulfill the requirements
	/// </summary>
	std::vector<int> nextLogicGroups;

	std::vector<int> viableLogicGroups;

	LogicHandler newLogicHandler = LogicHandler();
	std::unordered_map<int, RandomizedObject> objectMap;
	if (!newLogicHandler.alreadySetup)
	{
		for (const RandomizedObject& obj : RandomizedObjects)
		{
			newLogicHandler.objectsList[obj.RandoObjectID] = obj;
			newLogicHandler.normalLevelObjectsMapAll[obj.LevelIndex].push_back(obj.RandoObjectID);
		}

		for (const Entrance& obj : Entrances) //Go through all of our entrances and put them into their shuffle groups
		{
			newLogicHandler.EntranceList[obj.EntranceID] = obj;
			if (obj.shuffleGroup != -1)
				newLogicHandler.shuffleGroups[obj.shuffleGroup].push_back(obj.EntranceID);
		}

		for (const auto& obj : LogicGroups)
		{
			if (obj.second.AssociatedWarp != -1)
				newLogicHandler.entranceAssociations[obj.second.AssociatedWarp] = obj.first;
		}
	}
	newLogicHandler.alreadySetup = true;
	newLogicHandler.options = &OptionObjects;
	newLogicHandler.worldPrices.clear();
	std::vector<std::string> lookupIds = { "World1Jiggy","World2Jiggy","World3Jiggy" ,"World4Jiggy" ,"World5Jiggy" ,"World6Jiggy" ,"World7Jiggy","World8Jiggy","World9Jiggy","Hag1Jiggy" };
	for (const std::string& lookup : lookupIds)
	{
		OptionData jiggyOption = GetOption(lookup.c_str());
		if (jiggyOption.active)
			newLogicHandler.worldPrices.push_back(jiggyOption.GetCurrentValueInt());
		else
			newLogicHandler.worldPrices.push_back(jiggyOption.GetDefaultValueInt());
	}

	newLogicHandler.objectsNotRandomized = CheckOptionActive("ObjectsNotRandomized");
	if (newLogicHandler.objectsNotRandomized)
	{
		newLogicHandler.NoRandomizationIDs = GetIdsFromNameSelection(GetVectorFromString(GetOption("ObjectsNotRandomized").currentValue.GetString(), ","));
	}
	else
		newLogicHandler.NoRandomizationIDs.clear();

	LogicHandler::AccessibleThings state;

	LogicHandler::AccessibleThings doneState;
	if (CheckOptionActive("WorldsRandomized") == false)
	{
		state.SetWarps.push_back(std::make_pair(0x1, 0x2));
		state.SetWarps.push_back(std::make_pair(0x3, 0x4));
		state.SetWarps.push_back(std::make_pair(0x5, 0x6));
		state.SetWarps.push_back(std::make_pair(0x7, 0x8));
		state.SetWarps.push_back(std::make_pair(0x9, 0xA));
		state.SetWarps.push_back(std::make_pair(0xB, 0xC));
		state.SetWarps.push_back(std::make_pair(0xD, 0xE));
		state.SetWarps.push_back(std::make_pair(0xF, 0x10));
		state.SetWarps.push_back(std::make_pair(0x11, 0x12));
	}
	else if (CheckOptionActive("CKRando") == false) //If Cauldron Keep should be randomized
	{
		state.SetWarps.push_back(std::make_pair(0x11, 0x12));
	}

	std::vector<int> BKMOVES{ 0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22 ,0x23 ,0x24 ,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D };
	//TODO: Replace with BK Moves Option
	bool BKMoveRandomize = false;

	for (int MoveID : BKMOVES)
	{
		MoveObjects[GetMoveFromID(MoveID)].randomized = BKMoveRandomize;
	}
	for (int i = 0; i < MoveObjects.size(); i++)
	{
		if (MoveObjects[i].MoveType == "Start" && !MoveObjects[i].randomized)
			state.SetAbilities.push_back(std::make_pair(MoveObjects[i].MoveID, MoveObjects[i]));
	}

	for (OptionData data : OptionObjects)
	{
		if (data.active && !data.logicKey.empty())
			state.Keys.push_back(data.logicKey);
	}


	generator = default_random_engine(seed);
	LogicHandler::DebugPrint("RNG Test: " + std::to_string(generator()));
	m_progressBar.SetPos(65);

	if (CheckOptionActive("LogicDisabled") == false)
	{
		doneState = newLogicHandler.TryRoute(LogicGroups[startingLogicGroup], LogicGroups, lookedAtLogicGroups, nextLogicGroups, state, viableLogicGroups, RandomizedObjects, MoveObjects, 0, generator);
		m_progressBar.SetPos(75);

		if (doneState.done == false)
		{
			int iResults = MessageBox(NULL, "Could not find a valid logic path(please try a different seed)\n or continue without logic", MB_OKCANCEL | MB_ICONINFORMATION);
			if (iResults == IDCANCEL)
				return;
		}
		
	}
	else
	{
		doneState = state;
	}
	char message[256];
	ClearSpoilers();
	sprintf(message, "Seed: %d\n", seed);
	AddSpoilerToLog((std::string)message);
	for (int i = 0; i < OptionObjects.size(); i++)
	{
		if (OptionObjects[i].active)
		{
			if (OptionObjects[i].currentValue.IsEmpty() == true)
			{
				sprintf(message, "Option Active: %s\n", OptionObjects[i].optionName);
			}
			else
			{
				sprintf(message, "Option Active: %s, Current Value %s\n", OptionObjects[i].optionName, OptionObjects[i].currentValue);
			}
			AddSpoilerToLog((std::string)message);
		}
	}

	RandomizeWarps(doneState);//Edit the done state to include the leftover worlds so we can assign the note prices for the world order
	m_progressBar.SetPos(80);

	std::vector<int> worldOrder = newLogicHandler.GetWorldsInOrder(&doneState);

	//Map of the Unique Move location identifiers to the level in which they exist
	std::unordered_map<int, int> worldAssociations = { 
		{0xB,0},{0xC,0},{0xE,0},{0xD,0}, //IOH
		{2,1},{ 1,1 },{0,1}, //MT
		{3,2},{4,2}, //GGM
		{7,3},{6,3},{5,3}, //WW
		{0xA,4},{0x9,4},{0x8,4}, //JRL
		{0x14,5},{0x15,5},{0x16,5}, //TDL
		{0x12,6},{0x13,6},{0x11,6}, //GI
		{0xF,7}, {0x10,7}, //HFP
		{0x17,8}//CCL
	}; //MoveID, Associated Level
	std::unordered_map<int, int> siloLevelIndex = {
		{0xB,0},{0xC,1},{0xE,2},{0xD,3},//IOH
		{2,0},{ 1,1 },{0,2}, //MT
		{3,0},{4,1}, //GGM
		{6,0},{7,1},{5,2}, //WW
		{0xA,0},{0x9,1},{0x8,2}, //JRL
		{0x14,0},{0x15,1},{0x16,2}, //TDL
		{0x12,0},{0x13,1},{0x11,2}, //GI
		{0xF,0}, {0x10,1}, //HFP
		{0x17,0}//CCL
	}; //MoveID, Index in Level e.g. Egg aim would be 2,0 because egg aim has the lowest price inside the level

	for (int i = 0; i < MoveObjects.size(); i++)
	{
		if (MoveObjects[i].MoveType == "Silo") 	//Find all silo moveobjects
		{
			int siloIndex = 0;
			for (int j = 0; j < worldOrder.size(); j++)
			{
				if (worldAssociations[MoveObjects[i].MoveID] == worldOrder[j]) //Find the associated world in the world order
				{
					int currentWorld = worldOrder[j];

					int inLevelIndex = siloLevelIndex[MoveObjects[i].MoveID];

					//Set Note Price
					SetMovePrice(MoveObjects[i].MoveID, newLogicHandler.notePrices[siloIndex + inLevelIndex]);
					break;
				}
				//If the silo was not in this world increment the silo counter by the amount of silos were in the current world in the order
				siloIndex += newLogicHandler.siloIndexStep[worldOrder[j]];
				//The first four worlds all have a silo that comes after them in ioh
				if (j  < 4) //Handle changing all of the ioh silo notes
				{

					if (worldAssociations[MoveObjects[i].MoveID] == 0 && j == siloLevelIndex[MoveObjects[i].MoveID])
					{
						SetMovePrice(MoveObjects[i].MoveID, newLogicHandler.notePrices[siloIndex]);
						break;
					}
					siloIndex++;
				}
			}
		}
	}

    RandomizeMoves(doneState);
	m_progressBar.SetPos(90);

    RandomizeObjects(doneState);
	m_progressBar.SetPos(100);

	//Add the seed to the crash screen

	if (files.find(GetScriptString("gzpublic")) == files.end())
	{
		return;
	}
	CString gzPublicFileLocation = files["gzpublic"].second;
	CreateTempFile(gzPublicFileLocation);
	CString editableFile = TooieRandoDlg::GetTempFileString(gzPublicFileLocation);
	std::string seedString = std::to_string(seed);
	ReplaceFileDataAtAddress(0x2F0, editableFile, seedString.length(), (unsigned char*)seedString.c_str());
	InjectFile(editableFile, files["gzpublic"].first);
	////OutputDebugString("Completed Randomization");

}

/// <summary>
/// Returns the index of a move that is unused and not restricted for the current location
/// </summary>
int TooieRandoDlg::FindUnusedMove(std::vector<int> moveObjects,std::vector<int> restrictedMoves)
{
	char message[256];
	bool alreadyRandomized = false;
	std::uniform_int_distribution<int>  distr(0, RewardObjects.size() - 1);
	for (int find = 0; find < moveObjects.size(); find++)
	{
		int replacementIndex = (distr(generator) + find) % moveObjects.size();
		bool foundConflict = 0;
		for (int i = 0; i < restrictedMoves.size(); i++)
		{
			if (restrictedMoves[i] == MoveObjects[moveObjects[replacementIndex]].Ability || MoveObjects[moveObjects[replacementIndex]].randomized == false)
				foundConflict=true;
		}
		if (foundConflict)
			continue;

		return moveObjects[replacementIndex];
		alreadyRandomized = true;
		break;
	}
	return -1;
}

/// <summary>
/// Returns the index of an item that can be used as a reward object in the provided vector
/// </summary>
int TooieRandoDlg::FindUnusedRewardObject(std::vector<int> objects)
{
	char message[256];
	bool alreadyRandomized = false;
	std::uniform_int_distribution<int>  distr(0, RewardObjects.size() - 1);
	for (int find = 0; find < RewardObjects.size(); find++)
	{
		int replacementIndex = (distr(generator) + find) % RewardObjects.size();
		auto newSourceit = std::find(objects.begin(), objects.end(), RewardObjects[replacementIndex].associatedRandoObjectID);
		if (newSourceit != objects.end())
		{
			return replacementIndex;
			alreadyRandomized = true;
			break;
		}
	}
	return -1;
}
/// <summary>
/// Get the level index of the given MapID
/// </summary>
int TooieRandoDlg::GetLevelIndexFromMapId(int MapID)
{

	for (int levelIndex = 0; levelIndex < mapIDGroups.size(); levelIndex++)
	{
		if (std::find(mapIDGroups[levelIndex].begin(), mapIDGroups[levelIndex].end(), MapID) != mapIDGroups[levelIndex].end())//Check through each level and see if the map is within one of these
		{
			return levelIndex;
		}
	}
	char message[256];
	sprintf(message, "Could not find Level for Map %s\n", MapID);
	////OutputDebugString(_T(message));
	return -1;
}

/// <summary>
/// Find a location in the given level that has not been used yet and return its rando object id
/// </summary>
int TooieRandoDlg::FindFreeLocationInLevel(std::vector<int> locations, int levelIndex)
{
	char message[256];
	sprintf(message, "Map Found In Level %d with %d items\n", levelIndex, levelObjects[levelIndex].size());
	////OutputDebugString(_T(message));

	std::vector<int> freeLocationsInLevel;
	for (int i = 0; i < levelObjects[levelIndex].size(); i++)
	{
		auto replacementit = std::find(locations.begin(), locations.end(), levelObjects[levelIndex][i]);
		if (replacementit != locations.end())
		{
			freeLocationsInLevel.push_back(levelObjects[levelIndex][i]);
		}
	}
	std::shuffle(freeLocationsInLevel.begin(), freeLocationsInLevel.end(), generator);
	if(freeLocationsInLevel.size()>0)
	return freeLocationsInLevel[0];

	::MessageBox(NULL, "Logic Error Could Not Find Free Location (please try another seed)", "Error", NULL);
	////OutputDebugString("Location Could Not Be Found \n");
 	return -1;
}

std::vector<int> TooieRandoDlg::FindFreeLocationsInLevel(std::vector<int> locations, int levelIndex)
{
	std::vector<int> freeLocationsInLevel;
	for (int i = 0; i < levelObjects[levelIndex].size(); i++)
	{
		auto replacementit = std::find(locations.begin(), locations.end(), levelObjects[levelIndex][i]);
		if (replacementit != locations.end())
		{
			freeLocationsInLevel.push_back(levelObjects[levelIndex][i]);
		}
	}
	std::shuffle(freeLocationsInLevel.begin(), freeLocationsInLevel.end(), generator);
	return freeLocationsInLevel;
}

void TooieRandoDlg::SaveSeedToFile()
{
	std::fstream myfile;
	myfile.open("UsedSeeds.txt", std::ios::app);
	time_t timestamp;
	char message[256];
	sprintf(message, "Seed %i\n", seed);
	////OutputDebugString(_T(message));
	time(&timestamp);
	std::string str = std::to_string(seed)+" "+ std::ctime(&timestamp);
	myfile << str;
}
void TooieRandoDlg::ClearSpoilers()
{
	remove("SpoilerLog.txt");
}
void TooieRandoDlg::AddSpoilerToLog(std::string spoiler)
{
	std::fstream myfile;
	myfile.open("SpoilerLog.txt", std::ios::app);
	time_t timestamp;
	char message[256];
	time(&timestamp);
	std::string str = spoiler + " \n";
	myfile << str;
}

/// <summary>
/// Load the options from the options file
/// </summary>
void TooieRandoDlg::LoadOptions(CString filePath)
{
	OptionObjects.clear();
	option_list.DeleteAllItems();
	std::ifstream myfile(filePath);
	std::string line;
	try {
		if (!myfile.is_open()) {
			throw std::runtime_error("Error: Could not open the file 'RandomizerOptions.txt'.");
		}
	}
	catch (const std::exception& ex) {
		::MessageBox(NULL, ex.what(), "Error", NULL);
		return;
	}
	char message[1024];
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
		AddOption(OptionData::Deserialize(line));
	}
	myfile.close();
}

/// <summary>
/// Load the Moves found in the silo files as determined by the addresses and offsets within the file
/// </summary>
void TooieRandoDlg::LoadMoves(bool extractFromFiles)
{
    MoveObjects.clear();
    std::ifstream myfile("SiloRandomizerAddresses.txt");
    std::string line;
    try {
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
		MoveObject moveObject = MoveObject::Deserialize(line);
		if (extractFromFiles && moveObject.scriptAddress.size()>0) //Used for when we're actually going to edit files using this data
		{
			int scriptIndex = GetScriptIndex(moveObject.scriptAddress.c_str()); //Get the asset index for the script address
			if (scriptIndex == -1)
				return;
			CString originalFileLocation = m_list.GetItemText(scriptIndex, 4);
			unsigned char buffer[0x10];
			GetFileDataAtAddress(moveObject.associatedOffset, originalFileLocation, 0x10, buffer);
			std::vector<unsigned char> tempVector;
			for (int i = 0; i < 0x10; i++)
			{
				tempVector.push_back(buffer[i]);
			}
			moveObject.Data = tempVector;
			moveObject.fileIndex = scriptIndex;
		}
        MoveObjects.push_back(moveObject);
    }
    myfile.close();
}

/// <summary>
/// Load the edits to be done to scripts from the file
/// </summary>
void TooieRandoDlg::LoadScriptEdits()
{
    ScriptEdits.clear();
    std::ifstream myfile("RewardScriptEditAddresses.txt");
    std::string line;
    try {
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
		std::string ScriptIdStr = GetStringAfterTag(line, "AssociatedScript:", ",");
		std::string ScriptEditOffset = GetStringAfterTag(line, "ScriptOffset:", ",");
		std::string EditType = GetStringAfterTag(line, "EditType:", ",");
		std::string RewardIndex = GetStringAfterTag(line, "RewardIndex:", ",");

        if (line[0] == '/')
            continue;

		char* endPtr;
        int index = GetScriptIndex(ScriptIdStr.c_str()); //Get the asset index for the script address
        if (index == -1)
            return;
        int offset = strtol(ScriptEditOffset.c_str(), &endPtr, 16);
        int editType = strtol(EditType.c_str(), &endPtr, 16);
        int rewardIndex = strtol(RewardIndex.c_str(), &endPtr, 16);

        ScriptEdit scriptEdit = ScriptEdit(index, editType, offset, rewardIndex);
        ScriptEdits.push_back(scriptEdit);

    }
    myfile.close();
}

/// <summary>
/// Load the entrances from the file
/// </summary>
void TooieRandoDlg::LoadEntrances()
{
	Entrances.clear();
	std::ifstream myfile("Entrances.txt");
	std::string line;
	try {
		if (!myfile.is_open()) {
			throw std::runtime_error("Error: Could not open the file 'Entrances.txt'.");
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

		std::string EntranceName = GetStringAfterTag(line, "EntranceName:", ",");
		std::string loadingZoneStr = GetStringAfterTag(line, "LoadingZone:", ",");
		std::string warpStr = GetStringAfterTag(line, "WarpID:", ",");
		std::string mapIDStr = GetStringAfterTag(line, "MapID:", ",");
		std::string scriptStr = GetStringAfterTag(line, "AssociatedScript:", ",");
		std::string EntranceIDStr = GetStringAfterTag(line, "EntranceID:", ",");
		std::string ShuffleGroupStr = GetStringAfterTag(line, "ShuffleGroup:", ",");
		std::string ExitIdStr = GetStringAfterTag(line, "ExitId:", ",");

		std::vector<int> fileOffsetsVector;
		fileOffsetsVector = GetIntVectorFromString(GetStringAfterTag(line, "FileOffsets:[", "],").c_str(), ",");
		std::vector<int> entranceOffsetsVector;
		entranceOffsetsVector = GetIntVectorFromString(GetStringAfterTag(line, "EntranceOffsets:[", "],").c_str(), ",");

		char* endPtr;
		if (files.find(mapIDStr.c_str()) == files.end() && GetScriptIndex(scriptStr.c_str()) == -1)
		{
			return;
		}
		int scriptIndex = GetScriptIndex(scriptStr.c_str()); //Get the asset index for the script address
		if (scriptIndex == -1 && files[mapIDStr.c_str()].first == -1)
			return;

		int entranceId = strtol(EntranceIDStr.c_str(), &endPtr, 16);

		int warpId = strtol(warpStr.c_str(), &endPtr, 16);
		int loadingZone = strtol(loadingZoneStr.c_str(), &endPtr, 16);

		int exitId = strtol(ExitIdStr.c_str(), &endPtr, 16);
		int shuffleGroup = ShuffleGroupStr.size()>0 ? strtol(ShuffleGroupStr.c_str(), &endPtr, 16) : -1;

		Entrance entrance = Entrance();
		entrance.EntranceName = EntranceName;
		entrance.EntranceID = entranceId;
		entrance.fileOffsets = fileOffsetsVector;
		entrance.entranceOffsets = entranceOffsetsVector;
		entrance.MapID = mapIDStr.c_str();
		entrance.shuffleGroup = shuffleGroup;
		entrance.EntranceIndex = files.find(mapIDStr.c_str()) != files.end() ? files[mapIDStr.c_str()].first : scriptIndex; //If there is a map index use it otherwise use the script index
		entrance.ExitId = exitId;
		entrance.LoadingZoneId = loadingZone;
		entrance.WarpId = warpId;
		Entrances.push_back(entrance);

	}
	myfile.close();
}

/// <summary>
/// Make both warps connected so they maintain continuity when walking through them in either direction
/// </summary>
/// <param name="entrance"></param>
/// <param name="exit"></param>
void TooieRandoDlg::ConnectWarp(int entrance, int exit)
{
	int entranceIndex = GetEntranceByID(entrance);
	int exitIndex = GetEntranceByID(exit);
	if (entranceIndex == -1 || exitIndex == -1)
	{
		return;
	}
	CString entranceFileLocation = m_list.GetItemText(Entrances[entranceIndex].EntranceIndex, 4);
	CString exitFileLocation = m_list.GetItemText(Entrances[exitIndex].EntranceIndex, 4);
	//This is for if we are editing the maps or a script because it's offset by a0 in maps
	int entranceMapAdjustment = 0;
	if (!Entrances[entranceIndex].MapID.IsEmpty())
	{
		entranceMapAdjustment = 0xA0;
	}
	int exitMapAdjustment = 0;
	if (!Entrances[exitIndex].MapID.IsEmpty())
	{
		exitMapAdjustment = 0xA0;
	}
	for (int i = 0; i < Entrances[exitIndex].fileOffsets.size();i++)
	{
		std::vector<unsigned char> warp1Buffer(2, 0);
		WriteIntToBuffer(warp1Buffer.data(), 0, Entrances[entranceIndex].WarpId - exitMapAdjustment, 2);
		ReplaceFileDataAtAddress(Entrances[exitIndex].fileOffsets[i], exitFileLocation, 0x2, &(warp1Buffer[0]));

		std::vector<unsigned char> entrance1Buffer(1, 0);
		WriteIntToBuffer(entrance1Buffer.data(), 0, Entrances[entranceIndex].LoadingZoneId, 1);
		ReplaceFileDataAtAddress(Entrances[exitIndex].entranceOffsets[i], exitFileLocation, 0x1, &(entrance1Buffer[0]));
	}
	
	for (int i = 0; i < Entrances[entranceIndex].fileOffsets.size(); i++)
	{
		std::vector<unsigned char> warp2Buffer(2, 0);
		WriteIntToBuffer(warp2Buffer.data(), 0, Entrances[exitIndex].WarpId - entranceMapAdjustment, 2);
		ReplaceFileDataAtAddress(Entrances[entranceIndex].fileOffsets[i], entranceFileLocation, 0x2, &(warp2Buffer[0]));

		std::vector<unsigned char> entrance2Buffer(1, 0);
		WriteIntToBuffer(entrance2Buffer.data(), 0, Entrances[exitIndex].LoadingZoneId, 1);
		ReplaceFileDataAtAddress(Entrances[entranceIndex].entranceOffsets[i], entranceFileLocation, 0x1, &(entrance2Buffer[0]));
	}
	char message[256];
	sprintf(message, "Warp %s Connected to %s\n", Entrances[entranceIndex].EntranceName.c_str(), Entrances[exitIndex].EntranceName.c_str());
	AddSpoilerToLog((std::string)(message));
	////OutputDebugString(_T(message));
	InjectFile(entranceFileLocation, Entrances[entranceIndex].EntranceIndex);
	InjectFile(exitFileLocation, Entrances[exitIndex].EntranceIndex);

}

/// <summary>
/// Return the ID
/// </summary>
int TooieRandoDlg::GetEntranceByID(int entranceID)
{
	auto findEntrance = [entranceID](Entrance& object) {return object.EntranceID == entranceID; };
	auto it = std::find_if(Entrances.begin(), Entrances.end(), findEntrance);
	if (it != Entrances.end())
		return it - Entrances.begin();
	else
		return -1;
}

/// <summary>
/// Get a vector of entranceIDs that are in the shuffle group
/// </summary>
/// <param name="shuffleGroup"></param>
/// <returns></returns>
std::vector<int> TooieRandoDlg::GetAllEntrancesInShuffleGroup(int shuffleGroup)
{
	std::vector<int> returnVector;
	for (int i = 0; i < Entrances.size(); i++)
	{
		if (Entrances[i].shuffleGroup == shuffleGroup)
			returnVector.push_back(Entrances[i].EntranceID);
	}
	return returnVector;
}

void TooieRandoDlg::RandomizeWarps(LogicHandler::AccessibleThings& state)
{
	//Shuffle groups containing the all of the groups that have a shufle group id
	std::unordered_map<int,std::vector<int>> shuffleGroups;
	//Contains the exitIDs associated from the groups with the given shuffle group id
	std::unordered_map<int, std::vector<int>> pairShuffleGroups;
	std::vector<int> shuffleGroupsPresent;
	for (int i = 0; i < Entrances.size(); i++)
	{
		if (Entrances[i].shuffleGroup > 0)
		{
			if (std::find(shuffleGroupsPresent.begin(), shuffleGroupsPresent.end(), Entrances[i].shuffleGroup) == shuffleGroupsPresent.end()) {
				shuffleGroupsPresent.push_back(Entrances[i].shuffleGroup);
			}
			shuffleGroups[Entrances[i].shuffleGroup].push_back(Entrances[i].EntranceID);
			pairShuffleGroups[Entrances[i].shuffleGroup].push_back(Entrances[i].ExitId);
		}
	}
	for (int i = 0; i < state.SetWarps.size(); i++)
	{
		int entrance = std::get<0>(state.SetWarps[i]);
		int exit = std::get<1>(state.SetWarps[i]);
		int shuffleGroup = Entrances[GetEntranceByID(exit)].shuffleGroup;

		shuffleGroups[shuffleGroup].erase(std::remove(shuffleGroups[shuffleGroup].begin(), shuffleGroups[shuffleGroup].end(),exit), shuffleGroups[shuffleGroup].end());
		pairShuffleGroups[shuffleGroup].erase(std::remove(pairShuffleGroups[shuffleGroup].begin(), pairShuffleGroups[shuffleGroup].end(), entrance), pairShuffleGroups[shuffleGroup].end());

		ConnectWarp(entrance, exit);
	}
	AddSpoilerToLog("End Logic Warps\n");
	AddSpoilerToLog("Randomize Leftover Warps\n");

	for (int i = 0;i< shuffleGroupsPresent.size();i++)
	{
		std::vector<int> entrances;
		std::vector<int> exits;
		entrances = shuffleGroups[shuffleGroupsPresent[i]];
		exits = pairShuffleGroups[shuffleGroupsPresent[i]];
		std::shuffle(entrances.begin(), entrances.end(), generator);
		std::shuffle(exits.begin(), exits.end(), generator);
		for (int i = 0; i < entrances.size();i++)
		{
			state.SetWarps.push_back(std::make_pair(exits[i], entrances[i]));
			ConnectWarp(exits[i], entrances[i]);
		}
	}
}
void TooieRandoDlg::SetMovePrice(int source, int price)
{
	int sourceIndex = GetMoveFromID(source);
	CString newFileLocation = m_list.GetItemText(MoveObjects[sourceIndex].fileIndex, 4);
	std::vector<unsigned char> buffer(2, 0);
	WriteIntToBuffer(buffer.data(), 0, price, 2);
	ReplaceFileDataAtAddress(MoveObjects[sourceIndex].associatedOffset+0xC, newFileLocation, 2, &buffer[0]);
	InjectFile(newFileLocation, MoveObjects[sourceIndex].fileIndex);
}
void TooieRandoDlg::RandomizeMove(int source, int target)
{
		int sourceIndex = GetMoveFromID(source);
		int targetIndex = GetMoveFromID(target);

        //CString newFileLocation = m_list.GetItemText(SiloOffset[target][0], 4);
        CString newFileLocation = m_list.GetItemText(MoveObjects[targetIndex].fileIndex, 4);
        std::vector<unsigned char> MainData;
        std::vector<unsigned char> TitleData;
		std::vector<unsigned char> DataToUse;//This is the pointer to the data for the various tutorial information surrounding learning a move at the silo
		if (MoveObjects[sourceIndex].MoveType == "Silo")
		{
			DataToUse = MoveObjects[sourceIndex].Data;
		}
		else if (MoveObjects[sourceIndex].MoveType == "Individual")
		{
			DataToUse = MoveObjects[targetIndex].Data;
		}
		if (MoveObjects[targetIndex].MoveType == "Silo")
		{
			for (int i = 0; i < 0x10; i++)
			{
				if (i >= 4 && i < 10)
					MainData.push_back(DataToUse[i]);
				if (i == 10)
					MainData.push_back(MoveObjects[sourceIndex].Ability);
				if (i == 11)
					MainData.push_back(DataToUse[i]);
				if (i == 14) //The title to show
				{
					if (MoveObjects[sourceIndex].MoveType == "Individual")
					{
						std::string TitleIndex = GetStringAfterTag(MoveObjects[sourceIndex].dialogData, "TitleIndex:", ","); //The Index of the title to use
						char* endPtr;
						int titleIndex = strtol(TitleIndex.c_str(), &endPtr, 16);
						TitleData.push_back(titleIndex);
					}
					TitleData.push_back(DataToUse[i]);
				}
				if (i >= 15)
					TitleData.push_back(DataToUse[i]);
			}
			ReplaceFileDataAtAddress(MoveObjects[targetIndex].associatedOffset + 0x4, newFileLocation, 0x8, &(MainData[0]));
			ReplaceFileDataAtAddress(MoveObjects[targetIndex].associatedOffset + 0xE, newFileLocation, 0x2, &(TitleData[0]));

		}
		else if (MoveObjects[targetIndex].MoveType == "Individual")
		{
			MainData.push_back(MoveObjects[sourceIndex].Ability);
			ReplaceFileDataAtAddress(MoveObjects[targetIndex].associatedOffset, newFileLocation, 0x1, &(MainData[0]));
			if (MoveObjects[targetIndex].dialogData.length() > 0)
			{
				std::string dialogData = MoveObjects[targetIndex].dialogData;
				std::string AssociatedDialog = GetStringAfterTag(dialogData, "AssociatedDialog:", ","); //The address of the dialog to reference
				std::string DialogStartOffset = GetStringAfterTag(dialogData, "DialogStartOffset:", ","); //The offset to the changes within the dialog file
				std::string DialogLength = GetStringAfterTag(dialogData, "DialogLength:", ","); //The length of the data to replace
				std::string DialogLineLengthOffset = GetStringAfterTag(dialogData, "DialogLineLengthOffset:", ","); //The offset in the dialog file to the length control for the changed line
				if (files.find(AssociatedDialog.c_str()) == files.end())
				{
					return;
				}
				CString dialogFileLocation = files[AssociatedDialog.c_str()].second;
				char* endPtr;
				int dialogOffset = strtol(DialogStartOffset.c_str(), &endPtr, 16);
				int dialogLength = strtol(DialogLength.c_str(), &endPtr, 16);
				int dialogLineLengthOffset = strtol(DialogLineLengthOffset.c_str(), &endPtr, 16);
				unsigned char buffer[1];
				GetFileDataAtAddress(dialogLineLengthOffset, dialogFileLocation, 0x1, buffer);
				int result = int(buffer[0]);
				result -= dialogLength;
				result += MoveObjects[sourceIndex].MoveName.length();
				buffer[0] = result;
				CreateTempFile(dialogFileLocation);
				CString editableFile = GetTempFileString(dialogFileLocation);
				ReplaceFileDataAtAddress(dialogLineLengthOffset, editableFile, 1, buffer);
				ReplaceFileDataAtAddressResize(dialogOffset, editableFile, dialogLength, MoveObjects[sourceIndex].MoveName.length(),(unsigned char *) MoveObjects[sourceIndex].MoveName.c_str());
				InjectFile(editableFile, GetAssetIndex(AssociatedDialog.c_str()));
			}
		}
		
		char message[256];
		sprintf(message, "Move %s Replaced with %s\n", MoveObjects[targetIndex].MoveName.c_str(), MoveObjects[sourceIndex].MoveName.c_str());
		AddSpoilerToLog((std::string)(message));
		////OutputDebugString(_T(message));
        InjectFile(newFileLocation, MoveObjects[targetIndex].fileIndex);
}

void TooieRandoDlg::CreateTempFile(CString filePath)
{
	std::ifstream inputFile(filePath.GetString(), std::ios::binary);

	std::string FilePath = filePath.GetString();
	if (!inputFile.is_open()) {
		std::cerr << "Error opening file for readinf: " << filePath << std::endl;
		return;
	}

	// Create a temporary file to write the modified data
	std::string tempFilePath = FilePath + "_modified.bin";
	std::ofstream tempFile(tempFilePath, std::ios::binary);
	if (!tempFile.is_open()) {
		std::cerr << "Error opening temporary file for writing: " << tempFilePath << std::endl;
		inputFile.close();
		return;
	}
	tempFile << inputFile.rdbuf();
}
CString TooieRandoDlg::GetTempFileString(CString filePath)
{
	return filePath + "_modified.bin";
}

void TooieRandoDlg::RandomizeMoves(LogicHandler::AccessibleThings state)
{
	char message[256];
	//Source contains the moves that have not been used
	//Target is the locations where moves are placed
    std::vector<int> sourceMoves, moveLocations;
	for (int i = 0; i < MoveObjects.size(); ++i) //Load all of the moves into an array for moves and an array for locations
	{
		sourceMoves.push_back(MoveObjects[i].MoveID);
		moveLocations.push_back(MoveObjects[i].MoveID);
	}

	//Iterated through all of the moves that were placed by the logic and actually put them into the files
	for (int i = 0; i < state.SetAbilities.size(); i++)
	{
		int sourceMove = std::get<1>(state.SetAbilities[i]).MoveID;
		int location = std::get<0>(state.SetAbilities[i]);
		if (state.SetAbilities[i].second.randomized)
			RandomizeMove(sourceMove, location);
		auto sourceit = std::find(sourceMoves.begin(), sourceMoves.end(), sourceMove);
		auto targetit = std::find(moveLocations.begin(), moveLocations.end(), location);
		sourceMoves.erase(sourceit);
		moveLocations.erase(targetit);
	}
	AddSpoilerToLog("End Logic Moves\n");

	//Iterate through the leftover movelocations and place moves in restrictive move locations
	for (int i = 0; i < moveLocations.size(); ++i) 
	{
		if (MoveObjects[moveLocations[i]].restrictedMoves.size() > 0) //Randomize moves with restrictions first so that they can for sure find something that works outside of their restrictions
		{
			int foundMove = FindUnusedMove(sourceMoves, MoveObjects[i].restrictedMoves);
			RandomizeMove(foundMove, moveLocations[i]);
			auto sourceit = std::find(sourceMoves.begin(), sourceMoves.end(), foundMove);
			auto targetit = std::find(moveLocations.begin(), moveLocations.end(), moveLocations[i]);
			sourceMoves.erase(sourceit);
			moveLocations.erase(targetit);
		}
    }
    std::shuffle(sourceMoves.begin(), sourceMoves.end(), generator);
    std::shuffle(moveLocations.begin(), moveLocations.end(), generator);

	//Randomize anything leftover
    for (int i = 0; i < sourceMoves.size(); ++i) 
	{
        RandomizeMove(sourceMoves[i], moveLocations[i]);
    }
}

int TooieRandoDlg::FindRewardFlagOffset(int itemType, int itemFlag)
{
    int offset = 0;
    switch (itemType)
    {

    case 0: //Jinjo
        offset = itemFlag * 3;
            offset += 0x2EDF;
        break;

    case 1: //Jiggy
        offset = itemFlag *2;
        offset += 0x2F67;
        break;

    case 2: //Honeycomb
        offset = itemFlag *2;
        offset += 0x301B;
        break;

    case 3: //Glowbo
        offset = itemFlag *2;
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
	case 9: //Move Item
		offset = itemFlag + 0x834;
		break;
    default:
        break;
    }
    return offset;

}

int TooieRandoDlg::GetReward(int itemType, int itemFlag)
{
	char hexString[9];
	snprintf(hexString, sizeof(hexString), "%08X", core4Start);
	if (files.find(hexString) == files.end())
	{
		return -1;
	}

    CString newFileLocation = files[hexString].second;
    unsigned char* buffer;
    buffer = new unsigned char[1];
	int offset = FindRewardFlagOffset(itemType, itemFlag);
    GetFileDataAtAddress(offset, newFileLocation, 0x1, buffer);
	int result = int(buffer[0]);
	delete[] buffer;
	return result;
}

/// <summary>
/// Sets the spawn flag for reward items
/// </summary>
/// <param name="itemType"></param>
/// <param name="itemFlag"></param>
/// <param name="value">The index of the flag from the start of the spawned item section of flags</param>
void TooieRandoDlg::SetReward(int itemType, int itemFlag, int value)
{
	std::vector<unsigned char> buffer;
	CString editableFile;
	switch (itemType)
	{
		case 9:
			buffer.push_back((unsigned char)value);
			editableFile = TooieRandoDlg::GetTempFileString(files["chjigsawdance"].second);
			ReplaceFileDataAtAddress(FindRewardFlagOffset(itemType, itemFlag), editableFile, 0x1, &buffer[0]);
			InjectFile(editableFile, files["chjigsawdance"].first);
			break;
		default:
			char hexString[9];
			snprintf(hexString, sizeof(hexString), "%08X", core4Start);
			if (files.find(hexString) == files.end())
			{
				return;
			}
			CString newFileLocation = files[hexString].second;
			buffer.push_back((unsigned char)value);

			ReplaceFileDataAtAddress(FindRewardFlagOffset(itemType, itemFlag), newFileLocation, 0x1, &buffer[0]);
			InjectFile(newFileLocation, files[hexString].first);
			break;
	}
	
}

void TooieRandoDlg::SetRewardScript(int reward,int itemType, int itemFlag, int objectID)
{
    for (int j = 0;j < RewardObjects[reward].associatedScripts.size();j++)
    {
        CString newFileLocation = m_list.GetItemText(RewardObjects[reward].associatedScripts[j], 4);
		char message[256];
		sprintf(message, "Script %s\n", newFileLocation);
		////OutputDebugString(_T(message));
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
    case Prop_Jinjo: //Jinjo
        break;
    case Prop_Jiggy: //Jiggy
        break;
    case Prop_Honeycomb: //Honeycomb
        break;
    case Prop_Glowbo: //Glowbo
        break;
    case Prop_Ticket: //Ticket
        break;
    case Prop_Doubloon: //Doubloon
        break;
    case Prop_CheatoPage: //Cheato
        break;
	case Prop_CUSTOM_MOVE_ITEM: //Custom Move Item
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
	char* p;
	long converted = strtol(inputText.GetString(), &p, 10);
	//Check if the string is numerical or if it exceeds 10 million this is used so that you can use all generated hash seeds without needing the original input
	if (*p || abs(converted)<10000000) {
		size_t hash_value = hash_fn(inputText.GetString());
		seed = static_cast<int>(hash_value);
	}
	else {
		seed = strtol(inputText.GetString(), &p, 10);
	}
	char message[256];
	sprintf(message, "Seed: %d\n", seed);
	////OutputDebugString(_T(message));
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
	if (pNMItemActivate->iItem != -1)
	{
		//*pResult = 0;
		this->SetForegroundWindow();    // Bring the first dialog to the front
		char* endPtr;
		CChangeLength cChangeLengthDialog(this, pNMItemActivate->iItem);
		cChangeLengthDialog.DoModal();
	}
}

UINT RandomizationThread(LPVOID pParam) {
	TooieRandoDlg* dlg = (TooieRandoDlg*)(pParam);

	while (!dlg->stopNow) 
	{
		dlg->LoadElements(); //Load Objects/Moves/Edits
		dlg->m_progressBar.SetPos(50);
		dlg->RandomizeElements(); //Randomize
		dlg->m_progressBar.SetPos(100);
		AfxMessageBox(_T("Randomization Complete!"));
		dlg->m_reRandomizeButton.ShowWindow(SW_SHOW);
		dlg->OnBnClickedButtonsaverom();
		dlg->stopNow = true;
	}

	// Signal main thread to close/destroy progress bar
	//::PostMessage(dlg->randobar->GetSafeHwnd(), WM_CLOSE, 0, 0);
	return TRUE;
}

void TooieRandoDlg::OnBnClickedDecompressgame2()
{
	Randomize = true;
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

	// Set up the startup info structure
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	// Set up the process information structure
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	// Create a full command line (including cmd.exe for the `/c` flag)
	CString fullCommand;
	fullCommand.Format("cmd.exe %s", command);

	// Start the process
	if (CreateProcess(
		NULL,                            // Application name (NULL means use command line)
		fullCommand.GetBuffer(),         // Command line
		NULL,                            // Process security attributes
		NULL,                            // Thread security attributes
		FALSE,                           // Handle inheritance
		0,                               // Creation flags
		NULL,                            // Environment block (use parent process's environment)
		currentDirectory,                // Current directory
		&si,                             // Startup information
		&pi                              // Process information
	))
	{
		// Wait for the process to complete
		WaitForSingleObject(pi.hProcess, INFINITE);

		// Close process and thread handles
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		////OutputDebugString(_T("Process completed successfully.\n"));
	}
	else
	{
		// Handle error
		DWORD error = GetLastError();
		CString errorMessage;
		errorMessage.Format(_T("Failed to create process. Error code: %lu\n"), error);
		////OutputDebugString(errorMessage);
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

	mDecompressFileButton.ShowWindow(SW_SHOW);


	int zlibGame = GetZLibGameName(gameNameStr);
	mCompressFileButton.ShowWindow(SW_SHOW);
	m_injectButton.ShowWindow(SW_SHOW);
	m_saveROMButton.ShowWindow(SW_SHOW);

	
}

LRESULT TooieRandoDlg::OnThreadComplete(WPARAM wParam, LPARAM lParam)
{
	AfxBeginThread(RandomizationThread, this);


	return 0;
}


void TooieRandoDlg::OnDblclkOptionList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (OptionObjects[option_list.GetItemData(pNMItemActivate->iItem)].alwaysTrue)
	{
		return;
	}

	if (option_list.GetItemText(pNMItemActivate->iItem, 0) == "Y")
	{
		option_list.SetItemText(pNMItemActivate->iItem, 0, "N");
	}
	else
	{
		option_list.SetItemText(pNMItemActivate->iItem, 0, "Y");

	}
	OptionObjects[option_list.GetItemData(pNMItemActivate->iItem)].active = !OptionObjects[option_list.GetItemData(pNMItemActivate->iItem)].active;
	*pResult = 0;
}

void TooieRandoDlg::OnEnChangeVariableEdit()
{
	if (selectedOption != -1)
	{
		VariableEdit.GetWindowTextA(OptionObjects[selectedOption].currentValue);
		option_list.SetItemText(selectedOption, 2, OptionObjects[selectedOption].currentValue);
	} 
}


void TooieRandoDlg::OnBnClickedSelectAdd()
{
	int optionIndex = option_list.GetItemData(selectedOption);
	if (SelectionList.GetCurSel() == -1)
		return;
	if(OptionObjects[optionIndex].currentValue!="")
		OptionObjects[optionIndex].currentValue.Append(",");
	OptionObjects[optionIndex].currentValue.Append(OptionObjects[optionIndex].possibleSelections[SelectionList.GetCurSel()].c_str());
	option_list.SetItemText(selectedOption, 2, OptionObjects[optionIndex].currentValue);
	SelectionList.InsertString(SelectionList.GetCurSel(), ("X: "+OptionObjects[optionIndex].possibleSelections[SelectionList.GetCurSel()]).c_str());
	SelectionList.DeleteString(SelectionList.GetCurSel());
}


void TooieRandoDlg::OnBnClickedSelectRemove()
{
	int optionIndex = option_list.GetItemData(selectedOption);
	if (SelectionList.GetCurSel() == -1)
		return;

	CString valueToRemove= "";
	CString copyCurrent = OptionObjects[optionIndex].currentValue;
	copyCurrent.Insert(0,",");
	copyCurrent.Append(",");
	valueToRemove.Format(",%s,", OptionObjects[optionIndex].possibleSelections[SelectionList.GetCurSel()].c_str());
	if (OptionObjects[optionIndex].currentValue == valueToRemove)
		OptionObjects[optionIndex].currentValue = "";
	int foundIndex = copyCurrent.Find(valueToRemove);
	if (foundIndex == 0)
	{
		OptionObjects[optionIndex].currentValue.Delete(foundIndex, valueToRemove.GetLength() - 1);
	}
	else if (foundIndex != -1)
	{
		OptionObjects[optionIndex].currentValue.Delete(foundIndex-1, valueToRemove.GetLength() - 1);
	}
	option_list.SetItemText(selectedOption, 2, OptionObjects[optionIndex].currentValue);
	SelectionList.InsertString(SelectionList.GetCurSel(), OptionObjects[optionIndex].possibleSelections[SelectionList.GetCurSel()].c_str());
	SelectionList.DeleteString(SelectionList.GetCurSel());
}

void TooieRandoDlg::OnBnClickedLogicEditorButton()
{
	LogicCreator logicCreator = new LogicCreator(this);
	logicCreator.DoModal();
}

/// <summary>
/// Load the file names and paths for the option selections
/// </summary>
void TooieRandoDlg::LoadLogicFileOptions()
{
	LogicFilePaths = LogicGroup::LoadLogicFileOptions();
	UpdateLogicSelector();
}

void TooieRandoDlg::UpdateLogicSelector()
{
	LogicSelector.ResetContent();
	for (int i = 0; i < LogicFilePaths.size(); i++)
	{
		LogicSelector.AddString(std::get<0>(LogicFilePaths[i]).c_str());
		LogicSelector.SetItemData(i, i);
	}
	if (LogicFilePaths.size() > 0)
		LogicSelector.SetCurSel(0);
	LogicGroup::LoadLogicGroupsFromFile(LogicGroups, std::get<1>(LogicFilePaths[LogicSelector.GetItemData(0)]).c_str());
}


void TooieRandoDlg::OnBnClickedLogicCheck()
{
	if (m_list.GetItemCount() == 0)
	{
		MessageBox("Please Load a Tooie Rom Before making a logic check");
		return;
	}
	LoadObjects(false);
	LoadMoves(false);
	LogicGroup::LoadLogicGroupsFromFile(LogicGroups, std::get<1>(LogicFilePaths[LogicSelector.GetItemData(LogicSelector.GetCurSel())]).c_str());

	int startingLogicGroup = std::get<2>(LogicFilePaths[LogicSelector.GetCurSel()]);


	/// <summary>
	/// All of the LogicGroups that have already been recognized as accessible
	/// </summary>
	std::vector<int> lookedAtLogicGroups;

	/// <summary>
	/// All of the dependents of added logic groups that did not fulfill the requirements
	/// </summary>
	std::vector<int> nextLogicGroups;

	std::vector<int> viableLogicGroups;

	LogicHandler newLogicHandler;
	std::unordered_map<int, RandomizedObject> objectMap;
	for (const auto& obj : RandomizedObjects) {
		newLogicHandler.objectsList[obj.RandoObjectID] = obj;
		newLogicHandler.normalLevelObjectsMapAll[obj.LevelIndex].push_back(obj.RandoObjectID);
	}

	for (const auto& obj : Entrances) {
		newLogicHandler.EntranceList[obj.EntranceID] = obj;
	}
	newLogicHandler.options = &OptionObjects;
	std::vector<std::string> lookupIds = {"World1Jiggy","World2Jiggy","World3Jiggy" ,"World4Jiggy" ,"World5Jiggy" ,"World6Jiggy" ,"World7Jiggy","World8Jiggy","World9Jiggy","Hag1Jiggy" };
	newLogicHandler.worldPrices.clear();
	for (const std::string& lookup : lookupIds)
	{
		OptionData jiggyOption = GetOption(lookup.c_str());
		if(jiggyOption.active)
		newLogicHandler.worldPrices.push_back(jiggyOption.GetCurrentValueInt());
		else
		newLogicHandler.worldPrices.push_back(jiggyOption.GetDefaultValueInt());
	}
	newLogicHandler.objectsNotRandomized = CheckOptionActive("ObjectsNotRandomized");
	if (newLogicHandler.objectsNotRandomized)
	{
		newLogicHandler.NoRandomizationIDs = GetIdsFromNameSelection(GetVectorFromString(GetOption("ObjectsNotRandomized").currentValue.GetString(), ","));
	}
	else
		newLogicHandler.NoRandomizationIDs.clear();
	LogicHandler::AccessibleThings state;
	generator = default_random_engine(seed);

	LogicHandler::AccessibleThings doneState;
	doneState = newLogicHandler.TryRoute(LogicGroups[startingLogicGroup],LogicGroups,lookedAtLogicGroups, nextLogicGroups,state, viableLogicGroups,RandomizedObjects,MoveObjects,0, generator);
	
	if(doneState.done)
		MessageBox("Logic Check Successful");
}


void TooieRandoDlg::OnClickedDevmode()
{

	if (m_devMode.GetCheck() == BST_CHECKED)
	{
		mDecompressFileButton.ShowWindow(SW_SHOW);
		mCompressEncryptedButton.ShowWindow(SW_SHOW);
		mCompressFileButton.ShowWindow(SW_SHOW);
		mEncryptedFileNumber.ShowWindow(SW_SHOW);
		mFileNumberStatic.ShowWindow(SW_SHOW);
		m_loadEditedRomButton.ShowWindow(SW_SHOW);
		m_logicEditorButton.ShowWindow(SW_SHOW);
		m_logicCheckButton.ShowWindow(SW_SHOW);
	}
	else
	{
		mDecompressFileButton.ShowWindow(SW_HIDE);
		mCompressEncryptedButton.ShowWindow(SW_HIDE);
		mCompressFileButton.ShowWindow(SW_HIDE);
		mEncryptedFileNumber.ShowWindow(SW_HIDE);
		mFileNumberStatic.ShowWindow(SW_HIDE);
		m_loadEditedRomButton.ShowWindow(SW_HIDE);
		m_logicEditorButton.ShowWindow(SW_HIDE);
		m_logicCheckButton.ShowWindow(SW_HIDE);

	}
}

void TooieRandoDlg::OnBnClickedLogicTrackerButton()
{
	LogicTracker logicTracker = new LogicTracker(this);
	logicTracker.DoModal();
}

void TooieRandoDlg::OnBnClickedExportSettingsButton()
{
	OutputDebugString("\nAAA\n");

	CString fileOpen;

	CFileDialog m_svFile(FALSE, NULL, ("RandomizerOptions.txt"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "OUT Settings (*.txt)|*.txt|", this);

	int isFileOpened2 = m_svFile.DoModal();
	if (isFileOpened2 == IDCANCEL|| m_svFile.GetFileName() == "")
		return;
	fileOpen = m_svFile.GetPathName();
	FILE* outFile = fopen(fileOpen, "wb");
	if (outFile == NULL)
	{
		MessageBox("Cannot open output file");
		return;
	}

	for (auto const& option : OptionObjects)
	{
		std::string optionString = OptionData::Serialize(option);
		fwrite(optionString.c_str(), 1, strlen(optionString.c_str()), outFile);
		fwrite("\n", 1, 1, outFile);
	}
	fclose(outFile);
}

void TooieRandoDlg::OnBnClickedImportSettingsButton()
{
	CString fileOpen;

	CFileDialog m_ldFile(TRUE, NULL, "RandomizerOptions.txt", NULL, "IN Settings (*.txt) |*.txt| ", this);
	int didRead = m_ldFile.DoModal();
	if ((didRead == IDCANCEL) || (m_ldFile.GetPathName() == ""))
		return;
	if (didRead == FALSE)
		return;
	fileOpen = m_ldFile.GetPathName();
	LoadOptions(fileOpen);
}

void TooieRandoDlg::OnItemclickOptionList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	

	int OptionIndex = option_list.GetItemData(pNMItemActivate->iItem);
	selectedOption = pNMItemActivate->iItem;
	if (OptionIndex == -1)
		return;
	if (OptionObjects[OptionIndex].OptionType == "value")
	{
		VariableEdit.SetWindowText(OptionObjects[OptionIndex].currentValue);
		VariableEdit.ShowWindow(SW_SHOW);
		SelectionList.ShowWindow(SW_HIDE);
		SelectionListAdd.ShowWindow(SW_HIDE);
		SelectionListRemove.ShowWindow(SW_HIDE);
		VariableEdit.SetReadOnly(FALSE);
		VariableEdit.ModifyStyle(0, ES_NUMBER);
	}
	else if (OptionObjects[OptionIndex].OptionType == "multiselect")
	{

		SelectionList.ResetContent();
		for (int i = 0; i < OptionObjects[OptionIndex].possibleSelections.size(); i++)
		{
			CString valueToFind = "";
			CString copyCurrent = OptionObjects[OptionIndex].currentValue;
			copyCurrent.Insert(0, ",");
			copyCurrent.Append(",");
			valueToFind.Format(",%s,", OptionObjects[OptionIndex].possibleSelections[i].c_str());
			int foundIndex = copyCurrent.Find(valueToFind);
			//If we have the element in the current value insert an X: to the start
			SelectionList.AddString((
				(foundIndex == -1?"":"X: ") +
				OptionObjects[OptionIndex].possibleSelections[i]).c_str());
		}
		VariableEdit.ShowWindow(SW_HIDE);
		SelectionList.ShowWindow(SW_SHOW);
		SelectionListAdd.ShowWindow(SW_SHOW);
		SelectionListRemove.ShowWindow(SW_SHOW);
		SelectionList.SetWindowText(OptionObjects[OptionIndex].currentValue);
	}
	else
	{
		VariableEdit.ShowWindow(SW_SHOW);
		VariableEdit.SetReadOnly(TRUE);
		SelectionList.ShowWindow(SW_HIDE);
		SelectionListAdd.ShowWindow(SW_HIDE);
		SelectionListRemove.ShowWindow(SW_HIDE);
	}
	*pResult = 1;
		
}
void TooieRandoDlg::OnIdok()
{
}


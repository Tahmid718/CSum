#include "csum.h"
#include "hfs.h"
#include "ase.h"
#include "../hash/calculate.h"

#include <wx/filedlg.h>
#include <thread>
#include <fstream>
#include <windows.h>
#include <time.h>

enum MENU_ID
{
    ID_OpenFile = 44,
    ID_QuitApp = 18,
    ID_AboutApp = 58,
    ID_HFS = 14,
    ID_ASE = 23
};

wxButton* CSumFrame::CalculateButton = nullptr;
wxButton* CSumFrame::CopyButton = nullptr;
wxButton* CSumFrame::RecalculateButton = nullptr;
wxButton* CSumFrame::MatchButton = nullptr;
wxButton* CSumFrame::OpenFileButton = nullptr;
wxTextCtrl* CSumFrame::PathEntry = nullptr;
wxTextCtrl* CSumFrame::HashEntry = nullptr;
wxTextCtrl* CSumFrame::CheckHashEntry = nullptr;
wxChoice* CSumFrame::AlgorithmEntry = nullptr;

std::string CSumFrame::StrHash = "";

CSumFrame::CSumFrame(const wxString& title, std::string ver)
    : wxFrame(nullptr, wxID_ANY, title)
{    
    SetMinSize(wxSize(905, 391));
    SetMaxSize(wxSize(1280, 391));
    
    wxFont TitleFont(18, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Segoe UI");
    wxFont LabelFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Segoe UI"); 

    wxArrayString algorithms;
    algorithms.Add("MD5");
    algorithms.Add("SHA1");
    algorithms.Add("SHA256");
    algorithms.Add("SHA512");

    version = ver;

    CSumFrame::SettingUpMenu();
    
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    panel->Bind(wxEVT_KEY_DOWN, &CSumFrame::KeyboardEvent, this);
    panel->SetFont(LabelFont);
    panel->SetBackgroundColour(wxColour(249, 248, 237));

    wxStaticText* FileLabel = new wxStaticText(panel, wxID_ANY, "File", wxPoint(29, 108));
    FileLabel->SetFont(TitleFont);

    OpenFileButton = new wxButton(panel, wxID_ANY, "Open", wxPoint(29, 30), wxSize(97, 33));
    OpenFileButton->Bind(wxEVT_BUTTON, &CSumFrame::OpenFileDialog, this);

    CalculateButton = new wxButton(panel, wxID_ANY, "Calculate", wxPoint(343, 30), wxSize(94, 30));
    CalculateButton->Bind(wxEVT_BUTTON, &CSumFrame::CalculateHashThread, this);
    CalculateButton->Disable();

    CopyButton = new wxButton(panel, wxID_ANY, "Copy", wxPoint(743, 212), wxSize(80, 30));
    CopyButton->Bind(wxEVT_BUTTON, &CSumFrame::Copy, this);
    CopyButton->Disable();

    RecalculateButton = new wxButton(panel, wxID_ANY, "Recalculate", wxPoint(460, 30), wxSize(114, 30));
    RecalculateButton->Bind(wxEVT_BUTTON, &CSumFrame::CalculateHashThread, this);
    RecalculateButton->Disable();

    MatchButton = new wxButton(panel, wxID_ANY, "Match", wxPoint(743, 272), wxSize(80, 30));
    MatchButton->Bind(wxEVT_BUTTON, &CSumFrame::Match, this);
    MatchButton->Disable();
    
    PathEntry = new wxTextCtrl(panel, wxID_ANY, *wxEmptyString, wxPoint(29, 144), wxSize(700, 30), wxTE_READONLY);
    HashEntry = new wxTextCtrl(panel, wxID_ANY, *wxEmptyString, wxPoint(29, 212), wxSize(700, 30), wxTE_READONLY);
    CheckHashEntry = new wxTextCtrl(panel, wxID_ANY, *wxEmptyString, wxPoint(29, 272), wxSize(700, 30), wxTE_READONLY);
    HashEntry->SetHint("Checksum");
    CheckHashEntry->SetHint("Match");

    AlgorithmEntry = new wxChoice(panel, wxID_ANY, wxPoint(149, 30), wxSize(176, 28), algorithms);
    AlgorithmEntry->SetSelection(0);

    Bind(wxEVT_SIZE, &CSumFrame::OnResizing, this);
}

void CSumFrame::OpenFileDialog(wxCommandEvent& event)
{
    CSumFrame::OpenFile();
}

void CSumFrame::CalculateHashThread(wxCommandEvent& event)
{
    Hash HashObject;

    std::thread HashT(&Hash::CalculateHash, &HashObject);
    HashT.detach();
}

void CSumFrame::SettingUpMenu()
{
    wxMenu* MenuFile = new wxMenu();
    wxMenu* MenuTools = new wxMenu();
    MenuFile->Append(ID_OpenFile, "&Open File\tCtrl+O");
    MenuFile->Append(ID_AboutApp, "&About");
    MenuFile->AppendSeparator();
    MenuFile->Append(ID_QuitApp, "&Quit\tAlt+F4");

    MenuTools->Append(ID_HFS, "Hash from String");
    MenuTools->Append(ID_ASE, "AES Symmetric Encryption");
    
    wxMenuBar* MenuBar = new wxMenuBar();
    MenuBar->Append(MenuFile, "&File");
    MenuBar->Append(MenuTools, "&Tools");

    Bind(wxEVT_MENU, &CSumFrame::OpenFileDialog, this, ID_OpenFile);
    Bind(wxEVT_MENU, &CSumFrame::QuitApp, this, ID_QuitApp);
    Bind(wxEVT_MENU, &CSumFrame::AboutApp, this, ID_AboutApp);

    Bind(wxEVT_MENU, &CSumFrame::Csumhfs, this, ID_HFS);
    Bind(wxEVT_MENU, &CSumFrame::Csumase, this, ID_ASE);

    SetMenuBar(MenuBar);
}

void CSumFrame::QuitApp(wxCommandEvent& event)
{
    Close(true);
} 

void CSumFrame::AboutApp(wxCommandEvent& event)
{
    wxString message = wxString::Format("CSum v%s\n\nA simple lightweight program written in C++ which can calculate hash of a given file from 4 different algorithms. Which can be a great way to detect authenticity of any file regarding its type.", version);
    wxString title = wxString::Format("CSum v%s", version);
    
    wxMessageBox(message, title, wxOK|wxICON_INFORMATION);
}

void CSumFrame::OpenFile()
{
    wxFileDialog openFileDialog(this, "Open File", "", "", "All Files (*.*)|*.*", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if(openFileDialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    wxString path = openFileDialog.GetPath();
    PathEntry->SetValue(path);
    CalculateButton->Enable();
}

void CSumFrame::Copy(wxCommandEvent& event)
{
    const size_t len = StrHash.length() + 1;
    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, len);
    memcpy(GlobalLock(memory), StrHash.c_str(), len);

    GlobalUnlock(memory);
    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, memory);
    CloseClipboard();
}

void CSumFrame::Match(wxCommandEvent& event)
{
    std::string GivenHash = CheckHashEntry->GetValue().ToStdString();

    for(char& ch: GivenHash)
    {
        ch = std::tolower(ch);
    }

    if(GivenHash == StrHash)
    {
        wxMessageBox("The hash matches.", "CSum Info", wxOK|wxICON_INFORMATION);
    }
    else
    {
        wxMessageBox("The hash does not match.", "CSum Info", wxOK|wxICON_WARNING);
    }
}

void CSumFrame::KeyboardEvent(wxKeyEvent& event)
{
    if (event.ControlDown() && event.GetKeyCode() == 'O')
    {
        CSumFrame::OpenFile();
    }
    if (event.AltDown() && event.GetKeyCode() == WXK_F4)
    {
        Close(true);
    }
}

void CSumFrame::OnResizing(wxSizeEvent& event)
{
    wxSize Resize = GetClientSize();
    unsigned int width = Resize.GetWidth();
    
    CopyButton->SetPosition(wxPoint(width-162, 212));
    MatchButton->SetPosition(wxPoint(width-162, 272));

    PathEntry->SetSize(wxSize(700+(width-905), 30));
    HashEntry->SetSize(wxSize(700+(width-905), 30));
    CheckHashEntry->SetSize(wxSize(700+(width-905), 30));

    event.Skip();
}

void CSumFrame::Csumhfs(wxCommandEvent& event)
{    
    hfsFrame* hfs = new hfsFrame("CSum | Hash from String", this);
    hfs->Show();
}

void CSumFrame::Csumase(wxCommandEvent& event)
{
    aseFrame* ase = new aseFrame("Csum | AES Symmetric Encryption", this);
    ase->Show();
}

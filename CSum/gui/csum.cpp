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
    ID_hfs = 14,
    ID_ase = 23
};

wxStaticText* CSumFrame::Status = nullptr;
wxButton* CSumFrame::CalculateHashButton = nullptr;
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
    SetMinSize(wxSize(905, 441));
    SetMaxSize(wxSize(1280, 441));
    
    wxFont TitleFont(20, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Gill Sans");
    wxFont LabelFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Gill Sans"); 
    wxFont ChoiceFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Gill Sans");
    wxFont SmallFont(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Gill Sans");

    wxArrayString algorithms;
    algorithms.Add("MD5");
    algorithms.Add("SHA1");
    algorithms.Add("SHA256/SHA2");

    version = ver;

    CSumFrame::SettingUpMenu();
    
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    panel->Bind(wxEVT_KEY_DOWN, &CSumFrame::KeyboardEvent, this);
    panel->SetFont(LabelFont);

    wxStaticText* Title = new wxStaticText(panel, wxID_ANY, "CSum", wxPoint(400, 25)); //Main Label
    Title->SetFont(TitleFont);
    wxStaticText* PathLabel = new wxStaticText(panel, wxID_ANY, "File Path:", wxPoint(10, 85));
    wxStaticText* ALabel = new wxStaticText(panel, wxID_ANY, "Algorithm", wxPoint(10, 155));
    wxStaticText* HashLabel = new wxStaticText(panel, wxID_ANY, "Hash:", wxPoint(10, 210));
    wxStaticText* CheckHashDescription = new wxStaticText(panel, wxID_ANY, "Match the hash", wxPoint(10, 273));
    Status = new wxStaticText(panel, wxID_ANY, *wxEmptyString, wxPoint(10, 350));
    Status->SetFont(SmallFont);

    OpenFileButton = new wxButton(panel, wxID_ANY, "Open File", wxPoint(10, 25), wxSize(95, 30));
    OpenFileButton->Bind(wxEVT_BUTTON, &CSumFrame::OpenFileDialog, this);

    CalculateHashButton = new wxButton(panel, wxID_ANY, "Hash", wxPoint(713, 81), wxSize(80, 25));
    CalculateHashButton->Bind(wxEVT_BUTTON, &CSumFrame::CalculateHashThread, this);
    CalculateHashButton->Disable();

    CopyButton = new wxButton(panel, wxID_ANY, "Copy", wxPoint(680, 206), wxSize(80, 25));
    CopyButton->Bind(wxEVT_BUTTON, &CSumFrame::Copy, this);
    CopyButton->Disable();

    RecalculateButton = new wxButton(panel, wxID_ANY, "Recalculate", wxPoint(780, 206), wxSize(110, 25));
    RecalculateButton->Bind(wxEVT_BUTTON, &CSumFrame::CalculateHashThread, this);
    RecalculateButton->Disable();

    MatchButton = new wxButton(panel, wxID_ANY, "Match", wxPoint(722, 300), wxSize(80, 25));
    MatchButton->Bind(wxEVT_BUTTON, &CSumFrame::Match, this);
    MatchButton->Disable();
    
    PathEntry = new wxTextCtrl(panel, wxID_ANY, *wxEmptyString, wxPoint(87, 83), wxSize(600, 25), wxTE_READONLY);
    HashEntry = new wxTextCtrl(panel, wxID_ANY, *wxEmptyString, wxPoint(67, 208), wxSize(600, 25), wxTE_READONLY);
    CheckHashEntry = new wxTextCtrl(panel, wxID_ANY, *wxEmptyString, wxPoint(10, 300), wxSize(700, 25), wxTE_READONLY);

    AlgorithmEntry = new wxChoice(panel, wxID_ANY, wxPoint(90, 155), wxSize(110, 90), algorithms);
    AlgorithmEntry->SetFont(ChoiceFont);
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

    MenuTools->Append(ID_hfs, "Hash from String");
    MenuTools->Append(ID_ase, "AES Symmetric Encryption");
    
    wxMenuBar* MenuBar = new wxMenuBar();
    MenuBar->Append(MenuFile, "&File");
    MenuBar->Append(MenuTools, "&Tools");

    Bind(wxEVT_MENU, &CSumFrame::OpenFileDialog, this, ID_OpenFile);
    Bind(wxEVT_MENU, &CSumFrame::QuitApp, this, ID_QuitApp);
    Bind(wxEVT_MENU, &CSumFrame::AboutApp, this, ID_AboutApp);

    Bind(wxEVT_MENU, &CSumFrame::Csumhfs, this, ID_hfs);
    Bind(wxEVT_MENU, &CSumFrame::Csumase, this, ID_ase);

    SetMenuBar(MenuBar);
}

void CSumFrame::QuitApp(wxCommandEvent& event)
{
    Close(true);
} 

void CSumFrame::AboutApp(wxCommandEvent& event)
{
    wxString message = wxString::Format("CSum v%s\n\nA simple lightweight program written in C++ which can calculate hash of a given file from 3 different algorithms. Which can be a great way to detect authenticity of any file regarding its type.", version);
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
    CalculateHashButton->Enable();
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
    
    CalculateHashButton->SetPosition(wxPoint(width-192, 81));
    CopyButton->SetPosition(wxPoint(width-225, 206));
    RecalculateButton->SetPosition(wxPoint(width-125, 206));
    MatchButton->SetPosition(wxPoint(width-183, 300));

    PathEntry->SetSize(wxSize(600+(width-905), 25));
    HashEntry->SetSize(wxSize(600+(width-905), 25));
    CheckHashEntry->SetSize(wxSize(700+(width-905), 25));

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
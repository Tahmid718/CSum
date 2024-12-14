#include "ase.h"
#include "../hash/calculate.h"

#include <wx/filedlg.h>
#include <fstream>
#include <iomanip>

wxArrayString Ciphers, Keys, Options;

wxTextCtrl* aseFrame::TextEntry = nullptr;
wxTextCtrl* aseFrame::KeyEntry = nullptr;
wxChoice* aseFrame::KeySizeChoice = nullptr;
wxRadioBox* aseFrame::PerformWhat = nullptr;
wxChoice* aseFrame::CipherChoice = nullptr;
wxTextCtrl* aseFrame::IvEntry = nullptr;

std::vector<unsigned char> VectoredHash;
std::string aseHash;

wxFont DefaultFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Segoe UI");

class aseOutput: public wxFrame
{
    public:
        aseOutput(const wxString& title, wxWindow* parent);
    private:
        void CopyCipher(wxCommandEvent& event);
        void SaveFileasStr(wxCommandEvent& event);
        void SaveFileasBin(wxCommandEvent& event);
};

aseOutput::aseOutput(const wxString& title, wxWindow* parent)
    : wxFrame(parent,wxID_ANY, title, wxDefaultPosition, wxSize(830, 500), wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER|wxMAXIMIZE_BOX))
{   
    wxPanel* panel = new wxPanel(this);
    
    panel->SetFont(DefaultFont);

    wxStaticText* Label = new wxStaticText(panel, wxID_ANY, "AES Symmetric Encryption Output", wxPoint(40, 30));

    wxTextCtrl* OutputEntry = new wxTextCtrl(panel, wxID_ANY, *wxEmptyString, wxPoint(40, 65), wxSize(635, 365), wxTE_MULTILINE|wxTE_READONLY);
    OutputEntry->SetValue(aseHash);

    wxButton* CopyButton = new wxButton(panel, wxID_ANY, "Copy", wxPoint(700, 65), wxSize(100, 40));
    CopyButton->Bind(wxEVT_BUTTON, &aseOutput::CopyCipher, this);
    wxButton* SaveFileButton = new wxButton(panel, wxID_ANY, "Save output", wxPoint(700, 145), wxSize(100, 40));
    SaveFileButton->Bind(wxEVT_BUTTON, &aseOutput::SaveFileasStr, this);
    wxButton* SaveFileBinaryButton = new wxButton(panel, wxID_ANY, "Save output\nas Binary", wxPoint(700, 205), wxSize(100, 40));
    SaveFileBinaryButton->Bind(wxEVT_BUTTON, &aseOutput::SaveFileasBin, this);
}

aseFrame::aseFrame(const wxString& title, wxWindow* parent)
    : wxFrame(parent, wxID_ANY, title, wxDefaultPosition, wxSize(800, 540), wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER|wxMAXIMIZE_BOX))
{
    Ciphers.Clear();
    Keys.Clear();
    Options.Clear();   
    
    Ciphers.Add("CBC");
    Ciphers.Add("ECB");
    Ciphers.Add("CTR");

    Keys.Add("128 Bits");
    Keys.Add("192 Bits");
    Keys.Add("256 Bits");

    Options.Add("Encrypt      ");
    Options.Add("Decrypt");
        
    wxPanel* panel = new wxPanel(this);
    panel->SetFont(DefaultFont);

    wxStaticText* KeyLabel = new wxStaticText(panel, wxID_ANY, "Secret Key", wxPoint(20, 30));
    wxStaticText* CipherLabel = new wxStaticText(panel, wxID_ANY, "Cipher Mode", wxPoint(20, 100));
    wxStaticText* SizeLabel = new wxStaticText(panel, wxID_ANY, "Key Size", wxPoint(270, 100));
    wxStaticText* Text_ = new wxStaticText(panel, wxID_ANY, "Plain/Encrypted Text", wxPoint(20, 140));
    wxStaticText* IvLabel = new wxStaticText(panel, wxID_ANY, "IV", wxPoint(420, 30));
    TextLength = new wxStaticText(panel, wxID_ANY, *wxEmptyString, wxPoint(20, 468));

    KeyEntry = new wxTextCtrl(panel, wxID_ANY, *wxEmptyString, wxPoint(96, 29), wxSize(280, -1));
    KeyEntry->SetMaxLength(32);
    IvEntry = new wxTextCtrl(panel, wxID_ANY, *wxEmptyString, wxPoint(446, 29), wxSize(240, -1));
    IvEntry->SetMaxLength(16);
    TextEntry = new wxTextCtrl(panel, wxID_ANY, *wxEmptyString, wxPoint(20, 160), wxSize(650, 285), wxTE_MULTILINE);
    TextEntry->Bind(wxEVT_TEXT, &aseFrame::SetLength, this);

    wxButton* PerformButton = new wxButton(panel, wxID_ANY, "Perform", wxPoint(690, 170), wxSize(75, 35));
    PerformButton->Bind(wxEVT_BUTTON, &aseFrame::Performation, this);
    wxButton* Q = new wxButton(panel, wxID_ANY, "?", wxPoint(700, 29), wxSize(20, 20));
    Q->Bind(wxEVT_BUTTON, &aseFrame::IvInfo, this);

    CipherChoice = new wxChoice(panel, wxID_ANY, wxPoint(110, 97), wxSize(100, -1), Ciphers);
    CipherChoice->SetSelection(0);
    KeySizeChoice = new wxChoice(panel, wxID_ANY, wxPoint(335, 98), wxSize(100, -1), Keys);
    KeySizeChoice->SetSelection(2);

    PerformWhat = new wxRadioBox(panel, wxID_ANY, "Do what?", wxPoint(488, 80), wxSize(184, -1), Options);
}

void aseFrame::SetLength(wxCommandEvent& event)
{
    int length_ = TextEntry->GetValue().ToStdString().length();
    
    if(length_ == 0)
    {
        TextLength->SetLabel(*wxEmptyString);
        return;
    }
    wxString LengthString = wxString::Format("Text Length: %d", length_);

    TextLength->SetLabel(LengthString);
}

void aseFrame::Performation(wxCommandEvent& event)
{
    Hash HashObject;
    VectoredHash = HashObject.Symmetric();

    if(VectoredHash.size() > 0)
    {
        if(!PerformWhat->GetSelection())
        {
            std::ostringstream Hash_;
            for(unsigned char c: VectoredHash)
            {
                Hash_ << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
            }
            aseHash = Hash_.str();    
        }
        else
        {
            std::string Hash_(VectoredHash.begin(), VectoredHash.end());
            aseHash = Hash_;
        }

        aseOutput* aseObject = new aseOutput("CSum | ASE Output", wxTheApp->GetTopWindow());
        aseObject->Show();
    }
    else
    {
        wxMessageBox("The program returned an empty vector. Recheck your Input/Key/IV.", "CSum AES Symmetric Encryption", wxOK|wxICON_ERROR);
    }
}

void aseOutput::CopyCipher(wxCommandEvent& event)
{
    const size_t len = aseHash.length() + 1;
    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, len);
    memcpy(GlobalLock(memory), aseHash.c_str(), len);

    GlobalUnlock(memory);
    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, memory);
    CloseClipboard();
}

void aseOutput::SaveFileasStr(wxCommandEvent& event)
{
    wxFileDialog FileDialog(this, "Save File", "", "", "All Files (*.*)|*.*", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

    if(FileDialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    std::string Path = FileDialog.GetPath().ToStdString();

    std::ofstream File(Path);
    File << aseHash;

    File.close();
}

void aseOutput::SaveFileasBin(wxCommandEvent& event)
{
    wxFileDialog FileDialog(this, "Save File", "", "", "All Files (*.*)|*.*", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

    if(FileDialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    std::string Path = FileDialog.GetPath().ToStdString();

    std::ofstream File(Path, std::ios::binary);

    File.write(reinterpret_cast<const char*>(VectoredHash.data()), VectoredHash.size());

    File.close();
}

void aseFrame::IvInfo(wxCommandEvent& event)
{
    wxMessageBox("Initialize Vector (IV) is a 128 bit string which is used in CBC and CTR encryption mode in order to make the cipher text to be unique. It's recommended to use a randomly generated IV, leave this field empty for that.", "CSum | AES Symmetric Encryption");
}
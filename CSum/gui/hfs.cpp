#include "hfs.h"
#include "../hash/calculate.h"

#include <vector>

Hash HashObject;

wxChoice* hfsFrame::AlgorithmChoice = nullptr;
wxTextCtrl* hfsFrame::InputHash = nullptr;

hfsFrame::hfsFrame(const wxString& title, wxWindow* parent)
    : wxFrame(parent, wxID_ANY, title, wxDefaultPosition, wxSize(800, 500), wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER|wxMAXIMIZE_BOX))
{
    wxFont DefaultFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Gill Sans"); 
    
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    panel->SetFont(DefaultFont);

    wxArrayString Algorithmhfs;
    Algorithmhfs.Add("MD5");
    Algorithmhfs.Add("SHA1");
    Algorithmhfs.Add("SHA2/SHA256");

    SeperateString = new wxCheckBox(panel, wxID_ANY, "Treat each line as different string.", wxPoint(45, 30), wxSize(400, -1));

    AlgorithmChoice = new wxChoice(panel, wxID_ANY, wxPoint(570, 30), wxSize(110, 90), Algorithmhfs);
    AlgorithmChoice->SetSelection(0);

    InputString = new wxTextCtrl(panel, wxID_ANY, *wxEmptyString, wxPoint(45, 90), wxSize(700, 160), wxTE_MULTILINE);
    InputString->Bind(wxEVT_TEXT, &hfsFrame::hfsInstrument, this);

    InputHash = new wxTextCtrl(panel, wxID_ANY, *wxEmptyString, wxPoint(45, 280), wxSize(700, 160), wxTE_MULTILINE|wxTE_READONLY);
}

std::vector<std::string> splitlines(std::string& tofilter)
{
    std::vector<std::string>filtered;
    size_t start = 0;
    size_t length = tofilter.length();

    for(size_t i=0; i<length; ++i)
    {
        if(tofilter[i] == '\n')
        {
            filtered.push_back(tofilter.substr(start, i-start));
            start = i+1;
        }
    }

    if(start < length)
    {
        filtered.push_back(tofilter.substr(start));
    }

    return filtered;
}

void hfsFrame::hfsInstrument(wxCommandEvent& event)
{    
    std::vector<std::string> strings;
    std::string input_ = InputString->GetValue().ToStdString();
    int AIndex = AlgorithmChoice->GetSelection();

    if(AIndex > 2)
    {
        return;
    }

    if (input_.length() == 0)
    {
        InputHash->SetValue("");
        return;
    }

    if(SeperateString->IsChecked())
    {
        strings = splitlines(input_);
    }
    else
    {
        strings.push_back(input_);
    }

    HashObject.hfsCalculation(strings);
}
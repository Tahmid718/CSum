#ifndef HFS_H
#define HFS_H

#include <wx/wx.h>

class hfsFrame: public wxFrame
{
    public:
        static wxChoice* AlgorithmChoice;
        static wxTextCtrl* InputHash;

        hfsFrame(const wxString& title, wxWindow* parent);
    private:
        wxCheckBox* SeperateString;
        wxTextCtrl* InputString;

        void hfsInstrument(wxCommandEvent& event);
};

#endif
#ifndef ASE_H
#define ASE_H

#include <wx/wx.h>

class aseFrame: public wxFrame
{
    public:
        wxStaticText* TextLength;
        static wxTextCtrl* TextEntry;
        static wxTextCtrl* KeyEntry;
        static wxChoice* KeySizeChoice;
        static wxRadioBox* PerformWhat;
        static wxChoice* CipherChoice;
        static wxTextCtrl* IvEntry;
        
        aseFrame(const wxString& title, wxWindow* parent);
        
    private:
        void SetLength(wxCommandEvent& event);
        void Performation(wxCommandEvent& event);
        void IvInfo(wxCommandEvent& event);
};

#endif
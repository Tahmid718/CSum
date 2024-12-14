#ifndef CSUM_H
#define CSUM_H

#include <wx/wx.h>

class CSumFrame: public wxFrame
{
    public:
        static wxButton* CalculateButton;
        static wxButton* CopyButton;
        static wxButton* RecalculateButton;
        static wxButton* MatchButton;
        static wxButton* OpenFileButton;
        static wxTextCtrl* PathEntry;
        static wxTextCtrl* HashEntry;
        static wxTextCtrl* CheckHashEntry;
        static wxChoice* AlgorithmEntry;
        static wxStatusBar* StatusBar;

        static std::string StrHash;

        CSumFrame(const wxString& title, std::string ver);

    private:
        std::string version;
        
        void SettingUpMenu();
        void OpenFile();
        void QuitApp(wxCommandEvent& event);
        void AboutApp(wxCommandEvent& event);
        void OpenFileDialog(wxCommandEvent& event);
        void KeyboardEvent(wxKeyEvent& event);
        void CalculateHashThread(wxCommandEvent& event);
        void Copy(wxCommandEvent& event);
        void Match(wxCommandEvent& event);
        void OnResizing(wxSizeEvent& event);
        void Csumhfs(wxCommandEvent& event);
        void Csumase(wxCommandEvent& event);
};

#endif
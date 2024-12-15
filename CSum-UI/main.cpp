/////////////////////////////
// Main Source of CSum.exe 
// November 18, 2024 

#include "gui/csum.h"

#include <wx/wx.h>
#include <fstream>
#include <windows.h>

/* VERSION */
std::string version = "1.2";

wxStatusBar* CSumFrame::StatusBar = nullptr;

wxIMPLEMENT_APP_NO_MAIN(wxApp);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, LPSTR lcmdline, int nCmdShow)
{
    if(!wxEntryStart(hInstance, hInstancePrev, lcmdline, nCmdShow))
    {
        MessageBoxW(NULL, L"The app cannot be run due to an unknown error.", L"CSum Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    wxTheApp->CallOnInit();

    CSumFrame* frame = new CSumFrame("CSum", version);

    // wxStatusBar() needs to be called from init.
    // Since its referenced in calculate.cpp.
    CSumFrame::StatusBar = frame->CreateStatusBar(1);

    frame->SetClientSize(905, 391);

    // Displaying the frame.
    frame->Show();

    int result = wxTheApp->OnRun();
    wxEntryCleanup();

    return result; // <- Useless ¯\_(ツ)_/¯
}

// End of source.

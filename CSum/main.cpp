#include "gui/csum.h"

#include <wx/wx.h>
#include <fstream>
#include <windows.h>

/* VERSION */
std::string version = "1.0";

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
    frame->SetClientSize(905, 391);
    frame->Show();

    int result = wxTheApp->OnRun();
    wxEntryCleanup();

    return result;
}
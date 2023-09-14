
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include "wx/log.h"
#endif

#include "MainFrame.hpp"

#include <atomic>

std::atomic<uint32_t> g_num_failed_files = 0;


// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type
class cCeresDataRepairApp : public wxApp
{
public:
    cCeresDataRepairApp();
    virtual ~cCeresDataRepairApp();

    virtual bool OnInit() wxOVERRIDE;
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// The CeresConvert Application
// ----------------------------------------------------------------------------

cCeresDataRepairApp::cCeresDataRepairApp()
{
    SetAppDisplayName("Ceres Data Repair");
}

cCeresDataRepairApp::~cCeresDataRepairApp()
{
}

// `Main program' equivalent, creating windows and returning main app frame
bool cCeresDataRepairApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    // Create the main frame window
    cMainFrame* frame = new cMainFrame();

    // Show the frame
    frame->Show(true);

    return true;
}


// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type
// (i.e. cCeresDataRepairApp and not wxApp)
wxIMPLEMENT_APP(cCeresDataRepairApp);

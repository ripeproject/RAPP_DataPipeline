
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include "wx/log.h"
#endif

#include "MainFrame.hpp"

#include <atomic>


// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type
class cCeresFileRepairApp : public wxApp
{
public:
    cCeresFileRepairApp();
    virtual ~cCeresFileRepairApp();

    virtual bool OnInit() wxOVERRIDE;
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// The Ceres File Repair Application
// ----------------------------------------------------------------------------

cCeresFileRepairApp::cCeresFileRepairApp()
{
    SetAppDisplayName("Ceres File Repair");
}

cCeresFileRepairApp::~cCeresFileRepairApp()
{
}

// `Main program' equivalent, creating windows and returning main app frame
bool cCeresFileRepairApp::OnInit()
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
// (i.e. cCeresFileRepairApp and not wxApp)
wxIMPLEMENT_APP(cCeresFileRepairApp);

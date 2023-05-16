
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include "wx/log.h"
#endif

#include "MainFrame.hpp"


// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type
class cCeresConvertApp : public wxApp
{
public:
    cCeresConvertApp();
    virtual ~cCeresConvertApp();

    virtual bool OnInit() wxOVERRIDE;
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// The CeresConvert Application
// ----------------------------------------------------------------------------

cCeresConvertApp::cCeresConvertApp()
{
    SetAppDisplayName("Plot Split");
}

cCeresConvertApp::~cCeresConvertApp()
{
}

// `Main program' equivalent, creating windows and returning main app frame
bool cCeresConvertApp::OnInit()
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
// (i.e. cCeresInfoApp and not wxApp)
wxIMPLEMENT_APP(cCeresConvertApp);

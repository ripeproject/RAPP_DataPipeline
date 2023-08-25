
#pragma once

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wx/dataview.h"

// Forward Declarations
class cFileProgressDataModel;

class cFileProgressCtrl: public wxDataViewCtrl
{
public:
    cFileProgressCtrl();
    cFileProgressCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxDataViewCtrlNameStr);

    void Append(const wxString& text);

private:
    wxObjectDataPtr<cFileProgressDataModel> mpProgressModel;
};


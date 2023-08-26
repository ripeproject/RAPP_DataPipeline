
#pragma once

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wx/dataview.h"

#include <map>

// Forward Declarations
class cFileProgressDataModel;

//
class cFileProgressEvent : public wxEvent
{
public:
    cFileProgressEvent(wxEventType eventType, int id = 0);

    // You *must* copy here the data to be transported
    cFileProgressEvent(const cFileProgressEvent& event);

    // Required for sending with wxPostEvent()
    wxEvent* Clone() const;

    int GetFileProcessID() const;
    void SetFileProcessID(int id);

    wxString GetFileName() const;
    void SetFileName(const wxString& file_name);

    int GetProcess_pct() const;
    void SetProgress_pct(int progress_pct);

private:
    int mFileProcessID = 0; // <- id assigned to the file progress
    int mProgress_pct = 0;  // <- range is o to 100

    wxString mFileName;
};

wxDECLARE_EVENT(NEW_FILE_PROGRESS, cFileProgressEvent);
wxDECLARE_EVENT(UPDATE_FILE_PROGRESS, cFileProgressEvent);


// The File Progress Control is a wxWidget that displays a three columns:
//  timestamp   Filename    Progress
//
class cFileProgressCtrl: public wxDataViewCtrl
{
public:
    cFileProgressCtrl();
    cFileProgressCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxDataViewCtrlNameStr);

    int Append(const wxString& text);

    // Event Handlers
    void OnNewFileProgress(cFileProgressEvent& event);
    void OnUpdateFileProgress(cFileProgressEvent& event);

private:
    std::map<int, int>  mID_to_Row;
    wxObjectDataPtr<cFileProgressDataModel> mpProgressModel;
};


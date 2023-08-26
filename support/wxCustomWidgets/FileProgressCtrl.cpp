
#include "FileProgressCtrl.hpp"
#include "FileProgressDataModel.hpp"

#include <wx/event.h>


wxDEFINE_EVENT(NEW_FILE_PROGRESS, cFileProgressEvent);
wxDEFINE_EVENT(UPDATE_FILE_PROGRESS, cFileProgressEvent);


cFileProgressEvent::cFileProgressEvent(wxEventType eventType, int id)
:
	wxEvent(id, eventType) 
{}

// You *must* copy here the data to be transported
cFileProgressEvent::cFileProgressEvent(const cFileProgressEvent& event)
:
    wxEvent(event), mFileProcessID(event.mFileProcessID), mFileName(event.mFileName), mProgress_pct(event.mProgress_pct)
{}

wxEvent* cFileProgressEvent::Clone() const 
{
    return new cFileProgressEvent(*this); 
}

int cFileProgressEvent::GetFileProcessID() const
{
    return mFileProcessID;
}

void cFileProgressEvent::SetFileProcessID(int id)
{
    mFileProcessID = id;
}

wxString cFileProgressEvent::GetFileName() const
{
    return mFileName;
}

void cFileProgressEvent::SetFileName(const wxString& file_name)
{
    mFileName = file_name;
}

int cFileProgressEvent::GetProcess_pct() const
{
    return mProgress_pct;
}

void cFileProgressEvent::SetProgress_pct(int progress_pct)
{
    mProgress_pct = progress_pct;
}




cFileProgressCtrl::cFileProgressCtrl()
	:
	wxDataViewCtrl()
{
	mpProgressModel = new cFileProgressDataModel();
	AssociateModel(mpProgressModel.get());
}

cFileProgressCtrl::cFileProgressCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name)
:
	wxDataViewCtrl(parent, id, pos, size, style, validator, name)
{
	mpProgressModel = new cFileProgressDataModel();
	AssociateModel(mpProgressModel.get());

	auto timestamp = AppendTextColumn("Time", cFileProgressDataModel::Col_Timestamp);
	timestamp->SetAlignment(wxALIGN_CENTER_HORIZONTAL);

	auto filename = AppendTextColumn("File Name", cFileProgressDataModel::Col_Filename, wxDATAVIEW_CELL_INERT, 400);
	filename->SetAlignment(wxALIGN_CENTER_HORIZONTAL);
	filename->SetMinWidth(300);

	auto progress = AppendProgressColumn("Progress", cFileProgressDataModel::Col_Progress);
	progress->SetAlignment(wxALIGN_CENTER_HORIZONTAL);

	Bind(NEW_FILE_PROGRESS, &cFileProgressCtrl::OnNewFileProgress, this);
	Bind(UPDATE_FILE_PROGRESS, &cFileProgressCtrl::OnUpdateFileProgress, this);
}

int cFileProgressCtrl::Append(const wxString& text)
{
	auto row = mpProgressModel->Append(text);
	Refresh();

	return row;
}

void cFileProgressCtrl::OnNewFileProgress(cFileProgressEvent& event)
{
	auto row = Append(event.GetFileName());
	mID_to_Row[event.GetFileProcessID()] = row;
}

void cFileProgressCtrl::OnUpdateFileProgress(cFileProgressEvent& event)
{
	auto row = mID_to_Row[event.GetFileProcessID()];
	mpProgressModel->SetValueByRow(wxVariant(event.GetProcess_pct()), row, 2);
	Refresh();
}

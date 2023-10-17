
#include "FileProgressCtrl.hpp"
#include "FileProgressDataModel.hpp"

#include <wx/event.h>


wxDEFINE_EVENT(NEW_FILE_PROGRESS, cFileProgressEvent);
wxDEFINE_EVENT(UPDATE_FILE_PROGRESS, cFileProgressEvent);
wxDEFINE_EVENT(COMPLETE_FILE_PROGRESS, cFileProgressEvent);


cFileProgressEvent::cFileProgressEvent(wxEventType eventType, int id)
:
	wxEvent(id, eventType) 
{}

cFileProgressEvent::~cFileProgressEvent()
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

wxString cFileProgressEvent::GetPrefix() const
{
	return mPrefix;
}

void cFileProgressEvent::SetPrefix(const wxString& prefix)
{
	mPrefix = prefix;
}

int cFileProgressEvent::GetProcess_pct() const
{
    return mProgress_pct;
}

void cFileProgressEvent::SetProgress_pct(int progress_pct)
{
    mProgress_pct = progress_pct;
}

wxString cFileProgressEvent::GetResult() const
{
	return mResult;
}

void cFileProgressEvent::SetResult(const wxString& result)
{
	mResult = result;
}

//EVT_DATAVIEW_ITEM_VALUE_CHANGED(wxEVT_DATAVIEW_ITEM_VALUE_CHANGED, func)

cFileProgressCtrl::cFileProgressCtrl()
	:
	wxDataViewCtrl()
{
	mpProgressModel = new cFileProgressDataModel();
	AssociateModel(mpProgressModel.get());
}

cFileProgressCtrl::cFileProgressCtrl(wxWindow* parent, wxWindowID id,
	const wxString& prefixColumnLabel, const int prefixWidth, const wxString& resultColumnLabel, const int resultWidth,
	const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name)
:
	wxDataViewCtrl(parent, id, pos, size, style | wxDV_HORIZ_RULES | wxDV_VERT_RULES, validator, name)
{
	mpProgressModel = new cFileProgressDataModel();
	AssociateModel(mpProgressModel.get());

	auto timestamp = AppendTextColumn("Time", cFileProgressDataModel::columnTimestamp);
	timestamp->SetAlignment(wxALIGN_CENTER_HORIZONTAL);

	auto filename = AppendTextColumn("File Name", cFileProgressDataModel::columnFilename, wxDATAVIEW_CELL_INERT, 400);
	filename->SetAlignment(wxALIGN_CENTER_HORIZONTAL);
	filename->SetMinWidth(300);

	if (!prefixColumnLabel.empty() && (prefixWidth > 0))
	{
		auto result = AppendTextColumn(prefixColumnLabel, cFileProgressDataModel::columnPrefix, wxDATAVIEW_CELL_INERT, prefixWidth);
		result->SetAlignment(wxALIGN_CENTER_HORIZONTAL);
		result->SetMinWidth(prefixWidth);
	}

	auto progress = AppendProgressColumn("Progress", cFileProgressDataModel::columnProgress);
	progress->SetAlignment(wxALIGN_CENTER_HORIZONTAL);

	if (!resultColumnLabel.empty() && (resultWidth > 0))
	{
		auto result = AppendTextColumn(resultColumnLabel, cFileProgressDataModel::columnResult, wxDATAVIEW_CELL_INERT, resultWidth);
		result->SetAlignment(wxALIGN_CENTER_HORIZONTAL);
		result->SetMinWidth(resultWidth);
	}

	Bind(NEW_FILE_PROGRESS, &cFileProgressCtrl::OnNewFileProgress, this);
	Bind(UPDATE_FILE_PROGRESS, &cFileProgressCtrl::OnUpdateFileProgress, this);
	Bind(COMPLETE_FILE_PROGRESS, &cFileProgressCtrl::OnCompleteFileProgress, this);
}

std::size_t cFileProgressCtrl::Append(const wxString& text)
{
	// Freeze the window to prevent scrollbar jumping
	Freeze();

	auto row = mpProgressModel->Append(text);

	EnsureVisible(mpProgressModel->GetItem(row));

	// Allow the window to redraw
	Thaw();

	Refresh();

	return row;
}

std::size_t cFileProgressCtrl::Append(const wxString& text, const wxString& prefix)
{
	// Freeze the window to prevent scrollbar jumping
	Freeze();

	auto row = mpProgressModel->Append(text, prefix);

	EnsureVisible(mpProgressModel->GetItem(row));

	// Allow the window to redraw
	Thaw();

	Refresh();

	return row;
}

std::size_t cFileProgressCtrl::Append(const wxString& text, const wxString& prefix, const wxString& result)
{
	// Freeze the window to prevent scrollbar jumping
	Freeze();

	auto row = mpProgressModel->Append(text, prefix, result);

	EnsureVisible(mpProgressModel->GetItem(row));

	// Allow the window to redraw
	Thaw();

	Refresh();

	return row;
}

void cFileProgressCtrl::OnNewFileProgress(cFileProgressEvent& event)
{
	size_t row = 0;

	row = Append(event.GetFileName(), event.GetPrefix(), event.GetResult());

	mID_to_Row[event.GetFileProcessID()] = row;
}

void cFileProgressCtrl::OnUpdateFileProgress(cFileProgressEvent& event)
{
	auto row = mID_to_Row[event.GetFileProcessID()];
	if (!event.GetFileName().empty())
	{
		mpProgressModel->SetValueByRow(wxVariant(event.GetFileName()), row, cFileProgressDataModel::columnFilename);
	}

	if (!event.GetPrefix().empty())
	{
		mpProgressModel->SetValueByRow(wxVariant(event.GetPrefix()), row, cFileProgressDataModel::columnPrefix);
	}

	if (!event.GetResult().empty())
	{
		mpProgressModel->SetValueByRow(wxVariant(event.GetResult()), row, cFileProgressDataModel::columnResult);
	}

	mpProgressModel->SetValueByRow(wxVariant(event.GetProcess_pct()), row, cFileProgressDataModel::columnProgress);
	Refresh();
}

void cFileProgressCtrl::OnCompleteFileProgress(cFileProgressEvent& event)
{
	auto row = mID_to_Row[event.GetFileProcessID()];
	if (!event.GetFileName().empty())
	{
		mpProgressModel->SetValueByRow(wxVariant(event.GetFileName()), row, cFileProgressDataModel::columnFilename);
	}

	if (!event.GetPrefix().empty())
	{
		mpProgressModel->SetValueByRow(wxVariant(event.GetPrefix()), row, cFileProgressDataModel::columnPrefix);
	}

	if (!event.GetResult().empty())
	{
		mpProgressModel->SetValueByRow(wxVariant(event.GetResult()), row, cFileProgressDataModel::columnResult);
	}

	mpProgressModel->SetValueByRow(wxVariant(100), row, cFileProgressDataModel::columnProgress);
	Refresh();
}


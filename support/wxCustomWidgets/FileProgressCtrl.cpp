
#include "FileProgressCtrl.hpp"
#include "FileProgressDataModel.hpp"


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
}

void cFileProgressCtrl::Append(const wxString& text)
{
	mpProgressModel->Append(text);
	Refresh();
}

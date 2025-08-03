
#include "ResultsDlg.hpp"


// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(cResultsDlg, wxDialog)
//EVT_CLOSE(cResultsDlg::OnClose)
wxEND_EVENT_TABLE()



//-----------------------------------------------------------------------------
cResultsDlg::cResultsDlg()
	: wxDialog(NULL, wxID_ANY, "Daily Check Results")
{
}

cResultsDlg::~cResultsDlg()
{
}

void cResultsDlg::setFileVerificationResults(int num_failed)
{
	mNumFailedFiles = num_failed;
}

void cResultsDlg::setFileRepairResults(int num_partial_repair, int num_fully_repaired)
{
	mNumPartialRepairFiles = num_partial_repair;
	mNumRepairedFiles = num_fully_repaired;
}

void cResultsDlg::setDataVerificationResults(int num_failed, int num_invalid_data, int num_missing_data)
{
	mNumFailedDataFiles = num_failed;
	mNumInvalidDataFiles = num_invalid_data;
	mNumDataFilesMissingData = num_missing_data;
}

void cResultsDlg::setDataRepairResults(int num_partial_repair, int num_fully_repaired)
{
	mNumPartialRepairDataFiles = num_partial_repair;
	mNumRepairedDataFiles = num_fully_repaired;
}

int cResultsDlg::ShowModal()
{
//	wxPanel* panel = new wxPanel(this, -1);

	wxGridSizer* gbox = new wxGridSizer(4);

	gbox->AddSpacer(5);

	wxString label = "Number of files failing verification:";
	wxStaticText* text = new wxStaticText(this, wxID_ANY, label);
	gbox->Add(text, 1);

	gbox->AddStretchSpacer();

	label = wxString::Format("%d", mNumFailedFiles);
	text = new wxStaticText(this, wxID_ANY, label);
	gbox->Add(text, 1);

	gbox->AddSpacer(5);

	label = "Number of files partially repaired:";
	text = new wxStaticText(this, wxID_ANY, label);
	gbox->Add(text, 1);

	gbox->AddStretchSpacer();

	label = wxString::Format("%d", mNumPartialRepairFiles);
	text = new wxStaticText(this, wxID_ANY, label);
	gbox->Add(text, 1);

	gbox->AddSpacer(0);

	label = "Number of files fully repaired:";
	text = new wxStaticText(this, wxID_ANY, label);
	gbox->Add(text, 1);

	gbox->AddStretchSpacer();

	label = wxString::Format("%d", mNumRepairedFiles);
	text = new wxStaticText(this, wxID_ANY, label);
	gbox->Add(text, 1);

	gbox->AddSpacer(5);

	wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
	wxButton* okButton = new wxButton(this, wxID_OK, wxT("Ok"), wxDefaultPosition, wxSize(70, 30));
	hbox->Add(okButton, 1);

	wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
	vbox->Add(gbox, 1);
	vbox->Add(hbox, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

	SetSizer(vbox);

	Centre();	
	
	return wxDialog::ShowModal();
}

// event handlers

/*
void cMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo info;
	info.SetName(wxTheApp->GetAppDisplayName());
	info.SetVersion("1.0", "0.1.0");
	info.SetDescription(_("Verifies the data integrity of Ceres data files.\n"));
	info.SetCopyright(wxT("Copyright (c) 2023, Carl R. Woese Institute for Genomic Biology,\n"
		"University of Illinois.\n"
		"All rights reserved.\n"));
	info.SetIcon(wxICON(CeresDataVerification));
	info.AddDeveloper("Brett Feddersen");
	info.SetLicense("BSD 3 - Clause License\n"
		"\n"
		"This license applies to all files in the data_verificator repository and source\n"
		"distribution.This includes data_verificator’s source code, the examples, and\n"
		"tests, as well as the documentation.\n"
		"\n"
		"Copyright(c) 2023, Carl R.Woese Institute for Genomic Biology\n"
		"All rights reserved.\n"
		"\n"
		"Redistribution and use in source and binary forms, with or without\n"
		"modification, are permitted provided that the following conditions are met :\n"
		"\n"
		"1. Redistributions of source code must retain the above copyright notice, this\n"
		"   list of conditions and the following disclaimer.\n"

		"2. Redistributions in binary form must reproduce the above copyright notice,\n"
		"   this list of conditions and the following disclaimer in the documentation\n"
		"   and /or other materials provided with the distribution.\n"
		"\n"
		"3. Neither the name of the copyright holder nor the names of its\n"
		"   contributors may be used to endorse or promote products derived from\n"
		"   this software without specific prior written permission.\n"
		"\n"
		"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"\n"
		"AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n"
		"IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n"
		"DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE\n"
		"FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL\n"
		"DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR\n"
		"SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\n"
		"CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,\n"
		"OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE\n"
		"OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.");

	wxAboutBox(info);
}
*/

/*
void cResultsDlg::OnClose(wxCloseEvent&)
{
}
*/



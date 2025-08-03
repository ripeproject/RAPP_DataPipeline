
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
	wxString label;
	wxStaticText* text = nullptr;

	wxPanel* panel = new wxPanel(this, wxID_ANY);

	wxBoxSizer* panel_box = new wxBoxSizer(wxVERTICAL);

	panel_box->AddSpacer(20);

	if (mNumFailedFiles == 0)
	{
		wxBoxSizer* passed_box = new wxBoxSizer(wxHORIZONTAL);
		passed_box->AddSpacer(20);
		label = "All files passed file verification.";
		text = new wxStaticText(this, wxID_ANY, label);
		passed_box->Add(text, 1, wxALIGN_CENTER);
		passed_box->AddSpacer(20);
		panel_box->Add(passed_box, 1, wxALIGN_CENTER);
	}
	else
	{
		wxBoxSizer* failed_box = new wxBoxSizer(wxHORIZONTAL);

		failed_box->AddSpacer(20);

		label = "Number of files failing verification:";
		text = new wxStaticText(this, wxID_ANY, label);
		failed_box->Add(text, 1);

		label = wxString::Format("%d", mNumFailedFiles);
		failed_box->AddSpacer(100 - label.length());

		label = wxString::Format("%d", mNumFailedFiles);
		text = new wxStaticText(this, wxID_ANY, label);
		failed_box->Add(text, 0, wxALIGN_RIGHT);
		failed_box->AddSpacer(20);

		panel_box->Add(failed_box, 1);
		panel_box->AddSpacer(20);

		if ((mNumPartialRepairFiles == 0) && (mNumRepairedFiles == mNumFailedFiles))
		{
			label = "All files were able to be repaired.";
			text = new wxStaticText(this, wxID_ANY, label);
			panel_box->Add(text, 1, wxALIGN_CENTER);
		}
		else
		{
			wxBoxSizer* repaired_box = new wxBoxSizer(wxHORIZONTAL);

			repaired_box->AddSpacer(20);

			label = "Number of files partially repaired:";
			text = new wxStaticText(this, wxID_ANY, label);
			repaired_box->Add(text, 1);

			label = wxString::Format("%d", mNumPartialRepairFiles);
			repaired_box->AddSpacer(112 - label.length());

			text = new wxStaticText(this, wxID_ANY, label);
			repaired_box->Add(text, 0);
			repaired_box->AddSpacer(20);

			panel_box->Add(repaired_box, 1);
			panel_box->AddSpacer(1);

			repaired_box = new wxBoxSizer(wxHORIZONTAL);

			repaired_box->AddSpacer(20);

			label = "Number of files fully repaired:";
			text = new wxStaticText(this, wxID_ANY, label);
			repaired_box->Add(text, 1);

			label = wxString::Format("%d", mNumRepairedFiles);
			repaired_box->AddSpacer(130 - label.length());

			text = new wxStaticText(this, wxID_ANY, label);
			repaired_box->Add(text, 0);
			repaired_box->AddSpacer(20);

			panel_box->Add(repaired_box, 1);
		}
	}

	panel_box->AddSpacer(20);

	auto num_bad_data_files = mNumFailedDataFiles + mNumInvalidDataFiles + mNumDataFilesMissingData;

	if (num_bad_data_files == 0)
	{
		wxBoxSizer* passed_box = new wxBoxSizer(wxHORIZONTAL);
		passed_box->AddSpacer(20);
		label = "All files passed data verification.";
		text = new wxStaticText(this, wxID_ANY, label);
		passed_box->Add(text, 1, wxALIGN_CENTER);
		passed_box->AddSpacer(20);
		panel_box->Add(passed_box, 1, wxALIGN_CENTER);
	}
	else
	{
		wxBoxSizer* failed_box = new wxBoxSizer(wxHORIZONTAL);

		failed_box->AddSpacer(20);

		label = "Number of files failing due to bad files:";
		text = new wxStaticText(this, wxID_ANY, label);
		failed_box->Add(text, 1);

		label = wxString::Format("%d", mNumFailedDataFiles);
		failed_box->AddSpacer(82 - label.length());

		text = new wxStaticText(this, wxID_ANY, label);
		failed_box->Add(text, 0, wxALIGN_RIGHT);
		failed_box->AddSpacer(20);

		panel_box->Add(failed_box, 1);
		panel_box->AddSpacer(1);

		failed_box = new wxBoxSizer(wxHORIZONTAL);

		failed_box->AddSpacer(20);

		label = "Number of files failing due to invalid data:";
		text = new wxStaticText(this, wxID_ANY, label);
		failed_box->Add(text, 1);

		label = wxString::Format("%d", mNumInvalidDataFiles);
		failed_box->AddSpacer(65 - label.length());

		text = new wxStaticText(this, wxID_ANY, label);
		failed_box->Add(text, 0, wxALIGN_RIGHT);
		failed_box->AddSpacer(20);

		panel_box->Add(failed_box, 1);
		panel_box->AddSpacer(1);

		failed_box = new wxBoxSizer(wxHORIZONTAL);

		failed_box->AddSpacer(20);

		label = "Number of files failing due to missing data:";
		text = new wxStaticText(this, wxID_ANY, label);
		failed_box->Add(text, 1);

		label = wxString::Format("%d", mNumDataFilesMissingData);
		failed_box->AddSpacer(59 - label.length());

		text = new wxStaticText(this, wxID_ANY, label);
		failed_box->Add(text, 0, wxALIGN_RIGHT);
		failed_box->AddSpacer(20);

		panel_box->Add(failed_box, 1);
		panel_box->AddSpacer(20);

		if ((mNumPartialRepairDataFiles == 0) && (mNumRepairedDataFiles == num_bad_data_files))
		{
			label = "All files were able to be repaired.";
			text = new wxStaticText(this, wxID_ANY, label);
			panel_box->Add(text, 1, wxALIGN_CENTER);
		}
		else
		{
			wxBoxSizer* repaired_box = new wxBoxSizer(wxHORIZONTAL);

			repaired_box->AddSpacer(20);

			label = "Number of files partially repaired:";
			text = new wxStaticText(this, wxID_ANY, label);
			repaired_box->Add(text, 1);

			label = wxString::Format("%d", mNumPartialRepairDataFiles);
			repaired_box->AddSpacer(112 - label.length());

			label = wxString::Format("%d", mNumPartialRepairDataFiles);
			text = new wxStaticText(this, wxID_ANY, label);
			repaired_box->Add(text, 0);
			repaired_box->AddSpacer(20);

			panel_box->Add(repaired_box, 1);
			panel_box->AddSpacer(1);

			repaired_box = new wxBoxSizer(wxHORIZONTAL);

			repaired_box->AddSpacer(20);

			label = "Number of files fully repaired:";
			text = new wxStaticText(this, wxID_ANY, label);
			repaired_box->Add(text, 1);

			label = wxString::Format("%d", mNumRepairedDataFiles);
			repaired_box->AddSpacer(130 - label.length());

			text = new wxStaticText(this, wxID_ANY, label);
			repaired_box->Add(text, 0, wxALIGN_RIGHT);
			repaired_box->AddSpacer(20);

			panel_box->Add(repaired_box, 1);
		}
	}

	panel_box->AddSpacer(20);

	panel->SetSizer(panel_box);

	wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
	wxButton* okButton = new wxButton(this, wxID_OK, wxT("Ok"), wxDefaultPosition, wxSize(70, 30));
	hbox->Add(okButton, 1);

	wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
	vbox->Add(panel, 1);
	vbox->Add(hbox, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

	SetSizerAndFit(vbox);

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



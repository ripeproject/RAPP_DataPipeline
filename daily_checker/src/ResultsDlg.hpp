
#pragma once

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

// Forward Declarations


// Define a new frame type: this is going to be our main frame
class cResultsDlg : public wxDialog
{
public:
	// ctor(s)
	cResultsDlg();
	virtual ~cResultsDlg();

	void setFileVerificationResults(int num_failed);
	void setFileRepairResults(int num_partial_repair, int num_fully_repaired);
	void setDataVerificationResults(int num_failed, int num_invalid_data, int num_missing_data);
	void setDataRepairResults(int num_partial_repair, int num_fully_repaired);

	// event handlers (these functions should _not_ be virtual)
	int ShowModal() override;

private:
	int mNumFailedFiles = 0;

	int mNumPartialRepairFiles = 0;
	int mNumRepairedFiles = 0;

	int mNumFailedDataFiles = 0;
	int mNumInvalidDataFiles = 0;
	int mNumDataFilesMissingData = 0;

	int mNumPartialRepairDataFiles = 0;
	int mNumRepairedDataFiles = 0;


	// any class wishing to process wxWidgets events must use this macro
	wxDECLARE_EVENT_TABLE();
};


#pragma once

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wx/dataview.h"


class cFileProgressDataModel: public wxDataViewVirtualListModel
{
public:
    enum
    {
        Col_Timestamp,
        Col_Filename,
        Col_Progress
    };

    cFileProgressDataModel();

    // helper methods to change the model

    void Append( const wxString &text );
    void DeleteItem( const wxDataViewItem &item );
    void DeleteItems( const wxDataViewItemArray &items );
    void AddMany();


    // implementation of base class virtuals to define model

    unsigned int GetColumnCount() const override;
    wxString GetColumnType(unsigned int col) const override;

    void GetValueByRow( wxVariant &variant, unsigned int row, unsigned int col ) const override;
    bool GetAttrByRow( unsigned int row, unsigned int col, wxDataViewItemAttr &attr ) const override;
    bool SetValueByRow( const wxVariant &variant, unsigned int row, unsigned int col ) override;

private:
    wxArrayString mTimestampColValues;
    wxArrayString mFilenameColValues;
    wxArrayInt    mProgressColValues;
};


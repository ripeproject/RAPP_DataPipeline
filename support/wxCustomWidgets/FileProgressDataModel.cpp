
#include "FileProgressDataModel.hpp"


#define INITIAL_NUMBER_OF_ITEMS 1000

cFileProgressDataModel::cFileProgressDataModel()
:
    wxDataViewVirtualListModel(INITIAL_NUMBER_OF_ITEMS)
{
    // the first 100 items are really stored in this model;
    // all the others are synthesized on request
    static const unsigned NUMBER_REAL_ITEMS = 100;

    mTimestampColValues.reserve(NUMBER_REAL_ITEMS);
    mFilenameColValues.reserve(NUMBER_REAL_ITEMS);
    mProgressColValues.reserve(NUMBER_REAL_ITEMS);
}

void cFileProgressDataModel::Append(const wxString& text)
{
    mTimestampColValues.Add(wxDateTime::Now().FormatISOTime());
    mFilenameColValues.Add(text);
    mProgressColValues.Add(0);
    RowAppended();
}

void cFileProgressDataModel::DeleteItem(const wxDataViewItem& item)
{
    unsigned int row = GetRow(item);

    if (row >= mTimestampColValues.size())
        return;

    mTimestampColValues.erase(mTimestampColValues.begin() + row);

    if (row >= mFilenameColValues.GetCount())
        return;

    mFilenameColValues.RemoveAt(row);
    mProgressColValues.RemoveAt(row);
    RowDeleted(row);
}

void cFileProgressDataModel::DeleteItems(const wxDataViewItemArray& items)
{
    unsigned i;
    wxArrayInt rows;
    for (i = 0; i < items.GetCount(); i++)
    {
        unsigned int row = GetRow(items[i]);
        if (row < mFilenameColValues.GetCount())
        {
            wxASSERT(row < mFilenameColValues.size());
            rows.Add(row);
        }
    }

    if (rows.GetCount() == 0)
    {
        // none of the selected items were in the range of the items
        // which we store... for simplicity, don't allow removing them
        wxLogError("Cannot remove rows with an index greater than %u", unsigned(mFilenameColValues.GetCount()));
        return;
    }

    // Sort in descending order so that the last
    // row will be deleted first. Otherwise the
    // remaining indices would all be wrong.
//    rows.Sort(my_sort_reverse);
//    for (i = 0; i < rows.GetCount(); i++)
//    {
//        m_toggleColValues.erase(m_toggleColValues.begin() + rows[i]);
//        m_textColValues.RemoveAt(rows[i]);
//    }

    // This is just to test if wxDataViewCtrl can
    // cope with removing rows not sorted in
    // descending order
//    rows.Sort(my_sort);
    RowsDeleted(rows);
}

void cFileProgressDataModel::AddMany()
{
    Reset(GetCount() + 1000);
}

unsigned int cFileProgressDataModel::GetColumnCount() const
{
    return 3;
}

wxString cFileProgressDataModel::GetColumnType(unsigned int col) const
{
    if (col == Col_Progress)
        return "long";

    return "string";
}


void cFileProgressDataModel::GetValueByRow(wxVariant& variant, unsigned int row, unsigned int col) const
{
    switch (col)
    {
    case Col_Timestamp:
        if (row < mTimestampColValues.GetCount())
            variant = mTimestampColValues[row];

        break;

    case Col_Filename:
        if (row < mFilenameColValues.GetCount())
            variant = mFilenameColValues[row];

        break;

    case Col_Progress:
        if (row < mProgressColValues.GetCount())
            variant = wxVariant(mProgressColValues[row]);
        break;
    }
}

bool cFileProgressDataModel::GetAttrByRow(unsigned int row, unsigned int col, wxDataViewItemAttr& attr) const
{
    return true;
}

bool cFileProgressDataModel::SetValueByRow(const wxVariant& variant, unsigned int row, unsigned int col)
{
    switch (col)
    {
    case Col_Timestamp:
    case Col_Filename:
        wxLogError("Cannot edit the column %d", col);
        break;

    case Col_Progress:
        mProgressColValues[row] = variant.GetInteger();
        break;
    }

    return false;
}



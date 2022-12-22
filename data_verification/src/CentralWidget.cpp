

#include "CentralWidget.hpp"

#include "BlockDataFile.hpp"


#include <QtWidgets>
#include <QLayout>
#include <QStackedLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QThreadPool>

#include <memory>


//-----------------------------------------------------------------------------
cCentralWidget::cCentralWidget(QWidget* parent) :
    QWidget(parent)
{
    mActiveScanCount = 0;
    initialize();
}

//-----------------------------------------------------------------------------
cCentralWidget::~cCentralWidget()
{
}

//-----------------------------------------------------------------------------
void cCentralWidget::initialize()
{
    mpLoadSrcPath = new QLineEdit();
    mpLoadSrcPath->setReadOnly(true);
    mpLoadSrcPath->setMinimumWidth(500);

    mpLoadSrcButton = new QPushButton(this);
    mpLoadSrcButton->setText("Browse");
    mpLoadSrcButton->setEnabled(true);
    connect(mpLoadSrcButton, &QPushButton::pressed, this, &cCentralWidget::browseSourceFile);

    mpFailedPath = new QLineEdit();
    mpFailedPath->setReadOnly(true);
    mpFailedPath->setMinimumWidth(500);

    mpScanButton = new QPushButton(this);
    mpScanButton->setText("Scan");
    mpScanButton->setEnabled(false);
    connect(mpScanButton, &QPushButton::pressed, this, &cCentralWidget::scanDataFiles);

    mpScanResults = new QListWidget(this);
    mpScanResults->setSelectionMode(QAbstractItemView::NoSelection);

    // Layout the dialog...

    QVBoxLayout* pMainLayout = new QVBoxLayout();

    auto* srcLayout = new QHBoxLayout();

    QLabel* pSourceLabel = new QLabel();
    pSourceLabel->setText("Source Directory:");

    srcLayout->addWidget(pSourceLabel);
    srcLayout->addSpacing(24);
    srcLayout->addWidget(mpLoadSrcPath);
    srcLayout->addSpacing(3);
    srcLayout->addWidget(mpLoadSrcButton);

    pMainLayout->addLayout(srcLayout, 1);

    auto* failedLayout = new QHBoxLayout();

    QLabel* pFailedLabel = new QLabel();
    pFailedLabel->setText("Failed Directory:");

    failedLayout->addWidget(pFailedLabel);
    failedLayout->addSpacing(29);
    failedLayout->addWidget(mpFailedPath);

    pMainLayout->addLayout(failedLayout, 1);

    pMainLayout->addWidget(mpScanButton, 1);
    pMainLayout->addWidget(mpScanResults, 1);

    setLayout(pMainLayout);
}

//-----------------------------------------------------------------------------
void cCentralWidget::browseSourceFile()
{
    QFileDialog dlg(this, "Open Directory");
    dlg.setFileMode(QFileDialog::Directory);

    auto result = dlg.exec();
    if (result == QDialog::Rejected)
        return;

    QStringList fileNames = dlg.selectedFiles();
    if (fileNames.size() == 0)
        return;

    QString path = fileNames[0];

    if (path.isEmpty())
        return;

    mCurrentDataDirectory = path;

    mpLoadSrcPath->setText(path);

    path += "/failed";
    mpFailedPath->setText(path);

    mpScanButton->setEnabled(true);

    mpScanResults->clear();
}

void cCentralWidget::scanDataFiles()
{
    mpLoadSrcButton->setEnabled(false);
    mpScanButton->setEnabled(false);

    emit statusMessage("Scan started...");

    update();

    mFilesToTest.clear();

    QDir dir(mCurrentDataDirectory);

    auto files = dir.entryInfoList(QDir::Files);
    for (auto file : files)
    {
        QString test_name = file.fileName();
        if (test_name.contains("ceres") || test_name.contains("lidar_data"))
        {
            mFilesToTest.push_back(file.absoluteFilePath());
        }
    }

    if (mFilesToTest.empty())
    {
        mpLoadSrcButton->setEnabled(true);

        emit statusMessage("Could not find any Ceres data files to scan.");

        return;
    }
    QDir failed(mCurrentDataDirectory + "/failed");
    if (!failed.exists())
    {

    }

    auto maxThreads = QThreadPool::globalInstance()->maxThreadCount();
    mActiveScanCount = 0;

    for (; mActiveScanCount < maxThreads;)
    {
        if (mFilesToTest.empty())
            break;

        mCurrentFileName = mFilesToTest.front();
        mFilesToTest.pop_front();

        auto* pItem = new QListWidgetItem();
        QString text = "Scanning ";
        text += mCurrentFileName;
        text += "...";
        pItem->setText(text);
        auto n = mpScanResults->count();
        mpScanResults->insertItem(n, pItem);
        mpScanResults->setCurrentItem(pItem);

        cDataVerifier* pVerifier = new cDataVerifier(n, mCurrentDataDirectory);
        connect(pVerifier, &cDataVerifier::fileResults, this, &cCentralWidget::fileResultsUpdated);
        connect(pVerifier, &cDataVerifier::statusMessage, this, &cCentralWidget::statusMessage);

        if (!pVerifier->open(mCurrentFileName.toStdString()))
        {
            QString text = mCurrentFileName;
            text += "... could not be loaded.";
            pItem->setText(text);
            delete pVerifier;
            continue;
        }

        QThreadPool::globalInstance()->start(pVerifier);
        ++mActiveScanCount;
    }
}

//-----------------------------------------------------------------------------
void cCentralWidget::fileResultsUpdated(int id, bool valid, QString msg)
{
    --mActiveScanCount;

    emit statusMessage(msg);

    auto* pItem = mpScanResults->item(id);
//    auto* pItem = mpScanResults->currentItem();

    if (valid)
    {
        QString text = mCurrentFileName;
        text += "... passed.";
        pItem->setText(text);
    }
    else
    {
        QString text = mCurrentFileName;
        text += "... failed!";
        pItem->setText(text);
    }

    if (mFilesToTest.empty())
    {
        if (mActiveScanCount <= 0)
        {
            mpLoadSrcButton->setEnabled(true);
            emit statusMessage("Scan Finished.");
        }

        return;
    }

    mCurrentFileName = mFilesToTest.front();
    mFilesToTest.pop_front();

    pItem = new QListWidgetItem();
    QString text = "Scanning ";
    text += mCurrentFileName;
    text += "...";
    pItem->setText(text);
    auto n = mpScanResults->count();
    mpScanResults->insertItem(n, pItem);
    mpScanResults->setCurrentItem(pItem);

    cDataVerifier* pVerifier = new cDataVerifier(n, mCurrentDataDirectory);
    connect(pVerifier, &cDataVerifier::fileResults, this, &cCentralWidget::fileResultsUpdated);

    pVerifier->open(mCurrentFileName.toStdString());

    QThreadPool::globalInstance()->start(pVerifier);
    ++mActiveScanCount;
}


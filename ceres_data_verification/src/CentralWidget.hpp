
#pragma once

#include "DataVerifier.hpp"

#include <QWidget>
#include <QTimer>
#include <QStringList>


// Qt Forward Declaration
QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
QT_END_NAMESPACE


// Forward Declarations


class cCentralWidget : public QWidget
{
    Q_OBJECT

public:
    explicit cCentralWidget(QWidget* parent = nullptr);
    ~cCentralWidget();

    void initialize();
    
signals:
    void statusMessage(QString msg);
    void errorMessage(QString title, QString msg);

private slots:
    void browseSourceFile();
    void scanDataFiles();
    void fileResultsUpdated(int id, bool valid, QString msg);

private:
    QLineEdit*   mpLoadSrcPath = nullptr;
    QPushButton* mpLoadSrcButton = nullptr;
    QLineEdit*   mpFailedPath = nullptr;
    QPushButton* mpScanButton = nullptr;
    QListWidget* mpScanResults = nullptr;

    QString     mCurrentDataDirectory;
    QStringList mFilesToTest;
    QString     mCurrentFileName;
    int         mActiveScanCount;
};


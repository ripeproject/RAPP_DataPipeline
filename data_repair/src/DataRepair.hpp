/**
 * @file
 */
#pragma once

#include "BlockDataFile.hpp"

#include <QObject>
#include <QRunnable>

class cDataRepair : public QObject, public QRunnable, private cBlockDataFileReader
{
    Q_OBJECT

public:
	cDataRepair(int id, const QString& dataDir, const QString& repairedDir, 
                QObject* parent = nullptr);
    ~cDataRepair();

    bool open(const std::string& file_name);

    void run() override;

signals:
    void statusMessage(QString msg);
    void fileResults(int id, bool valid, QString msg);

private:
    void processBlock(const cBlockID& id) override;
    void processBlock(const cBlockID& id, const std::byte* buf, std::size_t len) override;

    bool moveFileToRepaired(bool size_check = true);

private:
    const int mId;
    cBlockDataFileWriter mFileWriter;

    QString     mCurrentDataDirectory;
    QString     mRepairedDataDirectory;
    QString     mCurrentFileName;
    QString     mRepairedFileName;
};


/**
 * @file
 */
#pragma once

#include <CBDF/BlockDataFile.hpp>
//#include "AxisCommunicationsParser.hpp"

#include <string>

//#include <QObject>
//#include <QRunnable>

class cDataVerifier 
	//: public QObject, public QRunnable
{
public:
    cDataVerifier(int id, const std::string& dataDir, void* parent = nullptr);
    ~cDataVerifier();

    bool open(const std::string& file_name);

    void run();

//signals:
	void statusMessage(std::string msg) {};
	void fileResults(int id, bool valid, std::string msg) {};

protected:
	void moveFileToFailed();

private:

/*
	void onActiveCameraId(int id) override;
	void onFramesPerSecond(int frames_per_sec) override;
	void onBitmap(const QBitmap& in) override;
	void onJPEG(const QImage& image) override;
	void onMpegFrame(const QImage& image) override;
	void onImageSize(int width, int height) override;
*/

private:
	const int mId;
    cBlockDataFileReader mFileReader;

	std::string mCurrentDataDirectory;
	std::string mCurrentFileName;
};


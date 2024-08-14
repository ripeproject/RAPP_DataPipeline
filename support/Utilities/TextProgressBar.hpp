#pragma once

#include <string>
#include <map>


class cTextProgressBar
{
public:
	cTextProgressBar() = default;

	void clear();

	bool quietMode() const;
	void setQuietMode(bool quiet_mode);

	bool verboseMode() const;
	void setVerboseMode(bool verbose_mode);

	void setMaxID(int max_id);

	void setFillChars(char half_full, char full);
	void setLength(uint8_t length);

	void addProgressEntry(int id, const std::string& filename);
	void addProgressEntry(int id, const std::string& filename, const std::string& prefix);
	void addProgressEntry(int id, const std::string& filename, const std::string& prefix, const std::string& suffix);

	void updateProgressEntry(int id, float progress_pct);
	void updateProgressEntry(int id, const std::string& prefix, float progress_pct);
	void updateProgressEntry(int id, const std::string& prefix, const std::string& suffix, float progress_pct);

	void finishProgressEntry(int id);
	void finishProgressEntry(int id, const std::string& suffix);
	void finishProgressEntry(int id, const std::string& prefix, const std::string& suffix);

private:
	int mMaxID = -1;

	bool mQuietMode = false;
	bool mVerboseMode = false;

	uint8_t mProgressLength = 50;

	char mEmptyChar = ' ';
	char mFullChar  = '=';
	char mHalfChar  = '-';

	struct sLineInfo_t
	{
		int line_number = 0;
		std::string filename;
		std::string prefix;
		std::string suffix;
		int num_fill_char = 0;
		bool needs_half_fill = false;
		int max_length = 0;
	};

	std::map<int, sLineInfo_t> mProgressEntries;
};
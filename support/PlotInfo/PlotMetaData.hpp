#pragma once


#include <list>
#include <vector>
#include <memory>
#include <optional>
#include <string>


class cPlotMetaData
{
public:
	explicit cPlotMetaData(int plotId);
	~cPlotMetaData();

	int id() const;

	const std::string& name() const;

	const std::string& event() const;
	const std::string& description() const;
	const std::string& species() const;
	const std::string& cultivar() const;
	const std::string& constructName() const;
	const std::string& potLabel() const;
	const std::string& seedGeneration() const;
	const std::string& copyNumber() const;

	void setPlotName(const std::string& name);

	void setEvent(const std::string& event);
	void setDescription(const std::string& description);
	void setSpecies(const std::string& species);
	void setCultivar(const std::string& cultivar);
	void setConstructName(const std::string& constructName);
	void setPotLabel(const std::string& potLabel);
	void setSeedGeneration(const std::string& seedGeneration);
	void setCopyNumber(const std::string& copyNumber);

	void addTreatment(const std::string& treatment);

private:
	const int mID;

	std::string mName;
	std::string mDescription;

	std::string mEvent;
	std::string mSpecies;
	std::string mCultivar;
	std::string mConstructName;
	std::string mPotLabel;
	std::string mSeedGeneration;
	std::string mCopyNumber;
	std::vector<std::string> mTreatments;
};


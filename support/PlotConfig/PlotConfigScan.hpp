
#pragma once

#include "PlotConfigBoundary.hpp"
#include "PlotConfigIsolationMethod.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <limits>


// Forward Declaration
class cPlotConfigPlotInfo;

constexpr uint8_t	INVALID_SUB_PLOT_NUMBER = 255;


class cPlotConfigScan
{
public:
	typedef std::vector<cPlotConfigPlotInfo> Plot_t;

	typedef Plot_t::iterator				iterator;
	typedef Plot_t::const_iterator			const_iterator;

public:
	cPlotConfigScan();
	~cPlotConfigScan();

	bool isDirty() const;

	const std::string& getMeasurementName() const;

	void setMeasurementName(const std::string& name);

	bool hasGroundLevel() const;

	std::int32_t getGroundLevel_mm() const;
	void setGroundLevel_mm(std::int32_t ground_level_mm);

	bool empty() const;
	std::size_t size() const;

	bool contains(int32_t id) const;
	bool contains(const std::string& name) const;

	const_iterator find(int32_t id) const;
	iterator find(int32_t id);

	const_iterator find(const std::string& name) const;
	iterator find(const std::string& name);

	const cPlotConfigPlotInfo& front() const;
	cPlotConfigPlotInfo& front();

	iterator		begin();
	iterator		end();

	const_iterator	begin() const;
	const_iterator	end() const;

	void append(const cPlotConfigPlotInfo& info);
	void append(const std::vector<cPlotConfigPlotInfo>& info);
	cPlotConfigPlotInfo& append();
	void remove(const cPlotConfigPlotInfo& plot);

	const cPlotConfigPlotInfo& operator[](int index) const;
	cPlotConfigPlotInfo& operator[](int index);

	std::vector<cPlotConfigPlotInfo>& data();
	const std::vector<cPlotConfigPlotInfo>& data() const;

	void clearDirtyFlag();
	void setDirtyFlag(bool dirty);

protected:
	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	bool mDirty = false;

	std::string mMeasurementName;
	std::int32_t mGroundLevel_mm = std::numeric_limits<std::int32_t>::lowest();

	std::vector<cPlotConfigPlotInfo> mPlots;

	friend class cPlotConfigFile;
};


class cPlotConfigPlotInfo
{
public:
	cPlotConfigPlotInfo() = default;
	~cPlotConfigPlotInfo() = default;

	cPlotConfigPlotInfo(const cPlotConfigPlotInfo& rhs) = default;
	const cPlotConfigPlotInfo& operator=(const cPlotConfigPlotInfo& rhs);

	void clear();

	bool isDirty() const;

	bool empty() const;

	uint32_t	getPlotNumber() const;
/*
	bool		hasSubPlotNumber() const;
	uint8_t		getSubPlotNumber() const;
*/
	const std::string& getPlotName() const;
	const std::string& getEvent() const;
	const std::string& getDescription() const;
	const std::string& getSpecies() const;
	const std::string& getCultivar() const;
	const std::string& getConstructName() const;
	const std::string& getPotLabel() const;
	const std::string& getSeedGeneration() const;
	const std::string& getCopyNumber() const;
	const std::string& getLeafType() const;
	const std::vector<std::string>& getTreatments() const;

	bool contains(rfm::rappPoint2D_t point);
	bool contains(std::int32_t x_mm, std::int32_t y_mm);

	const cPlotConfigBoundary& getBounds() const;
	cPlotConfigBoundary& getBounds();

	const cPlotConfigIsolationMethod& getIsolationMethod() const;
	cPlotConfigIsolationMethod& getIsolationMethod();

	void setPlotNumber(uint32_t num);
//	void setSubPlotNumber(uint8_t num);
	void setPlotName(const std::string& name);
	void setEvent(const std::string& event);
	void setDescription(const std::string& desc);
	void setSpecies(const std::string& species);
	void setCultivar(const std::string& cultivar);
	void setConstructName(const std::string& name);
	void setPotLabel(const std::string& pot);
	void setSeedGeneration(const std::string& generation);
	void setCopyNumber(const std::string& num);
	void setLeafType(const std::string& leaf_type);

	void setTreatment(const std::string& treatment);
	void setTreatments(const std::vector<std::string>& treatments);

	void clearTreatments();
	void addTreatment(const std::string& treatment);

	void setBounds(const cPlotConfigBoundary& bounds);
	void setIsolationMethod(const cPlotConfigIsolationMethod& method);

	void clearDirtyFlag();
	void setDirtyFlag(bool dirty);

protected:
	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	bool mDirty = false;

	uint32_t	mPlotNumber = 0;
	std::string mPlotName;
	std::string mDescription;
	std::string mSpecies;
	std::string mCultivar;
	std::string mEvent;
	std::string mConstructName;
	std::string mPotLabel;
	std::string mSeedGeneration;
	std::string mCopyNumber;
	std::string mLeafType;
	std::vector<std::string> mTreatments;

	cPlotConfigBoundary mBounds;
	cPlotConfigIsolationMethod mIsolationMethod;

	friend class cPlotConfigScan;
};


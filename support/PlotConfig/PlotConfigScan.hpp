
#pragma once

#include "PlotConfigBoundary.hpp"
#include "PlotConfigIsolationMethod.hpp"
#include "PlotConfigExclusion.hpp"
#include "PlotConfigCorrections.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <optional>


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

	void clearGroundLevel();
	bool hasGroundLevel() const;

	std::optional<std::int32_t> getGroundLevel_mm() const;
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

	const cPlotConfigPlotInfo& back() const;
	cPlotConfigPlotInfo& back();

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
	std::optional<std::int32_t> mGroundLevel_mm;

	std::vector<cPlotConfigPlotInfo> mPlots;

	friend class cPlotConfigFile;
};


class cPlotConfigPlotInfo
{
public:
	typedef cPlotConfigCorrections::iterator		iterator;
	typedef cPlotConfigCorrections::const_iterator	const_iterator;

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

	bool contains_point(rfm::rappPoint2D_t point) const;
	bool contains_point(std::int32_t x_mm, std::int32_t y_mm) const;

	bool contains_point(const int date, rfm::rappPoint2D_t point) const;
	bool contains_point(const int date, std::int32_t x_mm, std::int32_t y_mm) const;

	bool contains_point(const int month, const int day, rfm::rappPoint2D_t point) const;
	bool contains_point(const int month, const int day, std::int32_t x_mm, std::int32_t y_mm) const;

	bool contains(const int date) const;
	bool contains(const int month, const int day) const;

	const cPlotConfigCorrection& front() const;
	cPlotConfigCorrection& front();

	iterator		begin();
	iterator		end();

	const_iterator	begin() const;
	const_iterator	end() const;

	const_iterator	find(const int date) const;
	iterator		find(const int date);

	const_iterator	find(const int month, const int day) const;
	iterator		find(const int month, const int day);

	const_iterator	find_exact(const int month, const int day) const;
	iterator		find_exact(const int month, const int day);

	const_iterator	find_prev(const int date) const;
	iterator		find_prev(const int date);

	const_iterator	find_prev(const int month, const int day) const;
	iterator		find_prev(const int month, const int day);

	const cPlotConfigBoundary* const getBounds(const int date) const;
	cPlotConfigBoundary* const getBounds(const int date);

	const cPlotConfigBoundary* const getBounds(const int month, const int day) const;
	cPlotConfigBoundary* const getBounds(const int month, const int day);

	const cPlotConfigIsolationMethod* const getIsolationMethod(const int date) const;
	cPlotConfigIsolationMethod* const getIsolationMethod(const int date);

	const cPlotConfigIsolationMethod* const getIsolationMethod(const int month, const int day) const;
	cPlotConfigIsolationMethod* const getIsolationMethod(const int month, const int day);

	const std::vector<cPlotConfigExclusion>* const getExclusions(const int date) const;
	std::vector<cPlotConfigExclusion>* const getExclusions(const int date);

	const std::vector<cPlotConfigExclusion>* const getExclusions(const int month, const int day) const;
	std::vector<cPlotConfigExclusion>* const getExclusions(const int month, const int day);

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

	cPlotConfigCorrection& add(const int month, const int day);

	void setBounds(const int month, const int day, const cPlotConfigBoundary& bounds);
	void setIsolationMethod(const int month, const int day, const cPlotConfigIsolationMethod& method);
	void setExclusions(const int month, const int day, const std::vector<cPlotConfigExclusion>& exclusions);

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

	cPlotConfigCorrections mCorrections;

	friend class cPlotConfigScan;
};


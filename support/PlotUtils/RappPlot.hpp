#pragma once

#include "Constants.hpp"

#include <cbdf/PlotPointCloud.hpp>

#include <vector>
#include <string>


/**
 * Class to store basic plot information and point cloud
 */
class cRappPlot
{
public:
	typedef cPlotPointCloud::value_type					value_type;
	typedef cPlotPointCloud::size_type					size_type;
	typedef cPlotPointCloud::difference_type			difference_type;
	typedef cPlotPointCloud::reference					reference;
	typedef cPlotPointCloud::const_reference			const_reference;
	typedef cPlotPointCloud::pointer					pointer;
	typedef cPlotPointCloud::const_pointer				const_pointer;
	typedef cPlotPointCloud::iterator					iterator;
	typedef cPlotPointCloud::const_iterator				const_iterator;
	typedef cPlotPointCloud::reverse_iterator			reverse_iterator;
	typedef cPlotPointCloud::const_reverse_iterator		const_reverse_iterator;

public:
	explicit cRappPlot(int plotId);
	cRappPlot(int plotId, int subPlotId);
	cRappPlot(const cRappPlot& pc) = default;
	~cRappPlot();

	int id() const;
	int subPlotId() const;

	const std::string& name() const;

	const std::string& event() const;
	const std::string& description() const;
	const std::string& species() const;
	const std::string& cultivar() const;
	const std::string& constructName() const;
	const std::string& potLabel() const;
	const std::string& seedGeneration() const;
	const std::string& copyNumber() const;
	const std::string& leafType() const;

	const bool vegetationOnly() const;

	const cPlotPointCloud& pointCloud() const { return mCloud; }
	cPlotPointCloud& pointCloud() { return mCloud; }

    bool empty() const;

	void setPlotName(const std::string& name);

	void setEvent(const std::string& event);
	void setDescription(const std::string& description);
	void setSpecies(const std::string& species);
	void setCultivar(const std::string& cultivar);
	void setConstructName(const std::string& constructName);
	void setPotLabel(const std::string& potLabel);
	void setSeedGeneration(const std::string& seedGeneration);
	void setCopyNumber(const std::string& copyNumber);
	void setLeafType(const std::string& leafType);

	void setVegetationOnly(const bool vegetation_only);

	void setPointCloud(const cPlotPointCloud& pointCloud);


	plot::sPoint3D_t getPoint(int x_mm, int y_mm, int r_mm) const;

	iterator begin() { return mCloud.begin(); }
	const_iterator begin() const { return mCloud.begin(); }
	const_iterator cbegin() { return mCloud.cbegin(); }

	iterator end() { return mCloud.end(); }
	const_iterator end() const { return mCloud.end(); }
	const_iterator cend() { return mCloud.cend(); }

	reverse_iterator rbegin() { return mCloud.rbegin(); }
	const_reverse_iterator rbegin() const { return mCloud.rbegin(); }
	const_reverse_iterator crbegin() { return mCloud.crbegin(); }

	reverse_iterator rend() { return mCloud.rend(); }
	const_reverse_iterator rend() const { return mCloud.rend(); }
	const_reverse_iterator crend() { return mCloud.crend(); }

	plot::sPoint3D_t operator[](int i) const { return mCloud[i]; }

	const cPlotPointCloud& data() const { return mCloud; }

private:
	int mID;
	int mSubID = 0;

	std::string mName;
	std::string mDescription;

	std::string mEvent;
	std::string mSpecies;
	std::string mCultivar;
	std::string mConstructName;
	std::string mPotLabel;
	std::string mSeedGeneration;
	std::string mCopyNumber;
	std::string mLeafType;

	cPlotPointCloud mCloud;
};


///////////////////////////////////////////////////////////////////////////////
// Implementation Details
///////////////////////////////////////////////////////////////////////////////


#pragma once

#include "RappPointCloud.hpp"

#include "Constants.hpp"

#include <vector>
#include <string>


/**
 * Class to store basic plot information and point cloud
 */
class cRappPlot
{
public:
	typedef cRappPointCloud::value_type					value_type;
	typedef cRappPointCloud::size_type					size_type;
	typedef cRappPointCloud::difference_type			difference_type;
	typedef cRappPointCloud::reference					reference;
	typedef cRappPointCloud::const_reference			const_reference;
	typedef cRappPointCloud::pointer					pointer;
	typedef cRappPointCloud::const_pointer				const_pointer;
	typedef cRappPointCloud::iterator					iterator;
	typedef cRappPointCloud::const_iterator				const_iterator;
	typedef cRappPointCloud::reverse_iterator			reverse_iterator;
	typedef cRappPointCloud::const_reverse_iterator		const_reverse_iterator;

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

	const cRappPointCloud& pointCloud() const { return mCloud; }
	cRappPointCloud& pointCloud() { return mCloud; }

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

	void setPointCloud(const cRappPointCloud& pointCloud);


	rfm::sPoint3D_t getPoint(int x_mm, int y_mm, int r_mm) const;

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

	rfm::sPoint3D_t operator[](int i) const { return mCloud[i]; }

	const cRappPointCloud& data() const { return mCloud; }

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

	cRappPointCloud mCloud;
};


///////////////////////////////////////////////////////////////////////////////
// Implementation Details
///////////////////////////////////////////////////////////////////////////////


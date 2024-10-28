#pragma once

#include "PlotMetaData.hpp"
#include "PlotPointCloud.hpp"

#include <vector>
#include <memory>
#include <string>

// Forward Declarations
namespace pointcloud
{
	struct sCloudPoint_t;
	struct sCloudPoint_FrameID_t;
	struct sCloudPoint_SensorInfo_t;
}


class cPlot : public cPlotMetaData, public cPlotPointCloud
{
public:
	explicit cPlot(int plotId);

	void assign(const cBasePointCloud<pointcloud::sCloudPoint_t>& pc);
	void assign(const cBasePointCloud<pointcloud::sCloudPoint_FrameID_t>& pc);
	void assign(const cBasePointCloud<pointcloud::sCloudPoint_SensorInfo_t>& pc);
};


class cPlotInfo : public std::enable_shared_from_this<cPlotInfo>
{
public:
	typedef std::vector<std::unique_ptr<cPlot>>::value_type				value_type;
	typedef std::vector<std::unique_ptr<cPlot>>::size_type				size_type;
	typedef std::vector<std::unique_ptr<cPlot>>::difference_type		difference_type;
	typedef std::vector<std::unique_ptr<cPlot>>::reference				reference;
	typedef std::vector<std::unique_ptr<cPlot>>::const_reference		const_reference;
	typedef std::vector<std::unique_ptr<cPlot>>::pointer				pointer;
	typedef std::vector<std::unique_ptr<cPlot>>::const_pointer			const_pointer;
	typedef std::vector<std::unique_ptr<cPlot>>::iterator				iterator;
	typedef std::vector<std::unique_ptr<cPlot>>::const_iterator			const_iterator;
	typedef std::vector<std::unique_ptr<cPlot>>::reverse_iterator		reverse_iterator;
	typedef std::vector<std::unique_ptr<cPlot>>::const_reverse_iterator	const_reverse_iterator;

public:
	cPlotInfo();
	~cPlotInfo();

	bool empty() const;

	std::size_t size() const;

	iterator begin() { return mPlots.begin(); }
	const_iterator begin() const { return mPlots.begin(); }
	const_iterator cbegin() { return mPlots.cbegin(); }

	iterator end() { return mPlots.end(); }
	const_iterator end() const { return mPlots.end(); }
	const_iterator cend() { return mPlots.cend(); }

	reverse_iterator rbegin() { return mPlots.rbegin(); }
	const_reverse_iterator rbegin() const { return mPlots.rbegin(); }
	const_reverse_iterator crbegin() { return mPlots.crbegin(); }

	reverse_iterator rend() { return mPlots.rend(); }
	const_reverse_iterator rend() const { return mPlots.rend(); }
	const_reverse_iterator crend() { return mPlots.crend(); }

	reference		 operator[](int i) { return mPlots[i]; }
	const_reference  operator[](int i) const { return mPlots[i]; }

	const std::vector<std::unique_ptr<cPlot>>& data() const { return mPlots; }

protected:
	void addPlot(std::unique_ptr<cPlot> plot);

private:
	std::vector<std::unique_ptr<cPlot>> mPlots;

	friend class cPlotInfoLoader;
};


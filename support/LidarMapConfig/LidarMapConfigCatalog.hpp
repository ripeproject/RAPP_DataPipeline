
#pragma once

#include "LidarMapConfigScan.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <optional>


class cLidarMapConfigConfiguration
{
public:
	typedef std::vector<cLidarMapConfigScan> LidarScan_t;

	typedef LidarScan_t::iterator				iterator;
	typedef LidarScan_t::const_iterator			const_iterator;

public:
	cLidarMapConfigConfiguration();
	~cLidarMapConfigConfiguration() = default;

	void clear();

	bool isDirty() const;

	bool empty() const;
	std::size_t size() const;

	bool contains(const std::string& name);

	const cLidarMapConfigScan& front() const;
	cLidarMapConfigScan& front();

	iterator		begin();
	iterator		end();

	const_iterator	begin() const;
	const_iterator	end() const;

	const_iterator	find_by_filename(const std::string& measurement_filename) const;
	iterator		find_by_filename(const std::string& measurement_filename);

	const_iterator	find(const std::string& name) const;
	iterator		find(const std::string& name);

	cLidarMapConfigScan& add(const std::string& name);
	void remove(const std::string& name);

	const cLidarMapConfigScan& operator[](int index) const;
	cLidarMapConfigScan& operator[](int index);

protected:
	void setDirty(bool dirty);

	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	template<typename T>
	void reset(std::optional<T>& var);

private:
	bool mDirty = false;

	LidarScan_t mScans;


	friend class cLidarMapConfigFile;
};

/***
 * Implementation Details
 ***/
template<typename T>
inline void cLidarMapConfigConfiguration::reset(std::optional<T>& var)
{
	if (var.has_value())
	{
		var.reset();
		mDirty = true;
	}
}

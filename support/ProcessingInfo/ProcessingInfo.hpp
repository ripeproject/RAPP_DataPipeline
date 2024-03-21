#pragma once

#include "ProcessingInfoTypes.hpp"

#include <vector>
#include <memory>


class cProcessingInfo : public std::enable_shared_from_this<cProcessingInfo>
{
public:
	typedef std::vector<processing_info::sProcessInfo_1_t>  vProcessingSteps_t;

	typedef vProcessingSteps_t::value_type				value_type;
	typedef vProcessingSteps_t::size_type				size_type;
	typedef vProcessingSteps_t::difference_type			difference_type;
	typedef vProcessingSteps_t::reference				reference;
	typedef vProcessingSteps_t::const_reference			const_reference;
	typedef vProcessingSteps_t::pointer					pointer;
	typedef vProcessingSteps_t::const_pointer			const_pointer;
	typedef vProcessingSteps_t::iterator				iterator;
	typedef vProcessingSteps_t::const_iterator			const_iterator;
	typedef vProcessingSteps_t::reverse_iterator		reverse_iterator;
	typedef vProcessingSteps_t::const_reverse_iterator	const_reverse_iterator;

public:
	cProcessingInfo() = default;
	virtual ~cProcessingInfo() = default;

	void clear();

	bool empty() const;
	std::size_t size() const;

	const_iterator begin() const;
	const_iterator end() const;

protected:
	void addProcessingInfo(processing_info::sProcessInfo_1_t info);

private:
	vProcessingSteps_t mProcessingSteps;

	friend class cProcessingInfoLoader;
};


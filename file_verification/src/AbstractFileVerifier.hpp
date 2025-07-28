/**
 * @file
 */
#pragma once


class cAbstractFileVerifier 
{
public:
    enum class eRETURN_TYPE { PASSED, COULD_NOT_OPEN_FILE, FAILED };


public:
    virtual ~cAbstractFileVerifier() {};

	virtual eRETURN_TYPE process_file() = 0;
};


/**
 * @file
 */
#pragma once


class cAbstractFileVerifier 
{
public:
    enum class eRETURN_TYPE { PASSED, COULD_NOT_OPEN_FILE, INVALID_DATA, INVALID_FILE };

public:
    virtual ~cAbstractFileVerifier() {};

	virtual void process_file() = 0;
};


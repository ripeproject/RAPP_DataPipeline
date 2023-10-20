/**
 * @file
 */
#pragma once


class cAbstractFileVerifier 
{
public:
    virtual ~cAbstractFileVerifier() {};

	virtual void process_file() = 0;
};


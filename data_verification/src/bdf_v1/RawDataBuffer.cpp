
#include "RawDataBuffer.hpp"

#include <cstring>


namespace
{
	/**
	 * \brief write
	 * \par Description
	 *		A template member function to write a variable into the internal buffer.
	 *
	 * \param X		-- the parameter to write into the buffer.
	 */
	template<typename T>
	std::size_t write(const T& X, std::byte* start, const std::byte* end, bool& overrun)
	{
		// Check to make sure we have enough buffer space to put this variable into
		// our internal storage.
		std::size_t free = std::distance<const std::byte*>(start, end);
		if (free < sizeof(T))
		{
			overrun = true;
			return 0;
		}

		// copy the host order value into the buffer
		memcpy(start, &X, sizeof(T));

		return sizeof(T);
	};

}


///////////////////////////////////////////////////////////////////////////////
//
// RAW DATA BUFFER
//
///////////////////////////////////////////////////////////////////////////////


v1::cRawDataBuffer::cRawDataBuffer()
	: mpBuffer(nullptr), mWriteIndex(0), mCapacity(0), mOverrun(false)
{
}

v1::cRawDataBuffer::cRawDataBuffer(size_type capacity)
	: mpBuffer(nullptr), mWriteIndex(0), mCapacity(0), mOverrun(false)
{
	mpBuffer = new std::byte[capacity];

	if (mpBuffer != nullptr)
	{
		mCapacity = capacity;
	}
}

v1::cRawDataBuffer::cRawDataBuffer(const v1::cRawDataBuffer& objToCopy)
	: mpBuffer(nullptr),
	mWriteIndex(objToCopy.mWriteIndex),
	mCapacity(objToCopy.mCapacity),
	mOverrun(objToCopy.mOverrun)
{
	mpBuffer = new std::byte[objToCopy.mCapacity];

	memcpy(&mpBuffer[0], objToCopy.mpBuffer, mCapacity);
}

v1::cRawDataBuffer::cRawDataBuffer(v1::cRawDataBuffer&& objToMove) noexcept
{
	mpBuffer = objToMove.mpBuffer;
	mCapacity = objToMove.mCapacity;
	mWriteIndex = objToMove.mWriteIndex;
	mOverrun = objToMove.mOverrun;

	objToMove.mpBuffer = nullptr;
	objToMove.mCapacity = 0;
	objToMove.mWriteIndex = 0;
	objToMove.mOverrun = false;
}

void v1::cRawDataBuffer::operator=(const cRawDataBuffer& objToCopy)
{
	delete[] mpBuffer;
	mpBuffer = nullptr;
	mpBuffer = new std::byte[objToCopy.mCapacity];

	if (mpBuffer != nullptr)
	{
		mCapacity = objToCopy.mCapacity;
	}

	mWriteIndex = objToCopy.mWriteIndex;
	mOverrun = objToCopy.mOverrun;

	memcpy(&mpBuffer[0], objToCopy.mpBuffer, mCapacity);
}

v1::cRawDataBuffer::~cRawDataBuffer()
{
	delete[] mpBuffer;
	mpBuffer = nullptr;

	mWriteIndex = 0;
	mCapacity = 0;
	mOverrun = false;
}

std::size_t v1::cRawDataBuffer::write_size() const
{
	if (mWriteIndex > mCapacity) return 0;
	return mCapacity - mWriteIndex;
}

std::size_t v1::cRawDataBuffer::size() const
{
	return mWriteIndex;
}

std::size_t v1::cRawDataBuffer::capacity() const
{
	return mCapacity;
}

void v1::cRawDataBuffer::capacity(size_type capacity)
{
	if (mpBuffer == nullptr)
	{
		mpBuffer = new std::byte[capacity];

		if (mpBuffer != nullptr)
		{
			mCapacity = capacity;
		}

		return;
	}

	if (capacity > mCapacity)
	{
		std::byte* pBuffer = new std::byte[capacity];

		if (pBuffer != nullptr)
		{
			memcpy(pBuffer, mpBuffer, mCapacity);

			delete[] mpBuffer;

			mpBuffer = pBuffer;
			mCapacity = capacity;
			mOverrun = false;
		}

		return;
	}

	std::byte* pBuffer = new std::byte[capacity];

	if (pBuffer != nullptr)
	{
		reset();

		delete[] mpBuffer;

		mpBuffer = pBuffer;
		mCapacity = capacity;
	}
}

bool v1::cRawDataBuffer::empty() const
{
	return (mpBuffer == nullptr) || (mWriteIndex == 0);
}

bool v1::cRawDataBuffer::overrun() const
{
	return mOverrun;
}

const std::byte* v1::cRawDataBuffer::data() const
{
	return mpBuffer;
}

void v1::cRawDataBuffer::attach(std::byte* buffer, size_type size)
{
	delete[] mpBuffer;

	mpBuffer = buffer;
	mCapacity = size;
	reset();
	mWriteIndex = size;
}

std::byte* v1::cRawDataBuffer::detach()
{
	std::byte* retPtr = mpBuffer;

	// Clear our internal pointer and reset the indexes
	mpBuffer = nullptr;
	mCapacity = 0;

	reset();

	return retPtr;
}

void v1::cRawDataBuffer::reset()
{
	mWriteIndex = 0;
	mOverrun = false;
}

void v1::cRawDataBuffer::clear()
{
	reset();

	if (mpBuffer != nullptr)
		memset(mpBuffer, 0, mCapacity);
}

std::size_t v1::cRawDataBuffer::insert(const unsigned char* buffer, size_type size)
{
	if (size > write_size())
	{
		mOverrun = true;
		size = write_size();
	}

	memcpy(&mpBuffer[mWriteIndex], buffer, size);
	mWriteIndex += size;
	return size;
}

void v1::cRawDataBuffer::padTo(size_type size, std::byte value)
{
	if (size > mWriteIndex)
	{
		std::size_t newBytes = size - mWriteIndex;

		if (newBytes > write_size())
		{
			mOverrun = true;
			newBytes = write_size();
		}

		if (newBytes > 0)
		{
			memset(mpBuffer + mWriteIndex, static_cast<unsigned char>(value), newBytes);
			mWriteIndex += newBytes;
		}
	}
}

std::size_t v1::cRawDataBuffer::erase(std::size_t pos, size_type size)
{
	// If the start position is passed the current size of the buffer we can't
	// erase any elements.
	if (pos > mWriteIndex)
		return 0;

	// If the size is more then the number of element left in the control
	// sequence, zero fill the remaining in sequence.
	if (size > (mWriteIndex - pos))
	{
		size = mWriteIndex - pos;
		memset(&mpBuffer[pos], 0, size);
		mWriteIndex = pos;
		return size;
	}

	memcpy(&mpBuffer[pos], &mpBuffer[pos + size], mWriteIndex - (pos + size));
	mWriteIndex -= size;
	memset(&mpBuffer[mWriteIndex], 0, size);
	return size;
}

///////////////////////////////////////////////////////////////////////////////
// Output Stream Operators
///////////////////////////////////////////////////////////////////////////////

v1::cRawDataBuffer& v1::cRawDataBuffer::operator<<(const v1::cRawDataBuffer& in)
{
	// Check to make sure we have enough buffer space to put this variable into
	// our internal storage.
	std::size_t size = in.size();
	if (write_size() < size)
	{
		mOverrun = true;
		return *this;
	}

	memcpy(&mpBuffer[mWriteIndex], in.data(), size);
	mWriteIndex += size;
	return *this;
}

v1::cRawDataBuffer& v1::cRawDataBuffer::operator<<(const v1::cRawDataBuffer*& in)
{
	// Check to make sure we have enough buffer space to put this variable into
	// our internal storage.
	std::size_t size = in->size();
	if (write_size() < size)
	{
		mOverrun = true;
		return *this;
	}

	memcpy(&mpBuffer[mWriteIndex], in->data(), size);
	mWriteIndex += size;
	return *this;
}

v1::cRawDataBuffer& v1::cRawDataBuffer::operator<<(const bool in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cRawDataBuffer& v1::cRawDataBuffer::operator<<(const char in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cRawDataBuffer& v1::cRawDataBuffer::operator<<(const int8_t in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cRawDataBuffer& v1::cRawDataBuffer::operator<<(const uint8_t in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cRawDataBuffer& v1::cRawDataBuffer::operator<<(const int16_t in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cRawDataBuffer& v1::cRawDataBuffer::operator<<(const uint16_t in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cRawDataBuffer& v1::cRawDataBuffer::operator<<(const int32_t in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cRawDataBuffer& v1::cRawDataBuffer::operator<<(const uint32_t in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cRawDataBuffer& v1::cRawDataBuffer::operator<<(const int64_t in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cRawDataBuffer& v1::cRawDataBuffer::operator<<(const uint64_t in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cRawDataBuffer& v1::cRawDataBuffer::operator<<(const float in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cRawDataBuffer& v1::cRawDataBuffer::operator<<(const double in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cRawDataBuffer& v1::cRawDataBuffer::operator<<(const std::string& in)
{
	// Check to make sure we have enough buffer space to put the length into
	// our internal storage.
	if (write_size() < sizeof(uint16_t))
	{
		mOverrun = true;
		return *this;
	}

	if (in.empty())
	{
		*this << static_cast<uint16_t>(0);
		return *this;
	}

	auto len = in.length();

	if (write_size() < (len+2))
	{
		len = write_size() - 2;
	}

	if (len > 65535)
		len = 65535;

	*this << static_cast<uint16_t>(len);

	// copy the string into the buffer
	memcpy(wrtbuf(), in.data(), len);
	mWriteIndex += len;

	return *this;
}

/*
void v1::cRawDataBuffer::write(const std::string& in)
{
	if (in.empty())
	{
		return;
	}

	auto len = in.length();

	// Check to make sure we have enough buffer space to put this variable into
	// our internal storage.
	if (write_size() < len)
	{
		mOverrun = true;
		return;
	}

	// put the string into the buffer
	memcpy(&mpBuffer[mWriteIndex], in.data(), len);

	mWriteIndex += len;
}
*/

void v1::cRawDataBuffer::write(const std::byte* in, unsigned char len)
{
	// Check to make sure we have enough buffer space to put this variable into
	// our internal storage.
	if (write_size() < len)
	{
		mOverrun = true;
		return;
	}

	if (in == nullptr)
	{
		// null out the char array in the buffer
		memset(&mpBuffer[mWriteIndex], 0, len);
		mWriteIndex += len;
		return;
	}

	// put the string into the buffer
	memcpy(&mpBuffer[mWriteIndex], in, len);

	mWriteIndex += len;
}




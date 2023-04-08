
#include "DataBuffer.hpp"

namespace
{
	template <typename T>
	T byte_swap(T b)
	{
		if (sizeof(T) == 1)
			return b;
	}

	/**
	 * \brief read
	 * \par Description
	 *		A template member function to read a variable from the internal buffer.
	 *
	 * \param X		-- a reference to the parameter to read from the buffer.
	 */
	template<typename T>
	std::size_t read(T& X, const std::byte* start, const std::byte* end, bool& underrun)
	{
		std::size_t len = std::distance(start, end);

		// Check to make sure we have enough buffer space to put this variable into
		// our internal storage.
		if (start >= end)
		{
			underrun = true;
			return 0;
		}

		auto s = sizeof(T);
		if (len < s)
		{
			underrun = true;
			return 0;
		}

//		if (mByteOrder == enumByteOrder_LITTLE_ENDIAN)
//		{
			X = *(reinterpret_cast<const T*>(start));
//		}
//		else
		{
//			unpack_s(X, start, mCapacity, mReadIndex, true);
		}

		return sizeof(T);
	}

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

//		if (mByteOrder == enumByteOrder_LITTLE_ENDIAN)
		{
			// copy the host order value into the buffer
			memcpy(start, &X, sizeof(T));
		}
//		else
		{
//			pack_s(X, mpBuffer, mCapacity, mWriteIndex, true);
		}

		return sizeof(T);
	};

}


///////////////////////////////////////////////////////////////////////////////
//
// Data Buffer
//
///////////////////////////////////////////////////////////////////////////////


v1::cDataBuffer::cDataBuffer()
: mpBuffer(nullptr), mReadIndex(0), mWriteIndex(0), mCapacity(0), 
	mUnderrun(false), mOverrun(false)
{
}

v1::cDataBuffer::cDataBuffer(size_type capacity)
: mpBuffer(nullptr), mReadIndex(0), mWriteIndex(0), mCapacity(0),
	mUnderrun(true), mOverrun(false)
{
	mpBuffer = new std::byte[capacity];

	if (mpBuffer != nullptr)
	{
		mCapacity = capacity;
	}
}

v1::cDataBuffer::cDataBuffer(const v1::cDataBuffer& objToCopy)
	: mpBuffer(nullptr), mReadIndex(0), 
	mWriteIndex(objToCopy.mWriteIndex),
	mCapacity(objToCopy.mCapacity),
	mUnderrun(objToCopy.mUnderrun), mOverrun(objToCopy.mOverrun)
{
	mpBuffer = new std::byte[objToCopy.mCapacity];

	memcpy(&mpBuffer[0], objToCopy.mpBuffer, mCapacity);
}

v1::cDataBuffer::cDataBuffer(v1::cDataBuffer&& objToMove) noexcept
{
	mpBuffer = objToMove.mpBuffer;
	mCapacity = objToMove.mCapacity;
	mReadIndex = objToMove.mReadIndex;
	mWriteIndex = objToMove.mWriteIndex;
	mUnderrun = objToMove.mUnderrun;
	mOverrun = objToMove.mOverrun;

	objToMove.mpBuffer = nullptr;
	objToMove.mCapacity = 0;
	objToMove.mReadIndex = 0;
	objToMove.mWriteIndex = 0;
	objToMove.mUnderrun = false;
	objToMove.mOverrun = false;
}

void v1::cDataBuffer::operator=(const v1::cDataBuffer& objToCopy)
{
	delete[] mpBuffer;
	mpBuffer = nullptr;
	mpBuffer = new std::byte[objToCopy.mCapacity];

	if (mpBuffer != nullptr)
	{
		mCapacity = objToCopy.mCapacity;
	}

	mReadIndex = 0;
	mWriteIndex = objToCopy.mWriteIndex;
	mUnderrun = objToCopy.mUnderrun;
	mOverrun = objToCopy.mOverrun;

	memcpy(&mpBuffer[0], objToCopy.mpBuffer, mCapacity);
}

v1::cDataBuffer::~cDataBuffer()
{
	delete[] mpBuffer;
	mpBuffer = nullptr;

	mReadIndex = 0;
	mWriteIndex = 0;
	mCapacity = 0;
	mUnderrun = false;
	mOverrun = false;
}

v1::cDataBuffer::operator v1::cDataBufferView()
{
	return cDataBufferView(rdbuf(), mWriteIndex-mReadIndex);
}

std::size_t v1::cDataBuffer::write_size() const
{
	if (mWriteIndex > mCapacity) return 0;
	return mCapacity - mWriteIndex;
}

std::size_t v1::cDataBuffer::read_size() const
{
	return mWriteIndex - mReadIndex;
}


std::size_t v1::cDataBuffer::size() const
{
	return mWriteIndex;
}

std::size_t v1::cDataBuffer::capacity() const
{
	return mCapacity;
}

void v1::cDataBuffer::capacity(size_type capacity)
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
			mUnderrun = false;
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

bool v1::cDataBuffer::empty() const
{
	return (mpBuffer == nullptr) || (mWriteIndex == 0);
}

bool v1::cDataBuffer::underrun() const
{
	return mUnderrun;
}

bool v1::cDataBuffer::overrun() const
{
	return mOverrun;
}

const std::byte* v1::cDataBuffer::data() const
{
	return mpBuffer;
}

std::byte* v1::cDataBuffer::data(std::size_t len)
{
	mWriteIndex += len;
	return mpBuffer;
}

void v1::cDataBuffer::attach(std::byte* buffer, std::size_t size)
{
	delete[] mpBuffer;

	mpBuffer = buffer;
	mCapacity = size;
	reset();
	mWriteIndex = size;
}

std::byte* v1::cDataBuffer::detach()
{
	std::byte* retPtr = mpBuffer;

	// Clear our internal pointer and reset the indexes
	mpBuffer = nullptr;
	mCapacity = 0;

	reset();

	return retPtr;
}

void v1::cDataBuffer::reset()
{
	mReadIndex = 0;
	mWriteIndex = 0;
	mUnderrun = false;
	mOverrun = false;
}

void v1::cDataBuffer::clear()
{
	reset();

	if (mpBuffer != nullptr)
		memset(mpBuffer, 0, mCapacity);
}

std::size_t v1::cDataBuffer::insert(const unsigned char* buffer, std::size_t size)
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

void v1::cDataBuffer::padTo(std::size_t size, std::byte value)
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

std::size_t v1::cDataBuffer::erase(std::size_t pos, std::size_t size)
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
// Input Stream Operators
///////////////////////////////////////////////////////////////////////////////

v1::cDataBuffer& v1::cDataBuffer::operator>>(cDataBuffer& out)
{
	out << *this;
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator>>(cDataBuffer*& out)
{
	(*out) << *this;
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator>>(bool& out)
{
	mReadIndex += ::read(out, rdbuf(), wrtbuf(), mUnderrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator>>(char& out)
{
	uint8_t ch;
	mReadIndex += ::read(ch, rdbuf(), wrtbuf(), mUnderrun);
	out = ch;
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator>>(int8_t& out)
{
	mReadIndex += ::read(out, rdbuf(), wrtbuf(), mUnderrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator>>(uint8_t& out)
{
	mReadIndex += ::read(out, rdbuf(), wrtbuf(), mUnderrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator>>(int16_t& out)
{
	mReadIndex += ::read(out, rdbuf(), wrtbuf(), mUnderrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator>>(uint16_t& out)
{
	mReadIndex += ::read(out, rdbuf(), wrtbuf(), mUnderrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator>>(int32_t& out)
{
	mReadIndex += ::read(out, rdbuf(), wrtbuf(), mUnderrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator>>(uint32_t& out)
{
	mReadIndex += ::read(out, rdbuf(), wrtbuf(), mUnderrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator>>(int64_t& out)
{
	mReadIndex += ::read(out, rdbuf(), wrtbuf(), mUnderrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator>>(uint64_t& out)
{
	mReadIndex += ::read(out, rdbuf(), wrtbuf(), mUnderrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator>>(float& out)
{
	mReadIndex += ::read(out, rdbuf(), wrtbuf(), mUnderrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator>>(double& out)
{
	mReadIndex += ::read(out, rdbuf(), wrtbuf(), mUnderrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator>>(std::string& out)
{
	read(out);
	return *this;
}

void v1::cDataBuffer::read(std::string& out)
{
	if (read_size() == 0)
	{
		// In earlier versions, an empty string was written with
		// zero data.  Now, a length of zero is written into the
		// data buffer.  This is to support the older version.
		out.clear();
		return;
	}

	uint16_t len = 0;
	mReadIndex += ::read(len, rdbuf(), wrtbuf(), mUnderrun);

	if (mUnderrun)
		return;

	// Check to make sure we have enough data in the read size of the buffer
	// to fulfill the read request.
	if ((mReadIndex > mWriteIndex) || (read_size() < len))
	{
		mUnderrun = true;
		return;
	}

	out.clear();

	if (len > 0)
	{
		out.resize(len);

		// copy the string from the buffer
		out.assign(reinterpret_cast<char*>(&mpBuffer[mReadIndex]), len);
	}

	mReadIndex += len;
}

void v1::cDataBuffer::read(std::string& out, uint16_t len)
{
	// Check to make sure we have enough data in the read size of the buffer
	// to fulfill the read request.
	if ((mReadIndex >= mWriteIndex) || (read_size() < len))
	{
		mUnderrun = true;
		return;
	}

	out.clear();

	// copy the string from the buffer
	out.assign(reinterpret_cast<char*>(&mpBuffer[mReadIndex]), len);

	mReadIndex += len;
}

void v1::cDataBuffer::read(std::byte*& out, uint16_t len)
{
	// Check to make sure we have enough data in the read size of the buffer
	// to fulfill the read request.
	if ((mReadIndex >= mWriteIndex) || (read_size() < len))
	{
		mUnderrun = true;
		return;
	}

	// copy the byte block from the buffer
	memcpy(out, reinterpret_cast<char*>(&mpBuffer[mReadIndex]), len);

	mReadIndex += len;
}

void v1::cDataBuffer::read(char* out, std::size_t len)
{
	// Check to make sure we have enough data in the read size of the buffer
	// to fulfill the read request.
	if ((mReadIndex >= mWriteIndex) || (read_size() < len))
	{
		mUnderrun = true;
		return;
	}

	// copy the char block from the buffer
	memcpy(out, reinterpret_cast<char*>(&mpBuffer[mReadIndex]), len);

	mReadIndex += len;
}

///////////////////////////////////////////////////////////////////////////////
// Output Stream Operators
///////////////////////////////////////////////////////////////////////////////

v1::cDataBuffer& v1::cDataBuffer::operator<<(const cDataBuffer& in)
{
	// Check to make sure we have enough buffer space to put this variable into
	// our internal storage.
	std::size_t size = in.read_size();
	if (write_size() < size)
	{
		mOverrun = true;
		return *this;
	}

	memcpy(&mpBuffer[mWriteIndex], in.rdbuf(), size);
	mWriteIndex += size;
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator<<(const cDataBuffer*& in)
{
	// Check to make sure we have enough buffer space to put this variable into
	// our internal storage.
	std::size_t size = in->read_size();
	if (write_size() < size)
	{
		mOverrun = true;
		return *this;
	}

	memcpy(&mpBuffer[mWriteIndex], in->rdbuf(), size);
	mWriteIndex += size;
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator<<(const bool in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator<<(const char in)
{
	mWriteIndex += ::write<uint8_t>(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator<<(const int8_t in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator<<(const uint8_t in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator<<(const int16_t in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator<<(const uint16_t in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator<<(const int32_t in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator<<(const uint32_t in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator<<(const int64_t in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator<<(const uint64_t in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator<<(const float in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator<<(const double in)
{
	mWriteIndex += ::write(in, wrtbuf(), end(), mOverrun);
	return *this;
}

v1::cDataBuffer& v1::cDataBuffer::operator<<(const std::string& in)
{
	write(in);
	return *this;
}

void v1::cDataBuffer::write(const std::string& in)
{
	uint16_t len = 0;
	if (in.length() > 65533)
		len = 65533;
	else
		len = in.length();

	// Check to make sure we have enough buffer space to put this variable into
	// our internal storage.
	if (write_size() < (len + sizeof(len)))
	{
		mOverrun = true;
		return;
	}

	// put the string length into the data buffer
	mWriteIndex += ::write(len, wrtbuf(), end(), mOverrun);

	if (len > 0)
	{
		// put the string into the buffer
		memcpy(&mpBuffer[mWriteIndex], in.data(), len);
	}

	mWriteIndex += len;
}

void v1::cDataBuffer::write(const std::byte* in, uint16_t len)
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

void v1::cDataBuffer::write(const char* in, std::size_t len)
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

	// put the data into the buffer
	memcpy(&mpBuffer[mWriteIndex], in, len);

	mWriteIndex += len;
}


///////////////////////////////////////////////////////////////////////////////
//
// Data Buffer View
//
///////////////////////////////////////////////////////////////////////////////


v1::cDataBufferView::cDataBufferView()
:
	mpBuffer(nullptr), mReadIndex(0), mCapacity(0), mUnderrun(false)
{}

v1::cDataBufferView::cDataBufferView(const std::byte* buffer, size_type length)
:
	mpBuffer(buffer), mReadIndex(0), mCapacity(length), mUnderrun(false)
{}

v1::cDataBufferView::cDataBufferView(const v1::cDataBufferView& objToCopy)
:
	mpBuffer(objToCopy.mpBuffer), mReadIndex(objToCopy.mReadIndex), 
	mCapacity(objToCopy.mCapacity), mUnderrun(objToCopy.mUnderrun)
{}

v1::cDataBufferView::cDataBufferView(v1::cDataBufferView&& objToMove) noexcept
:
	mpBuffer(objToMove.mpBuffer), mReadIndex(objToMove.mReadIndex),
	mCapacity(objToMove.mCapacity), mUnderrun(objToMove.mUnderrun)
{
	objToMove.mpBuffer = nullptr;
	objToMove.mReadIndex = 0;
	objToMove.mCapacity = 0;
	objToMove.mUnderrun = false;
}

void v1::cDataBufferView::operator=(const v1::cDataBufferView& objToCopy)
{
	mpBuffer = objToCopy.mpBuffer;
	mReadIndex = objToCopy.mReadIndex;
	mCapacity = objToCopy.mCapacity;
	mUnderrun = objToCopy.mUnderrun;
}

v1::cDataBufferView::size_type v1::cDataBufferView::size() const
{
	return mCapacity - mReadIndex;
}

v1::cDataBufferView::size_type v1::cDataBufferView::capacity() const
{
	return mCapacity;
}

bool v1::cDataBufferView::empty() const
{
	return (mpBuffer == nullptr) || (mReadIndex >= mCapacity);
}

bool v1::cDataBufferView::underrun() const
{
	return mUnderrun;
}

const std::byte* v1::cDataBufferView::data() const
{
	return rdbuf();
}

void v1::cDataBufferView::reset()
{
	mReadIndex = 0;
	mUnderrun = false;
}

void v1::cDataBufferView::advance(size_type n)
{
	mReadIndex += n;
	if (mReadIndex >= mCapacity)
	{
		mUnderrun = true;
		mReadIndex = mCapacity;
	}
}

v1::cDataBufferView& v1::cDataBufferView::operator>>(v1::cDataBufferView& out)
{
	mReadIndex += ::read(out, rdbuf(), end(), mUnderrun);
	return *this;
}

v1::cDataBufferView& v1::cDataBufferView::operator>>(v1::cDataBufferView* &out)
{
	mReadIndex += ::read(out, rdbuf(), end(), mUnderrun);
	return *this;
}

v1::cDataBufferView& v1::cDataBufferView::operator>>(bool& out)
{
	mReadIndex += ::read(out, rdbuf(), end(), mUnderrun);
	return *this;
}

v1::cDataBufferView& v1::cDataBufferView::operator>>(char& out)
{
	mReadIndex += ::read(out, rdbuf(), end(), mUnderrun);
	return *this;
}

v1::cDataBufferView& v1::cDataBufferView::operator>>(int8_t & out)
{
	mReadIndex += ::read(out, rdbuf(), end(), mUnderrun);
	return *this;
}

v1::cDataBufferView& v1::cDataBufferView::operator>>(uint8_t & out)
{
	mReadIndex += ::read(out, rdbuf(), end(), mUnderrun);
	return *this;
}

v1::cDataBufferView& v1::cDataBufferView::operator>>(int16_t & out)
{
	mReadIndex += ::read(out, rdbuf(), end(), mUnderrun);
	return *this;
}

v1::cDataBufferView& v1::cDataBufferView::operator>>(uint16_t & out)
{
	mReadIndex += ::read(out, rdbuf(), end(), mUnderrun);
	return *this;
}

v1::cDataBufferView& v1::cDataBufferView::operator>>(int32_t & out)
{
	mReadIndex += ::read(out, rdbuf(), end(), mUnderrun);
	return *this;
}

v1::cDataBufferView& v1::cDataBufferView::operator>>(uint32_t & out)
{
	mReadIndex += ::read(out, rdbuf(), end(), mUnderrun);
	return *this;
}

v1::cDataBufferView& v1::cDataBufferView::operator>>(int64_t & out)
{
	mReadIndex += ::read(out, rdbuf(), end(), mUnderrun);
	return *this;
}

v1::cDataBufferView& v1::cDataBufferView::operator>>(uint64_t & out)
{
	mReadIndex += ::read(out, rdbuf(), end(), mUnderrun);
	return *this;
}

v1::cDataBufferView& v1::cDataBufferView::operator>>(float& out)
{
	mReadIndex += ::read(out, rdbuf(), end(), mUnderrun);
	return *this;
}

v1::cDataBufferView& v1::cDataBufferView::operator>>(double& out)
{
	mReadIndex += ::read(out, rdbuf(), end(), mUnderrun);
	return *this;
}

void v1::cDataBufferView::read(std::string& out, unsigned char len)
{
	// Check to make sure we have enough buffer space to put this variable into
	// our internal storage.
	if ((mReadIndex >= mCapacity) || (size() < len))
	{
		mUnderrun = true;
		return;
	}

	out.clear();

	// copy the string from the buffer
	out.assign(reinterpret_cast<const char*>(&mpBuffer[mReadIndex]), len);

	mReadIndex += len;
}

void v1::cDataBufferView::read(std::byte*& out, unsigned char len)
{
	// Check to make sure we have enough buffer space to put this variable into
	// our internal storage.
	if ((mReadIndex >= mCapacity) || (size() < len))
	{
		mUnderrun = true;
		return;
	}

	// copy the string from the buffer
	memcpy(out, reinterpret_cast<const char*>(&mpBuffer[mReadIndex]), len);

	mReadIndex += len;

}



#pragma once

#include <cstdint>
#include <vector>
#include <string>

/******************************************************************************
 *
 * cRawDataBuffer: A write only data buffer.
 *
 * The data buffer writes into its internal buffer in the raw machine byte order.
 *
 ******************************************************************************/

namespace v1
{
    class cRawDataBuffer
    {
    public:

        typedef std::size_t	size_type;

    public:
        /** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * \name Construction/Destruction
         *
         */
         //@{

             /**
              * \brief Default Constructor
              * \par Description
              *		Default Constructor for the net_buffer class.  Creates a buffer
              *		of zero size.
              */
        explicit cRawDataBuffer();

        /**
         * \brief Constructor
         * \par Description
         *		Constructor for the net_buffer class.  Creates a buffer of
         *		length capacity.
         *
         * \param capacity		-- the number of bytes to allocate for the buffer.
         */
        explicit cRawDataBuffer(size_type capacity);

        /**
        *	\brief	Copy/move constructors.
        *
        *	\par	Description:
        *			Copy/move constructor. (deep copy);
        */
        cRawDataBuffer(const cRawDataBuffer& objToCopy);
        cRawDataBuffer(cRawDataBuffer&& objToCopy) noexcept;


        /**
        *	\brief	Assignment operator.
        *
        *	\par	Description:
        *			Assignment operator. (deep copy)
        */
        void operator=(const cRawDataBuffer& objToCopy);


        /**
         * \brief Destructor
         * \par Description
         *		Frees up any resources used by data buffer.
         */
        ~cRawDataBuffer();
        //@}

        /** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * \name Properties
         *
         */
         //@{
             /**
              * \brief write_size
              * \par Description
              *		Returns the difference between the buffer's capacity and it current
              *		size.
              *
              * The write size is the diffence between the buffer capacity and
              * current write position
              */
        size_type write_size() const;

        /**
         * \brief size
         * \par Description
         *		Returns the total length of the buffer data that can be read from.
         *
         * The read size is the diffence between the current write position
         * and the start of the buffer.
         */
        size_type size() const;

        /**
         * \brief capacity
         * \par Description
         *		Returns the storage currently allocated to hold the buffer
         *		data, a value at least as large as size().
         */
        size_type capacity() const;
        void capacity(size_type capacity);

        /**
         * \brief empty
         * \par Description
         *		Checks if the current the buffer data is empty.
         */
        bool empty() const;

        /**
         * \brief overrun
         * \par Description
         *		True, if there is not enough space in the data buffer to write more data.
         */
        bool overrun() const;

        /**
         * \brief data
         * \par Description
         *		Returns a pointer to the currently allocated buffer.
         *
         * Note: data returns a const char* for use
         */
        const std::byte* data() const;
        //@}

        /** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * \name Operations
         *
         */
         //@{
        void attach(std::byte* buffer, size_type size);
        std::byte* detach();

        /**
         * \brief reset
         * \par Description
         *		Resets the current read/write position to the beginning of the
         *		control sequence.
         */
        void reset();

        /**
         * \brief clear
         * \par Description
         *		Clears the current contents of the of the control sequence.
         */
        void clear();

        /**
         * \brief insert
         * \par Description
         *		Inserts a unsigned char buffer into the control sequence at the
         *		current write position.
         *
         * \return unsigned long	-- number of elements inserted into control sequence.
         */
        size_type insert(const unsigned char* buffer, size_type size);

        /**
         * \brief padTo
         * \par   Description
         *        Inserts zeroes into the buffer until it is the specified size.
         */
        void padTo(size_type size, std::byte value = static_cast<std::byte>(0));

        /**
         * \brief erase
         * \par Description
         *		Removes size number of elements from the control sequence starting at
         *		position pos.
         *
         * \return unsigned long	-- number of elements removed from control sequence.
         */
        size_type erase(std::size_t pos, size_type size);
        //@}

        /** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * \name Output Stream Operators
         *
         */
         //@{
             /**
              * \brief operator<<
              * \par Description
              *		Overloaded output stream operators to handle packing the variable into
              *		the internal buffer.
              *
              * \param in	-- the parameter to write into the buffer.
              */
        cRawDataBuffer& operator<<(const cRawDataBuffer& in);
        cRawDataBuffer& operator<<(const cRawDataBuffer*& in);
        cRawDataBuffer& operator<<(const bool in);
        cRawDataBuffer& operator<<(const char in);
        cRawDataBuffer& operator<<(const int8_t in);
        cRawDataBuffer& operator<<(const uint8_t in);
        cRawDataBuffer& operator<<(const int16_t in);
        cRawDataBuffer& operator<<(const uint16_t in);
        cRawDataBuffer& operator<<(const int32_t in);
        cRawDataBuffer& operator<<(const uint32_t in);
        cRawDataBuffer& operator<<(const int64_t in);
        cRawDataBuffer& operator<<(const uint64_t in);
        cRawDataBuffer& operator<<(const float in);
        cRawDataBuffer& operator<<(const double in);
        cRawDataBuffer& operator<<(const std::string& in);

        //        void write(const std::string& in);
        void write(const std::byte* in, unsigned char len);
        //@}

    protected:
        /** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * \name Helper Methods
         *
         */
         //@{
            /**
             * \brief wrtbuf
             * \par Description
             *		Returns a pointer to the current write position.
             */
        std::byte* wrtbuf() const { return &mpBuffer[mWriteIndex]; };

        /**
         * \brief end
         * \par Description
         *		Returns a pointer to the end of the buffer.
         */
        const std::byte* end() const { return &mpBuffer[mCapacity]; };
        //@}

    private:
        /** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * \name Data Members
         *
         */
         //@{

            /**
            *  \var	mBuffer
            *  \brief  The internal buffer storage.
            */
        std::byte* mpBuffer;

        /**
         *  \var	mWriteIndex
         *  \brief  The current write index in the internal buffer.
         */
        size_type mWriteIndex;

        /**
         *  \var	mCapacity
         *  \brief  The capacity (number of bytes) of the internal buffer storage.
         */
        size_type mCapacity;

        /**
         *  \var	mOverrun
         *  \brief  The write index is at the end of the internal buffer storage.
         */
        bool mOverrun;
        //@}
    };
}


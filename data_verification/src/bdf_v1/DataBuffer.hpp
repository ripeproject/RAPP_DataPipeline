
#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace v1
{
    class cDataBufferView;


    /******************************************************************************
     *
     * cDataBuffer: A read/write data buffer.
     *
     * The data buffer will auto
     *
     ******************************************************************************/


    class cDataBuffer
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
        explicit cDataBuffer();

        /**
         * \brief Constructor
         * \par Description
         *		Constructor for the net_buffer class.  Creates a buffer of
         *		length capacity.
         *
         * \param capacity		-- the number of bytes to allocate for the buffer.
         */
        explicit cDataBuffer(size_type capacity);

        /**
        *	\brief	Copy/move constructors.
        *
        *	\par	Description:
        *			Copy/move constructor. (deep copy);
        */
        cDataBuffer(const cDataBuffer& objToCopy);
        cDataBuffer(cDataBuffer&& objToCopy) noexcept;


        /**
        *	\brief	Assignment operator.
        *
        *	\par	Description:
        *			Assignment operator. (deep copy)
        */
        void operator=(const cDataBuffer& objToCopy);


        /**
         * \brief Destructor
         * \par Description
         *		Frees up any resources used by data buffer.
         */
        ~cDataBuffer();
        //@}

        operator cDataBufferView();

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
         * \brief read_size
         * \par Description
         *		Returns the length of the buffer data that can be read from.
         *
         * The read size is the diffence between the current write position
         * and the current read position.
         */
        size_type read_size() const;

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
         * \brief underrun
         * \par Description
         *		True, if there is not enough space in the data buffer to read more data.
         */
        bool underrun() const;

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
        std::byte* data(std::size_t len);
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
        virtual void clear();

        /**
         * \brief insert
         * \par Description
         *		Inserts a unsigned char buffer into the control sequence at the
         *		current write position.
         *
         * \return unsigned long	-- number of elements inserted into control sequence.
         */
        virtual size_type insert(const unsigned char* buffer, size_type size);

        /**
         * \brief padTo
         * \par   Description
         *        Inserts zeroes into the buffer until it is the specified size.
         */
        virtual void padTo(size_type size, std::byte value = static_cast<std::byte>(0));

        /**
         * \brief erase
         * \par Description
         *		Removes size number of elements from the control sequence starting at
         *		position pos.
         *
         * \return unsigned long	-- number of elements removed from control sequence.
         */
        virtual size_type erase(std::size_t pos, size_type size);
        //@}

        /** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * \name Input Stream Operators
         *
         */
         //@{

             /**
              * \brief operator>>
              * \par Description
              *		The input stream operator to unpack a variable from the internal buffer.
              *
              * \param out	-- a reference to the parameter to read from the buffer.
              */
        cDataBuffer& operator>>(cDataBuffer& out);
        cDataBuffer& operator>>(cDataBuffer*& out);
        cDataBuffer& operator>>(bool& out);
        cDataBuffer& operator>>(char& out);
        cDataBuffer& operator>>(int8_t& out);
        cDataBuffer& operator>>(uint8_t& out);
        cDataBuffer& operator>>(int16_t& out);
        cDataBuffer& operator>>(uint16_t& out);
        cDataBuffer& operator>>(int32_t& out);
        cDataBuffer& operator>>(uint32_t& out);
        cDataBuffer& operator>>(int64_t& out);
        cDataBuffer& operator>>(uint64_t& out);
        cDataBuffer& operator>>(float& out);
        cDataBuffer& operator>>(double& out);
        cDataBuffer& operator>>(std::string& out);

        template<typename T>
        T get()
        {
            T result;
            operator>>(result);
            return result;
        }

        void read(std::string& out);
        void read(std::string& out, uint16_t len);
        void read(std::byte*& out, uint16_t len);
        void read(char* out, std::size_t len);
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
        cDataBuffer& operator<<(const cDataBuffer& in);
        cDataBuffer& operator<<(const cDataBuffer*& in);
        cDataBuffer& operator<<(const bool in);
        cDataBuffer& operator<<(const char in);
        cDataBuffer& operator<<(const int8_t in);
        cDataBuffer& operator<<(const uint8_t in);
        cDataBuffer& operator<<(const int16_t in);
        cDataBuffer& operator<<(const uint16_t in);
        cDataBuffer& operator<<(const int32_t in);
        cDataBuffer& operator<<(const uint32_t in);
        cDataBuffer& operator<<(const int64_t in);
        cDataBuffer& operator<<(const uint64_t in);
        cDataBuffer& operator<<(const float in);
        cDataBuffer& operator<<(const double in);
        cDataBuffer& operator<<(const std::string& in);

        template<typename T>
        void put(const T& in)
        {
            operator<<(in);
        }

        void write(const std::string& in);
        void write(const std::byte* in, uint16_t len);
        void write(const char* in, std::size_t len);
        //@}

    protected:
        /** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * \name Helper Methods
         *
         */
         //@{
             /**
              * \brief rdbuf
              * \par Description
              *		Returns a pointer to the current read position.
              */
        const std::byte* rdbuf() const { return &mpBuffer[mReadIndex]; };

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
         *  \var	mReadIndex
         *  \brief  The current read index in the internal buffer.
         */
        size_type mReadIndex;

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
         *  \var	mUnderrun
         *  \brief  The read index is at the end of the internal buffer storage.
         */
        bool mUnderrun;

        /**
         *  \var	mOverrun
         *  \brief  The write index is at the end of the internal buffer storage.
         */
        bool mOverrun;
        //@}
    };



    class cDataBufferView
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
        explicit cDataBufferView();

        /**
         * \brief Constructor
         * \par Description
         *		Constructor for the net_buffer class.  Creates a buffer of
         *		length capacity.
         *
         * \param length		-- the number of bytes in the view.
         */
        explicit cDataBufferView(const std::byte* buffer, size_type length);

        /**
        *	\brief	Copy/move constructors.
        *
        *	\par	Description:
        *			Copy/move constructor. (shallow copy);
        */
        cDataBufferView(const cDataBufferView& objToCopy);
        cDataBufferView(cDataBufferView&& objToCopy) noexcept;

        /**
        *	\brief	Assignment operator.
        *
        *	\par	Description:
        *			Assignment operator. (deep copy)
        */
        void operator=(const cDataBufferView& objToCopy);

        /**
         * \brief Destructor
         * \par Description
         *		Frees up any resources used by data buffer view.
         */
        ~cDataBufferView() = default;
        //@}

        /** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * \name Properties
         *
         */
         //@{
            /**
             * \brief size
             * \par Description
             *		Returns the length of the buffer data.
             *
             * This is the number of bytes that can be read from the buffer.
             */
        size_type size() const;

        /**
         * \brief capacity
         * \par Description
         *		Returns the storage currently allocated to hold the buffer
         *		data, a value at least as large as size().
         */
        size_type capacity() const;

        /**
         * \brief empty
         * \par Description
         *		Checks if the current the buffer data is empty.
         */
        bool empty() const;

        /**
         * \brief underrun
         * \par Description
         *		True, if there is not enough space in the data buffer to read more data.
         */
        bool underrun() const;

        /**
         * \brief data
         * \par Description
         *		Returns a pointer to the currently read position buffer.
         */
        const std::byte* data() const;
        //@}

        /** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * \name Operations
         *
         */
         //@{
            /**
             * \brief reset
             * \par Description
             *		Resets the current read position to the beginning of the
             *		control sequence.
             */
        void reset();

        /**
         * \brief advance
         * \par Description
         *		Advances the read position n positions in the
         *		control sequence.
         */
        void advance(size_type n);
        //@}

    /** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * \name Input Stream Operators
     *
     */
     //@{

         /**
          * \brief operator>>
          * \par Description
          *		The input stream operator to unpack a variable from the internal buffer.
          *
          * \param out	-- a reference to the parameter to read from the buffer.
          */
        cDataBufferView& operator>>(cDataBufferView& out);
        cDataBufferView& operator>>(cDataBufferView*& out);
        cDataBufferView& operator>>(bool& out);
        cDataBufferView& operator>>(char& out);
        cDataBufferView& operator>>(int8_t& out);
        cDataBufferView& operator>>(uint8_t& out);
        cDataBufferView& operator>>(int16_t& out);
        cDataBufferView& operator>>(uint16_t& out);
        cDataBufferView& operator>>(int32_t& out);
        cDataBufferView& operator>>(uint32_t& out);
        cDataBufferView& operator>>(int64_t& out);
        cDataBufferView& operator>>(uint64_t& out);
        cDataBufferView& operator>>(float& out);
        cDataBufferView& operator>>(double& out);

        void read(std::string& out, unsigned char len);
        void read(std::byte*& out, unsigned char len);
        //@}

    protected:
        /** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * \name Helper Methods
         *
         */
         //@{
             /**
              * \brief rdbuf
              * \par Description
              *		Returns a pointer to the current read position.
              */
        const std::byte* rdbuf() const { return &mpBuffer[mReadIndex]; };

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
        const std::byte* mpBuffer;

        /**
         *  \var	mReadIndex
         *  \brief  The current read index in the internal buffer.
         */
        size_type mReadIndex;

        /**
         *  \var	mCapacity
         *  \brief  The capacity (number of bytes) of the internal buffer storage.
         */
        size_type mCapacity;

        /**
         *  \var	mUnderrun
         *  \brief  The read index is at the end of the internal buffer storage.
         */
        bool mUnderrun;
        //@}
    };

}


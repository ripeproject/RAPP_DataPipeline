
#pragma once

#include "BlockId.hpp"

#include <stdexcept>
#include <string>

namespace v1
{
    namespace bdf
    {
        class crc_error : public std::runtime_error
        {
        public:
            using base = std::runtime_error;

            explicit crc_error(BLOCK_CLASS_ID_t _class_id, BLOCK_MAJOR_VERSION_t _major_ver,
                BLOCK_MINOR_VERSION_t _minor_ver, BLOCK_DATA_ID_t _data_id,
                const std::string& _Message)
                : base(_Message.c_str()), class_id(_class_id), major_version(_major_ver),
                minor_version(_minor_ver), data_id(_data_id)
            {}

            explicit crc_error(BLOCK_CLASS_ID_t _class_id, BLOCK_MAJOR_VERSION_t _major_ver,
                BLOCK_MINOR_VERSION_t _minor_ver, BLOCK_DATA_ID_t _data_id,
                const char* _Message)
                : base(_Message), class_id(_class_id), major_version(_major_ver),
                minor_version(_minor_ver), data_id(_data_id)
            {}

            const BLOCK_CLASS_ID_t        class_id;
            const BLOCK_MAJOR_VERSION_t   major_version;
            const BLOCK_MINOR_VERSION_t   minor_version;
            const BLOCK_DATA_ID_t         data_id;
        };

        class io_error : public std::runtime_error
        {
        public:
            using base = std::runtime_error;

            explicit io_error(const std::string& _Message) : base(_Message.c_str())
            {}

            explicit io_error(const char* _Message) : base(_Message)
            {}
        };

        class stream_error : public io_error
        {
        public:
            using base = bdf::io_error;

            explicit stream_error(int error, const std::string& _Message)
                : base(_Message.c_str()), error_number(error)
            {}

            explicit stream_error(int error, const char* _Message)
                : base(_Message), error_number(error)
            {}

            const int error_number;
        };

        class formatting_error : public io_error
        {
        public:
            using base = bdf::io_error;

            explicit formatting_error(const std::string& _Message)
                : base(_Message.c_str())
            {}

            explicit formatting_error(const char* _Message) : base(_Message)
            {}
        };
    }
}

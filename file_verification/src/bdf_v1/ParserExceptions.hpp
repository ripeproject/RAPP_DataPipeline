
#pragma once

#include <stdexcept>
#include <string>

namespace v1
{
    namespace bdf
    {
        class parse_error : public std::runtime_error
        {
        public:
            using base = std::runtime_error;

            explicit parse_error(const std::string& _Message)
                : base(_Message.c_str())
            {}

            explicit parse_error(const char* _Message)
                : base(_Message)
            {}
        };

        class invalid_data : public std::runtime_error
        {
        public:
            using base = std::runtime_error;

            explicit invalid_data(const std::string& _Message)
                : base(_Message.c_str())
            {}

            explicit invalid_data(const char* _Message)
                : base(_Message)
            {}
        };
    }
}

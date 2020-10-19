#pragma once
#include <cstdint>

namespace netcom 
{
class Message
{
    struct Header{
        uint32_t Id;
        uint32_t Size;
    };
    public:
    Header header;
};

} // namespace netcom
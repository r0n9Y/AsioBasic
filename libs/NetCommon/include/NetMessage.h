#pragma once
#include <cstdint>
#include "NetCommon.h"

namespace net
{
    template <typename ID>
    struct MessageHeader
    {
        ID id{};
        uint32_t size = 0; // in bytes
    };

    template <typename T>
    class Message
    {
    public:
        MessageHeader<T> header{};
        std::vector<uint8_t> body;

        size_t size()
        {
            return sizeof(MessageHeader<T>) + body.size();
        }

        friend std::ostream& operator<<(std::ostream &os, const Message<T>& message)
        {
            os << int(message.header.id) << " size: " << message.size();
            return os;
        }

        template<typename DataType>
        friend Message<T>& operator << (Message<T>& message, const DataType& data)
        {
            // https://docs.microsoft.com/en-us/cpp/cpp/trivial-standard-layout-and-pod-types?view=vs-2019
            static_assert(std::is_standard_layout<DataType>::value, "Unable to push non standard layout type data");
            size_t size = message.body.size();
            message.body.resize(size + sizeof(data));
            memcpy(message.body.data() + size, &data, sizeof(data));
            message.header.size = message.size();
            return message;
        }

        template<typename DataType>
        friend Message<T>& operator >> (Message<T>& message, DataType& data)
        {
            static_assert(std::is_standard_layout<DataType>::value, "Unable to output non-standard layout type data");
            size_t i = message.body.size() - sizeof(DataType);
            memcpy(&data, message.body().data() + i, sizeof(DataType));
            message.body.resize(i);
            message.header.size = message.size();
            return message;

        }
    };

} // namespace net
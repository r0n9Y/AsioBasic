#pragma once
#include <cstdint>
#include <memory>
#include "net_common.h"

namespace net
{
    template <typename ID>
    struct message_header
    {
        ID id{};
        uint32_t size = 0; // in bytes
    };

    template <typename T>
    class message
    {
    public:
        message_header<T> header{};
        std::vector<uint8_t> body;

        size_t size()
        {
            return sizeof(message_header<T>) + body.size();
        }

        friend std::ostream& operator<<(std::ostream &os, const message<T>& msg)
        {
            os << int(msg.header.id) << " size: " << msg.size();
            return os;
        }

        template<typename DataType>
        friend message<T>& operator << (message<T>& msg, const DataType& data)
        {
            // https://docs.microsoft.com/en-us/cpp/cpp/trivial-standard-layout-and-pod-types?view=vs-2019
            static_assert(std::is_standard_layout<DataType>::value, "Unable to push non standard layout type data");
            size_t size = msg.body.size();
            msg.body.resize(size + sizeof(data));
            memcpy(msg.body.data() + size, &data, sizeof(data));
            msg.header.size = msg.size();
            return msg;
        }

        template<typename DataType>
        friend message<T>& operator >> (message<T>& msg, DataType& data)
        {
            static_assert(std::is_standard_layout<DataType>::value, "Unable to output non-standard layout type data");
            size_t i = msg.body.size() - sizeof(DataType);
            memcpy(&data, msg.body().data() + i, sizeof(DataType));
            msg.body.resize(i);
            msg.header.size = msg.size();
            return msg;

        }
    };

    template<typename T> class connection;
    template<typename T>
    struct owned_message
    {
        std::shared_ptr<connection<T>> remote = nullptr;
        message<T> content;

        friend std::ostream& operator << (std::ostream& os, const owned_message<T>& msg)
        {
            os << msg.content;
            return os;
        }
        
    };
} // namespace net
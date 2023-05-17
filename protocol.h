#pragma once

#include "decoder.h"

#pragma pack(push, 1)
namespace protocol
{
    using ChannelId = uint32_t;
    using ChannelOpcode = uint16_t;
    using Status = int32_t;
    constexpr ChannelId MaxL2capChannel = 0xFFFF;
    constexpr uint32_t Magic = 0xABBACAFE;

    struct Header
    {
        uint32_t Magic;
        ChannelId Channel;
        uint32_t Length;
    };

    namespace Gap
    {
        constexpr ChannelId channelId = 0x10000;

        enum class OpCode : ChannelOpcode
        {
            Connect = 42,
            GetState = 43,
        };

        
        // This is the line presentation of BLE address
        typedef struct ble_address
        {
            // Little-endian byte order
            std::array<std::byte, 6> address;
            uint8_t type;
        } ble_address_t;

        struct Connect
        {
            struct Req
            {
                ble_address_t address;
            };
            struct Resp
            {
                int32_t status;
            };
            using ReturnType = protocol::Status;
            using Decoder = protocol::Decoder<Resp, ReturnType>;
        };
    
        struct State
        {
            bool connected;

        };

        struct GetState
        {
            struct Req
            {
            };
            struct Resp
            {
                int state;
            };
            using ReturnType = protocol::Gap::State;
            using Decoder = protocol::Decoder<Resp, ReturnType>;
        };
    };
};
#pragma pack(pop)


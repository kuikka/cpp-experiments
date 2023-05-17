#include <cstddef>
#include <cstring>
#include <gsl/span>

#include "bluetooth.h"
#include "buffer.h"
#include "decoder.h"
#include "protocol.h"
#include "xputils.h"

struct ChannelReceiver
{
    std::map<protocol::ChannelOpcode, std::shared_ptr<protocol::DecoderBase>> responseHandlers;

    void Receive(buffer::Span<std::byte> channelData)
    {
        buffer::SpanReader reader(channelData);
        auto channelOpcode = reader.Get<protocol::ChannelOpcode>();
        auto it = responseHandlers.find(channelOpcode);
        if (it != responseHandlers.end())
        {
            auto decoder = it->second;
            decoder->Decode(reader);
        }
    }
};

template<typename R, typename T>
auto CreateDecoder(T&& arg)
{
    return std::make_shared<protocol::Decoder<typename R::Resp, typename R::ReturnType>>(std::forward<T>(arg));
}



// auto connectRespDecoder = std::make_shared<protocol::Gap::Connect::Decoder>(
//                 [](const protocol::Gap::Connect::Resp& resp, protocol::Gap::Connect::Decoder& decoder){
//                     log("{}\n", resp.status);
//                     if (resp.status == 0) {
//                         decoder.CompletePendingRequests(resp.status);
//                     }
//                 });

auto connectRespDecoder = CreateDecoder<protocol::Gap::Connect>(
                [](const protocol::Gap::Connect::Resp& resp, protocol::Gap::Connect::Decoder& decoder){
                    log("{}\n", resp.status);
                    if (resp.status == 0) {
                        decoder.CompletePendingRequests(resp.status);
                    }
                });

struct GapChannelReceiver : ChannelReceiver
{
    GapChannelReceiver()
    {
        responseHandlers[as_integer(protocol::Gap::OpCode::Connect)] = connectRespDecoder;
    }
};


namespace ble
{
    namespace Gap
    {
        // std::future<int> connect()
        // {
        //     bluetooth::Gap::Connect
        //     return connectRespDecoder->addPendingRequest();
        // }
    };
};


struct ChannelSender
{

};

struct DataReceiver
{
    std::vector<std::byte> buffer;
    std::map<protocol::ChannelId, std::shared_ptr<ChannelReceiver>> channelReceivers;

    DataReceiver()
    {
        channelReceivers[protocol::Gap::channelId] = std::make_shared<GapChannelReceiver>();
    }

    /**
     * @brief Receive data from transport layer
     * 
     * All data must be received, caller will not retain it after the return of this call.
     * 
     * @param data Data to receive.
     */
    void ReceiveData(buffer::Span<const std::byte> data)
    {
        buffer.insert(buffer.end(), data.begin(), data.end());
        ParseAndDispatchData();
    }

    void ParseAndDispatchData()
    {
        buffer::VectorReader reader(buffer);
        auto header = reader.Get<protocol::Header>();

        if (header.Length > reader.Remaining()) {
            // Return early, we don't have enough data for this message yet
            // Note that we do NOT call Commit() on reader, we retain data/state in buffer.
            return;
        }

        // Create a view into the message
        auto channelData = reader.Consume(header.Length);

        // Check if this is a raw L2CAP channel
        if (header.Channel <= protocol::MaxL2capChannel)
        {
            
        }
        else
        {
            // Get the receiver for this channel
            auto it = channelReceivers.find(header.Channel);
            if (it != channelReceivers.end())
            {
                auto channelReceiver = it->second;
                channelReceiver->Receive(channelData);
            }
            else
            {
                log("ERROR: Received message for unknown channel 0x{:x}\n", header.Channel);
            }
        }

        // Remove all consumed data from buffer
        reader.Commit();
    }
};

auto main() -> int
{
    #pragma pack(push, 1)
    struct packet
    {
        protocol::Header header;
        protocol::ChannelOpcode OpCode;
        protocol::Gap::Connect::Req req;
    };

    struct packet2
    {
        protocol::Header header;
        protocol::ChannelOpcode OpCode;
        protocol::Gap::Connect::Resp resp;
    };
    #pragma pack(pop)

    union
    {
        struct packet p;
        std::array<std::byte, sizeof(p) + 1> arr;
    } u;

    union
    {
        struct packet p;
        std::array<std::byte, sizeof(p)> arr;
    } u2;

    u.p.header.Channel = protocol::Gap::channelId;
    u.p.header.Length = sizeof(packet) - sizeof(protocol::Header);
    u.p.header.Magic = protocol::Magic;
    u.p.OpCode = 42;
    // u.p.req.address = 1234;
    // u.p.req.type = 2;

    buffer::Span<std::byte> s(u.arr);
    DataReceiver dr;
    dr.ReceiveData(s);
}
#include <cstddef>
#include <cstring>
#include <gsl/span>

#include "xputils.h"
#include "buffer.h"
#include "decoder.h"

#pragma pack(push, 1)
struct Header
{
    uint32_t Magic;
    uint32_t Channel;
    uint32_t Length;
};

namespace Gap {
    struct Header
    {
        uint16_t Opcode;
    };
    struct EnableScan
    {
        struct Header Header;
        uint8_t Enable;
    };
};


struct ConnectReq
{
    uint64_t address;
    uint8_t type;
};

struct ConnectResp
{
    int32_t status;
};

#pragma pack(pop)



// auto ConnectReqDecoder = Decoder<ConnectReq>(
//         [](const ConnectReq& req){
//             log("{} {}\n", req.address, req.type);
//         });

// auto EnableScanDecoder = Decoder<Gap::EnableScan>(
//         [](const Gap::EnableScan& req){
//             log("{} {}\n", req.Header.Opcode, req.Enable);
//         });

// std::map<uint16_t, DecoderBase&> requestHandlers = {
//     { 42, ConnectReqDecoder },
//     { 43, EnableScanDecoder }
// };

template<>
struct fmt::formatter<Header>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(Header const& msg, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), "Magic={0}", msg.Magic);
    }
};

void DecodeArray(buffer::Span<std::byte> s)
{
    buffer::SpanReader reader(s);

    // uint16_t opcode = reader.Get<uint16_t>();
    // auto it = requestHandlers.find(opcode);
    // auto ret = it->second.Decode(reader);
    // log("ret = {}\n", ret);
}

void DecodeTest()
{
    std::array<std::byte, 11> buffer = {
        std::byte{42}, std::byte{0},
        std::byte{1}, std::byte{0}, std::byte{0}, std::byte{0}, std::byte{0}, std::byte{0}, std::byte{0}, std::byte{0},
        std::byte{0}
    };
    DecodeArray(buffer);
    buffer[0] = static_cast<std::byte>(43);
    DecodeArray(buffer);
}

struct BleAddress
{
    std::array<std::byte, 6> Address;
    uint8_t Type;
};

// auto connect(BleAddress address) -> std::future<bool>
// {
//    std::promise<bool> promise;

// }


namespace Gatt {
    using AttributeHandle = uint16_t;
    using DataBuffer = std::vector<std::byte>;

    // auto ReadCharacteristic(AttributeHandle handle) -> std::future<DataBuffer>
    // {

    // }
}

auto main() -> int
{
    std::array<std::byte, 10> buffer = {
        std::byte{1}, std::byte{2}, std::byte{0}, std::byte{0}, std::byte{100},
        std::byte{2}, std::byte{2}, std::byte{0}, std::byte{10}, std::byte{200},
    };
    buffer::Span<const std::byte> s{buffer};
    DecodeTest();
    // buffer::SpanReader sr(s);

    // log("sizeof(my_msg) = {}\n", sizeof(my_msg));
    // log("sizeof(my_msg2) = {}\n", sizeof(my_msg2));

    // auto msg1 = sr.Get<struct my_msg>();
    // auto msg2 = sr.Get<struct my_msg>();

    // log("msg1 {}\n", msg1);
    // log("msg2 {}\n", msg2);
}
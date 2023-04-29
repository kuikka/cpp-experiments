#include <iostream>
#include <vector>
#include <cstddef>
#include <gsl/span>
#include <cstring>
#include <fmt/core.h>

#include "xputils.h"


namespace buffer
{
    template <typename T>
    using Span = gsl::span<T>;

    class BufferBuilder
    {
    private:
        std::vector<std::byte> buffer;
    public:
        BufferBuilder() = default;

        template<typename T>
        void Put(const T& value)
        {

        }
    };

    class SpanReader
    {
    private:
        Span<const std::byte> view;
        size_t consumed = 0;

    private:
        auto consume(size_t bytes_to_consume) -> Span<const std::byte>
        {
            auto ret = view.subspan(consumed, bytes_to_consume);
            consumed += bytes_to_consume;
            return ret;
        }

    public:
        SpanReader(Span<const std::byte>& span) : view(span) {}

        template<typename T>
        auto Get() -> T
        {
            static_assert(std::is_trivially_copyable_v<T>);
            if constexpr (std::alignment_of<T>::value == 1)
            {
                const T* value = reinterpret_cast<const T*>(consume(sizeof(T)).data());
                return *value;
            }
            else
            {
                auto data = consume(sizeof(T));
                union {
                    std::array<std::byte, sizeof(T)> buffer;
                    T value;
                } u;
                std::copy(data.begin(), data.end(), u.buffer.begin());
                return u.value;
            }
        }
    };
};


#pragma pack(push, 1)
struct my_msg
{
    uint32_t magic;
    uint8_t value;
};
#pragma pack(pop)

struct my_msg2
{
    uint32_t magic;
    uint8_t value;
};

template<>
struct fmt::formatter<my_msg>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(my_msg const& msg, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), "magic={0}, value={1}", msg.magic, msg.value);
    }
};

auto main() -> int
{
    std::array<std::byte, 10> buffer = {
        std::byte{1}, std::byte{2}, std::byte{0}, std::byte{0}, std::byte{100},
        std::byte{2}, std::byte{2}, std::byte{0}, std::byte{0}, std::byte{200},
    };
    buffer::Span<const std::byte> s{buffer};
    buffer::SpanReader sr(s);

    log("sizeof(my_msg) = {}\n", sizeof(my_msg));
    log("sizeof(my_msg2) = {}\n", sizeof(my_msg2));

    auto msg1 = sr.Get<struct my_msg>();
    auto msg2 = sr.Get<struct my_msg>();

    log("msg1 {}\n", msg1);
    log("msg2 {}\n", msg2);
}
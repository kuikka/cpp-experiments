#include <gtest/gtest.h>

#include <array>
#include <cstring>
#include <gsl/span>
#include <memory>
#include <numeric>
#include <vector>

#include "buffer.h"
// #include "xputils.h"


TEST(SpanReaderTests, Construct)
{
    std::array<std::byte, 8> data;
    buffer::Span<std::byte> span(data);
    buffer::SpanReader reader(span);

    EXPECT_EQ(data.size(), span.size());
    EXPECT_EQ(data.size(), span.size_bytes());
    EXPECT_EQ(data.size(), reader.remaining());
}

TEST(SpanReaderTests, GetBytes)
{
    std::array<std::byte, 8> data;
    buffer::Span<std::byte> span(data);
    buffer::SpanReader reader(span);

    std::fill(data.begin(), data.end(), static_cast<std::byte>('X'));

    for (int i = 0; i < data.size(); i++) {
        auto v = reader.Get<char>();
        EXPECT_EQ(v, 'X');
    }
    EXPECT_EQ(reader.remaining(), 0);
}

TEST(SpanReaderTests, GetWords)
{
    #pragma pack(push, 1)
    struct Words
    {
        uint16_t w1;
        uint32_t w2;
        int32_t w3;
        int16_t w4;
    };
    #pragma pack(pop)
    union
    {
        struct Words words;
        std::array<std::byte, sizeof(Words)> data;
    };

    words.w1 = 1;
    words.w2 = 2;
    words.w3 = -3;
    words.w4 = -4;

    buffer::Span<std::byte> span(data);
    buffer::SpanReader reader(span);

    auto w1 = reader.Get<uint16_t>();
    auto w2 = reader.Get<uint32_t>();
    auto w3 = reader.Get<int32_t>();
    auto w4 = reader.Get<int16_t>();

    EXPECT_EQ(w1, 1);
    EXPECT_EQ(w2, 2);
    EXPECT_EQ(w3, -3);
    EXPECT_EQ(w4, -4);
    EXPECT_EQ(reader.remaining(), 0);
}

TEST(SpanReaderTests, GetPODs)
{
    #pragma pack(push, 1)
    struct Test
    {
        uint8_t foo[6];
        uint8_t bar;
    };
    #pragma pack(pop)
    union
    {
        struct Test test;
        std::array<uint8_t, sizeof(Test)> data;
    };

    std::iota(std::begin(test.foo), std::end(test.foo), 0);
    test.bar = 42;

    buffer::Span<uint8_t> span(data);
    buffer::SpanReader reader(span);

    auto t = reader.Get<Test>();
    EXPECT_EQ(t.bar, 42);

    std::ranges::equal(data.begin(), data.end(), std::begin(t.foo), std::end(t.foo),
                         [](auto& a, auto& b) { 
                            EXPECT_EQ(a, b);
                            std::cout <<"stf\n";
                            return true;
                            });
}

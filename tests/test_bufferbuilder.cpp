#include <gtest/gtest.h>

#include <gsl/span>

#include "buffer.h"

using BB = buffer::BufferBuilder;

TEST(BufferBuilderTests, DefaultConstructor)
{
    BB bb;
}

TEST(BufferBuilderTests, PutInt)
{
    BB bb;
    const int value = 52;
    bb.Put<int>(value);
    std::unique_ptr<BB::Buffer> buffer = bb.GetBuffer();
    int b;
    memcpy(&b, buffer->data(), sizeof(int));
    ASSERT_EQ(value, b);
}

TEST(BufferBuilderTests, PutStruct)
{
    struct A {
        int a, b;
    };
    BB bb;
    A a{1, 2};
    bb.Put(a);
    std::unique_ptr<BB::Buffer> buffer = bb.GetBuffer();
    A b;
    memcpy(&b, buffer->data(), sizeof(A));
    ASSERT_EQ(a.a, b.a);
    ASSERT_EQ(a.b, b.b);
}

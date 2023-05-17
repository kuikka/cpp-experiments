#include <gtest/gtest.h>

#include <gsl/span>

#include "bluetooth.h"
// #include "xputils.h"

using namespace ble;

auto getRawAddress() -> std::array<std::byte, 6>
{
    std::array<std::byte, 6> raw = {
        static_cast<std::byte>(1),
        static_cast<std::byte>(2),
        static_cast<std::byte>(3),
        static_cast<std::byte>(4),
        static_cast<std::byte>(5),
        static_cast<std::byte>(6),
    };
    return raw;
}

TEST(AddressTests, DefaultConstructor)
{
    Address a;
    std::string s(a);
    EXPECT_EQ(a.type(), Address::Type::PUBLIC);
    EXPECT_EQ(s, "00:00:00:00:00:00 (public)");
}

TEST(AddressTests, Constructor)
{
    auto raw = getRawAddress();

    Address a(raw, false);
    std::string s(a);
    EXPECT_EQ(a.type(), Address::Type::PUBLIC);
    EXPECT_EQ(s, "06:05:04:03:02:01 (public)");
}

TEST(AddressTests, Defaults)
{
    auto raw = getRawAddress();

    Address a1(raw, false);
    Address a2(a1);
    Address a3;
    a3 = a2;

    EXPECT_EQ(a1, a2);
    EXPECT_EQ(a1, a3);
    EXPECT_EQ(a2, a3);

    std::string s(a3);
    EXPECT_EQ(s, "06:05:04:03:02:01 (public)");
}

TEST(AddressTests, ToProtocolBleAddress)
{
    auto raw = getRawAddress();
    Address a1(raw, true);
    protocol::Gap::ble_address_t bleAddress;
    a1.to(bleAddress);
    EXPECT_EQ(raw, bleAddress.address);
    EXPECT_EQ(1, bleAddress.type);
}

TEST(AddressTests, FromProtocolBleAddress)
{
    auto raw = getRawAddress();
    protocol::Gap::ble_address_t bleAddress;
    bleAddress.address = raw;
    bleAddress.type = 0;

    Address a1(bleAddress);
    std::string s(a1);
    EXPECT_EQ(s, "06:05:04:03:02:01 (public)");
}

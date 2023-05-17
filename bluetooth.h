#pragma once

#include <fmt/core.h>
#include "protocol.h"

namespace bluetooth::Gap
{
    
};

namespace ble
{
    class Address
    {
    private:
        // Little-endian byte order
        std::array<std::byte, 6> address;
        bool isRandom;
    public:
        enum class Type
        {
            PUBLIC,
            RANDOM,
        };
    public:
        Address() : address(), isRandom(0) {}
        Address(const std::array<std::byte, 6>& addr, bool random) : address(addr), isRandom(random) {}
        Address(const Address& other) = default;
        bool operator==(const Address& other) const = default;
        Address& operator=(const Address& other) = default;

        Address(const protocol::Gap::ble_address_t& addr)
        {
            std::copy(addr.address.begin(), addr.address.end(),
                        address.begin());
            isRandom = addr.type == 1;
        }

        Type type() const
        {
            return isRandom ? Type::RANDOM : Type::PUBLIC;
        }

        void to(protocol::Gap::ble_address_t& addr) const
        {
            std::copy(address.begin(), address.end(),
                        addr.address.begin());
            addr.type = isRandom ? 1 : 0;
        }
        
        operator std::string() const
        {
            return fmt::format("{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x} {}",
                static_cast<unsigned>(address[5]),
                static_cast<unsigned>(address[4]),
                static_cast<unsigned>(address[3]),
                static_cast<unsigned>(address[2]),
                static_cast<unsigned>(address[1]),
                static_cast<unsigned>(address[0]),
                isRandom ? "(random)" : "(public)"
            );
        }

    };

    class Uuid
    {
    public:
        static constexpr std::array<std::byte, 16> BluetoothSigBase = {
            static_cast<std::byte>(0xFB),
            static_cast<std::byte>(0x34),
            static_cast<std::byte>(0x9B),
            static_cast<std::byte>(0x5F),
            static_cast<std::byte>(0x80),
            static_cast<std::byte>(0x00),
            static_cast<std::byte>(0x00),
            static_cast<std::byte>(0x80),
            static_cast<std::byte>(0x00),
            static_cast<std::byte>(0x10),
            static_cast<std::byte>(0x00),
            static_cast<std::byte>(0x00),
            static_cast<std::byte>(0x00), //
            static_cast<std::byte>(0x00), //
            static_cast<std::byte>(0x00),
            static_cast<std::byte>(0x00),
        };
    private:
        // 0000xxxx-0000-1000-8000-00805F9B34FB
        std::array<std::byte, 16> uuid;

    public:
        bool operator==(const Uuid& other)
        {
            return std::equal(uuid.begin(), uuid.end(),
                              other.uuid.begin(), other.uuid.end());
        }
        static Uuid fromBase(const std::array<std::byte, 16>& base, uint16_t uuid)
        {
            Uuid ret;
            std::copy(base.begin(), base.end(), ret.uuid.begin());
            ret.uuid[12] = static_cast<std::byte>(uuid & 0xFF);
            ret.uuid[13] = static_cast<std::byte>((uuid >> 8) & 0xFF);
            return ret;
        }
        static Uuid sigUuid(uint16_t uuid)
        {
            return fromBase(BluetoothSigBase, uuid);
        }

    };
};
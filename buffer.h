#pragma once

#include <cassert>

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
            Span s(value);
            buffer.insert(buffer.begin(), s.begin(), s.end());
        }
    };

    class SpanReader
    {
    private:
        using Span = buffer::Span<std::byte>;
        Span view;
        size_t consumed = 0;

    private:
        auto consume(size_t bytes_to_consume) -> Span
        {
            auto ret = view.subspan(consumed, bytes_to_consume);
            consumed += bytes_to_consume;
            return ret;
        }

    public:
        SpanReader(Span& span) : view(span) {}
        SpanReader(const SpanReader&) = delete;

        template<typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
        auto Get() -> T
        {
            T to;
            auto from = consume(sizeof(T));
            std::memcpy(&to, from.data(), from.size_bytes());
            return to;
        }

        size_t remaining() const
        {
            return view.size_bytes() - consumed;
        }
    };

    class VectorReader
    {
    private:
        std::vector<std::byte>& buffer;
        size_t consumed = 0;

    public:
        VectorReader(std::vector<std::byte>& buffer) : buffer(buffer) {}
        // ~VectorReader() { assert(consumed == 0); }
        template<typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
        auto Get() -> T
        {
            T to;
            auto from = Consume(sizeof(T));
            std::memcpy(&to, from.data(), from.size_bytes());
            return to;
        }

        size_t Remaining() const
        {
            return buffer.size() - consumed;
        }

        void Commit()
        {
            buffer.erase(buffer.begin(), buffer.begin() + consumed);
            consumed = 0;
        }

        auto Consume(size_t bytesToConsume) -> Span<std::byte>
        {
            Span<std::byte> ret(buffer.data() + consumed, bytesToConsume);
            consumed += bytesToConsume;
            return ret;
        }

    };

};

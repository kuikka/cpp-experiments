#pragma once

#include <cassert>

namespace buffer
{
    template <typename T>
    using Span = gsl::span<T>;

    class BufferBuilder
    {
    public:
        using Buffer = std::vector<std::byte>;
    private:
        std::unique_ptr<Buffer> buffer = std::make_unique<Buffer>();
    public:
        BufferBuilder() = default;
        BufferBuilder(const BufferBuilder&) = delete;
        BufferBuilder(BufferBuilder&&) = delete;

        template<typename T>
        void Put(const T& value)
        {
            assert(buffer);
            std::array<std::byte, sizeof(T)> data;
            memcpy(data.data(), &value, data.size());
            buffer->insert(buffer->end(), data.begin(), data.end());
        }

        std::unique_ptr<Buffer> GetBuffer()
        {
            return std::move(buffer);
        }
    };

    template<class V>
    class SpanReader
    {
    private:
        using Span = buffer::Span<V>;
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
        SpanReader operator==(SpanReader& reader) = delete;

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
    using ByteReader = SpanReader<std::byte>;

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

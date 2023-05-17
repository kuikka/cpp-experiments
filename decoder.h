#pragma once

#include "buffer.h"

namespace protocol
{

template <class T>
struct PendingRequest
{
    std::promise<T> promise;
};

class DecoderBase
{
public:
    virtual int Decode(buffer::Span<std::byte> input) = 0;
    virtual int Decode(buffer::ByteReader& reader) = 0;
};

/**
 * @brief Base class for Decoder objects.
 * 
 * This object is instantiated with two types, MSG is the message
 * to decode from incoming data and pass to `receiver`.
 * 
 * R is the "return type" for the decoder, usually an incoming message 
 * or response of some sort. It will be passed to the user
 * through a promise/future pair when they are waiting for the
 * Response to a request.
 * 
 * Decoder will receive raw data through either Decode() method.
 * The incoming data will be decoded into MSG type and passed
 * into user-provided functor `receiver` for handling.
 * 
 * @tparam MSG message type to decode
 * @tparam R return type
 */
template <class MSG, class R>
class Decoder : public DecoderBase
{
public:
    using MessageType = MSG;
    using ReturnValueType = R;
    using MyPendingRequest = PendingRequest<R>;
    using PendingRequests = std::list<std::shared_ptr<MyPendingRequest>>;
    using Receiver = std::function<void(const MSG&, Decoder<MSG, R>&)>;

private:
    Receiver receiver;
    PendingRequests pendingRequests;

public:
    Decoder(Receiver receiver) : receiver(receiver){}

    int Decode(buffer::Span<std::byte> input) override
    {
        if (input.size_bytes() < sizeof(MSG)) {
            return -1;
        }
        buffer::ByteReader reader(input);
        return this->Decode(reader);
    }

    int Decode(buffer::ByteReader& reader) override
    {
        auto req = reader.Get<MSG>();
        if (receiver)
        {
            receiver(req, *this);
        }
        return sizeof(MSG);
    }

    void CompletePendingRequests(const R &returnValue)
    {
        for (auto pendingRequest : pendingRequests)
        {
            pendingRequest->promise.set_value(returnValue);
        }
        pendingRequests.clear();
    }

    std::future<R> addPendingRequest()
    {
        auto pendingRequest = std::make_shared<MyPendingRequest>();
        pendingRequests.push_back(pendingRequest);
        return pendingRequest->promise.get_future();
    }
};

}; // namespace protocol

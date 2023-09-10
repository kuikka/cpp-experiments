#pragma once

#include "boost_serial.h"

class Transport
{
public:
    using Data = buffer::Span<std::byte>;
    using Message = std::unique_ptr<std::vector<std::byte>>;
public:
    virtual void SendMessage(Message) = 0;
    virtual void RegisterDataReceiver(std::function<void(Data)>) = 0;
};

class Controller
{
private:
    Transport& transport;

public:
    using Request = Transport::Message;

public:
    Controller(Transport& transport) : transport(transport){}

    void QueueRequest(Request request)
    {
        transport.SendMessage(std::move(request));
    }
};


class AsioSerialTransport : public Transport
{
private:
    BoostSerial serialPort;
    std::list<Controller::Request> requestQueue;
    std::function<void(Data)> dataReceiver;

private:
    void ReceiveMessage(Data message)
    {

    }

public:
    AsioSerialTransport(boost::asio::io_service& ioService) : serialPort(ioService){}
    virtual ~AsioSerialTransport() = default;

    void Open(std::string_view portName, int baudRate)
    {
        serialPort.Start(portName, baudRate);
    }                                                                  

    void SendMessage(Message message) override
    {
        serialPort.WriteToSerialPort(std::move(message));
    }

    void RegisterDataReceiver(std::function<void(Data)> newReceiver) override
    {
        dataReceiver = newReceiver;
    }
};


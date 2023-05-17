#pragma once

#include "boost_serial.h"

class Transport
{
public:
    using Message = std::unique_ptr<std::vector<std::byte>>;
    virtual bool queueRequest(Message message) = 0;
    virtual void registerReceiver(std::function<void(Message)>) = 0;
};

class AsioSerialTransport : Transport
{
private:
    std::list<Controller::Request> requestQueue;
    BoostSerial serialPort;
public:
    void open(std::string_view portName, int baudRate)
    {
        serialPort.start(portName, baudRate);
    }                                                                  
};

class Controller
{
private:
    Transport& transport;

public:
    using Request = Transport::Message;
public:
    Controller(Transport& transport) : transport(transport){}

    void queueRequest(Request request)
    {
        requestQueue.push_back(std::move(request));
    }
};


#include <atomic>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <vector>
#include <thread>

#include <fmt/core.h>
#include <gsl/span>

#include "xputils.h"

// #include <boost/asio.hpp>
// #include <boost/array.hpp>
// #include <boost/bind.hpp>
// #include <boost/thread.hpp>
// #include "cmake_pch.hxx"

class BoostSerial
{
private:
	boost::asio::io_service ioService;
	boost::asio::serial_port serialPort;
    boost::thread ioThread;
    boost::array<uint8_t, 512> readBuffer;
private:

    auto openSerialPort(const std::string& portName) -> bool
    {
        boost::system::error_code errorCode;

        serialPort.open(portName, errorCode);
        if (errorCode)
        {
            log("Opening serial port {} failed: {}", portName, errorCode.message());
            return false;
        }
        return true;
    }

    void configureSerialPort(int baudRate)
    {
        serialPort.set_option(boost::asio::serial_port_base::baud_rate(baudRate));
        serialPort.set_option(boost::asio::serial_port_base::character_size(8));
        serialPort.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
        serialPort.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
        serialPort.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
    }

    void startIoThread()
    {
        ioThread = boost::thread([this](){ ioService.run(); });
    }

    void startReadingSerialPort()
    {
        if (!serialPort.is_open())
            return;

        serialPort.async_read_some(
            boost::asio::buffer(readBuffer),
            [this](const boost::system::error_code& error, std::size_t bytesTransferred)
            {
                this->OnReadCompleted(error, bytesTransferred);
            }
        );
    }

    void OnReadCompleted(const boost::system::error_code& ec, size_t bytes_transferred)
    {

    }

    void OnWriteCompleted(const boost::system::error_code& ec, size_t bytes_transferred)
    {
    }

public:
    BoostSerial() : serialPort(ioService){}
    ~BoostSerial() = default;

    auto start(const std::string& portName, int baudRate) -> bool
    {
        if (!openSerialPort(portName)) {
            return false;
        }
        configureSerialPort(baudRate);
        startIoThread();
        startReadingSerialPort();
        return true;
    }

    void writeToSerialPort(std::span<const uint8_t> data)
    {
        if (!serialPort.is_open())
            return;

        serialPort.async_write_some(
            boost::asio::buffer(data.data(), data.size_bytes()),
            [this](const boost::system::error_code& error, std::size_t bytesTransferred)
            {
                this->OnWriteCompleted(error, bytesTransferred);
            }
        );
    }
};

auto main() -> int
{
    BoostSerial serial;
    serial.start("COM1", 115200);
}
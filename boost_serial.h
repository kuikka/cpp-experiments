#pragma once

#include "xputils.h"

class BoostSerial
{
private:
	boost::asio::io_context& ioCtx;
	boost::asio::serial_port serialPort;
    boost::thread ioThread;
    boost::array<std::byte, 512> readBuffer;
    std::mutex writeQueueLock;
    std::list<std::unique_ptr<std::vector<std::byte>>> writeQueue;
    bool isWriteOngoing = false;

private:
    auto OpenSerialPort(std::string_view portName) -> bool
    {
        boost::system::error_code errorCode;

        serialPort.open(std::string(portName), errorCode);
        if (errorCode.failed())
        {
            log("Opening serial port {} failed: {}", portName, errorCode.message());
            return false;
        }
        return true;
    }

    void ConfigureSerialPort(int baudRate)
    {
        serialPort.set_option(boost::asio::serial_port_base::baud_rate(baudRate));
        serialPort.set_option(boost::asio::serial_port_base::character_size(8));
        serialPort.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
        serialPort.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
        serialPort.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
    }

    void StartIoThread()
    {
        ioThread = boost::thread([this](){ ioCtx.run(); });
    }

    void StartReadingSerialPort()
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

    void OnReadCompleted(const boost::system::error_code& ec, size_t bytesTransferred)
    {

    }

    void OnWriteCompleted(const boost::system::error_code& ec, size_t bytesTransferred)
    {
        // log("{} {} {}", __func__, ec, bytesTransferred);

        { // Lock queue and remove the written buffer from queue.
            std::lock_guard<std::mutex> lock(writeQueueLock);
            if (bytesTransferred != writeQueue.front()->size())
            {
                log("Partial write! Tried {} got {} bytes.",
                    writeQueue.front()->size(),
                    bytesTransferred);
            }
            writeQueue.pop_front();
            isWriteOngoing = false;
        }

        // TODO: Call application callback. Must allow queuing more data during.

        // In case application did not queue more data in callback,
        // check if we have data in queue.
        std::lock_guard<std::mutex> lock(writeQueueLock);
        if (!isWriteOngoing && !writeQueue.empty()) {
            WriteNextBuffer();
        }
    }

    void WriteNextBuffer()
    {
        assert(!isWriteOngoing);
        
        isWriteOngoing = true;
        std::span<const std::byte> data = *(writeQueue.front());
        boost::asio::async_write(
            serialPort,
            boost::asio::buffer(data.data(), data.size_bytes()),
            [this](const boost::system::error_code& error, std::size_t bytesTransferred)
            {
                this->OnWriteCompleted(error, bytesTransferred);
            }
        );
    }

public:
    BoostSerial(boost::asio::io_context& ioCtx) : ioCtx(ioCtx), serialPort(ioCtx){}
    ~BoostSerial() = default;

    auto Start(std::string_view portName, int baudRate) -> bool
    {
        if (!OpenSerialPort(portName)) {
            return false;
        }
        ConfigureSerialPort(baudRate);
        StartIoThread();
        StartReadingSerialPort();
        return true;
    }

    void WriteToSerialPort(std::unique_ptr<std::vector<std::byte>> buffer)
    {
        if (!serialPort.is_open())
            return;

        std::lock_guard<std::mutex> lock(writeQueueLock);
        writeQueue.emplace_back(std::move(buffer));
        if (!isWriteOngoing)
        {
            WriteNextBuffer();
        }
    }

};

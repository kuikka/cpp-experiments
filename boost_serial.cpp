#include <cstddef>
#include <cstring>

#include "xputils.h"
#include "boost_serial.h"

auto main(int argc, char *argv[]) -> int
{
    boost::asio::io_context io;
    BoostSerial serial(io);
    serial.Start("COM1", 115200);
}
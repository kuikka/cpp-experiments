#include <atomic>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <vector>
#include <thread>

#include <fmt/core.h>
#include <gsl/span>

#include "xputils.h"
#include "boost_serial.h"

auto main() -> int
{
    BoostSerial serial;
    serial.start("COM1", 115200);
}
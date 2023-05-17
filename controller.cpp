#include "buffer.h"
#include "protocol.h"
#include "decoder.h"
#include "encoder.h"
#include "boost_serial.h"

#include "controller.h"
#include "xputils.h"

auto main(int argc, char* argv[]) -> int
{
    AsioSerialController controller;
    controller.open("COM1", 921600);
}
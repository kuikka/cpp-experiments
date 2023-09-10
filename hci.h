#include <cstdint>
#include <assert.h>

#include "buffer.h"

namespace blubridge::Hci
{
    namespace Line
    {
        struct Command
        {
            union Opcode
            {
                uint16_t Value;
                union
                {
                    unsigned Ogf:6;
                    unsigned Ocf:10;
                };

            };
        };
    };

    class Command
    {
    private:
        uint16_t opCode;
        buffer::BufferBuilder payload;

    public:
        Command(uint16_t opCode) : opCode(opCode) {}
        Command(uint8_t ogf, uint16_t ocf) {
            assert(ogf <= 0x3F);
            assert(ocf <= 0x03FF);
            opCode = (ogf << 10) | ocf;
        }

        template<typename T>
        void Put(const T& value)
        {
            payload.Put<T>(std::forward(value));
        }
    };
    
    class Event;
    class AclData;
    class IsoData;
    class SyncData;
};

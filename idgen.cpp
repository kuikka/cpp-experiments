#include <atomic>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <vector>

#include <fmt/core.h>
#include <gsl/span>

#include "xputils.h"

using ObjectId = std::size_t;

class ObjectWithId
{
private:
    static std::atomic<ObjectId> objectIdGenerator;
    const ObjectId id = objectIdGenerator++;
public:
     [[nodiscard]]  auto GetId() -> ObjectId const { return id; }
};

std::atomic<ObjectId> ObjectWithId::objectIdGenerator;

auto main() -> int
{
    ObjectWithId a;
    ObjectWithId b;
    log("{}, {}\n", a.GetId(), b.GetId());
}
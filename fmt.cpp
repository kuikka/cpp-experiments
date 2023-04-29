#include <iostream>
#include <vector>
#include <cstddef>
#include <gsl/span>
#include <cstring>
#include <fmt/core.h>

#include "xputils.h"


struct my_msg
{
    uint32_t fobar;
};

template<>
struct fmt::formatter<my_msg>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(my_msg const& msg, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), "fobar={0}", msg.fobar);
    }
};

auto main() -> int
{
    my_msg mm;
    mm.fobar = 42;

    log("my_msg = {}\n", mm);
}
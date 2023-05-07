#pragma once

namespace {
    auto puts(const std::string& s) -> int
    {
        return std::puts(s.c_str());
    }

    void vlog(fmt::string_view format, fmt::format_args args) {
        fmt::vprint(format, args);
    }

    template <typename S, typename ...Args>
    auto log(const S& format, Args&&... args) -> void
    {
        vlog(format, fmt::make_format_args(args...));
    }

    template <typename Enumeration>
    auto as_integer(Enumeration const value)
        -> typename std::underlying_type<Enumeration>::type
    {
        return static_cast<typename std::underlying_type<Enumeration>::type>(value);
    }

}

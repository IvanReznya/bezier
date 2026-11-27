#pragma once
#include <iostream>
#include <stdexcept>
inline void check(bool value, const char *message) {
    if (!value)
        throw std::runtime_error(message);
}

//
// Created by Forrest Jones on 2019-02-23.
//

#ifndef TERMU_ROMLOADEXCEPTION_H
#define TERMU_ROMLOADEXCEPTION_H

#include <stdexcept>
#include <string>

class rom_load_exception : public std::runtime_error {
public:
    explicit rom_load_exception(const std::string &message) : runtime_error(message) {
    }
};

#endif //TERMU_ROMLOADEXCEPTION_H

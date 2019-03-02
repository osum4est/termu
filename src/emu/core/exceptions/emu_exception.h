//
// Created by Forrest Jones on 2019-02-23.
//

#ifndef TERMU_EMUEXCEPTION_H
#define TERMU_EMUEXCEPTION_H

#include <stdexcept>
#include <string>

class emu_exception : public std::runtime_error {
public:
    explicit emu_exception(const std::string &message) : runtime_error(message) {
    }
};

#endif //TERMU_EMUEXCEPTION_H

//
// Created by Forrest Jones on 2019-02-23.
//

#ifndef TERMU_ROMPATH_H
#define TERMU_ROMPATH_H

#include <string>

class rom_path {
    std::string extension;
    std::string file;

public:
    explicit rom_path(const std::string &path);

    std::string get_path() const;

    std::string get_extension() const;

    uint8_t *read_all_bytes(uint64_t *size) const;
};

#endif //TERMU_ROMPATH_H

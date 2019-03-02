#include "rom_path.h"
#include <string>
#include <fstream>
#include "../exceptions/rom_load_exception.h"

rom_path::rom_path(const std::string &path) {
    std::ifstream infile(path);
    if (!infile.good())
        throw rom_load_exception("Cannot find " + path + ".");

    extension = "";
    std::string::size_type idx = path.rfind('.');
    if (idx != std::string::npos)
        extension = path.substr(idx + 1);

    file = path;
}

std::string rom_path::get_path() const {
    return file;
}

std::string rom_path::get_extension() const {
    return extension;
}

uint8_t *rom_path::read_all_bytes(uint64_t *size) const {
    std::ifstream infile(file, std::ios_base::binary);

    infile.seekg(0, infile.end);
    long long int length = infile.tellg();
    infile.seekg(0, infile.beg);

    if (length == -1)
        throw rom_load_exception("Could not read file bytes.");

    uint8_t *bytes = new uint8_t[length];

    *size = (uint64_t)length;
    infile.read(reinterpret_cast<char *>(bytes), length);
    return bytes;
}

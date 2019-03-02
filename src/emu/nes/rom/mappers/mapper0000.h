//
// Created by Forrest Jones on 2019-02-23.
//

#ifndef TERMU_MAPPER000_H
#define TERMU_MAPPER000_H

#include "mapper.h"

//class NesRom;

class mapper0000 : public mapper {
public:
    explicit mapper0000(nes_rom *rom);

    uint8_t &get_prg(uint16_t addr) override;

    void set_prg(uint16_t addr, uint8_t b) override;

    uint8_t &get_chr(uint16_t addr) override;

    void set_chr(uint16_t addr, uint8_t b) override;
};

#endif //TERMU_MAPPER000_H

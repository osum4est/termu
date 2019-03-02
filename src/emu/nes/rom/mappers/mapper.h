//
// Created by Forrest Jones on 2019-02-23.
//

#ifndef TERMU_MAPPER_H
#define TERMU_MAPPER_H

#include "../nes_rom.h"

class mapper {
protected:
    nes_rom *rom;
    uint8_t *trainer;
    uint8_t *prg_rom;
    uint8_t *prg_ram;
    uint8_t *chr_rom;
    uint8_t *misc_rom;

public:
    explicit mapper(nes_rom *rom);
    virtual ~mapper();

    virtual uint8_t &get_prg(uint16_t addr) = 0;

    virtual void set_prg(uint16_t addr, uint8_t b) = 0;

    virtual uint8_t &get_chr(uint16_t addr) = 0;

    virtual void set_chr(uint16_t addr, uint8_t b) = 0;

protected:
    nes_rom *get_rom();
};

#endif //TERMU_MAPPER_H

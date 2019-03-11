//
// Created by Forrest Jones on 2019-02-23.
//

#ifndef TERMU_MAPPER_H
#define TERMU_MAPPER_H

#include "../nes_rom.h"

class mapper {
protected:
    nes_rom *rom;
    mirroring_type mirroring;
    uint8_t *trainer;
    uint8_t *prg_rom;
    uint8_t *prg_ram;
    uint8_t *chr_rom;
    uint8_t *chr_ram;
    uint8_t *misc_rom;

    uint64_t *cpu_cycles;
    
    virtual uint8_t &map_prg(uint16_t addr, bool write) = 0;
    virtual uint8_t &map_chr(uint16_t addr, bool write) = 0;
    nes_rom *get_rom();

public:
    explicit mapper(nes_rom *rom);
    virtual ~mapper();

    virtual uint8_t &get_prg(uint16_t addr);
    virtual void set_prg(uint16_t addr, uint8_t b);
    virtual uint8_t &get_chr(uint16_t addr);
    virtual void set_chr(uint16_t addr, uint8_t b);

    mirroring_type get_mirroring_type();

    void set_cpu_cycles(uint64_t *cpu_cycles);
};

#endif //TERMU_MAPPER_H

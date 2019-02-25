//
// Created by Forrest Jones on 2019-02-24.
//

#ifndef TERMU_MEM_H
#define TERMU_MEM_H

#include "../rom/mappers/mapper.h"
#include "../../core/exceptions/emu_exception.h"

class mem {
    uint8_t *internal_ram;
    uint8_t *apu_io_regs;
    uint8_t *apu_io_test;

    mapper *mapper;

    uint8_t *ppu_regs;
    uint8_t *ppu_pattern_tables;
    uint8_t *ppu_name_tables;
    uint8_t *ppu_palette_indexes;

public:
    explicit mem(nes_rom *cartridge);

    ~mem();

    uint8_t get_cpu(uint16_t addr);

    void set_cpu(uint16_t addr, uint8_t b);

    uint8_t get_ppu(uint16_t addr);

    uint8_t set_ppu(uint16_t addr, uint8_t b);
};

#endif //TERMU_MEM_H

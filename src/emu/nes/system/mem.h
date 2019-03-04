//
// Created by Forrest Jones on 2019-02-24.
//

#ifndef TERMU_MEM_H
#define TERMU_MEM_H

#include <functional>
#include "../rom/mappers/mapper.h"
#include "../../core/exceptions/emu_exception.h"

class mem {
    typedef std::function<void(uint8_t &value, uint8_t new_value, bool write)> ppu_reg_handler;

    uint8_t *internal_ram;
    uint8_t *apu_io_regs;
    uint8_t *apu_io_test;

    nes_rom *cartridge;
    ::mapper *mapper;

    uint8_t *ppu_regs;
    uint8_t *ppu_nametables;
    uint8_t *ppu_palette_indexes;

    uint8_t *oam;
    uint8_t *secondary_oam;

    ppu_reg_handler ppu_handler = nullptr;

public:
    explicit mem(nes_rom *cartridge);

    ~mem();

    uint8_t &get_cpu(uint16_t addr);

    void set_cpu(uint16_t addr, uint8_t b);

    uint8_t &get_ppu(uint16_t addr);

    void set_ppu(uint16_t addr, uint8_t b);

    uint8_t &get_oam(uint16_t addr);

    void set_oam(uint16_t addr, uint8_t b);

    uint8_t &get_secondary_oam(uint16_t addr);

    void set_secondary_oam(uint16_t addr, uint8_t b);

    void set_ppu_reg_handler(ppu_reg_handler handler);

private:
    uint16_t nametable_mirror(uint16_t addr);
};

#endif //TERMU_MEM_H

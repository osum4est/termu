//
// Created by Forrest Jones on 2019-02-24.
//

#ifndef TERMU_MEM_H
#define TERMU_MEM_H

#include <functional>
#include "../rom/mappers/mapper.h"
#include "../../core/exceptions/emu_exception.h"
#include "../input/nes_input_device.h"

class mem {
    typedef std::function<void(uint8_t &value, uint8_t new_value, bool write)> ppu_reg_handler;

    uint8_t *internal_ram;
    uint8_t *apu_io_regs;

    nes_rom *cartridge;
    ::mapper *mapper;
    nes_input_device **inputs;

    uint8_t *ppu_regs;
    uint8_t *ppu_nametables;
    uint8_t *ppu_palette_indexes;

    uint8_t *oam;
    uint8_t *secondary_oam;

    uint64_t *cpu_cycles;
    ppu_reg_handler ppu_handler = nullptr;

    uint8_t &map_prg(uint16_t addr, bool write);
    uint8_t &map_chr(uint16_t addr, bool write);

public:
    mem(nes_rom *cartridge, nes_input_device *inputs[2]);

    ~mem();

    uint8_t &get_cpu(uint16_t addr);

    void set_cpu(uint16_t addr, uint8_t b);

    uint8_t &get_ppu(uint16_t addr);

    void set_ppu(uint16_t addr, uint8_t b);

    void set_cpu_cycles(uint64_t *cpu_cyles);

    void set_ppu_reg_handler(ppu_reg_handler handler);

private:
    uint16_t nametable_mirror(uint16_t addr);
};

#endif //TERMU_MEM_H

#include "mem.h"
#include "../../../utils/utils.h"

mem::mem(nes_rom *cartridge) {
    internal_ram = new uint8_t[0x0800]();
    apu_io_regs = new uint8_t[0x0018]();
    apu_io_test = new uint8_t[0x0008]();

    mapper = cartridge->get_mapper();

    ppu_regs = new uint8_t[0x0008]();
    ppu_pattern_tables = new uint8_t[0x2000]();
    ppu_name_tables = new uint8_t[0x1000]();
    ppu_palette_indexes = new uint8_t[0x0020]();
}

mem::~mem() {
    delete internal_ram;
    delete apu_io_regs;
    delete apu_io_test;
    delete ppu_regs;
    delete ppu_pattern_tables;
    delete ppu_name_tables;
    delete ppu_palette_indexes;
}

uint8_t mem::get_cpu(uint16_t addr) {
    if (addr < 0x2000)
        return internal_ram[addr % 0x0800];

    if (addr < 0x4000)
        return ppu_regs[(addr - 0x2000) % 0x0008];

    if (addr < 0x4018)
        return apu_io_regs[addr - 0x4000];

    if (addr < 0x4020)
        return apu_io_test[addr - 0x4018];

    return mapper->get(addr);
}

void mem::set_cpu(uint16_t addr, uint8_t b) {
    if (addr < 0x2000)
        internal_ram[addr % 0x0800] = b;

    else if (addr < 0x4000)
        ppu_regs[(addr - 0x2000) % 0x0008] = b;

    else if (addr < 0x4018)
        apu_io_regs[addr - 0x4000] = b;

    else if (addr < 0x4020)
        apu_io_test[addr - 0x4018] = b;

    else
        mapper->set(addr, b);
}

uint8_t mem::get_ppu(uint16_t addr) {
    if (addr < 0x3000)
        return ppu_pattern_tables[addr];

    if (addr < 0x3f00)
        return ppu_name_tables[(addr - 0x3000) % 0x1000];

    if (addr < 0x3fff)
        return ppu_palette_indexes[(addr - 0x3f00) % 0x0020];

    throw emu_exception(utils::string_format("Cannot access ppu memory at %x.", addr));
}

uint8_t mem::set_ppu(uint16_t addr, uint8_t b) {
    if (addr < 0x3000)
        return ppu_pattern_tables[addr] = b;

    if (addr < 0x3f00)
        return ppu_name_tables[(addr - 0x3000) % 0x1000] = b;

    if (addr < 0x3fff)
        return ppu_palette_indexes[(addr - 0x3f00) % 0x0020] = b;

    throw emu_exception(utils::string_format("Cannot access ppu memory at %x.", addr));
}

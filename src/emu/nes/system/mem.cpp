#include "mem.h"
#include "../../../utils/utils.h"

mem::mem(nes_rom *cartridge, nes_input_device *inputs[2]) {
    internal_ram = new uint8_t[0x0800]();
    apu_io_regs = new uint8_t[0x0018]();
    apu_io_test = new uint8_t[0x0008]();

    this->cartridge = cartridge;
    mapper = cartridge->get_mapper();
    this->inputs = inputs;

    ppu_regs = new uint8_t[0x0008]();
    ppu_nametables = new uint8_t[0x1000]();
    ppu_palette_indexes = new uint8_t[0x0020]();

    oam = new uint8_t[0x0100];
    secondary_oam = new uint8_t[0x0020];
}

mem::~mem() {
    delete[] internal_ram;
    delete[] apu_io_regs;
    delete[] apu_io_test;
    delete[] ppu_regs;
    delete[] ppu_nametables;
    delete[] ppu_palette_indexes;
    delete[] oam;
    delete[] secondary_oam;
}

uint8_t &mem::get_cpu(uint16_t addr) {
    if (addr < 0x2000)
        return internal_ram[addr % 0x0800];

    if (addr < 0x4000) {
        if (ppu_handler != nullptr)
            ppu_handler(ppu_regs[(addr - 0x2000) % 0x0008], 0, false);
        return ppu_regs[(addr - 0x2000) % 0x0008];
    }

    if (addr == 0x4016 && inputs[0] != nullptr)
        return inputs[0]->read();

    if (addr == 0x4017 && inputs[1] != nullptr)
        return inputs[1]->read();

    if (addr < 0x4018)
        return apu_io_regs[addr - 0x4000];

    if (addr < 0x4020)
        return apu_io_test[addr - 0x4018];

    return mapper->get_prg(addr);
}

void mem::set_cpu(uint16_t addr, uint8_t b) {
    if (addr < 0x2000)
        internal_ram[addr % 0x0800] = b;

    else if (addr < 0x4000) {
        if (ppu_handler != nullptr)
            ppu_handler(ppu_regs[(addr - 0x2000) % 0x0008], b, true);
        ppu_regs[(addr - 0x2000) % 0x0008] = b;
    }

    else if (addr == 0x4016 && inputs[0] != nullptr)
        inputs[0]->write(b);

    else if (addr < 0x4018)
        apu_io_regs[addr - 0x4000] = b;

    else if (addr < 0x4020)
        apu_io_test[addr - 0x4018] = b;

    else
        mapper->set_prg(addr, b);
}

uint8_t &mem::get_ppu(uint16_t addr) {
    if (addr < 0x2000)
        return mapper->get_chr(addr);

    if (addr < 0x3f00)
        return ppu_nametables[nametable_mirror(addr)];

    if (addr < 0x3fff)
        return ppu_palette_indexes[(addr - 0x3f00) % 0x0020];

    throw emu_exception(utils::string_format("Cannot access ppu memory at %x.", addr));
}

void mem::set_ppu(uint16_t addr, uint8_t b) {
    if (addr < 0x2000)
        mapper->set_chr(addr, b);

    else if (addr < 0x3f00)
        ppu_nametables[nametable_mirror(addr)] = b;

    else if (addr < 0x4000)
        ppu_palette_indexes[(addr - 0x3f00) % 0x0020] = b;

    else
        throw emu_exception(utils::string_format("Cannot access ppu memory at %x.", addr));
}

uint8_t &mem::get_oam(uint16_t addr) {
    if (addr < 0x0100)
        return oam[addr];

    throw emu_exception(utils::string_format("Cannot access oam memory at %x.", addr));
}

void mem::set_oam(uint16_t addr, uint8_t b) {
    if (addr < 0x0100)
        oam[addr] = b;

    else
        throw emu_exception(utils::string_format("Cannot access oam memory at %x.", addr));
}

uint8_t &mem::get_secondary_oam(uint16_t addr) {
    if (addr < 0x20)
        return secondary_oam[addr];

    throw emu_exception(utils::string_format("Cannot access oam memory at %x.", addr));
}

void mem::set_secondary_oam(uint16_t addr, uint8_t b) {
    if (addr < 0x20)
        secondary_oam[addr] = b;

    else
        throw emu_exception(utils::string_format("Cannot access oam memory at %x.", addr));
}

void mem::set_ppu_reg_handler(mem::ppu_reg_handler handler) {
    ppu_handler = handler;
}

uint16_t mem::nametable_mirror(uint16_t addr) {
    uint16_t addr_offset = (uint16_t) ((addr - 0x2000) % 0x1000);

    if (!cartridge->get_is_four_screen_mode()) {
        if (cartridge->get_mirroring_type() == horizontal)
            addr_offset =
                    (uint16_t) (addr_offset > 0x800 ? (0x800 + ((addr_offset - 0x800) % 0x400)) : addr_offset % 0x400);
        else
            addr_offset %= 0x0800;
    }

    return addr_offset;
}
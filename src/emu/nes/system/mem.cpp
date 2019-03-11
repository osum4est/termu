#include "mem.h"
#include "../../../utils/utils.h"

mem::mem(nes_rom *cartridge, nes_input_device *inputs[2]) {
    internal_ram = new uint8_t[0x0800]();
    apu_io_regs = new uint8_t[0x0020]();

    ppu_regs = new uint8_t[0x0008]();
    ppu_nametables = new uint8_t[0x1000]();
    ppu_palette_indexes = new uint8_t[0x0020]();

    this->cartridge = cartridge;
    mapper = cartridge->get_mapper();
    this->inputs = inputs;
}

mem::~mem() {
    delete[] internal_ram;
    delete[] apu_io_regs;
    delete[] ppu_regs;
    delete[] ppu_nametables;
    delete[] ppu_palette_indexes;
    delete[] oam;
    delete[] secondary_oam;
}

uint8_t &mem::map_prg(uint16_t addr, bool write) {
    if (addr < 0x2000)
        return internal_ram[addr % 0x0800];

    if (addr < 0x4000)
        return ppu_regs[(addr - 0x2000) % 0x0008];

    if (addr < 0x4020)
        return apu_io_regs[addr - 0x4000];

    throw emu_exception(utils::string_format("Cannot access cpu memory at %x.", addr));
}

uint8_t &mem::map_chr(uint16_t addr, bool write) {
    if (addr < 0x3f00)
        return ppu_nametables[nametable_mirror(addr)];

    if (addr < 0x4000)
        return ppu_palette_indexes[(addr - 0x3f00) % 0x0020];

    throw emu_exception(utils::string_format("Cannot access ppu memory at %x.", addr));
}

uint8_t &mem::get_cpu(uint16_t addr) {
    if (addr == 0x4016 && inputs[0] != nullptr)
        return inputs[0]->read();

    if (addr == 0x4017 && inputs[1] != nullptr)
        return inputs[1]->read();

    if (ppu_handler != nullptr && addr >= 0x2000 && addr < 0x4000)
        ppu_handler(map_prg(addr, false), 0, false);

    if (addr < 0x4020)
        return map_prg(addr, false);
    return mapper->get_prg(addr);
}

void mem::set_cpu(uint16_t addr, uint8_t b) {
    if (addr == 0x4016 && inputs[0] != nullptr)
        inputs[0]->write(b);

    if (addr == 0x4017 && inputs[1] != nullptr)
        inputs[1]->write(b);

    if (ppu_handler != nullptr && addr >= 0x2000 && addr < 0x4000)
        ppu_handler(map_prg(addr, true), b, true);

    if (addr < 0x4020)
        map_prg(addr, true) = b;
    else
        mapper->set_prg(addr, b);
}

uint8_t &mem::get_ppu(uint16_t addr) {
    if (addr < 0x2000)
        return mapper->get_chr(addr);
    return map_chr(addr, false);
}

void mem::set_ppu(uint16_t addr, uint8_t b) {
    if (addr < 0x2000)
        mapper->set_chr(addr, b);
    else
        map_chr(addr, true) = b;
}

void mem::set_cpu_cycles(uint64_t *cycles) {
    this->cpu_cycles = cycles;
    mapper->set_cpu_cycles(cpu_cycles);
}

void mem::set_ppu_reg_handler(mem::ppu_reg_handler handler) {
    ppu_handler = handler;
}

uint16_t mem::nametable_mirror(uint16_t addr) {
    uint16_t addr_offset = (uint16_t) ((addr - 0x2000) % 0x1000);

    if (!cartridge->get_is_four_screen_mode()) {
        switch (mapper->get_mirroring_type()) {
            case single_a:
                addr_offset %= 0x0400;
                break;
            case horizontal:
                addr_offset =
                (uint16_t) (addr_offset > 0x800 ? (0x800 + ((addr_offset - 0x800) % 0x400)) : addr_offset % 0x400);
                break;
            case vertical:
                addr_offset %= 0x0800;
                break;
            default:
                break;
        }
    }

    return addr_offset;
}

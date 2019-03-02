#include "nes_rom.h"
#include "mappers/mapper0000.h"

nes_rom::nes_rom(const rom_path &rom_path) {
    name = rom_path.get_path();
    load(rom_path);
}

nes_rom::~nes_rom() {
    delete mapper;
    delete[] bytes;
    delete[] header;
    delete[] trainer;
    delete[] prg_rom;
    delete[] prg_ram;
    delete[] chr_rom;
    delete[] misc_rom;
}

std::string nes_rom::get_name() {
    return name;
}

uint8_t *nes_rom::get_header() {
    return header;
}

uint8_t *nes_rom::get_trainer() {
    return trainer;
}

uint8_t *nes_rom::get_prg_rom() {
    return prg_rom;
}

uint8_t *nes_rom::get_prg_ram() {
    return prg_ram;
}

uint8_t *nes_rom::get_chr_rom() {
    return chr_rom;
}

uint8_t *nes_rom::get_misc_rom() {
    return misc_rom;
}

bool nes_rom::get_is_valid() {
    return is_valid;
}

uint32_t nes_rom::get_prg_rom_size() {
    return prg_rom_size;
}

uint32_t nes_rom::get_chr_rom_size() {
    return chr_rom_size;
}

mirroring_type nes_rom::get_mirroring_type() {
    return mirroring_type;
}

bool nes_rom::get_has_battery() {
    return has_battery;
}

bool nes_rom::get_has_trainer() {
    return has_trainer;
}

bool nes_rom::get_is_four_screen_mode() {
    return is_four_screen_mode;
}

console_type nes_rom::get_console_type() {
    return console_type;
}

bool nes_rom::get_is_nes_20() {
    return is_nes_20;
}

mapper *nes_rom::get_mapper() {
    return mapper;
}

uint8_t nes_rom::get_sub_mapper() {
    return sub_mapper;
}

uint32_t nes_rom::get_prg_ram_size() {
    return prg_ram_size;
}

uint32_t nes_rom::get_eeprom_size() {
    return eeprom_size;
}

uint32_t nes_rom::get_chr_ram_size() {
    return chr_ram_size;
}

uint32_t nes_rom::get_chr_nvram_size() {
    return ch_nvram_size;
}

timing_mode nes_rom::get_timing_mode() {
    return timing_mode;
}

uint8_t nes_rom::get_vs_ppu_type() {
    return vs_ppu_type;
}

uint8_t nes_rom::get_vs_hardware_type() {
    return vs_hardware_type;
}

uint8_t nes_rom::get_ext_console_type() {
    return ext_console_type;
}

uint8_t nes_rom::get_misc_rom_count() {
    return misc_rom_count;
}

uint8_t nes_rom::get_def_expansion_device() {
    return def_expansion_device;
}

uint32_t nes_rom::get_misc_rom_size() {
    return misc_rom_size;
}

void nes_rom::load(const rom_path &rom_path) {
    uint64_t size;
    bytes = rom_path.read_all_bytes(&size);

    if (bytes == nullptr)
        throw rom_load_exception("Could not read header.");

    header = new uint8_t[16];
    std::copy(&bytes[0], &bytes[16], header);

    is_valid = header[0] == 'N' && header[1] == 'E' && header[2] == 'S' && header[3] == 0x1a;
    if (!is_valid)
        throw rom_load_exception("Rom is not an nes rom.");

    // Rom is assumed to be correct at this point

    // Header info
    uint8_t prg_rom_size_lsb = header[4];
    uint8_t chr_rom_size_lsb = header[5];

    uint8_t flags6 = header[6];
    switch (flags6 & 0x01) {
        case 0:
            mirroring_type = mirroring_type::horizontal;
            break;
        case 1:
            mirroring_type = mirroring_type::vertical;
            break;
        default:
            throw rom_load_exception("Invalid mirroring type.");
    }

    has_battery = (flags6 >> 1 & 0x01) == 1;
    has_trainer = (flags6 >> 2 & 0x01) == 1;
    is_four_screen_mode = (flags6 >> 3 & 0x01) == 1;

    uint8_t flags7 = header[7];
    switch (flags7 & 0x03) {
        case 0:
            console_type = console_type::nintendo_entertainment_system;
            break;
        case 1:
            console_type = console_type::vs_system;
            break;
        case 2:
            console_type = console_type::playchoice;
            break;
        case 3:
            console_type = console_type::extended;
            break;
        default:
            throw rom_load_exception("Invalid console type.");
    }

    is_nes_20 = (flags7 & 0x0c) == 0x08;

    uint8_t mapper_msb = header[8];
    uint16_t mapper_id = (uint16_t) mapper_msb << 8 & (uint16_t) 0x0f00;
    mapper_id |= flags7 & 0xf0;
    mapper_id |= flags6 >> 4;
    sub_mapper = mapper_msb >> 4;

    uint8_t rom_size_msb = header[9];
    uint8_t prg_rom_size_msb = rom_size_msb & (uint8_t) 0x0f;
    uint8_t chr_rom_size_msb = rom_size_msb >> 4;
    if (prg_rom_size_msb != 0x0f) {
        prg_rom_size =
                (uint32_t) ((prg_rom_size_msb << 8 & 0xff00) | (prg_rom_size_lsb & 0x00ff)) * 16 * 1024; // 16 KiB blocks
    } else {
        // If you have a rom > 4GB then you should re-evaluate your life instead of trying to murder my
        // emulator.
        uint8_t e = prg_rom_size_lsb >> 2;
        uint8_t m = prg_rom_size_lsb & (uint8_t) 0x03;
        prg_rom_size = (uint32_t) (2 << (e - 1)) * (m * 2 + 1);
    }
    if (chr_rom_size_msb != 0x0f) {
        chr_rom_size =
                (uint32_t) ((chr_rom_size_msb << 8 & 0xff00) | (chr_rom_size_lsb & 0x00ff)) * 8 * 1024; // 8 KiB blocks
    } else {
        uint8_t e = chr_rom_size_lsb >> 2;
        uint8_t m = chr_rom_size_lsb & (uint8_t) 0x03;
        chr_rom_size = (uint32_t) (2 << (e - 1)) * (m * 2 + 1);
    }

    uint8_t prg_ram_eeprom_size = header[10];
    uint8_t prg_ram_shift = prg_ram_eeprom_size & (uint8_t) 0x0f;
    uint8_t eeprom_shift = prg_ram_eeprom_size >> 4;
    prg_ram_size = (uint32_t) (prg_ram_shift != 0 ? 64 << prg_ram_shift : 0);
    eeprom_size = (uint32_t) (eeprom_shift != 0 ? 64 << eeprom_shift : 0);

    uint8_t chr_ram_nvram_size = header[11];
    uint8_t chr_ram_shift = chr_ram_nvram_size & (uint8_t) 0x0f;
    uint8_t chr_nvram_shift = chr_ram_nvram_size >> 4;
    chr_ram_size = (uint32_t) (chr_ram_shift != 0 ? 64 << chr_ram_shift : 0);
    ch_nvram_size = (uint32_t) (chr_nvram_shift != 0 ? 64 << chr_nvram_shift : 0);

    switch (header[12] & 0x03) {
        case 0:
            timing_mode = timing_mode::ntsc;
            break;
        case 1:
            timing_mode = timing_mode::pal;
            break;
        case 2:
            timing_mode = timing_mode::multi_region;
            break;
        case 3:
            timing_mode = timing_mode::dendy;
            break;
        default:
            throw rom_load_exception("Invalid CPU/PPU timing mode.");
    }

    uint8_t console_type_info = header[13];
    if (console_type == console_type::vs_system) {
        vs_ppu_type = console_type_info & (uint8_t) 0x0f;
        vs_hardware_type = console_type_info >> 4;
    } else if (console_type == console_type::extended) {
        ext_console_type = console_type_info & (uint8_t) 0x0f;
    }

    misc_rom_count = header[14] & (uint8_t) 0x03;

    def_expansion_device = header[15] & (uint8_t) 0x3f;

    uint32_t byte_section_start = 16;

    // Trainer
    if (has_trainer) {
        trainer = new uint8_t[512];
        std::copy(&bytes[byte_section_start], &bytes[byte_section_start + 512], trainer);
        byte_section_start += 512;
    } else {
        trainer = new uint8_t[0];
    }

    // PRG-ROM
    prg_rom = new uint8_t[prg_rom_size];
    std::copy(&bytes[byte_section_start], &bytes[byte_section_start + prg_rom_size], prg_rom);
    byte_section_start += prg_rom_size;

    // CHR-ROM
    chr_rom = new uint8_t[chr_rom_size];
    std::copy(&bytes[byte_section_start], &bytes[byte_section_start + chr_rom_size], chr_rom);
    byte_section_start += chr_rom_size;

    // Misc ROM data
    misc_rom_size = (uint32_t) (size - 16 - (has_trainer ? 512 : 0) - prg_rom_size - chr_rom_size);
    misc_rom = new uint8_t[misc_rom_size];
    std::copy(&bytes[byte_section_start], &bytes[byte_section_start + misc_rom_size], misc_rom);

    // Ram
    prg_ram = new uint8_t[prg_ram_size];

    // Load Mapper
    switch (mapper_id) {
        case 0x0000:
            mapper = new mapper0000(this);
            break;
        default:
            throw rom_load_exception("Mapper " + std::to_string(mapper_id) + " is currently not supported.");
    }
}

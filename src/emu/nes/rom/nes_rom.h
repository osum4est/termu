//
// Created by Forrest Jones on 2019-02-24.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#ifndef TERMU_NESROM_H
#define TERMU_NESROM_H

#include "../../core/rom/rom.h"
#include "../../core/rom/rom_path.h"
#include "../exceptions/missing_mapper_exception.h"
#include "../../core/exceptions/rom_load_exception.h"

class mapper;

enum mirroring_type {
    single_a,
    single_b,
    vertical,
    horizontal,
};

enum console_type {
    nintendo_entertainment_system,
    vs_system,
    playchoice,
    extended
};

enum timing_mode {
    ntsc,
    pal,
    multi_region,
    dendy
};

/**
 * Only supports NES 2.0 (and the backwards compatible iNES) format.
 */
class nes_rom : public rom {

    std::string name;

    uint8_t *bytes;
    uint8_t *header;
    uint8_t *trainer;
    uint8_t *prg_rom;
    uint8_t *prg_ram;
    uint8_t *chr_rom;
    uint8_t *misc_rom;

    bool is_valid;
    uint32_t prg_rom_size;
    uint32_t chr_rom_size;
    ::mirroring_type mirroring_type;
    bool has_battery;
    bool has_trainer;
    bool is_four_screen_mode;
    ::console_type console_type;
    bool is_nes_20;
    ::mapper *mapper;
    uint8_t sub_mapper;
    uint32_t prg_ram_size;
    uint32_t eeprom_size;
    uint32_t chr_ram_size;
    uint32_t ch_nvram_size;
    ::timing_mode timing_mode;
    uint8_t vs_ppu_type;
    uint8_t vs_hardware_type;
    uint8_t ext_console_type;
    uint8_t misc_rom_count;
    uint8_t def_expansion_device;
    uint32_t misc_rom_size;

public:
    explicit nes_rom(const rom_path &rom_path);

    ~nes_rom();

    std::string get_name() override;

    uint8_t *get_header();

    uint8_t *get_trainer();

    uint8_t *get_prg_rom();

    uint8_t *get_prg_ram();

    uint8_t *get_chr_rom();

    uint8_t *get_misc_rom();

    bool get_is_valid();

    uint32_t get_prg_rom_size();

    uint32_t get_chr_rom_size();

    ::mirroring_type get_mirroring_type();

    bool get_has_battery();

    bool get_has_trainer();

    bool get_is_four_screen_mode();

    ::console_type get_console_type();

    bool get_is_nes_20();

    ::mapper *get_mapper();

    uint8_t get_sub_mapper();

    uint32_t get_prg_ram_size();

    uint32_t get_eeprom_size();

    uint32_t get_chr_ram_size();

    uint32_t get_chr_nvram_size();

    ::timing_mode get_timing_mode();

    uint8_t get_vs_ppu_type();

    uint8_t get_vs_hardware_type();

    uint8_t get_ext_console_type();

    uint8_t get_misc_rom_count();

    uint8_t get_def_expansion_device();

    uint32_t get_misc_rom_size();

private:
    void load(const rom_path &rom_path);
};

#endif //TERMU_NESROM_H

#pragma clang diagnostic pop

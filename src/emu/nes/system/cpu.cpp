#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

#include "cpu.h"

cpu::instruction::instruction() = default;

cpu::instruction::instruction(instr_func instr, addr_func addressing_mode, bool writes, bool official) {
    this->instr = instr;
    this->addressing_mode = addressing_mode;
    this->writes = writes;
    this->official = official;
}

cpu::cpu(::mem *mem, ::ppu *ppu) {
    this->mem = mem;
    this->ppu = ppu;
    setup_instructions();

    // TODO: Remove when done debugging
    // log->set_level(spdlog::level::trace);
}

void cpu::start() {
    PC = get_short(0xfffc, false);
    S = 0xfd;
	A = 0;
	X = 0;
	Y = 0;

    CF = false;
    ZF = false;
    IF = true;
    DF = false;
    BF = false;
    VF = false;
    NF = false;

    start_time = std::chrono::high_resolution_clock::now();
    benchmark_time = std::chrono::high_resolution_clock::now();
    current_cycle = 0;
    this->running = true;

    ppu->set_interrupt_handler(this);
    ppu->start();
    this->run();
}

void cpu::stop() {
    running = false;
}

void cpu::set_irq() {
    pending_irq = true;
}

void cpu::set_nmi() {
    pending_nmi = true;
}

void cpu::run() {
    while (running) {
        // TODO: #if DEBUG
        if (benchmark_cycles > 1790000 && std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::high_resolution_clock::now() - benchmark_time).count() >= 1) {
            log->info(utils::string_format("CPU Clock rate: %.2fMHz / 1.79MHz", benchmark_cycles / 1e+6));
            benchmark_cycles = 0;
            benchmark_time = std::chrono::high_resolution_clock::now();
        }

        if (log->should_log(spdlog::level::trace))
            log->trace(utils::to_upper(utils::string_format(
                    "%04x                                            A:%02x X:%02x Y:%02x P:%02x SP:%02x CPUC:%d",
                    PC, A, X, Y, get_status(), S, current_cycle)));

        // Fetch opcode
        uint8_t opCode = get_byte(PC);
        cpu::instruction &instr = instructions[opCode];
        PC++;

        // Run instruction
        // 8 @ y=233

        uint16_t addr = (this->*instr.addressing_mode)(instr.writes);
        (this->*instr.instr)(addr);

        // Check for interrupts
        // TODO: Poll for these during the correct cycles
        if (pending_nmi) {
            pending_nmi = false;
            get_byte(PC);
            get_byte(PC);
            interrupt(0xfffa, false);
        } else if (pending_irq && !IF) {
            pending_irq = false;
            get_byte(PC);
            get_byte(PC);
            interrupt(0xfffe, false);
        }
    }
}

/**
 * Cycles the CPU. Will block until the cycle lasted long enough.
 */
void cpu::cycle(uint16_t addr, bool write) {
    // TODO: Other cycle things

    // Will only work work for NTSC, since PAL is 3.2 cycles...
    ppu->cycle();
    ppu->cycle();
    ppu->cycle();

    if (log->should_log(spdlog::level::trace))
        log->trace(utils::to_upper(utils::string_format("      %s     $%04x", write ? "WRITE" : "READ ", addr)));

    benchmark_cycles++;
    current_cycle++;
}

// region Instruction Creators

cpu::instruction cpu::r_instr(instr_func instruction) {
    return instr(instruction, &cpu::implied, false, true);
}

cpu::instruction cpu::r_instr(instr_func instruction, bool official) {
    return instr(instruction, &cpu::implied, false, official);
}

cpu::instruction cpu::r_instr(instr_func instruction, addr_func mode) {
    return instr(instruction, mode, false, true);
}

cpu::instruction cpu::r_instr(instr_func instruction, addr_func mode, bool official) {
    return instr(instruction, mode, false, true);
}

cpu::instruction cpu::w_instr(instr_func instruction, addr_func mode) {
    return instr(instruction, mode, true, true);
}

cpu::instruction cpu::w_instr(instr_func instruction, addr_func mode, bool official) {
    return instr(instruction, mode, true, true);
}

cpu::instruction cpu::instr(instr_func instruction, addr_func mode, bool writes, bool official) {
    return {instruction, mode, writes, official};
}

// endregion

void cpu::setup_instructions() {
    instructions[0x00] = r_instr(&cpu::brk);
    instructions[0x01] = r_instr(&cpu::ora, &cpu::indirect_x);
    instructions[0x02] = r_instr(&cpu::stp, false);
    instructions[0x03] = w_instr(&cpu::slo, &cpu::indirect_x, false);
    instructions[0x04] = r_instr(&cpu::skb, &cpu::zero_page, false);
    instructions[0x05] = r_instr(&cpu::ora, &cpu::zero_page);
    instructions[0x06] = w_instr(&cpu::asl, &cpu::zero_page);
    instructions[0x07] = w_instr(&cpu::slo, &cpu::zero_page, false);
    instructions[0x08] = r_instr(&cpu::php);
    instructions[0x09] = r_instr(&cpu::ora, &cpu::immediate);
    instructions[0x0a] = w_instr(&cpu::asl, &cpu::accumulator);
    instructions[0x0b] = r_instr(&cpu::anc, &cpu::immediate, false);
    instructions[0x0c] = r_instr(&cpu::skb, &cpu::absolute, false);
    instructions[0x0d] = r_instr(&cpu::ora, &cpu::absolute);
    instructions[0x0e] = w_instr(&cpu::asl, &cpu::absolute);
    instructions[0x0f] = w_instr(&cpu::slo, &cpu::absolute, false);

    instructions[0x10] = r_instr(&cpu::bpl, &cpu::relative);
    instructions[0x11] = r_instr(&cpu::ora, &cpu::indirect_y);
    instructions[0x12] = r_instr(&cpu::stp, false);
    instructions[0x13] = w_instr(&cpu::slo, &cpu::indirect_y, false);
    instructions[0x14] = r_instr(&cpu::skb, &cpu::zero_page_x, false);
    instructions[0x15] = r_instr(&cpu::ora, &cpu::zero_page_x);
    instructions[0x16] = w_instr(&cpu::asl, &cpu::zero_page_x);
    instructions[0x17] = w_instr(&cpu::slo, &cpu::zero_page_x, false);
    instructions[0x18] = r_instr(&cpu::clc);
    instructions[0x19] = r_instr(&cpu::ora, &cpu::absolute_y);
    instructions[0x1a] = r_instr(&cpu::nop, false);
    instructions[0x1b] = w_instr(&cpu::slo, &cpu::absolute_y, false);
    instructions[0x1c] = r_instr(&cpu::skb, &cpu::absolute_x, false);
    instructions[0x1d] = r_instr(&cpu::ora, &cpu::absolute_x);
    instructions[0x1e] = w_instr(&cpu::asl, &cpu::absolute_x);
    instructions[0x1f] = w_instr(&cpu::slo, &cpu::absolute_x, false);

    instructions[0x20] = r_instr(&cpu::jsr, &cpu::absolute_jsr);
    instructions[0x21] = r_instr(&cpu::_and, &cpu::indirect_x);
    instructions[0x22] = r_instr(&cpu::stp, false);
    instructions[0x23] = w_instr(&cpu::rla, &cpu::indirect_x, false);
    instructions[0x24] = r_instr(&cpu::bit, &cpu::zero_page);
    instructions[0x25] = r_instr(&cpu::_and, &cpu::zero_page);
    instructions[0x26] = w_instr(&cpu::rol, &cpu::zero_page);
    instructions[0x27] = w_instr(&cpu::rla, &cpu::zero_page, false);
    instructions[0x28] = r_instr(&cpu::plp);
    instructions[0x29] = r_instr(&cpu::_and, &cpu::immediate);
    instructions[0x2a] = w_instr(&cpu::rol, &cpu::accumulator);
    instructions[0x2b] = r_instr(&cpu::anc, &cpu::immediate, false);
    instructions[0x2c] = r_instr(&cpu::bit, &cpu::absolute);
    instructions[0x2d] = r_instr(&cpu::_and, &cpu::absolute);
    instructions[0x2e] = w_instr(&cpu::rol, &cpu::absolute);
    instructions[0x2f] = w_instr(&cpu::rla, &cpu::absolute, false);

    instructions[0x30] = r_instr(&cpu::bmi, &cpu::relative);
    instructions[0x31] = r_instr(&cpu::_and, &cpu::indirect_y);
    instructions[0x32] = r_instr(&cpu::stp, false);
    instructions[0x33] = w_instr(&cpu::rla, &cpu::indirect_y, false);
    instructions[0x34] = r_instr(&cpu::skb, &cpu::zero_page_x, false);
    instructions[0x35] = r_instr(&cpu::_and, &cpu::zero_page_x);
    instructions[0x36] = w_instr(&cpu::rol, &cpu::zero_page_x);
    instructions[0x37] = w_instr(&cpu::rla, &cpu::zero_page_x, false);
    instructions[0x38] = r_instr(&cpu::sec);
    instructions[0x39] = r_instr(&cpu::_and, &cpu::absolute_y);
    instructions[0x3a] = r_instr(&cpu::nop, false);
    instructions[0x3b] = w_instr(&cpu::rla, &cpu::absolute_y, false);
    instructions[0x3c] = r_instr(&cpu::skb, &cpu::absolute_x, false);
    instructions[0x3d] = r_instr(&cpu::_and, &cpu::absolute_x);
    instructions[0x3e] = w_instr(&cpu::rol, &cpu::absolute_x);
    instructions[0x3f] = w_instr(&cpu::rla, &cpu::absolute_x, false);

    instructions[0x40] = r_instr(&cpu::rti);
    instructions[0x41] = r_instr(&cpu::eor, &cpu::indirect_x);
    instructions[0x42] = r_instr(&cpu::stp, false);
    instructions[0x43] = w_instr(&cpu::sre, &cpu::indirect_x, false);
    instructions[0x44] = r_instr(&cpu::skb, &cpu::zero_page, false);
    instructions[0x45] = r_instr(&cpu::eor, &cpu::zero_page);
    instructions[0x46] = w_instr(&cpu::lsr, &cpu::zero_page);
    instructions[0x47] = w_instr(&cpu::sre, &cpu::zero_page, false);
    instructions[0x48] = r_instr(&cpu::pha);
    instructions[0x49] = r_instr(&cpu::eor, &cpu::immediate);
    instructions[0x4a] = w_instr(&cpu::lsr, &cpu::accumulator);
    instructions[0x4b] = r_instr(&cpu::alr, &cpu::immediate, false);
    instructions[0x4c] = r_instr(&cpu::jmp, &cpu::absolute);
    instructions[0x4d] = r_instr(&cpu::eor, &cpu::absolute);
    instructions[0x4e] = w_instr(&cpu::lsr, &cpu::absolute);
    instructions[0x4f] = w_instr(&cpu::sre, &cpu::absolute, false);

    instructions[0x50] = r_instr(&cpu::bvc, &cpu::relative);
    instructions[0x51] = r_instr(&cpu::eor, &cpu::indirect_y);
    instructions[0x52] = r_instr(&cpu::stp, false);
    instructions[0x53] = w_instr(&cpu::sre, &cpu::indirect_y, false);
    instructions[0x54] = r_instr(&cpu::skb, &cpu::zero_page_x, false);
    instructions[0x55] = r_instr(&cpu::eor, &cpu::zero_page_x);
    instructions[0x56] = w_instr(&cpu::lsr, &cpu::zero_page_x);
    instructions[0x57] = w_instr(&cpu::sre, &cpu::zero_page_x, false);
    instructions[0x58] = r_instr(&cpu::cli);
    instructions[0x59] = r_instr(&cpu::eor, &cpu::absolute_y);
    instructions[0x5a] = r_instr(&cpu::nop, false);
    instructions[0x5b] = w_instr(&cpu::sre, &cpu::absolute_y, false);
    instructions[0x5c] = r_instr(&cpu::skb, &cpu::absolute_x, false);
    instructions[0x5d] = r_instr(&cpu::eor, &cpu::absolute_x);
    instructions[0x5e] = w_instr(&cpu::lsr, &cpu::absolute_x);
    instructions[0x5f] = w_instr(&cpu::sre, &cpu::absolute_x, false);

    instructions[0x60] = r_instr(&cpu::rts);
    instructions[0x61] = r_instr(&cpu::adc, &cpu::indirect_x);
    instructions[0x62] = r_instr(&cpu::stp, false);
    instructions[0x63] = w_instr(&cpu::rra, &cpu::indirect_x, false);
    instructions[0x64] = r_instr(&cpu::skb, &cpu::zero_page, false);
    instructions[0x65] = r_instr(&cpu::adc, &cpu::zero_page);
    instructions[0x66] = w_instr(&cpu::ror, &cpu::zero_page);
    instructions[0x67] = w_instr(&cpu::rra, &cpu::zero_page, false);
    instructions[0x68] = r_instr(&cpu::pla);
    instructions[0x69] = r_instr(&cpu::adc, &cpu::immediate);
    instructions[0x6a] = w_instr(&cpu::ror, &cpu::accumulator);
    instructions[0x6b] = r_instr(&cpu::arr, &cpu::immediate, false);
    instructions[0x6c] = r_instr(&cpu::jmp, &cpu::indirect);
    instructions[0x6d] = r_instr(&cpu::adc, &cpu::absolute);
    instructions[0x6e] = w_instr(&cpu::ror, &cpu::absolute);
    instructions[0x6f] = w_instr(&cpu::rra, &cpu::absolute, false);

    instructions[0x70] = r_instr(&cpu::bvs, &cpu::relative);
    instructions[0x71] = r_instr(&cpu::adc, &cpu::indirect_y);
    instructions[0x72] = r_instr(&cpu::stp, false);
    instructions[0x73] = w_instr(&cpu::rra, &cpu::indirect_y, false);
    instructions[0x74] = r_instr(&cpu::skb, &cpu::zero_page_x, false);
    instructions[0x75] = r_instr(&cpu::adc, &cpu::zero_page_x);
    instructions[0x76] = w_instr(&cpu::ror, &cpu::zero_page_x);
    instructions[0x77] = w_instr(&cpu::rra, &cpu::zero_page_x, false);
    instructions[0x78] = r_instr(&cpu::sei);
    instructions[0x79] = r_instr(&cpu::adc, &cpu::absolute_y);
    instructions[0x7a] = r_instr(&cpu::nop, false);
    instructions[0x7b] = w_instr(&cpu::rra, &cpu::absolute_y, false);
    instructions[0x7c] = r_instr(&cpu::skb, &cpu::absolute_x, false);
    instructions[0x7d] = r_instr(&cpu::adc, &cpu::absolute_x);
    instructions[0x7e] = w_instr(&cpu::ror, &cpu::absolute_x);
    instructions[0x7f] = w_instr(&cpu::rra, &cpu::absolute_x, false);

    instructions[0x80] = r_instr(&cpu::skb, &cpu::immediate, false);
    instructions[0x81] = w_instr(&cpu::sta, &cpu::indirect_x);
    instructions[0x82] = r_instr(&cpu::skb, &cpu::immediate, false);
    instructions[0x83] = w_instr(&cpu::sax, &cpu::indirect_x, false);
    instructions[0x84] = w_instr(&cpu::sty, &cpu::zero_page);
    instructions[0x85] = w_instr(&cpu::sta, &cpu::zero_page);
    instructions[0x86] = w_instr(&cpu::stx, &cpu::zero_page);
    instructions[0x87] = w_instr(&cpu::sax, &cpu::zero_page, false);
    instructions[0x88] = r_instr(&cpu::dey);
    instructions[0x89] = r_instr(&cpu::skb, &cpu::immediate, false);
    instructions[0x8a] = r_instr(&cpu::txa);
    instructions[0x8b] = r_instr(&cpu::xaa, &cpu::immediate, false);
    instructions[0x8c] = w_instr(&cpu::sty, &cpu::absolute);
    instructions[0x8d] = w_instr(&cpu::sta, &cpu::absolute);
    instructions[0x8e] = w_instr(&cpu::stx, &cpu::absolute);
    instructions[0x8f] = w_instr(&cpu::sax, &cpu::absolute, false);

    instructions[0x90] = r_instr(&cpu::bcc, &cpu::relative);
    instructions[0x91] = w_instr(&cpu::sta, &cpu::indirect_y);
    instructions[0x92] = r_instr(&cpu::stp, false);
    instructions[0x93] = w_instr(&cpu::sha, &cpu::indirect_y, false);
    instructions[0x94] = w_instr(&cpu::sty, &cpu::zero_page_x);
    instructions[0x95] = w_instr(&cpu::sta, &cpu::zero_page_x);
    instructions[0x96] = w_instr(&cpu::stx, &cpu::zero_page_y);
    instructions[0x97] = w_instr(&cpu::sax, &cpu::zero_page_y, false);
    instructions[0x98] = r_instr(&cpu::tya);
    instructions[0x99] = w_instr(&cpu::sta, &cpu::absolute_y);
    instructions[0x9a] = r_instr(&cpu::txs);
    instructions[0x9b] = w_instr(&cpu::shs, &cpu::absolute_y, false);
    instructions[0x9c] = w_instr(&cpu::shy, &cpu::absolute_x, false);
    instructions[0x9d] = w_instr(&cpu::sta, &cpu::absolute_x);
    instructions[0x9e] = w_instr(&cpu::shx, &cpu::absolute_y, false);
    instructions[0x9f] = w_instr(&cpu::sha, &cpu::absolute_y, false);

    instructions[0xa0] = r_instr(&cpu::ldy, &cpu::immediate);
    instructions[0xa1] = r_instr(&cpu::lda, &cpu::indirect_x);
    instructions[0xa2] = r_instr(&cpu::ldx, &cpu::immediate);
    instructions[0xa3] = r_instr(&cpu::lax, &cpu::indirect_x, false);
    instructions[0xa4] = r_instr(&cpu::ldy, &cpu::zero_page);
    instructions[0xa5] = r_instr(&cpu::lda, &cpu::zero_page);
    instructions[0xa6] = r_instr(&cpu::ldx, &cpu::zero_page);
    instructions[0xa7] = r_instr(&cpu::lax, &cpu::zero_page, false);
    instructions[0xa8] = r_instr(&cpu::tay);
    instructions[0xa9] = r_instr(&cpu::lda, &cpu::immediate);
    instructions[0xaa] = r_instr(&cpu::tax);
    instructions[0xab] = r_instr(&cpu::lax, &cpu::immediate, false);
    instructions[0xac] = r_instr(&cpu::ldy, &cpu::absolute);
    instructions[0xad] = r_instr(&cpu::lda, &cpu::absolute);
    instructions[0xae] = r_instr(&cpu::ldx, &cpu::absolute);
    instructions[0xaf] = r_instr(&cpu::lax, &cpu::absolute, false);

    instructions[0xb0] = r_instr(&cpu::bcs, &cpu::relative);
    instructions[0xb1] = r_instr(&cpu::lda, &cpu::indirect_y);
    instructions[0xb2] = r_instr(&cpu::stp, false);
    instructions[0xb3] = r_instr(&cpu::lax, &cpu::indirect_y, false);
    instructions[0xb4] = r_instr(&cpu::ldy, &cpu::zero_page_x);
    instructions[0xb5] = r_instr(&cpu::lda, &cpu::zero_page_x);
    instructions[0xb6] = r_instr(&cpu::ldx, &cpu::zero_page_y);
    instructions[0xb7] = r_instr(&cpu::lax, &cpu::zero_page_y, false);
    instructions[0xb8] = r_instr(&cpu::clv);
    instructions[0xb9] = r_instr(&cpu::lda, &cpu::absolute_y);
    instructions[0xba] = r_instr(&cpu::tsx);
    instructions[0xbb] = r_instr(&cpu::las, &cpu::absolute_y, false);
    instructions[0xbc] = r_instr(&cpu::ldy, &cpu::absolute_x);
    instructions[0xbd] = r_instr(&cpu::lda, &cpu::absolute_x);
    instructions[0xbe] = r_instr(&cpu::ldx, &cpu::absolute_y);
    instructions[0xbf] = r_instr(&cpu::lax, &cpu::absolute_y, false);

    instructions[0xc0] = r_instr(&cpu::cpy, &cpu::immediate);
    instructions[0xc1] = r_instr(&cpu::cmp, &cpu::indirect_x);
    instructions[0xc2] = r_instr(&cpu::skb, &cpu::immediate, false);
    instructions[0xc3] = w_instr(&cpu::dcp, &cpu::indirect_x, false);
    instructions[0xc4] = r_instr(&cpu::cpy, &cpu::zero_page);
    instructions[0xc5] = r_instr(&cpu::cmp, &cpu::zero_page);
    instructions[0xc6] = w_instr(&cpu::dec, &cpu::zero_page);
    instructions[0xc7] = w_instr(&cpu::dcp, &cpu::zero_page, false);
    instructions[0xc8] = r_instr(&cpu::iny);
    instructions[0xc9] = r_instr(&cpu::cmp, &cpu::immediate);
    instructions[0xca] = r_instr(&cpu::dex);
    instructions[0xcb] = r_instr(&cpu::axs, &cpu::immediate, false);
    instructions[0xcc] = r_instr(&cpu::cpy, &cpu::absolute);
    instructions[0xcd] = r_instr(&cpu::cmp, &cpu::absolute);
    instructions[0xce] = w_instr(&cpu::dec, &cpu::absolute);
    instructions[0xcf] = w_instr(&cpu::dcp, &cpu::absolute, false);

    instructions[0xd0] = r_instr(&cpu::bne, &cpu::relative);
    instructions[0xd1] = r_instr(&cpu::cmp, &cpu::indirect_y);
    instructions[0xd2] = r_instr(&cpu::stp, false);
    instructions[0xd3] = w_instr(&cpu::dcp, &cpu::indirect_y, false);
    instructions[0xd4] = r_instr(&cpu::skb, &cpu::zero_page_x, false);
    instructions[0xd5] = r_instr(&cpu::cmp, &cpu::zero_page_x);
    instructions[0xd6] = w_instr(&cpu::dec, &cpu::zero_page_x);
    instructions[0xd7] = w_instr(&cpu::dcp, &cpu::zero_page_x, false);
    instructions[0xd8] = r_instr(&cpu::cld);
    instructions[0xd9] = r_instr(&cpu::cmp, &cpu::absolute_y);
    instructions[0xda] = r_instr(&cpu::nop, false);
    instructions[0xdb] = w_instr(&cpu::dcp, &cpu::absolute_y, false);
    instructions[0xdc] = r_instr(&cpu::skb, &cpu::absolute_x, false);
    instructions[0xdd] = r_instr(&cpu::cmp, &cpu::absolute_x);
    instructions[0xde] = w_instr(&cpu::dec, &cpu::absolute_x);
    instructions[0xdf] = w_instr(&cpu::dcp, &cpu::absolute_x, false);

    instructions[0xe0] = r_instr(&cpu::cpx, &cpu::immediate);
    instructions[0xe1] = r_instr(&cpu::sbc, &cpu::indirect_x);
    instructions[0xe2] = r_instr(&cpu::skb, &cpu::immediate, false);
    instructions[0xe3] = w_instr(&cpu::isc, &cpu::indirect_x, false);
    instructions[0xe4] = r_instr(&cpu::cpx, &cpu::zero_page);
    instructions[0xe5] = r_instr(&cpu::sbc, &cpu::zero_page);
    instructions[0xe6] = w_instr(&cpu::inc, &cpu::zero_page);
    instructions[0xe7] = w_instr(&cpu::isc, &cpu::zero_page, false);
    instructions[0xe8] = r_instr(&cpu::inx);
    instructions[0xe9] = r_instr(&cpu::sbc, &cpu::immediate);
    instructions[0xea] = r_instr(&cpu::nop);
    instructions[0xeb] = r_instr(&cpu::sbc, &cpu::immediate, false);
    instructions[0xec] = r_instr(&cpu::cpx, &cpu::absolute);
    instructions[0xed] = r_instr(&cpu::sbc, &cpu::absolute);
    instructions[0xee] = w_instr(&cpu::inc, &cpu::absolute);
    instructions[0xef] = w_instr(&cpu::isc, &cpu::absolute, false);

    instructions[0xf0] = r_instr(&cpu::beq, &cpu::relative);
    instructions[0xf1] = r_instr(&cpu::sbc, &cpu::indirect_y);
    instructions[0xf2] = r_instr(&cpu::stp, false);
    instructions[0xf3] = w_instr(&cpu::isc, &cpu::indirect_y, false);
    instructions[0xf4] = r_instr(&cpu::skb, &cpu::zero_page_x, false);
    instructions[0xf5] = r_instr(&cpu::sbc, &cpu::zero_page_x);
    instructions[0xf6] = w_instr(&cpu::inc, &cpu::zero_page_x);
    instructions[0xf7] = w_instr(&cpu::isc, &cpu::zero_page_x, false);
    instructions[0xf8] = r_instr(&cpu::sed);
    instructions[0xf9] = r_instr(&cpu::sbc, &cpu::absolute_y);
    instructions[0xfa] = r_instr(&cpu::nop, false);
    instructions[0xfb] = w_instr(&cpu::isc, &cpu::absolute_y, false);
    instructions[0xfc] = r_instr(&cpu::skb, &cpu::absolute_x, false);
    instructions[0xfd] = r_instr(&cpu::sbc, &cpu::absolute_x);
    instructions[0xfe] = w_instr(&cpu::inc, &cpu::absolute_x);
    instructions[0xff] = w_instr(&cpu::isc, &cpu::absolute_x, false);
}

// region Addressing Modes

uint16_t cpu::accumulator(bool writes) {
    get_byte(PC);
    return (uint16_t) -1;
}

uint16_t cpu::implied(bool writes) {
    get_byte(PC);
    return (uint16_t) -1;
}

uint16_t cpu::immediate(bool writes) {
    PC++;
    return PC - (uint16_t) 1;
}

uint16_t cpu::absolute(bool writes) {
    PC += 2;
    return get_short(PC - (uint16_t) 2);
}

uint16_t cpu::absolute_jsr(bool writes) {
    PC++;
    return get_byte(PC - (uint16_t) 1);
}

uint16_t cpu::absolute_x(bool writes) {
    PC += 2;
    uint16_t addr = get_short(PC - (uint16_t) 2);
    if ((addr & 0x00ff) + X > 0xff || writes)
        get_byte((uint16_t) ((addr & 0xff00) | ((addr + X) & 0x00ff)));
    return addr + X;
}

uint16_t cpu::absolute_y(bool writes) {
    PC += 2;
    uint16_t addr = get_short(PC - (uint16_t) 2);
    if ((addr & 0x00ff) + Y > 0xff || writes)
        get_byte((uint16_t) ((addr & 0xff00) | ((addr + Y) & 0x00ff)));
    return addr + Y;
}

uint16_t cpu::zero_page(bool writes) {
    PC++;
    return get_byte(PC - (uint16_t) 1);
}

uint16_t cpu::zero_page_x(bool writes) {
    PC++;
    uint16_t addr = get_byte(PC - (uint16_t) 1);
    get_byte(addr);
    return (addr + X) & (uint16_t) 0x00ff;
}

uint16_t cpu::zero_page_y(bool writes) {
    PC++;
    uint16_t addr = get_byte(PC - (uint16_t) 1);
    get_byte(addr);
    return (addr + Y) & (uint16_t) 0x00ff;
}

uint16_t cpu::relative(bool writes) {
    PC++;
    return PC - (uint16_t) 1;
}

uint16_t cpu::indirect(bool writes) {
    PC += 2;
    return get_paged_short(get_short(PC - (uint16_t) 2));
}

uint16_t cpu::indirect_x(bool writes) {
    PC++;
    uint16_t addr = get_byte(PC - (uint16_t) 1);
    get_byte(addr);
    addr = (addr + X) & (uint16_t) 0x00ff;
    return get_paged_short(addr);
}

uint16_t cpu::indirect_y(bool writes) {
    PC++;
    uint16_t addr = get_byte(PC - (uint16_t) 1);
    addr = get_paged_short(addr);
    if ((addr & 0x00ff) + Y > 0xff || writes)
        get_byte((uint16_t) ((addr & 0xff00) | ((addr + Y) & 0x00ff)));
    return addr + Y;
}

// endregion

// region Instruction Helpers

void cpu::set_byte(uint16_t addr, uint8_t b) {
    mem->set_cpu(addr, b);
    cycle(addr, true);

    if (addr == 0x4014)
        oam_dma(b);
}

uint8_t cpu::get_byte(uint16_t addr, bool cycle) {
    uint8_t b = mem->get_cpu(addr);
    if (cycle)
        this->cycle(addr, false);
    return b;
}

uint16_t cpu::get_short(uint16_t addr, bool cycle) {
    return (uint16_t) ((get_byte(addr, cycle) & 0x00ff) | ((get_byte(addr + (uint16_t) 1, cycle) << 8) & 0xff00));
}

uint16_t cpu::get_paged_short(uint16_t addr, bool cycle) {
    uint16_t lo = addr;
    uint16_t hi = (uint16_t) ((addr & 0xff00) | ((addr + 1) & 0x00ff));
    return (uint16_t) ((get_byte(lo, cycle) & 0x00ff) | (get_byte(hi, cycle) << 8 & 0xff00));
}

void cpu::set_zn(uint8_t b) {
    ZF = b == 0;
    NF = (int8_t) b < 0;
}

void cpu::branch(bool branch, uint16_t addr) {
    int8_t offset = get_byte(addr);
    if (branch) {
        get_byte(PC);
        if ((PC + offset) >> 8 != PC >> 8)
            get_byte((uint16_t) ((PC & 0xff00) | ((PC + offset) & 0x00ff)));
        PC += offset;
    }
}

void cpu::compare(uint8_t a, uint8_t b) {
    CF = a >= b;
    ZF = a == b;
    NF = (bool) ((a - b) >> 7 & 0x01);
}

void cpu::accumulator_instr(bool cf, uint8_t b_old, uint8_t b_new, uint16_t addr) {
    CF = cf;

    if (addr == (uint16_t) -1) {
        A = b_new;
    } else {
        set_byte(addr, b_old);
        set_byte(addr, b_new);
    }

    set_zn(b_new);
}

void cpu::push_byte(uint8_t b) {
    set_byte((uint16_t) 0x0100 | (uint16_t) S, b);
    S--;
}

void cpu::push_short(uint16_t s) {
    push_byte((uint8_t) (s >> 8));
    push_byte((uint8_t) (s & 0x00ff));
}

uint8_t cpu::pop_byte() {
    return pop_byte(true);
}

uint8_t cpu::pop_byte(bool cycle) {
    if (cycle)
        get_byte((uint16_t) 0x0100 | S);

    S++;
    return get_byte((uint16_t) 0x0100 | S);
}

uint16_t cpu::pop_short() {
    return pop_short(true);
}

uint16_t cpu::pop_short(bool cycle) {
    if (cycle)
        get_byte((uint16_t) 0x0100 | S);

    uint8_t lo = pop_byte(false);
    uint8_t hi = pop_byte(false);
    return ((uint16_t) hi << 8) | lo;
}

uint8_t cpu::get_status() {
    uint8_t status = 0x20;

    if (NF == 1)
        status |= 0x80;
    if (VF == 1)
        status |= 0x40;
    if (BF == 1)
        status |= 0x10;
    if (DF == 1)
        status |= 0x08;
    if (IF == 1)
        status |= 0x04;
    if (ZF == 1)
        status |= 0x02;
    if (CF == 1)
        status |= 0x01;

    return status;
}

void cpu::set_status(uint8_t b) {
    NF = (bool) (b >> 7 & 0x01);
    VF = (bool) (b >> 6 & 0x01);
    DF = (bool) (b >> 3 & 0x01);
    IF = (bool) (b >> 2 & 0x01);
    ZF = (bool) (b >> 1 & 0x01);
    CF = (bool) (b & 0x01);
}

void cpu::oam_dma(uint8_t b) {
    get_byte(0x4014); // Dummy read cycle
    if (current_cycle % 2 == 1) // If odd
        get_byte(0x4014); // Dummy read cycle

    for (uint16_t addr = b << 8; addr < ((b << 8) | 0xff); addr++)
        set_byte(0x2004, get_byte(addr));
}

void cpu::interrupt(uint16_t interrupt_vector, bool b_flag) {
    PC++;
    BF = b_flag;
    push_short(PC);
    push_byte(get_status());
    PC = get_short(interrupt_vector);
    BF = false;
}

// endregion

// region Official Instructions

/**
 * Add with Carry
 */
void cpu::adc(uint16_t addr) {
    adc(get_byte(addr));
}

/**
 * Add with Carry op
 */
void cpu::adc(uint8_t op) {
    int16_t tempA = (int16_t) A + (int16_t) op + CF;
    int16_t tempASigned = (int16_t) (int8_t) A + (int16_t) (int8_t) op + CF;
    CF = tempA > 0xff;
    VF = tempASigned < -128 || tempASigned > 127;
    A = (uint8_t) tempA;
    set_zn(A);
}

/**
 * Logical AND
 */
void cpu::_and(uint16_t addr) {
    A &= get_byte(addr);
    set_zn(A);
}

/**
 * Arithmetic Shift Left
 */
void cpu::asl(uint16_t addr) {
    uint8_t op = addr == (uint16_t) -1 ? A : get_byte(addr);
    accumulator_instr(op >> 7, op, op << 1, addr);
}

/**
 * Branch if Carry Clear
 */
void cpu::bcc(uint16_t addr) {
    branch(!CF, addr);
}

/**
 * Branch if Carry Set
 */
void cpu::bcs(uint16_t addr) {
    branch(CF, addr);
}

/**
 * Branch if Equal
 */
void cpu::beq(uint16_t addr) {
    branch(ZF, addr);
}

/**
 * Bit Test
 */
void cpu::bit(uint16_t addr) {
    uint8_t op = get_byte(addr);
    ZF = (A & op) == 0;
    VF = (bool) (op >> 6 & 0x01);
    NF = (bool) (op >> 7 & 0x01);
}

/**
 * Branch if Minus
 */
void cpu::bmi(uint16_t addr) {
    branch(NF, addr);
}

/**
 * Branch if Not Equal
 */
void cpu::bne(uint16_t addr) {
    branch(!ZF, addr);
}

/**
 * Branch if Positive
 */
void cpu::bpl(uint16_t addr) {
    branch(!NF, addr);
}

/**
 * Force Interrupt
 */
void cpu::brk(uint16_t addr) {
    interrupt(0xfffe, true);
}

/**
 * Branch if Overflow Clear
 */
void cpu::bvc(uint16_t addr) {
    branch(!VF, addr);
}

/**
 * Branch if Overflow Set
 */
void cpu::bvs(uint16_t addr) {
    branch(VF, addr);
}

/**
 * Clear Carry Flag
 */
void cpu::clc(uint16_t addr) {
    CF = false;
}

/**
 * Clear Decimal Mode
 */
void cpu::cld(uint16_t addr) {
    DF = false;
}

/**
 * Clear Interrupt Disable
 */
void cpu::cli(uint16_t addr) {
    IF = false;
}

/**
 * Clear Overflow Flag
 */
void cpu::clv(uint16_t addr) {
    VF = false;
}

/**
 * Compare
 */
void cpu::cmp(uint16_t addr) {
    cmp(get_byte(addr));
}

/**
 * Compare op
 */
void cpu::cmp(uint8_t op) {
    compare(A, op);
}

/**
 * Compare X Register
 */
void cpu::cpx(uint16_t addr) {
    compare(X, get_byte(addr));
}

/**
 * Compare Y Register
 */
void cpu::cpy(uint16_t addr) {
    compare(Y, get_byte(addr));
}

/**
 * Decrement Memory
 */
void cpu::dec(uint16_t addr) {
    uint8_t op = get_byte(addr);
    set_byte(addr, op);
    op--;
    set_zn(op);
    set_byte(addr, op);
}

/**
 * Decrement X Register
 */
void cpu::dex(uint16_t addr) {
    X--;
    set_zn(X);
}

/**
 * Decrement Y Register
 */
void cpu::dey(uint16_t addr) {
    Y--;
    set_zn(Y);
}

/**
 * Exclusive OR
 */
void cpu::eor(uint16_t addr) {
    A ^= get_byte(addr);
    set_zn(A);
}

/**
 * Increment Memory
 */
void cpu::inc(uint16_t addr) {
    uint8_t op = get_byte(addr);
    set_byte(addr, op);
    op++;
    set_zn(op);
    set_byte(addr, op);
}

/**
 * Increment X Register
 */
void cpu::inx(uint16_t addr) {
    X++;
    set_zn(X);
}

/**
 * Increment Y Register
 */
void cpu::iny(uint16_t addr) {
    Y++;
    set_zn(Y);
}

/**
 * Jump
 */
void cpu::jmp(uint16_t addr) {
    PC = addr;
}

/**
 * Jump to Subroutine
 */
void cpu::jsr(uint16_t addr) {
    get_byte((uint16_t) 0x0100 | S);
    push_short(PC);
    uint8_t hi = get_byte(PC);
    PC = ((uint16_t) hi << 8) | addr;
}

/**
 * Load Accumulator
 */
void cpu::lda(uint16_t addr) {
    A = get_byte(addr);
    set_zn(A);
}

/**
 * Load X Register
 */
void cpu::ldx(uint16_t addr) {
    X = get_byte(addr);
    set_zn(X);
}

/**
 * Load Y Register
 */
void cpu::ldy(uint16_t addr) {
    Y = get_byte(addr);
    set_zn(Y);
}

/**
 * Logical Shift Right
 */
void cpu::lsr(uint16_t addr) {
    uint8_t op = addr == (uint16_t) -1 ? A : get_byte(addr);
    accumulator_instr((bool) (op & 0x01), op, op >> 1, addr);
}

/**
 * No Operation
 */
void cpu::nop(uint16_t addr) {
}

/**
 * Logical Inclusive OR
 */
void cpu::ora(uint16_t addr) {
    A |= get_byte(addr);
    set_zn(A);
}

/**
 * Push Accumulator
 */
void cpu::pha(uint16_t addr) {
    push_byte(A);
}

/**
 * Push Processor Status
 */
void cpu::php(uint16_t addr) {
    BF = true;
    push_byte(get_status());
    BF = false;
}

/**
 * Pull Accumulator
 */
void cpu::pla(uint16_t addr) {
    A = pop_byte();
    set_zn(A);
}

/**
 * Pull Processor Status
 */
void cpu::plp(uint16_t addr) {
    set_status(pop_byte());
}

/**
 * Rotate Left
 */
void cpu::rol(uint16_t addr) {
    uint8_t op = addr == (uint16_t) -1 ? A : get_byte(addr);
    accumulator_instr(op >> 7, op, op << 1 | (uint8_t)CF, addr);
}

/**
 * Rotate Right
 */
void cpu::ror(uint16_t addr) {
    uint8_t op = addr == (uint16_t) -1 ? A : get_byte(addr);
    accumulator_instr((bool) (op & 0x01), op, op >> 1 | CF << 7, addr);
}

/**
 * Return from Interrupt
 */
void cpu::rti(uint16_t addr) {
    set_status(pop_byte());
    PC = pop_short(false);
}

/**
 * Return from Subroutine
 */
void cpu::rts(uint16_t addr) {
    PC = pop_short();
    get_byte(PC);
    PC++;
}

/**
 * Subtract with Carry
 */
void cpu::sbc(uint16_t addr) {
    sbc(get_byte(addr));
}

/**
 * Subtract with Carry op
 */
void cpu::sbc(uint8_t op) {
    adc((uint8_t) ~op);
}

/**
 * Set Carry Flag
 */
void cpu::sec(uint16_t addr) {
    CF = true;
}

/**
 * Set Decimal Flag
 */
void cpu::sed(uint16_t addr) {
    DF = true;
}

/**
 * Set Interrupt Disable
 */
void cpu::sei(uint16_t addr) {
    IF = true;
}

/**
 * Store Accumulator
 */
void cpu::sta(uint16_t addr) {
    set_byte(addr, A);
}

/**
 * Store X Register
 */
void cpu::stx(uint16_t addr) {
    set_byte(addr, X);
}

/**
 * Store Y Register
 */
void cpu::sty(uint16_t addr) {
    set_byte(addr, Y);
}

/**
 * Transfer Accumulator to X
 */
void cpu::tax(uint16_t addr) {
    X = A;
    set_zn(X);
}

/**
 * Transfer Accumulator to Y
 */
void cpu::tay(uint16_t addr) {
    Y = A;
    set_zn(Y);
}

/**
 * Transfer Stack Pointer to X
 */
void cpu::tsx(uint16_t addr) {
    X = S;
    set_zn(X);
}

/**
 * Transfer X to Accumulator
 */
void cpu::txa(uint16_t addr) {
    A = X;
    set_zn(A);
}

/**
 * Transfer X to Stack Pointer
 */
void cpu::txs(uint16_t addr) {
    S = X;
}

/**
 * Transfer Y to Accumulator
 */
void cpu::tya(uint16_t addr) {
    A = Y;
    set_zn(Y);
}

// endregion

// region Unofficial Instructions

// TODO: Test

/**
 * AND then LSR
 */
void cpu::alr(uint16_t addr) {
    _and(addr);
    lsr((uint16_t) -1);
}

/**
 * AND with Carry
 */
void cpu::anc(uint16_t addr) {
    _and(addr);
    CF = NF;
}

/**
 * AND then ROR with Overflow
 */
void cpu::arr(uint16_t addr) {
    _and(addr);
    ror((uint16_t) -1);
    CF = (bool) ((A >> 6) & 0x01);
    VF = (bool) (CF ^ ((A >> 5) & 0x01));
}

/**
 * AND A to X then subtract Immediate
 */
void cpu::axs(uint16_t addr) {
    X &= A;
    X = X - get_byte(addr);
    set_zn(X);
    CF = (int8_t) X < 0;
}

/**
 * DEC then CMP
 */
void cpu::dcp(uint16_t addr) {
    dec(addr);
    cmp(mem->get_cpu(addr));
}

/**
 * INC then SBC
 */
void cpu::isc(uint16_t addr) {
    inc(addr);
    sbc(mem->get_cpu(addr));
}

/**
 * AND with Stack to Stack and X and Accumulator
 */
void cpu::las(uint16_t addr) {
    uint8_t op = get_byte(addr);
    S &= op;
    A = S;
    X = S;
    set_zn(S);
}

/**
 * Load Accumulator and X Register
 */
void cpu::lax(uint16_t addr) {
    lda(addr);
    tax(addr);
}

/**
 * ROL then AND
 */
void cpu::rla(uint16_t addr) {
    rol(addr);
    A &= mem->get_cpu(addr);
    set_zn(A);
}

/**
 * ROR then ADC
 */
void cpu::rra(uint16_t addr) {
    ror(addr);
    adc(mem->get_cpu(addr));
}

/**
 * Store AND X
 */
void cpu::sax(uint16_t addr) {
    set_byte(addr, A & X);
}

/**
 * Store A & X & (ADDR_HI + 1)
 */
void cpu::sha(uint16_t addr) {
    set_byte(addr, A & X & (uint8_t) ((addr >> 8) + 1));
}

/**
 * SHA and TXS, where X is replaced by (A & X)
 */
void cpu::shs(uint16_t addr) {
    S = X & A;
    set_byte(addr, S & (uint8_t) ((addr >> 8) + 1));
}

/**
 * Store X & (ADDR_HI + 1)
 */
void cpu::shx(uint16_t addr) {
    set_byte(addr, X & (uint8_t) ((addr >> 8) + 1));
}

/**
 * Store Y & (ADDR_HI + 1)
 */
void cpu::shy(uint16_t addr) {
    set_byte(addr, Y & (uint8_t) ((addr >> 8) + 1));
}

/**
 * NOP with Read
 */
void cpu::skb(uint16_t addr) {
    get_byte(addr);
}

/**
 * ASL then ORA
 */
void cpu::slo(uint16_t addr) {
    asl(addr);
    A |= mem->get_cpu(addr);
    set_zn(A);
}

/**
 * LSR then EOR
 */
void cpu::sre(uint16_t addr) {
    lsr(addr);
    A ^= mem->get_cpu(addr);
    set_zn(A);
}

/**
 * Stop the CPU
 */
void cpu::stp(uint16_t addr) {
    stop();
}

/**
 * Does some weird magic stuff apparently that I'm too dumb to understand
 */
void cpu::xaa(uint16_t addr) {
    get_byte(addr);
}

// endregion

#pragma clang diagnostic pop

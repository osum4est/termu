package main.java.com.eightbitforest.termu.emu.nes.system;

import java.io.IOException;
import java.util.logging.*;

class Cpu {
    private final Logger LOGGER = Logger.getLogger(getClass().getName());

    // Registers
    private int PC;
    private byte S;
    private byte A;
    private byte X;
    private byte Y;

    // Flags
    private byte CF;
    private byte ZF;
    private byte IF;
    private byte DF;
    private byte BF;
    private byte VF;
    private byte NF;

    private Mem mem;
    private Instruction[] instructions;

    private boolean running;
    private long currentCycle;

    private long startTime;
    private long benchmarkTime;
    private long benchmarkCycles;

    Cpu(Mem mem) {
        this.mem = mem;
        instructions = new Instruction[256];
        setupInstructions();

        // TODO: Turn off when not debugging
        setupDebug();
    }

    private void setupDebug() {
        try {
            FileHandler logFile = new FileHandler("./termu.log");
            logFile.setLevel(Level.FINEST);
            logFile.setFormatter(new Formatter() {
                @Override
                public String format(LogRecord record) {
                    return record.getMessage() + "\n";
                }
            });
            logFile.setFilter(record -> record.getLevel() == Level.FINEST);
            LOGGER.setLevel(Level.ALL);
            LOGGER.addHandler(logFile);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    void start() {
        // TODO: Set PC to reset vector. Using 0xC000 since PPU is not implemented yet
        PC = 0xc000;
        S = (byte) 0xfd;

        CF = 0;
        ZF = 0;
        IF = 1;
        DF = 0;
        BF = 0;
        VF = 0;
        NF = 0;

        startTime = System.nanoTime();
        benchmarkTime = System.nanoTime();
        currentCycle = 0;
        running = true;

        run();
    }

    void stop() {
        running = false;
    }

    private void run() {
        while (running) {

            if (LOGGER.isLoggable(Level.FINEST))
                LOGGER.finest(String.format(
                        "%04x                                            A:%02x X:%02x Y:%02x P:%02x SP:%02x CPUC:%d",
                        PC, A, X, Y, getStatus(), S, currentCycle).toUpperCase());

            byte opCode = getByte(PC);
            Instruction instruction = instructions[btoi(opCode)];
            PC++;

            int addr = instruction.addressingMode.call(instruction.writes);
            instruction.instruction.call(addr);
        }

        double elapsedTime = (System.nanoTime() - startTime) / 1e+9;
        LOGGER.info(String.format("Total avg clock rate: %.2fMHz / 1.79MHz", currentCycle / elapsedTime / 1e+6));
    }

    /**
     * Cycles the CPU. Will block until the cycle lasted long enough.
     */
    private void cycle(int addr, boolean write) {
        // TODO: Clock rate.
        // TODO: Other cycle things

        if (LOGGER.isLoggable(Level.FINEST))
            LOGGER.finest(String.format("      %s     $%04x", write ? "WRITE" : "READ ", addr).toUpperCase());

        benchmarkCycles++;
        if (System.nanoTime() - benchmarkTime >= 1e+9) {
            LOGGER.info(String.format("Clock rate: %.2fMHz / 1.79MHz", benchmarkCycles / 1e+6));
            benchmarkCycles = 0;
            benchmarkTime = System.nanoTime();
        }

        benchmarkCycles++;
        if (System.nanoTime() - benchmarkTime >= 1e+9){
            System.out.println(String.format("Clock rate: %.2fMHz / 1.79MHz", (float)benchmarkCycles / 1e+6));
            benchmarkCycles = 0;
            benchmarkTime = System.nanoTime();
        }

        currentCycle++;
    }

    // region Instruction Creators

    private Instruction rInstr(IInstruction instruction) {
        return instr(instruction, this::implied, false, true);
    }

    private Instruction rInstr(IInstruction instruction, boolean official) {
        return instr(instruction, this::implied, false, official);
    }

    private Instruction rInstr(IInstruction instruction, IAddressingMode mode) {
        return instr(instruction, mode, false, true);
    }

    private Instruction rInstr(IInstruction instruction, IAddressingMode mode, boolean official) {
        return instr(instruction, mode, false, true);
    }

    private Instruction wInstr(IInstruction instruction) {
        return instr(instruction, this::implied, true, true);
    }

    private Instruction wInstr(IInstruction instruction, IAddressingMode mode) {
        return instr(instruction, mode, true, true);
    }

    private Instruction wInstr(IInstruction instruction, IAddressingMode mode, boolean official) {
        return instr(instruction, mode, true, true);
    }

    private Instruction instr(IInstruction instruction, IAddressingMode mode, boolean writes, boolean official) {
        return new Instruction(instruction, mode, writes, official);
    }

    // endregion

    private void setupInstructions() {
        instructions[0x00] = rInstr(this::brk);
        instructions[0x01] = rInstr(this::ora, this::indirectX);
        instructions[0x02] = rInstr(this::stp, false);
        instructions[0x03] = wInstr(this::slo, this::indirectX, false);
        instructions[0x04] = rInstr(this::skb, this::zeroPage, false);
        instructions[0x05] = rInstr(this::ora, this::zeroPage);
        instructions[0x06] = wInstr(this::asl, this::zeroPage);
        instructions[0x07] = wInstr(this::slo, this::zeroPage, false);
        instructions[0x08] = rInstr(this::php);
        instructions[0x09] = rInstr(this::ora, this::immediate);
        instructions[0x0a] = wInstr(this::asl, this::accumulator);
        instructions[0x0b] = rInstr(this::anc, this::immediate, false);
        instructions[0x0c] = rInstr(this::skb, this::absolute, false);
        instructions[0x0d] = rInstr(this::ora, this::absolute);
        instructions[0x0e] = wInstr(this::asl, this::absolute);
        instructions[0x0f] = wInstr(this::slo, this::absolute, false);

        instructions[0x10] = rInstr(this::bpl, this::relative);
        instructions[0x11] = rInstr(this::ora, this::indirectY);
        instructions[0x12] = rInstr(this::stp, false);
        instructions[0x13] = wInstr(this::slo, this::indirectY, false);
        instructions[0x14] = rInstr(this::skb, this::zeroPageX, false);
        instructions[0x15] = rInstr(this::ora, this::zeroPageX);
        instructions[0x16] = wInstr(this::asl, this::zeroPageX);
        instructions[0x17] = wInstr(this::slo, this::zeroPageX, false);
        instructions[0x18] = rInstr(this::clc);
        instructions[0x19] = rInstr(this::ora, this::absoluteY);
        instructions[0x1a] = rInstr(this::nop, false);
        instructions[0x1b] = wInstr(this::slo, this::absoluteY, false);
        instructions[0x1c] = rInstr(this::skb, this::absoluteX, false);
        instructions[0x1d] = rInstr(this::ora, this::absoluteX);
        instructions[0x1e] = wInstr(this::asl, this::absoluteX);
        instructions[0x1f] = wInstr(this::slo, this::absoluteX, false);

        instructions[0x20] = rInstr(this::jsr, this::absoluteJsr);
        instructions[0x21] = rInstr(this::and, this::indirectX);
        instructions[0x22] = rInstr(this::stp, false);
        instructions[0x23] = wInstr(this::rla, this::indirectX, false);
        instructions[0x24] = rInstr(this::bit, this::zeroPage);
        instructions[0x25] = rInstr(this::and, this::zeroPage);
        instructions[0x26] = wInstr(this::rol, this::zeroPage);
        instructions[0x27] = wInstr(this::rla, this::zeroPage, false);
        instructions[0x28] = rInstr(this::plp);
        instructions[0x29] = rInstr(this::and, this::immediate);
        instructions[0x2a] = wInstr(this::rol, this::accumulator);
        instructions[0x2b] = rInstr(this::anc, this::immediate, false);
        instructions[0x2c] = rInstr(this::bit, this::absolute);
        instructions[0x2d] = rInstr(this::and, this::absolute);
        instructions[0x2e] = wInstr(this::rol, this::absolute);
        instructions[0x2f] = wInstr(this::rla, this::absolute, false);

        instructions[0x30] = rInstr(this::bmi, this::relative);
        instructions[0x31] = rInstr(this::and, this::indirectY);
        instructions[0x32] = rInstr(this::stp, false);
        instructions[0x33] = wInstr(this::rla, this::indirectY, false);
        instructions[0x34] = rInstr(this::skb, this::zeroPageX, false);
        instructions[0x35] = rInstr(this::and, this::zeroPageX);
        instructions[0x36] = wInstr(this::rol, this::zeroPageX);
        instructions[0x37] = wInstr(this::rla, this::zeroPageX, false);
        instructions[0x38] = rInstr(this::sec);
        instructions[0x39] = rInstr(this::and, this::absoluteY);
        instructions[0x3a] = rInstr(this::nop, false);
        instructions[0x3b] = wInstr(this::rla, this::absoluteY, false);
        instructions[0x3c] = rInstr(this::skb, this::absoluteX, false);
        instructions[0x3d] = rInstr(this::and, this::absoluteX);
        instructions[0x3e] = wInstr(this::rol, this::absoluteX);
        instructions[0x3f] = wInstr(this::rla, this::absoluteX, false);

        instructions[0x40] = rInstr(this::rti);
        instructions[0x41] = rInstr(this::eor, this::indirectX);
        instructions[0x42] = rInstr(this::stp, false);
        instructions[0x43] = wInstr(this::sre, this::indirectX, false);
        instructions[0x44] = rInstr(this::skb, this::zeroPage, false);
        instructions[0x45] = rInstr(this::eor, this::zeroPage);
        instructions[0x46] = wInstr(this::lsr, this::zeroPage);
        instructions[0x47] = wInstr(this::sre, this::zeroPage, false);
        instructions[0x48] = rInstr(this::pha);
        instructions[0x49] = rInstr(this::eor, this::immediate);
        instructions[0x4a] = wInstr(this::lsr, this::accumulator);
        instructions[0x4b] = rInstr(this::alr, this::immediate, false);
        instructions[0x4c] = rInstr(this::jmp, this::absolute);
        instructions[0x4d] = rInstr(this::eor, this::absolute);
        instructions[0x4e] = wInstr(this::lsr, this::absolute);
        instructions[0x4f] = wInstr(this::sre, this::absolute, false);

        instructions[0x50] = rInstr(this::bvc, this::relative);
        instructions[0x51] = rInstr(this::eor, this::indirectY);
        instructions[0x52] = rInstr(this::stp, false);
        instructions[0x53] = wInstr(this::sre, this::indirectY, false);
        instructions[0x54] = rInstr(this::skb, this::zeroPageX, false);
        instructions[0x55] = rInstr(this::eor, this::zeroPageX);
        instructions[0x56] = wInstr(this::lsr, this::zeroPageX);
        instructions[0x57] = wInstr(this::sre, this::zeroPageX, false);
        instructions[0x58] = rInstr(this::cli);
        instructions[0x59] = rInstr(this::eor, this::absoluteY);
        instructions[0x5a] = rInstr(this::nop, false);
        instructions[0x5b] = wInstr(this::sre, this::absoluteY, false);
        instructions[0x5c] = rInstr(this::skb, this::absoluteX, false);
        instructions[0x5d] = rInstr(this::eor, this::absoluteX);
        instructions[0x5e] = wInstr(this::lsr, this::absoluteX);
        instructions[0x5f] = wInstr(this::sre, this::absoluteX, false);

        instructions[0x60] = rInstr(this::rts);
        instructions[0x61] = rInstr(this::adc, this::indirectX);
        instructions[0x62] = rInstr(this::stp, false);
        instructions[0x63] = wInstr(this::rra, this::indirectX, false);
        instructions[0x64] = rInstr(this::skb, this::zeroPage, false);
        instructions[0x65] = rInstr(this::adc, this::zeroPage);
        instructions[0x66] = wInstr(this::ror, this::zeroPage);
        instructions[0x67] = wInstr(this::rra, this::zeroPage, false);
        instructions[0x68] = rInstr(this::pla);
        instructions[0x69] = rInstr(this::adc, this::immediate);
        instructions[0x6a] = wInstr(this::ror, this::accumulator);
        instructions[0x6b] = rInstr(this::arr, this::immediate, false);
        instructions[0x6c] = rInstr(this::jmp, this::indirect);
        instructions[0x6d] = rInstr(this::adc, this::absolute);
        instructions[0x6e] = wInstr(this::ror, this::absolute);
        instructions[0x6f] = wInstr(this::rra, this::absolute, false);

        instructions[0x70] = rInstr(this::bvs, this::relative);
        instructions[0x71] = rInstr(this::adc, this::indirectY);
        instructions[0x72] = rInstr(this::stp, false);
        instructions[0x73] = wInstr(this::rra, this::indirectY, false);
        instructions[0x74] = rInstr(this::skb, this::zeroPageX, false);
        instructions[0x75] = rInstr(this::adc, this::zeroPageX);
        instructions[0x76] = wInstr(this::ror, this::zeroPageX);
        instructions[0x77] = wInstr(this::rra, this::zeroPageX, false);
        instructions[0x78] = rInstr(this::sei);
        instructions[0x79] = rInstr(this::adc, this::absoluteY);
        instructions[0x7a] = rInstr(this::nop, false);
        instructions[0x7b] = wInstr(this::rra, this::absoluteY, false);
        instructions[0x7c] = rInstr(this::skb, this::absoluteX, false);
        instructions[0x7d] = rInstr(this::adc, this::absoluteX);
        instructions[0x7e] = wInstr(this::ror, this::absoluteX);
        instructions[0x7f] = wInstr(this::rra, this::absoluteX, false);

        instructions[0x80] = rInstr(this::skb, this::immediate, false);
        instructions[0x81] = wInstr(this::sta, this::indirectX);
        instructions[0x82] = rInstr(this::skb, this::immediate, false);
        instructions[0x83] = wInstr(this::sax, this::indirectX, false);
        instructions[0x84] = wInstr(this::sty, this::zeroPage);
        instructions[0x85] = wInstr(this::sta, this::zeroPage);
        instructions[0x86] = wInstr(this::stx, this::zeroPage);
        instructions[0x87] = wInstr(this::sax, this::zeroPage, false);
        instructions[0x88] = rInstr(this::dey);
        instructions[0x89] = rInstr(this::skb, this::immediate, false);
        instructions[0x8a] = rInstr(this::txa);
        instructions[0x8b] = rInstr(this::xaa, this::immediate, false);
        instructions[0x8c] = wInstr(this::sty, this::absolute);
        instructions[0x8d] = wInstr(this::sta, this::absolute);
        instructions[0x8e] = wInstr(this::stx, this::absolute);
        instructions[0x8f] = wInstr(this::sax, this::absolute, false);

        instructions[0x90] = rInstr(this::bcc, this::relative);
        instructions[0x91] = wInstr(this::sta, this::indirectY);
        instructions[0x92] = rInstr(this::stp, false);
        instructions[0x93] = wInstr(this::sha, this::indirectY, false);
        instructions[0x94] = wInstr(this::sty, this::zeroPageX);
        instructions[0x95] = wInstr(this::sta, this::zeroPageX);
        instructions[0x96] = wInstr(this::stx, this::zeroPageY);
        instructions[0x97] = wInstr(this::sax, this::zeroPageY, false);
        instructions[0x98] = rInstr(this::tya);
        instructions[0x99] = wInstr(this::sta, this::absoluteY);
        instructions[0x9a] = rInstr(this::txs);
        instructions[0x9b] = wInstr(this::shs, this::absoluteY, false);
        instructions[0x9c] = wInstr(this::shy, this::absoluteX, false);
        instructions[0x9d] = wInstr(this::sta, this::absoluteX);
        instructions[0x9e] = wInstr(this::shx, this::absoluteY, false);
        instructions[0x9f] = wInstr(this::sha, this::absoluteY, false);

        instructions[0xa0] = rInstr(this::ldy, this::immediate);
        instructions[0xa1] = rInstr(this::lda, this::indirectX);
        instructions[0xa2] = rInstr(this::ldx, this::immediate);
        instructions[0xa3] = rInstr(this::lax, this::indirectX, false);
        instructions[0xa4] = rInstr(this::ldy, this::zeroPage);
        instructions[0xa5] = rInstr(this::lda, this::zeroPage);
        instructions[0xa6] = rInstr(this::ldx, this::zeroPage);
        instructions[0xa7] = rInstr(this::lax, this::zeroPage, false);
        instructions[0xa8] = rInstr(this::tay);
        instructions[0xa9] = rInstr(this::lda, this::immediate);
        instructions[0xaa] = rInstr(this::tax);
        instructions[0xab] = rInstr(this::lax, this::immediate, false);
        instructions[0xac] = rInstr(this::ldy, this::absolute);
        instructions[0xad] = rInstr(this::lda, this::absolute);
        instructions[0xae] = rInstr(this::ldx, this::absolute);
        instructions[0xaf] = rInstr(this::lax, this::absolute, false);

        instructions[0xb0] = rInstr(this::bcs, this::relative);
        instructions[0xb1] = rInstr(this::lda, this::indirectY);
        instructions[0xb2] = rInstr(this::stp, false);
        instructions[0xb3] = rInstr(this::lax, this::indirectY, false);
        instructions[0xb4] = rInstr(this::ldy, this::zeroPageX);
        instructions[0xb5] = rInstr(this::lda, this::zeroPageX);
        instructions[0xb6] = rInstr(this::ldx, this::zeroPageY);
        instructions[0xb7] = rInstr(this::lax, this::zeroPageY, false);
        instructions[0xb8] = rInstr(this::clv);
        instructions[0xb9] = rInstr(this::lda, this::absoluteY);
        instructions[0xba] = rInstr(this::tsx);
        instructions[0xbb] = rInstr(this::las, this::absoluteY, false);
        instructions[0xbc] = rInstr(this::ldy, this::absoluteX);
        instructions[0xbd] = rInstr(this::lda, this::absoluteX);
        instructions[0xbe] = rInstr(this::ldx, this::absoluteY);
        instructions[0xbf] = rInstr(this::lax, this::absoluteY, false);

        instructions[0xc0] = rInstr(this::cpy, this::immediate);
        instructions[0xc1] = rInstr(this::cmp, this::indirectX);
        instructions[0xc2] = rInstr(this::skb, this::immediate, false);
        instructions[0xc3] = wInstr(this::dcp, this::indirectX, false);
        instructions[0xc4] = rInstr(this::cpy, this::zeroPage);
        instructions[0xc5] = rInstr(this::cmp, this::zeroPage);
        instructions[0xc6] = wInstr(this::dec, this::zeroPage);
        instructions[0xc7] = wInstr(this::dcp, this::zeroPage, false);
        instructions[0xc8] = rInstr(this::iny);
        instructions[0xc9] = rInstr(this::cmp, this::immediate);
        instructions[0xca] = rInstr(this::dex);
        instructions[0xcb] = rInstr(this::axs, this::immediate, false);
        instructions[0xcc] = rInstr(this::cpy, this::absolute);
        instructions[0xcd] = rInstr(this::cmp, this::absolute);
        instructions[0xce] = wInstr(this::dec, this::absolute);
        instructions[0xcf] = wInstr(this::dcp, this::absolute, false);

        instructions[0xd0] = rInstr(this::bne, this::relative);
        instructions[0xd1] = rInstr(this::cmp, this::indirectY);
        instructions[0xd2] = rInstr(this::stp, false);
        instructions[0xd3] = wInstr(this::dcp, this::indirectY, false);
        instructions[0xd4] = rInstr(this::skb, this::zeroPageX, false);
        instructions[0xd5] = rInstr(this::cmp, this::zeroPageX);
        instructions[0xd6] = wInstr(this::dec, this::zeroPageX);
        instructions[0xd7] = wInstr(this::dcp, this::zeroPageX, false);
        instructions[0xd8] = rInstr(this::cld);
        instructions[0xd9] = rInstr(this::cmp, this::absoluteY);
        instructions[0xda] = rInstr(this::nop, false);
        instructions[0xdb] = wInstr(this::dcp, this::absoluteY, false);
        instructions[0xdc] = rInstr(this::skb, this::absoluteX, false);
        instructions[0xdd] = rInstr(this::cmp, this::absoluteX);
        instructions[0xde] = wInstr(this::dec, this::absoluteX);
        instructions[0xdf] = wInstr(this::dcp, this::absoluteX, false);

        instructions[0xe0] = rInstr(this::cpx, this::immediate);
        instructions[0xe1] = rInstr(this::sbc, this::indirectX);
        instructions[0xe2] = rInstr(this::skb, this::immediate, false);
        instructions[0xe3] = wInstr(this::isc, this::indirectX, false);
        instructions[0xe4] = rInstr(this::cpx, this::zeroPage);
        instructions[0xe5] = rInstr(this::sbc, this::zeroPage);
        instructions[0xe6] = wInstr(this::inc, this::zeroPage);
        instructions[0xe7] = wInstr(this::isc, this::zeroPage, false);
        instructions[0xe8] = rInstr(this::inx);
        instructions[0xe9] = rInstr(this::sbc, this::immediate);
        instructions[0xea] = rInstr(this::nop);
        instructions[0xeb] = rInstr(this::sbc, this::immediate, false);
        instructions[0xec] = rInstr(this::cpx, this::absolute);
        instructions[0xed] = rInstr(this::sbc, this::absolute);
        instructions[0xee] = wInstr(this::inc, this::absolute);
        instructions[0xef] = wInstr(this::isc, this::absolute, false);

        instructions[0xf0] = rInstr(this::beq, this::relative);
        instructions[0xf1] = rInstr(this::sbc, this::indirectY);
        instructions[0xf2] = rInstr(this::stp, false);
        instructions[0xf3] = wInstr(this::isc, this::indirectY, false);
        instructions[0xf4] = rInstr(this::skb, this::zeroPageX, false);
        instructions[0xf5] = rInstr(this::sbc, this::zeroPageX);
        instructions[0xf6] = wInstr(this::inc, this::zeroPageX);
        instructions[0xf7] = wInstr(this::isc, this::zeroPageX, false);
        instructions[0xf8] = rInstr(this::sed);
        instructions[0xf9] = rInstr(this::sbc, this::absoluteY);
        instructions[0xfa] = rInstr(this::nop, false);
        instructions[0xfb] = wInstr(this::isc, this::absoluteY, false);
        instructions[0xfc] = rInstr(this::skb, this::absoluteX, false);
        instructions[0xfd] = rInstr(this::sbc, this::absoluteX);
        instructions[0xfe] = wInstr(this::inc, this::absoluteX);
        instructions[0xff] = wInstr(this::isc, this::absoluteX, false);
    }

    // region Addressing Modes

    private int accumulator(boolean writes) {
        getByte(PC);
        return -1;
    }

    private int implied(boolean writes) {
        getByte(PC);
        return -1;
    }

    private int immediate(boolean writes) {
        PC++;
        return PC - 1;
    }

    private int absolute(boolean writes) {
        PC += 2;
        return stoi(getShort(PC - 2));
    }

    private int absoluteJsr(boolean writes) {
        PC++;
        return btoi(getByte(PC - 1));
    }

    private int absoluteX(boolean writes) {
        PC += 2;
        int addr = stoi(getShort(PC - 2));
        if (btoi(addr) + btoi(X) > 0xff || writes)
            getByte((addr & 0xff00) | btoi(btoi(addr) + btoi(X)));
        return stoi(addr + btoi(X));
    }

    private int absoluteY(boolean writes) {
        PC += 2;
        int addr = stoi(getShort(PC - 2));
        if (btoi(addr) + btoi(Y) > 0xff || writes)
            getByte((addr & 0xff00) | btoi(btoi(addr) + btoi(Y)));
        return stoi(addr + btoi(Y));
    }

    private int zeroPage(boolean writes) {
        PC++;
        return btoi(getByte(PC - 1));
    }

    private int zeroPageX(boolean writes) {
        PC++;
        int addr = btoi(getByte(PC - 1));
        getByte(addr);
        return btoi(addr + btoi(X));
    }

    private int zeroPageY(boolean writes) {
        PC++;
        int addr = btoi(getByte(PC - 1));
        getByte(addr);
        return btoi(addr + btoi(Y));
    }

    private int relative(boolean writes) {
        PC++;
        return PC - 1;
    }

    private int indirect(boolean writes) {
        PC += 2;
        return stoi(getPagedShort(stoi(getShort(PC - 2))));
    }

    private int indirectX(boolean writes) {
        PC++;
        int addr = btoi(getByte(PC - 1));
        getByte(addr);
        addr = btoi(addr + btoi(X));
        return stoi(getPagedShort(addr));
    }

    private int indirectY(boolean writes) {
        PC++;
        int addr = btoi(getByte(PC - 1));
        addr = stoi(getPagedShort(addr));
        if (btoi(addr) + btoi(Y) > 0xff || writes)
            getByte((addr & 0xff00) | btoi(btoi(addr) + btoi(Y)));
        return stoi(addr + btoi(Y));
    }

    // endregion

    // region Instruction Helpers

    private int btoi(byte b) {
        return b & 0xff;
    }

    private int btoi(int b) {
        return b & 0xff;
    }

    private int stoi(short s) {
        return s & 0xffff;
    }

    private int stoi(int s) {
        return s & 0xffff;
    }

    private void setByte(int addr, byte b) {
        mem.set(addr, b);
        cycle(addr, true);
    }

    private byte getByte(int addr) {
        byte b = mem.get(addr);
        cycle(addr, false);
        return b;
    }

    private short getShort(int addr) {
        return (short) ((getByte(addr) & 0x00ff) | ((getByte(addr + 1) << 8) & 0xff00));
    }

    private short getPagedShort(int addr) {
        int lo = addr;
        int hi = (addr & 0xff00) | (addr + 1) & 0xff;
        return (short) ((getByte(lo) & 0x00ff) | (getByte(hi) << 8 & 0xff00));
    }

    private void setZN(byte b) {
        ZF = (byte) (b == 0 ? 1 : 0);
        NF = (byte) (b < 0 ? 1 : 0);
    }

    private void branch(boolean branch, int addr) {
        byte offset = getByte(addr);
        if (branch) {
            getByte(PC);
            if ((PC + offset) >>> 8 != PC >>> 8)
                getByte((PC & 0xff00) | ((PC + offset) & 0x00ff));
            PC += offset;
        }
    }

    private int compare(byte a, byte b) {
        int cmp = btoi(a) - btoi(b);
        CF = (byte) (btoi(a) >= btoi(b) ? 1 : 0);
        ZF = (byte) (btoi(a) == btoi(b) ? 1 : 0);
        NF = (byte) ((cmp >>> 7 & 0x01) == 1 ? 1 : 0);
        return 0;
    }

    private int accumulatorInstr(int cf, int bOld, int bNew, int addr) {
        CF = (byte) (cf);

        if (addr == -1) {
            A = (byte) bNew;
        } else {
            setByte(addr, (byte) bOld);
            setByte(addr, (byte) bNew);
        }

        setZN((byte) bNew);
        return 2;
    }

    private void pushByte(byte b) {
        setByte(0x0100 | btoi(S), b);
        S = (byte) (btoi(S) - 1);
    }

    private void pushShort(short s) {
        pushByte((byte) (s >>> 8));
        pushByte((byte) (s & 0xff));
    }

    private byte popByte() {
        return popByte(true);
    }

    private byte popByte(boolean cycle) {
        if (cycle)
            getByte(0x0100 | btoi(S));

        S = (byte) (btoi(S) + 1);
        return getByte(0x0100 | btoi(S));
    }

    private short popShort() {
        return popShort(true);
    }

    private short popShort(boolean cycle) {
        if (cycle)
            getByte(0x0100 | btoi(S));

        byte lo = popByte(false);
        byte hi = popByte(false);
        return (short) ((btoi(hi) << 8) | btoi(lo));
    }

    private byte getStatus() {
        byte status = 0x20;

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

    private void setStatus(byte b) {
        NF = (byte) (b >>> 7 & 0x01);
        VF = (byte) (b >>> 6 & 0x01);
        DF = (byte) (b >>> 3 & 0x01);
        IF = (byte) (b >>> 2 & 0x01);
        ZF = (byte) (b >>> 1 & 0x01);
        CF = (byte) (b & 0x01);
    }

    // endregion

    // region Official Instructions

    /**
     * Add with Carry
     */
    private void adc(int addr) {
        adc(getByte(addr));
    }

    /**
     * Add with Carry op
     */
    private void adc(byte op) {
        int tempA = btoi(A) + btoi(op) + CF;
        int tempASigned = A + op + CF;
        CF = (byte) (tempA > 0xff ? 1 : 0);
        VF = (byte) (tempASigned < -128 || tempASigned > 127 ? 1 : 0);
        A = (byte) tempA;
        setZN(A);
    }

    /**
     * Logical AND
     */
    private void and(int addr) {
        A &= btoi(getByte(addr));
        setZN(A);
    }

    /**
     * Arithmetic Shift Left
     */
    private void asl(int addr) {
        byte op = addr == -1 ? A : getByte(addr);
        accumulatorInstr(btoi(op) >>> 7, op, btoi(op) << 1, addr);
    }

    /**
     * Branch if Carry Clear
     */
    private void bcc(int addr) {
        branch(CF == 0, addr);
    }

    /**
     * Branch if Carry Set
     */
    private void bcs(int addr) {
        branch(CF == 1, addr);
    }

    /**
     * Branch if Equal
     */
    private void beq(int addr) {
        branch(ZF == 1, addr);
    }

    /**
     * Bit Test
     */
    private void bit(int addr) {
        byte op = getByte(addr);
        ZF = (byte) ((A & op) == 0 ? 1 : 0);
        VF = (byte) (op >>> 6 & 0x01);
        NF = (byte) (op >>> 7 & 0x01);
    }

    /**
     * Branch if Minus
     */
    private void bmi(int addr) {
        branch(NF == 1, addr);
    }

    /**
     * Branch if Not Equal
     */
    private void bne(int addr) {
        branch(ZF == 0, addr);
    }

    /**
     * Branch if Positive
     */
    private void bpl(int addr) {
        branch(NF == 0, addr);
    }

    /**
     * Force Interrupt
     */
    private void brk(int addr) {
        PC++;
        BF = 1;
        pushShort((short) PC);
        pushByte(getStatus());
        PC = stoi(getShort(0xfffe));
        BF = 0;
        // TODO: actually break in exec
        // TODO: test this
    }

    /**
     * Branch if Overflow Clear
     */
    private void bvc(int addr) {
        branch(VF == 0, addr);
    }

    /**
     * Branch if Overflow Set
     */
    private void bvs(int addr) {
        branch(VF == 1, addr);
    }

    /**
     * Clear Carry Flag
     */
    private void clc(int addr) {
        CF = 0;
    }

    /**
     * Clear Decimal Mode
     */
    private void cld(int addr) {
        DF = 0;
    }

    /**
     * Clear Interrupt Disable
     */
    private void cli(int addr) {
        IF = 0;
    }

    /**
     * Clear Overflow Flag
     */
    private void clv(int addr) {
        VF = 0;
    }

    /**
     * Compare
     */
    private void cmp(int addr) {
        cmp(getByte(addr));
    }

    /**
     * Compare op
     */
    private void cmp(byte op) {
        compare(A, op);
    }

    /**
     * Compare X Register
     */
    private void cpx(int addr) {
        compare(X, getByte(addr));
    }

    /**
     * Compare Y Register
     */
    private void cpy(int addr) {
        compare(Y, getByte(addr));
    }

    /**
     * Decrement Memory
     */
    private void dec(int addr) {
        byte op = getByte(addr);
        setByte(addr, op);
        op = (byte) (btoi(op) - 1);
        setZN(op);
        setByte(addr, op);
    }

    /**
     * Decrement X Register
     */
    private void dex(int addr) {
        X = (byte) (btoi(X) - 1);
        setZN(X);
    }

    /**
     * Decrement Y Register
     */
    private void dey(int addr) {
        Y = (byte) (btoi(Y) - 1);
        setZN(Y);
    }

    /**
     * Exclusive OR
     */
    private void eor(int addr) {
        A ^= btoi(getByte(addr));
        setZN(A);
    }

    /**
     * Increment Memory
     */
    private void inc(int addr) {
        byte op = getByte(addr);
        setByte(addr, op);
        op = (byte) (btoi(op) + 1);
        setZN(op);
        setByte(addr, op);
    }

    /**
     * Increment X Register
     */
    private void inx(int addr) {
        X = (byte) (btoi(X) + 1);
        setZN(X);
    }

    /**
     * Increment Y Register
     */
    private void iny(int addr) {
        Y = (byte) (btoi(Y) + 1);
        setZN(Y);
    }

    /**
     * Jump
     */
    private void jmp(int addr) {
        PC = addr;
    }

    /**
     * Jump to Subroutine
     */
    private void jsr(int addr) {
        getByte(0x0100 | btoi(S));
        pushShort((short) (PC));
        byte hi = getByte(PC);
        PC = (btoi(hi) << 8) | btoi(addr);
    }

    /**
     * Load Accumulator
     */
    private void lda(int addr) {
        A = getByte(addr);
        setZN(A);
    }

    /**
     * Load X Register
     */
    private void ldx(int addr) {
        X = getByte(addr);
        setZN(X);
    }

    /**
     * Load Y Register
     */
    private void ldy(int addr) {
        Y = getByte(addr);
        setZN(Y);
    }

    /**
     * Logical Shift Right
     */
    private void lsr(int addr) {
        byte op = addr == -1 ? A : getByte(addr);
        accumulatorInstr(op & 0x01, op, btoi(op) >>> 1, addr);
    }

    /**
     * No Operation
     */
    private void nop(int addr) {
    }

    /**
     * Logical Inclusive OR
     */
    private void ora(int addr) {
        A |= btoi(getByte(addr));
        setZN(A);
    }

    /**
     * Push Accumulator
     */
    private void pha(int addr) {
        pushByte(A);
    }

    /**
     * Push Processor Status
     */
    private void php(int addr) {
        BF = 1;
        pushByte(getStatus());
        BF = 0;
    }

    /**
     * Pull Accumulator
     */
    private void pla(int addr) {
        A = popByte();
        setZN(A);
    }

    /**
     * Pull Processor Status
     */
    private void plp(int addr) {
        setStatus(popByte());
    }

    /**
     * Rotate Left
     */
    private void rol(int addr) {
        byte op = addr == -1 ? A : getByte(addr);
        accumulatorInstr(btoi(op) >>> 7, op, btoi(op) << 1 | CF, addr);
    }

    /**
     * Rotate Right
     */
    private void ror(int addr) {
        byte op = addr == -1 ? A : getByte(addr);
        accumulatorInstr(op & 0x01, op, btoi(op) >>> 1 | CF << 7, addr);
    }

    /**
     * Return from Interrupt
     */
    private void rti(int addr) {
        setStatus(popByte());
        PC = stoi(popShort(false));
    }

    /**
     * Return from Subroutine
     */
    private void rts(int addr) {
        PC = stoi(popShort());
        getByte(PC);
        PC++;
    }

    /**
     * Subtract with Carry
     */
    private void sbc(int addr) {
        sbc(getByte(addr));
    }

    /**
     * Subtract with Carry op
     */
    private void sbc(byte op) {
        int tempA = btoi(A) - btoi(op) - (1 - CF);
        int tempASigned = A - op - (1 - CF);
        CF = (byte) (tempA < 0 ? 0 : 1);
        VF = (byte) (tempASigned < -128 || tempASigned > 127 ? 1 : 0);
        A = (byte) tempA;
        setZN(A);
    }

    /**
     * Set Carry Flag
     */
    private void sec(int addr) {
        CF = 1;
    }

    /**
     * Set Decimal Flag
     */
    private void sed(int addr) {
        DF = 1;
    }

    /**
     * Set Interrupt Disable
     */
    private void sei(int addr) {
        IF = 1;
    }

    /**
     * Store Accumulator
     */
    private void sta(int addr) {
        setByte(addr, A);
    }

    /**
     * Store X Register
     */
    private void stx(int addr) {
        setByte(addr, X);
    }

    /**
     * Store Y Register
     */
    private void sty(int addr) {
        setByte(addr, Y);
    }

    /**
     * Transfer Accumulator to X
     */
    private void tax(int addr) {
        X = A;
        setZN(X);
    }

    /**
     * Transfer Accumulator to Y
     */
    private void tay(int addr) {
        Y = A;
        setZN(Y);
    }

    /**
     * Transfer Stack Pointer to X
     */
    private void tsx(int addr) {
        X = S;
        setZN(X);
    }

    /**
     * Transfer X to Accumulator
     */
    private void txa(int addr) {
        A = X;
        setZN(A);
    }

    /**
     * Transfer X to Stack Pointer
     */
    private void txs(int addr) {
        S = X;
    }

    /**
     * Transfer Y to Accumulator
     */
    private void tya(int addr) {
        A = Y;
        setZN(Y);
    }

    // endregion

    // region Unofficial Instructions

    // TODO: Test

    /**
     * AND then LSR
     */
    private void alr(int addr) {
        and(addr);
        lsr(-1);
    }

    /**
     * AND with Carry
     */
    private void anc(int addr) {
        and(addr);
        CF = NF;
    }

    /**
     * AND then ROR with Overflow
     */
    private void arr(int addr) {
        and(addr);
        ror(-1);
        CF = (byte) ((btoi(A) >>> 6) & 0x01);
        VF = (byte) (CF ^ ((btoi(A) >>> 5) & 0x01));
    }

    /**
     * AND A to X then subtract Immediate
     */
    private void axs(int addr) {
        X &= btoi(A);
        X = (byte) (btoi(X) - btoi(getByte(addr)));
        setZN(X);
        CF = (byte) (X < 0 ? 0 : 1);
    }

    /**
     * DEC then CMP
     */
    private void dcp(int addr) {
        dec(addr);
        cmp(mem.get(addr));
    }

    /**
     * INC then SBC
     */
    private void isc(int addr) {
        inc(addr);
        sbc(mem.get(addr));
    }

    /**
     * AND with Stack to Stack and X and Accumulator
     */
    private void las(int addr) {
        byte op = getByte(addr);
        S &= btoi(op);
        A = S;
        X = S;
        setZN(S);
    }

    /**
     * Load Accumulator and X Register
     */
    private void lax(int addr) {
        lda(addr);
        tax(addr);
    }

    /**
     * ROL then AND
     */
    private void rla(int addr) {
        rol(addr);
        A &= btoi(mem.get(addr));
        setZN(A);
    }

    /**
     * ROR then ADC
     */
    private void rra(int addr) {
        ror(addr);
        adc(mem.get(addr));
    }

    /**
     * Store AND X
     */
    private void sax(int addr) {
        setByte(addr, (byte) (btoi(A) & btoi(X)));
    }

    /**
     * Store A & X & (ADDR_HI + 1)
     */
    private void sha(int addr) {
        setByte(addr, (byte) (btoi(A) & btoi(X) & ((addr >>> 8) + 1)));
    }

    /**
     * SHA and TXS, where X is replaced by (A & X)
     */
    private void shs(int addr) {
        S = (byte) (btoi(X) & btoi(A));
        setByte(addr, (byte) (btoi(S) & ((addr >>> 8) + 1)));
    }

    /**
     * Store X & (ADDR_HI + 1)
     */
    private void shx(int addr) {
        setByte(addr, (byte) (btoi(X) & ((addr >>> 8) + 1)));
    }

    /**
     * Store Y & (ADDR_HI + 1)
     */
    private void shy(int addr) {
        setByte(addr, (byte) (btoi(Y) & ((addr >>> 8) + 1)));
    }

    /**
     * NOP with Read
     */
    private void skb(int addr) {
        getByte(addr);
    }

    /**
     * ASL then ORA
     */
    private void slo(int addr) {
        asl(addr);
        A |= btoi(mem.get(addr));
        setZN(A);
    }

    /**
     * LSR then EOR
     */
    private void sre(int addr) {
        lsr(addr);
        A ^= btoi(mem.get(addr));
        setZN(A);
    }

    /**
     * Stop the CPU
     */
    private void stp(int addr) {
        stop();
    }

    /**
     * Does some weird magic stuff apparently that I'm too dumb to understand
     */
    private void xaa(int addr) {
        getByte(addr);
    }

    // endregion

    private static final class Instruction {
        private IInstruction instruction;
        private IAddressingMode addressingMode;
        private boolean writes;
        private boolean official;

        Instruction(IInstruction instruction, IAddressingMode addressingMode, boolean writes, boolean official) {
            this.instruction = instruction;
            this.addressingMode = addressingMode;
            this.writes = writes;
            this.official = official;
        }
    }

    private interface IAddressingMode {
        /**
         * Calls the addressing mode. Returns the address.
         */
        int call(boolean writes);
    }

    private interface IInstruction {
        /**
         * Calls the instruction.
         */
        void call(int addr);
    }
}

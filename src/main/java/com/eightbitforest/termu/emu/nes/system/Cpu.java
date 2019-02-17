package main.java.com.eightbitforest.termu.emu.nes.system;

import main.java.com.eightbitforest.termu.emu.nes.rom.NesRom;
import main.java.com.eightbitforest.termu.emu.nes.rom.mappers.Mapper;

public class Cpu {
    // Registers
    private int PC;
    private byte S;
    private byte A;
    private byte X;
    private byte Y;
    private byte P;

    private short nmiVector;
    private short resetVector;
    private short brkVector;

    private Mem mem;

    public Cpu(Mem mem) {
        this.mem = mem;
    }

    public void start() {
        nmiVector = mem.getShort(0xfffa);
        resetVector = mem.getShort(0xfffc);
        brkVector = mem.getShort(0xfffe);

        // TODO: Set PC to reset vector. Using 0xC000 since PPU is not implemented yet
        PC = 0xc000;
        S = (byte) 0xfd;
        A = (byte) 0x00;
        X = (byte) 0x00;
        Y = (byte) 0x00;
        P = (byte) 0x34;

        run();
    }

    private void run() {
        // TODO: Timing.
    }
}

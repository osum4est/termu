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

    // Memory
    private byte[] internalRam;
    private byte[] ppuRegs; // TODO: Move to ppu class
    private byte[] apuIoRegs;
    private byte[] apuIoTest;

    private NesRom cartridge;
    private Mapper cartridgeMapper;

    public void start(NesRom cartridge) {
        internalRam = new byte[0x0800];
        ppuRegs = new byte[0x0008];
        apuIoRegs = new byte[0x0018];
        apuIoTest = new byte[0x0008];

        this.cartridge = cartridge;
        cartridgeMapper = cartridge.getMapper();
    }

    private byte memget(int addr) {
        if (addr < 0x2000)
            return internalRam[addr % internalRam.length];

        if (addr < 0x4000)
            return ppuRegs[(addr - 0x2000) % ppuRegs.length];

        if (addr < 0x4018)
            return apuIoRegs[addr - 0x4000];

        if (addr < 0x4020)
            return apuIoTest[addr - 0x4018];

        return cartridgeMapper.memget(addr);
    }
}

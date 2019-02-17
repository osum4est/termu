package main.java.com.eightbitforest.termu.emu.nes.system;

import main.java.com.eightbitforest.termu.emu.nes.rom.NesRom;

public class Nes {

    private NesRom cartridge;
    private Cpu cpu;
    private Mem mem;

    public void insertCartridge(NesRom rom) {
        this.cartridge = rom;
    }

    public void start() {
        // TODO: Thread this
        mem = new Mem(cartridge);
        cpu = new Cpu(mem);
        cpu.start();
    }

    public void stop() {
    }
}

package main.java.com.eightbitforest.termu.emu.nes.system;

import main.java.com.eightbitforest.termu.emu.nes.rom.NesRom;

public class Nes {

    private NesRom cartridge;
    private Cpu cpu;
    private Ppu ppu;
    private Mem mem;
    private Clock clock;

    public void insertCartridge(NesRom rom) {
        this.cartridge = rom;
    }

    public void start() {
        // TODO: Thread this
        mem = new Mem(cartridge);
        cpu = new Cpu(mem);
        ppu = new Ppu(cpu, mem);
        clock = new Clock(cpu, ppu);

        new Thread(() -> cpu.start()).start();
        ppu.start();
//        new Thread(() -> ppu.start()).start();

//        clock.start();
    }

    public void stop() {
    }
}

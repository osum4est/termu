package main.java.com.eightbitforest.termu.emu.nes.system;

import main.java.com.eightbitforest.termu.emu.nes.rom.NesRom;
import main.java.com.eightbitforest.termu.emu.nes.system.Cpu;

public class Nes {

    private NesRom cartridge;
    private Cpu cpu;

    public void insertCartridge(NesRom rom) {
        this.cartridge = rom;
    }

    public void start() {
        // TODO: Thread this
        cpu = new Cpu();
        cpu.start(cartridge);
    }

    public void stop() {
    }
}

package main.java.com.eightbitforest.termu.emu.nes.system;

public class Clock {
    private Cpu cpu;
    private Ppu ppu;

    private int cycle;

    Clock(Cpu cpu, Ppu ppu) {
        this.cpu = cpu;
        this.ppu = ppu;
    }

    public void start() {
        // TODO: Run this at the same speed as the ppu
        while (true) {
            tick();
        }
    }

    private void tick() {
        if (cycle == 3) {
            cycle = 0;
        }
        else {
            cycle++;
            cpu.tick();
        }

//        ppu.tick();
    }
}

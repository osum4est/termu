package main.java.com.eightbitforest.termu.emu.nes.system;

import java.util.logging.Level;
import java.util.logging.Logger;

public class Ppu {
    private final Logger LOGGER = Logger.getLogger(getClass().getName());

    private Cpu cpu;
    private Mem mem;

    private boolean running;
    private long currentCycle;

    private long startTime;
    private long benchmarkTime;
    private long benchmarkCycles;

    private volatile boolean cycling = false;

    public Ppu(Cpu cpu, Mem mem) {
        this.cpu = cpu;
        this.mem = mem;
    }

    void start() {
        // TODO: other init stuff

        startTime = System.nanoTime();
        benchmarkTime = System.nanoTime();
        currentCycle = 0;
        running = true;

        run();
    }

    void stop() {
        running = false;
    }

//    void tick() {
//        cycling = true;
//        while (cycling) { }
//    }

    private void run() {
        while (running) {
            cycle();
        }

        double elapsedTime = (System.nanoTime() - startTime) / 1e+9;
        LOGGER.info(String.format("Total avg clock rate: %.2fMHz / 5.37MHz", currentCycle / elapsedTime / 1e+6));
    }

    /**
     * Cycles the PPU. Will block until the cycle lasted long enough.
     */
    private void cycle() {
//        while (!cycling) { }

        benchmarkCycles++;
        if (System.nanoTime() - benchmarkTime >= 1e+9) {
            LOGGER.info(String.format("PPU Clock rate: %.2fMHz / 5.37MHz", benchmarkCycles / 1e+6));
            benchmarkCycles = 0;
            benchmarkTime = System.nanoTime();
        }

        currentCycle++;

        if (currentCycle % 3 == 0)
            cpu.tick();
//        cycling = false;
    }
}

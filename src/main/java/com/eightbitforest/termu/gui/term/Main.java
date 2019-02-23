package main.java.com.eightbitforest.termu.gui.term;

import main.java.com.eightbitforest.termu.emu.core.EmulatorLoader;
import main.java.com.eightbitforest.termu.emu.core.IEmulator;
import main.java.com.eightbitforest.termu.emu.core.RomPath;
import main.java.com.eightbitforest.termu.emu.core.exceptions.RomLoadException;

public class Main {
    public static void main(String... args) {
        try {
            RomPath romPath = new RomPath("roms/test1.nes");
//            RomPath romPath = new RomPath("roms/nestest.nes");
            IEmulator emulator = EmulatorLoader.getEmulatorForRom(romPath);
            emulator.loadRom(romPath);
            System.out.println("Starting " + emulator.getLoadedRom().getName() + ".");
            emulator.start();
        } catch (RomLoadException e) {
            e.printStackTrace();
        }

        // TODO: Re-enable when ready for graphics
//        MainWindow mainWindow = new MainWindow();
//        mainWindow.start();
    }
}

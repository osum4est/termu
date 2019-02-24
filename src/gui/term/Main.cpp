#include "windows/MainWindow.h"

int main() {
//    try {
//        RomPath romPath = new RomPath("roms/test1.nes");
//            RomPath romPath = new RomPath("roms/nestest.nes");
//        IEmulator emulator = EmulatorLoader.getEmulatorForRom(romPath);
//        emulator.loadRom(romPath);
//        System.out.println("Starting " + emulator.getLoadedRom().getName() + ".");
//        emulator.start();
//    } catch (RomLoadException e) {
//        e.printStackTrace();
//    }

    // TODO: Re-enable when ready for graphics
        MainWindow* mainWindow = new MainWindow();
        mainWindow->Start();
}

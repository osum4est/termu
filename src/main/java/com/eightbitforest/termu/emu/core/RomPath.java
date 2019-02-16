package main.java.com.eightbitforest.termu.emu.core;

import main.java.com.eightbitforest.termu.emu.core.exceptions.RomLoadException;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;

public class RomPath {
    private String extension;
    private File file;

    public RomPath(String path) throws RomLoadException {
        file = new File(path);
        if (!file.exists() || file.isDirectory())
            throw new RomLoadException("Cannot find " + path + ".");

        extension = "";
        int i = path.lastIndexOf('.');
        if (i > 0)
            extension = path.substring(i+1);
    }

    public String getPath() {
        return file.getPath();
    }

    public String getFilenameWithoutExtension() {
        return file.getName().replace("." + getExtension(), "");
    }

    public String getExtension() {
        return extension;
    }

    public byte[] readAllBytes() {
        try {
            return Files.readAllBytes(file.toPath());
        } catch (IOException e) {
            return null;
        }
    }
}

package hr.fer.zemris.io;

import hr.fer.zemris.filters.FiltersFactory;
import hr.fer.zemris.image.BinaryImage;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.nio.file.Path;
import java.util.List;

;

public class ImagesTask extends SwingWorker<Void, Void> {

    private List<Path> images;
    private Path outputPath;

    public ImagesTask(List<Path> images, Path outputPath) {
        this.images = images;
        this.outputPath = outputPath;
    }

    @Override
    public Void doInBackground() {
        if (images == null || outputPath == null) {
            return null;
        }

        int max = images.size();
        int counter = 0;
        int i = 82;
        for (Path path : images) {
            try {
                counter++;i++;
                BufferedImage image = ImageIO.read(path.toFile());
//                BinaryImage binImage = new BinaryImage(image, FiltersFactory.getDefaultFilters());
                File newImage = new File(outputPath + "/" + path.getFileName()); //.toString().split("_")[0] + "_" + i + ".png");
                System.out.println(newImage.getPath());
                int cislo=Integer.parseInt(path.getFileName().toString().split("_")[1].split("\\.")[0]);System.out.println("sdfsd");
                if(!(cislo == 1 || cislo == 8 || cislo == 15 || cislo == 19 || cislo == 24 || cislo == 34 ||
                        cislo == 38 || cislo == 45 || cislo == 50 || cislo == 58 || cislo == 60 ||
                        cislo == 62 || cislo == 77 || cislo == 80 || cislo == 82 || cislo == 86 ||
                        cislo == 91 || cislo == 102 || cislo == 107 || cislo == 110 ||
                        cislo == 111 || cislo == 114 || cislo == 123)){
                ImageIO.write(image, "png", newImage);}
                setProgress((int) (100 * counter/(double) max));
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        setProgress(0);
        return null;
    }

    @Override
    public void done() {
        Toolkit.getDefaultToolkit().beep();
    }
}

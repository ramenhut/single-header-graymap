## Simple header-only PGM and PBM file loader in C++

PGM (Portable Gray Map) and PBM (Portable Bit Map) files are very simple image file formats used for storing grayscale images. Useful when you need to store raw uncompressed values and want a format that's supported by a wide variety of image editors.

## Example Usage

```
GraymapImage image;

if (!image.LoadImage("test_image.pgm")) {
  std::cout << "Failed to load image!" << std::endl;
}
```

## More Information
For more information, visit [www.bertolami.com](https://www.bertolami.com/).
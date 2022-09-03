/*
BSD 2-Clause License

 Copyright (c) 2022, ramenhut
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef GraymapImage_h
#define GraymapImage_h

// Single header PGM/PBM file reader.
//
// PGM (Portable Gray Map) and PBM (Portable Bit Map) files are very
// simple image file formats used for storing grayscale images.
// Useful when you need to store raw uncompressed values and want a
// format that's supported by a wide variety of image editors.
//
// Example usage:
//
//   GraymapImage image;
//
//   if (!image.LoadImage("test_image.pgm")) {
//     std::cout << "Failed to load image!" << std::endl;
//   }

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using std::ifstream;
using std::string;
using std::stringstream;
using std::vector;

namespace ImageUtils {

class GraymapImage {
private:
    vector<unsigned char> _data;
    unsigned int _width;
    unsigned int _height;
    unsigned short _max_value;
public:
    GraymapImage();
    // Returns true if the graymap is initialized. False otherwise.
    bool IsInitialized() const;
    // Load a simple pgm image file.
    bool LoadImage(const string & filename);
    // Returns the width of the image, in pixels.
    unsigned int GetWidth() const;
    // Returns the height of the image, in pixels.
    unsigned int GetHeight() const;
    // Returns the maximum possible pixel value.
    unsigned short GetMaxValue() const;
    // Returns the value at a pixel.
    unsigned char GetPixel(unsigned int x, unsigned int y);
};

GraymapImage::GraymapImage() : _width(0), _height(0), _max_value(255) {}

bool GraymapImage::IsInitialized() const {
    return _width != 0 && _height != 0 && _data.size() > 0;
}

bool GraymapImage::LoadImage(const string & filename) {
    ifstream infile(filename);
    stringstream stream;
    string formatBuffer = "";
    string commentBuffer = "";
    
    getline(infile, formatBuffer);
    
    if (formatBuffer.compare("P1") != 0 &&
        formatBuffer.compare("P2") != 0 &&
        formatBuffer.compare("P4") != 0 &&
        formatBuffer.compare("P5") != 0) {
        infile.close();
        return false;
    }

    if (infile.peek() == '#') {
        getline(infile, commentBuffer);
    }
    
    stream << infile.rdbuf();
    stream >> _width >> _height;
    _data.resize(_width * _height);
    
    if (formatBuffer.compare("P2") != 0 ||
        formatBuffer.compare("P5") != 0) {
        stream >> _max_value;
        // We don't support 16-bit image data, yet.
        if (_max_value > 255) {
            infile.close();
            return false;
        }
    }

    if (formatBuffer.compare("P1") == 0) {
        // Single bit format (only values 0, 1) stored as ascii.
        // Read each whitespace-separated value. Scale by 255
        // for uniformity across formats.
        for(unsigned int j = 0; j < _height; ++j) {
            for (unsigned int i = 0; i < _width; ++i) {
                stream >> _data[i + j * _width];
                _data[i + j * _width] *= 255;
            }
        }
    } else if (formatBuffer.compare("P2") == 0) {
        // Grayscale format (values 0 to _max_value) stored as
        // ascii. Read each whitespace-separated value.
        for(unsigned int j = 0; j < _height; ++j) {
            for (unsigned int i = 0; i < _width; ++i) {
                stream >> _data[i + j * _width];
            }
        }
    } else if (formatBuffer.compare("P4") == 0) {
        // Compact single bit format stored as binary. Each byte
        // contains 8 values. Scale by 255 for uniformity across
        // formats.
        while(isspace(stream.peek())) {
            unsigned char dummy = 0;
            stream.read((char *) &dummy, sizeof(unsigned char));
        }
        
        for (unsigned int j = 0; j < _height; ++j)
        for (unsigned int i = 0; i < _width; i+=8) {
            unsigned char temp = 0;
            stream.read((char *) &temp, sizeof(unsigned char));
            
            for (unsigned int k = 0; k < 8; ++k) {
                unsigned int index = i + k + j * _width;
                if (index >= _data.size()) {
                    break;
                }
                // High bit is our first pixel in this set.
                unsigned char shift = 7 - k;
                _data[index] = ((temp & (0x1 << shift)) >> shift) * 255;
            }
        }
    } else if (formatBuffer.compare("P5") == 0) {
        // Grayscale format (values 0 to _max_value) stored as
        // binary.
        while(isspace(stream.peek())) {
            unsigned char dummy = 0;
            stream.read((char *) &dummy, sizeof(unsigned char));
        }
        
        for (unsigned int j = 0; j < _height; ++j)
        for (unsigned int i = 0; i < _width; ++i) {
            stream.read((char *) &_data[i + j * _width], sizeof(unsigned char));
        }
    }
    
    infile.close();
    return true;
}

unsigned int GraymapImage::GetWidth() const { return _width; }

unsigned int GraymapImage::GetHeight() const { return _height; }

unsigned short GraymapImage::GetMaxValue() const { return _max_value; }

unsigned char GraymapImage::GetPixel(unsigned int x, unsigned int y) {
    return _data.at(y * _width + x);
}

} /* ImageUtils */

#endif /* GraymapImage_h */

#ifndef PPMIMAGE_H
#define PPMIMAGE_H

#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include "utils.hpp"
using namespace std;

char LF = 0x0A; // line feed, new line
char SPACE = 0x20;

class RGBColor {
private:
    char data[3];

public:
    RGBColor() {

    }

    RGBColor(unsigned int hexa) {
        data[2] = hexa % 0x100;
        hexa /= 0x100;
        data[1] = hexa % 0x100;
        hexa /= 0x100;
        data[0] = hexa % 0x100;
    }

    RGBColor(char r, char g, char b) {
        data[0] = r;
        data[1] = g;
        data[2] = b;
    }

    char * _data() {
        return data;
    }

    const char * getData() const {
        return data;
    }
};

class PPMImage {
private:
    string type;

    int maxSaturation;
    string maxSaturationStr;

    int width;
    string widthStr;
    int height;
    string heightStr;

    SharedPointer<char> data;

public:
    PPMImage(string fileName) {
        this->loadFrom(fileName);
    }

    PPMImage(int width, int height) {
        type = "P6";

        maxSaturation = 255;
        maxSaturationStr = mutils::int2string(maxSaturation);
        
        this->width = width;
        this->height = height;
        widthStr = mutils::int2string(width);
        heightStr = mutils::int2string(height);
        data = SharedPointer<char>(new char[width*height*3]);
        memset(data.get(), 0, width*height*3*sizeof(char));
    }

    PPMImage flipVertically() {
        PPMImage result(width, height);
        int numBytesPerRow = 3*width;
        for (int i = 0; i < height; ++i) {
            memcpy(result.data.get()+i*numBytesPerRow, data.get()+(height-i-1)*numBytesPerRow, numBytesPerRow);
        }
        return result;
    }

    int getWidth() { return width; }

    int getHeight() { return height; }

    bool setColorAt(int row, int col, const RGBColor color) {
        memcpy(&this->data[(row*this->width+col)*3], color.getData(), 3);
        return true;
    }

    RGBColor getColorAt(int row, int col) {
        RGBColor color;
        memcpy(color._data(), &this->data[(row*this->width+col)*3], 3);
        return color;
    }

    bool saveTo(string fileName) {
        ofstream ofs(fileName, ios::binary);
        if(!ofs) return false;
        /* type */
        ofs.write(this->type.c_str(), this->type.size());
        ofs.write(&LF, 1);
        /* size */
        ofs.write(this->widthStr.c_str(), this->widthStr.size());
        ofs.write(&SPACE, 1);
        ofs.write(this->heightStr.c_str(), this->heightStr.size());
        ofs.write(&LF, 1);
        /* largest saturation */
        ofs.write(this->maxSaturationStr.c_str(), this->maxSaturationStr.size());
        ofs.write(&LF, 1);
        /* pixel data */
        ofs.write(this->data.get(), width*height*3);
        ofs.close();
        return true;
    }

    bool loadFrom(string fileName) {
        ifstream ifs(fileName, ios::binary);
        if(!ifs) return false;
        string buffer;
        /* type */
        getline(ifs, buffer, LF);
        this->type = buffer;
        // printf("\'%s\'", buffer.c_str());
        /* size */
        getline(ifs, buffer, SPACE);
        this->widthStr = buffer;
        this->width = mutils::string2Int(buffer);
        getline(ifs, buffer, LF);
        this->heightStr = buffer;
        this->height = mutils::string2Int(buffer);
        // printf("%d %d\n", this->width, this->height);
        /* largest saturation */
        getline(ifs, buffer, LF);
        this->maxSaturationStr = buffer;
        this->maxSaturation = mutils::string2Int(buffer);
        // printf("%d\n", this->maxSaturation);
        /* pixel data */
        data = SharedPointer<char>(new char[width*height*3]);
        ifs.read(data.get(), width*height*3);
        return true;
    }
};

void drawLine(int x0, int y0, int x1, int y1, PPMImage &image, const RGBColor &color) {
    bool steep = false;
    if (abs(x0-x1) < abs(y0-y1)) {
        swap(x0, y0);
        swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }
    for (int x = x0; x < x1; ++x) {
        float t = (x-x0)/float(x1-x0);
        int y = (1-t)*y0 + t*y1;
        if (steep) {
            image.setColorAt(x, y, color);
        }
        else {
            image.setColorAt(y, x, color);
        }
    }
}

void test () {
    int width = 480, height = 480;
    PPMImage image(width, height);
    RGBColor black(0x0);
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            image.setColorAt(row, col, black);
        }
    }
    RGBColor white(0xffffff);
    drawLine(100, 100, 100, 300, image, white);
    image.saveTo("line.ppm");
}

/*
(x0, y0) (x1, y1)

k = (y0-y1)/(x0-x1)

(x-x0)/(y-y0)=(x1-x0)/(y1-y0)

(x-x0)/(x1-x0)=(y-y0)/(y1-y0)=t

(y-y1+y1-y0)/(y1-y0)=t
(y-y1)/(y1-y0)+1=t
y=(t-1)(y1-y0)+y1
y=(1-t)(y0-y1)+y1
y=(1-t)y0-y1+y1t+y1
y=(1-t)y0+ty1

*/

#endif
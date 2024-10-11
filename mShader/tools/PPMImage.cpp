#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
using namespace std;

char LF = 0x0A; // line feed, new line
char SPACE = 0x20;

string int2string(int x) {
    vector<char> chs;
    while (x) {
        chs.push_back(x%10+'0');
        x /= 10;
    }
    int start = 0, end = chs.size() - 1;
    while (start < end) {
        char tmp = chs[start];
        chs[start] = chs[end];
        chs[end] = tmp;
        start ++;
        end --;
    }
    return string(chs.data());
}

class RGBColor {
private:
    char data[3];

public:
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

    const char* getData() {
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

    char *data;

public:
    PPMImage(int width, int height) {
        type = "P6";

        maxSaturation = 255;
        maxSaturationStr = int2string(maxSaturation);
        
        this->width = width;
        this->height = height;
        widthStr = int2string(width);
        heightStr = int2string(height);
        data = new char[width*height*3];
    }

    bool setColorAt(int row, int col, RGBColor color) {
        memcpy(&this->data[(row*this->width+col)*3], color.getData(), 3);
        return true;
    }

    bool saveTo(string name) {
        ofstream ofs(name, ios::binary);
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
        ofs.write(this->data, width*height*3);
        ofs.close();
        return true;
    }

    ~PPMImage() {
        delete[] data;
    }
};

int main () {
    int width = 480, height = 480;
    PPMImage image(width, height);
    RGBColor red(0xff0000);
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            image.setColorAt(row, col, red);
        }
    }
    image.saveTo("output.ppm");
}
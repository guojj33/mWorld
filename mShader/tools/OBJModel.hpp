#include <iostream>
#include <fstream>
#include <vector>
#include "Matrix.hpp"
#include "PPMImage.hpp"
using namespace std;

class TriangleFace {
public:
    Vec3f vertices[3];
    Vec3f verticesTexture[3];
    Vec3f verticesNormal[3];

    TriangleFace() {

    }

    TriangleFace(Vec3f & A, Vec3f & B, Vec3f  & C) {
        vertices[0] = A;
        vertices[1] = B;
        vertices[2] = C;
    }

    const Vec3f & A() {
        return vertices[0];
    }

    const Vec3f & B() {
        return vertices[1];
    }

    const Vec3f & C() {
        return vertices[2];
    }

    Vec3f getNorm() {
        Vec3f BA = vertices[1] - vertices[0];
        Vec3f CA = vertices[2] - vertices[0];
        Vec3f result = CA.crossProduct(BA).normalize();
        return result;
    }
};

class OBJModel {
public:
    vector<Vec3f> vertices;
    vector<Vec3f> verticesTexture;
    vector<Vec3f> verticesNormal;
    vector<TriangleFace> trianlgeFaces;

    OBJModel(string fileName) {
        printf("Loading %s ...\n", fileName.c_str());
        ifstream ifs(fileName, ios::in);
        if(!ifs) {
            printf("Failed to open file.\n");
            return;
        };
        string elementType = "";
        while (true) {
            if (ifs.peek() == EOF) {
                // cout << "EOF\n";
                break;
            }
            while (ifs.peek() == '\n'){
                ifs.get();
            }
            getline(ifs, elementType, ' ');
            if (elementType == "v") {
                real x, y, z;
                ifs >> x >> y >> z;
                Vec3f v;
                v.setXYZ(x, y, z);
                vertices.push_back(v);
            }
            else if (elementType == "vt") {
                real x, y, z;
                ifs >> x >> y >> z;
                Vec3f vt;
                vt.setXYZ(x, y, z);
                verticesTexture.push_back(vt);
            }
            else if (elementType == "vn") {
                real x, y, z;
                ifs >> x >> y >> z;
                Vec3f vn;
                vn.setXYZ(x, y, z);
                verticesNormal.push_back(vn);
            }
            else if (elementType == "f") {  // v/vt/vn
                TriangleFace f;
                for (int i = 0; i < 3; ++i) {
                    string s;
                    ifs >> s;
                    vector<string> items = mutils::stringSplit(s, '/');
                    Vec3f v = vertices[mutils::string2Int(items[0])-1]; // start from 1
                    Vec3f vt = verticesTexture[mutils::string2Int(items[1])-1];
                    Vec3f vn = verticesNormal[mutils::string2Int(items[2])-1];
                    f.vertices[i] = v;
                    f.verticesTexture[i] = vt;
                    f.verticesNormal[i] = vn;
                }
                trianlgeFaces.push_back(f);
            }
            else {
                string line;
                getline(ifs, line, '\n');
            }
        }
        printf("%ld vertices, %ld texture coodinates, %ld vertex normals, %ld polygonal faces\n", vertices.size(), verticesTexture.size(), verticesNormal.size(), trianlgeFaces.size());
    }
};

void drawTriangleWithTexture(vector<Vec3f> & pts, Matrix & zbuffer, PPMImage & image, vector<Vec3f> & vts, PPMImage & texture, real intensity) {
    Vec3f bboxmin(image.getWidth()-1, image.getHeight()-1, 0);
    Vec3f bboxmax(0, 0, 0);
    Vec3f clamp(image.getWidth()-1, image.getHeight()-1, 0);
    for (int i = 0; i < 3; ++i) {
        bboxmin.setX(max(real(0), min(bboxmin.x(), pts[i].x())));
        bboxmin.setY(max(real(0), min(bboxmin.y(), pts[i].y())));

        bboxmax.setX(min(clamp.x(), max(bboxmax.x(), pts[i].x())));
        bboxmax.setY(min(clamp.y(), max(bboxmax.y(), pts[i].y())));
    }

    // cout << "bboxmin:\n";
    // bboxmin.println();
    // cout << "bboxmax:\n";
    // bboxmax.println();

    Vec3f P;
    P.setX(bboxmin.x());
    while (P.x() <= bboxmax.x()) {
        P.setY(bboxmin.y());
        while (P.y() <= bboxmax.y()) {
            Vec3f bc_screen = barycentric(pts, P);
            real thres = -0.03;
            if (!(bc_screen.x() < thres || bc_screen.y() < thres || bc_screen.z() < thres)) {
                P.setZ(
                    pts[0].z()*bc_screen.x() +
                    pts[1].z()*bc_screen.y() +
                    pts[2].z()*bc_screen.z()
                );
                // printf("%.4lf %.4lf\n", zbuffer.at(int(P.y()), int(P.x())), P.z());
                if (zbuffer.at(int(P.y()), int(P.x())) < P.z()) {
                    zbuffer.set(int(P.y()), int(P.x()), P.z());

                    // render this point

                    // interpolate texture coord
                    Vec3f P_text_coord;
                    P_text_coord.setX(
                        vts[0].x()*bc_screen.x() +
                        vts[1].x()*bc_screen.y() +
                        vts[2].x()*bc_screen.z()
                    );
                    P_text_coord.setY(
                        vts[0].y()*bc_screen.x() +
                        vts[1].y()*bc_screen.y() +
                        vts[2].y()*bc_screen.z()
                    );
                    // get texture color
                    int text_x = int(P_text_coord.x()*texture.getWidth());
                    int text_y = int(P_text_coord.y()*texture.getHeight());
                    // printf("%d %d\n", text_x, text_y);
                    RGBColor textureColor = texture.getColorAt(texture.getHeight()-text_y, text_x);

                    image.setColorAt(P.y(), P.x(), textureColor);
                }
            }
            P.setY(P.y()+1);
        }
        P.setX(P.x()+1);
    }
}

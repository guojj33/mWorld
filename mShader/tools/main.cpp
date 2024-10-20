#include <vector>
#include "Matrix.hpp"
#include "OBJModel.hpp"

void testCrossProduct() {
    vector<Vec3f> pts;
    pts.push_back(Vec3f(10, 10, 0));
    pts.push_back(Vec3f(190, 10, 0));
    pts.push_back(Vec3f(100, 190,0));
    Vec3f P(100, 11, 0);
    Vec3f bc_screen = barycentric(pts, pts[0]);
    bc_screen.println();
}

void drawTriangleMain () {
    int width, height;
    width = height = 720;
    PPMImage image(width, height);
    Matrix zbuffer(width, height, std::numeric_limits<real>::lowest());
    RGBColor green(0xff00);
    RGBColor red(0xff0000);
    RGBColor white(0xffffff);
    RGBColor blue(0xff);

    vector<Vec3f> pts;
    pts.push_back(Vec3f(10, 10, 0));
    pts.push_back(Vec3f(100, 30, 0));
    pts.push_back(Vec3f(190, 160,0));
    drawTriangle(pts, zbuffer, image, green);
    
    vector<Vec3f> pts1;
    pts1.push_back(Vec3f(10, 10, 0));
    pts1.push_back(Vec3f(30, 100, 0));
    pts1.push_back(Vec3f(190, 160, 0));
    drawTriangle(pts1, zbuffer, image, red);

    vector<Vec3f> pts2;
    pts2.push_back(Vec3f(10, 10, 0));
    pts2.push_back(Vec3f(30, 100, 0));
    pts2.push_back(Vec3f(10, 160, 0));
    drawTriangle(pts2, zbuffer, image, white);

    vector<Vec3f> pts3;
    pts3.push_back(Vec3f(10, 160, 0));
    pts3.push_back(Vec3f(30, 100, 0));
    pts3.push_back(Vec3f(190, 160, 0));
    drawTriangle(pts3, zbuffer, image, blue);

    image.saveTo("./output/triangle.ppm");
}

void drawHeadMain () {
    // source model
    OBJModel model("./assets/african_head.obj");
    Vec3f light_dir(0, 0, -1);
    int width, height;
    width = height = 1024;

    // texture image
    PPMImage texture("./assets/african_head_diffuse.ppm");
    
    // destination image
    PPMImage image(width, height);
    Matrix zbuffer(width, height, std::numeric_limits<real>::lowest());
    RGBColor white(0xffffff);

    
    for (int i = 0; i < model.trianlgeFaces.size(); ++i) {
        TriangleFace face = model.trianlgeFaces[i];
        Vec3f faceNorm = face.getNorm();
        real intensity = faceNorm.dotProduct(light_dir);
        RGBColor faceColor(intensity*255, intensity*255, intensity*255);
        vector<Vec3f> screen_coords;
        for (int j = 0; j < 3; ++j) {
            Vec3f word_coord = face.vertices[j];
            screen_coords.push_back(Vec3f(
                (word_coord.x()+1.)*real(width)/2.,
                (word_coord.y()+1.)*real(height)/2.,
                0.
            ));
        }
        if (intensity >= 0) {
            vector<Vec3f> vts;
            for (int j = 0; j < 3; ++j){
                vts.push_back(face.verticesTexture[j]);
            } 
            drawTriangleWithTexture(screen_coords, zbuffer, image, vts, texture, faceColor);
            // drawTriangle(screen_coords, zbuffer, image, faceColor);
        }
    }
    image.flipVertically().saveTo("./output/headTriangleTexture.ppm");
}

void testLoad() {
    PPMImage image("./assets/african_head_diffuse.ppm");
    image.saveTo("./output/diffuse.ppm");
}

int main () {
    drawHeadMain();
    // drawTriangleMain();
    // testCrossProduct();
    // testLoad();
}
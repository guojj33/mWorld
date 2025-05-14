#include <vector>
#include <limits>
#include "Matrix.hpp"
#include "OBJModel.hpp"

// modelview
Matrix lookat(Vec3f eye, Vec3f center, Vec3f up, Matrix & Minv) {
    Vec3f z = (eye - center).normalize();
    Vec3f x = up.crossProduct(z).normalize();
    Vec3f y = z.crossProduct(x).normalize();
    Minv = Matrix::identity(4);
    Matrix Tr = Matrix::identity(4);
    for (int i = 0; i < 3; ++i) {
        Minv.set(0, i, x[i]);
        Minv.set(1, i, y[i]);
        Minv.set(2, i, z[i]);
        Tr.set(i, 3, -eye[i]);
    }
    return Minv.dot(Tr);
}

Vec3f light_dir(-1, 1, -1);   // 在相机坐标系下

void drawTriangle(vector<Vec3f> & pts, vector<real> & zs, Matrix & zbuffer, vector<Vec3f> & vns, PPMImage & image, RGBColor & color) {
    Vec3f bboxmin(image.getWidth()-1, image.getHeight()-1, 0);
    Vec3f bboxmax(0, 0, 0);
    Vec3f clamp(image.getWidth()-1, image.getHeight()-1, 0);
    for (int i = 0; i < 3; ++i) {
        bboxmin.setX(max(real(0), min(bboxmin.x(), pts[i].x())));
        bboxmin.setY(max(real(0), min(bboxmin.y(), pts[i].y())));

        bboxmax.setX(min(clamp.x(), max(bboxmax.x(), pts[i].x())));
        bboxmax.setY(min(clamp.y(), max(bboxmax.y(), pts[i].y())));
    }

    bboxmin.setX(floor(bboxmin.x()));
    bboxmin.setY(floor(bboxmin.y()));
    bboxmax.setX(ceil(bboxmax.x()));
    bboxmax.setY(ceil(bboxmax.y()));

    Vec3f P;
    P.setX(bboxmin.x());
    Vec3f Pn;
    while (P.x() <= bboxmax.x()) {
        P.setY(bboxmin.y());
        while (P.y() <= bboxmax.y()) {
            // P.println();
            Vec3f bc_screen = barycentric(pts, P);
            real thres = 0;
            if (!(bc_screen.x() < thres || bc_screen.y() < thres || bc_screen.z() < thres)) {
                P.setZ(
                    zs[0]*bc_screen.x() +
                    zs[1]*bc_screen.y() +
                    zs[2]*bc_screen.z()
                );
                if (zbuffer.at(int(P.y()), int(P.x())) < P.z()) {
                    zbuffer.set(int(P.y()), int(P.x()), P.z());

                    // calculate vertex normal
                    Pn.setX(vns[0].x()*bc_screen.x() + vns[1].x()*bc_screen.y() + vns[2].x()*bc_screen.z());
                    Pn.setY(vns[0].y()*bc_screen.x() + vns[1].y()*bc_screen.y() + vns[2].y()*bc_screen.z());
                    Pn.setZ(vns[0].z()*bc_screen.x() + vns[1].z()*bc_screen.y() + vns[2].z()*bc_screen.z());
                    // calculate intensity
                    real intensity = (Pn).dotProduct(-light_dir); // 在相机坐标系下
                    if (intensity > 0) {
                        RGBColor faceColor(intensity*255, intensity*255, intensity*255);
                        image.setColorAt(P.y(), P.x(), faceColor);
                    }
                    else if (intensity <= 0) {
                        RGBColor black(0x000000);
                        image.setColorAt(P.y(), P.x(), black);
                    }
                }
            }
            P.setY(P.y()+1);
        }
        P.setX(P.x()+1);
    }
}

void drawHeadMain () {
    // source model
    OBJModel model("./assets/african_head.obj");
    int width, height;
    width = height = 1024;

    Vec3f eye(1, 0, 1);
    // Vec3f light_dir = -eye;
    Vec3f center(0, 0, 0);
    Vec3f up(0, 1, 0);
    real c = (eye-center).getLength();
    Matrix Minv;
    Matrix modelView = lookat(eye, center, up, Minv);

    // transform the light direction from world to camera
    Matrix light_word_homo(4, 1);
    light_word_homo.set(0, 0, light_dir.x());
    light_word_homo.set(1, 0, light_dir.y());
    light_word_homo.set(2, 0, light_dir.z());
    light_word_homo.set(3, 0, 0.);
    Matrix light_trans = modelView.dot(light_word_homo);
    light_dir.setX(light_trans.at(0, 0));
    light_dir.setY(light_trans.at(1, 0));
    light_dir.setZ(light_trans.at(2, 0));
    light_dir = light_dir.normalize();


    // texture image
    PPMImage texture("./assets/african_head_diffuse.ppm");
    
    // destination image
    PPMImage image(width, height);
    Matrix zbuffer(width, height, numeric_limits<real>::lowest());
    RGBColor white(0xffffff);

    
    for (int i = 0; i < model.trianlgeFaces.size(); ++i) {
        TriangleFace face = model.trianlgeFaces[i];
        Vec3f faceNorm = face.getNorm();
        real intensity = faceNorm.dotProduct(light_dir);
        RGBColor faceColor(intensity*255, intensity*255, intensity*255);
        vector<Vec3f> screen_coords;
        vector<real> zs;
        for (int j = 0; j < 3; ++j) {
            // projection
            Vec3f word_coord = face.vertices[j].clone();

            Matrix word_coord_homo(4, 1);
            word_coord_homo.set(0, 0, word_coord.x());
            word_coord_homo.set(1, 0, word_coord.y());
            word_coord_homo.set(2, 0, word_coord.z());
            word_coord_homo.set(3, 0, 1.);
            Matrix word_coord_trans = modelView.dot(word_coord_homo);
            word_coord.setX(word_coord_trans.at(0, 0));
            word_coord.setY(word_coord_trans.at(1, 0));
            word_coord.setZ(word_coord_trans.at(2, 0));
            
            real weight = c/(c-word_coord.z());
            // viewport
            screen_coords.push_back(Vec3f(
                (word_coord.x()*weight+1.)*real(width)/2.,
                (word_coord.y()*weight+1.)*real(height)/2.,
                0.
            ));
            zs.push_back(word_coord.z());
        }
        // if (intensity >= 0) {
        if (true) {
            vector<Vec3f> vts;
            vector<Vec3f> vns;
            for (int j = 0; j < 3; ++j){
                vts.push_back(face.verticesTexture[j]);

                // transform the vetex normals
                Vec3f cur_vn = face.verticesNormal[j].clone();
                // Matrix cur_vn_homo(4,1);
                // cur_vn_homo.set(0, 0, cur_vn.x());
                // cur_vn_homo.set(1, 0, cur_vn.y());
                // cur_vn_homo.set(2, 0, cur_vn.z());
                // cur_vn_homo.set(3, 0, 1.);
                // Matrix cur_vn_trans = Minv.transpose().dot(cur_vn_homo);
                // cur_vn.setX(cur_vn_trans.at(0, 0));
                // cur_vn.setY(cur_vn_trans.at(1, 0));
                // cur_vn.setZ(cur_vn_trans.at(2, 0));
                vns.push_back(cur_vn);
            }

            // drawTriangleWithTexture(screen_coords, zbuffer, image, vts, texture, intensity);
            drawTriangle(screen_coords, zs, zbuffer, vns, image, faceColor);
        }
    }
    // image.flipVertically().saveTo("./output/headTriangleTexture.ppm");
    image.flipVertically().saveTo("./output/headTriangle.ppm");
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
#ifndef MATRIX_H
#define MATRIX_H


#include <string>
#include <cstring>
#include <cmath>
#include <iostream>
#include <vector>
#include "utils.hpp"

typedef double real;

typedef std::vector< std::vector<real> > Array2d;

class Matrix {
protected:
    int numRow;
    int numCol;
    SharedPointer<real> data;

protected:
    bool _initWith(int numRow, int numCol) {
        if (numRow <= 0 || numCol <= 0) return false;
        this->numRow = numRow;
        this->numCol = numCol;

        data = SharedPointer<real>(new real[numRow*numCol]);
        memset(data.get(), 0, numRow*numCol*sizeof(real));
        
        return true;
    }

public:
    Matrix() {
        numCol = numRow = -1;
    }

    static Matrix identity(int shape) {
        Matrix result(shape, shape);
        for (int i = 0; i < shape; ++i) result.set(i, i, 1);
        return result;
    }

    Matrix(std::vector< std::vector<real> > array2d) {
        int numRow = array2d.size();
        int numCol = array2d[0].size();
        this->_initWith(numRow, numCol);
        for (int i = 0; i < numRow; ++i) {
            for (int j = 0; j < numCol; ++j) {
                data[i*numCol+j] = array2d[i][j];
            }
        }
    }

    Matrix(int numRow, int numCol, real value) {
        this->_initWith(numRow, numCol);
        for (int i = 0; i < numRow*numCol; ++i) {
            data[i] = value; 
        }
    }


    Matrix(int numRow, int numCol) {
        this->_initWith(numRow, numCol);
    }

    bool initWith(int numRow, int numCol) {
        if (this->numRow == -1 && this->numCol == -1) {
            return this->_initWith(numRow, numCol);
        }
        else return false;
    }

    bool isIndexValid(int row, int col) {
        return (row >= 0 && row < numRow && col >= 0 && col < numCol);
    }

    const real at(int row, int col) const {
        return data.at(row*numCol+col);
    }

    void set(int row, int col, real value) {
        data[row*numCol+col] = value;
    }

    Matrix clone() {
        Matrix result(numRow, numCol);
        memcpy(result.data.get(), data.get(), numRow*numCol*sizeof(real));
        return result;
    }

    void _dot(const Matrix & m2, Matrix & result) {
        for (int i = 0; i < result.numRow; ++i) {
            for (int j = 0; j < result.numCol; ++j) {
                real sum = 0;
                for (int k = 0; k < m2.numRow; ++k) {
                    sum += at(i, k) * m2.at(k, j);
                }
                result.set(i, j, sum);
            }
        }
    }

    void _mul(const real x, Matrix & result) {
        for (int i = 0; i < numRow; ++i) {
            for (int j = 0; j < numCol; ++j) {
                result.set(i, j, at(i, j)*x);
            }
        }
    }

    void _add(const Matrix & m2, Matrix & result) {
        for (int i = 0; i < numRow; ++i) {
            for (int j = 0; j < numCol; ++j) {
                result.set(i, j, at(i, j) + m2.at(i, j));
            }
        }
    }

    void _add(const real x, Matrix & result) {
        for (int i = 0; i < numRow; ++i) {
            for (int j = 0; j < numCol; ++j) {
                result.set(i, j, at(i, j) + x);
            }
        }
    }

    void _sub(const Matrix & m2, Matrix & result) {
        for (int i = 0; i < numRow; ++i) {
            for (int j = 0; j < numCol; ++j) {
                result.set(i, j, at(i, j) - m2.at(i, j));
            }
        }
    }

    void _div(const real x, Matrix & result) {
        for (int i = 0; i < numRow; ++i) {
            for (int j = 0; j < numCol; ++j) {
                result.set(i, j, at(i, j) / x);
            }
        }
    }

    void _negative(Matrix & result) {
        for (int i = 0; i < numRow; ++i) {
            for (int j = 0; j < numCol; ++j) {
                result.set(i, j, -at(i, j));
            }
        }
    }

    Matrix transpose() {
        Matrix result(numCol, numRow);
        for (int i = 0; i < numRow; ++i) {
            for (int j = 0; j < numCol; ++j) {
                result.set(j, i, at(i, j));
            }
        }
        return result;
    }

    Matrix dot(const Matrix & m2) {
        Matrix result(numRow, m2.numCol);
        _dot(m2, result);
        return result;
    }

    Matrix mul(const real x) {
        Matrix result = clone();
        _mul(x, result);
        return result;
    }

    Matrix add(const Matrix & m2) {
        Matrix result = clone();
        _add(m2, result);
        return result;
    }

    Matrix add(const real x) {
        Matrix result = clone();
        _add(x, result);
        return result;
    }

    Matrix sub(const Matrix & m2) {
        Matrix result = clone();
        _sub(m2, result);
        return result;
    }

    Matrix sub(const real x) {
        return add(-x);
    }

    Matrix div(const real x) {
        Matrix result = clone();
        _div(x, result);
        return result;
    }

    Matrix negative() {
        Matrix result = clone();
        _negative(result);
        return result;
    }

    Matrix operator*(real x) {
        return mul(x);
    }

    Matrix operator+(const Matrix & m2) {
        return add(m2);
    }

    Matrix operator+(const real x) {
        return add(x);
    }

    Matrix operator-(const Matrix & m2) {
        return sub(m2);
    }

    Matrix operator-(const real x) {
        return sub(x);
    }

    Matrix operator-() {
        return negative();
    }

    Matrix operator=(const Matrix & m2) {
        numRow = m2.numRow;
        numCol = m2.numCol;
        data = m2.data;
        return *this;
    }

    std::string toStr() {
        std::string s = "array([";
        for (int i = 0; i < numRow; ++i) {
            if (i != 0) s += "       ";
            s += "[";
            for (int j = 0; j < numCol; ++j) {
                s += (mutils::real2string(at(i, j)));
                if (j != numCol-1) s += ",\t";
            }
            s += "]";
            if (i != numRow-1) s += ",\n";
        }
        s += "])";
        return s;
    }

    void println() {
        printf("%s\n", toStr().c_str());
    }
};

class Vec3f {
private:
    Matrix m;

public:
    Vec3f() {
        m = Matrix(3, 1);
    }

    Vec3f(real x, real y, real z) {
        m = Matrix(3, 1);
        setXYZ(x, y, z);
    }

    Vec3f clone() {
        Vec3f result;
        result.setX(x());
        result.setY(y());
        result.setZ(z());
        return result;
    }

    const real x() const {
        return m.at(0, 0);
    }

    const real y() const {
        return m.at(1, 0);
    }

    const real z() const {
        return m.at(2, 0);
    }

    void setX(real x) {
        m.set(0, 0, x);
    }

    void setY(real y) {
        m.set(1, 0, y);
    }

    void setZ(real z) {
        m.set(2, 0, z);
    }

    Vec3f mul(const real x) {
        Vec3f result;
        result.m = m.mul(x);
        return result;
    }

    Vec3f add(const Vec3f & v2) {
        Vec3f result;
        result.m = m.add(v2.m);
        return result;
    }

    Vec3f sub(const Vec3f & v2) {
        Vec3f result;
        result.m = m.sub(v2.m);
        return result;
    }

    Vec3f negative() {
        Vec3f result;
        result.m = -m;
        return result;
    }

    Vec3f operator*(real x) {
        return mul(x);
    }

    Vec3f operator+(const Vec3f & v2) {
        return add(v2);
    }

    Vec3f operator-(const Vec3f & v2) {
        return sub(v2);
    }

    Vec3f operator-() {
        return negative();
    }

    real operator[](const int idx) {
        return m.at(idx,0);
    }

    void setXYZ(real x, real y, real z) {
        setX(x);
        setY(y);
        setZ(z);
    }

    real dotProduct(const Vec3f & v2) {
        return x()*v2.x() + y()*v2.y() + z()*v2.z();
    }

    Vec3f crossProduct(const Vec3f & v2) {
        Vec3f v3;
        v3.setXYZ(
          y()*v2.z() - z()*v2.y(),
          z()*v2.x() - x()*v2.z(),
          x()*v2.y() - y()*v2.x()
        );
        return v3;
    }

    std::string toStr() {
        return m.toStr();
    }

    void println() {
        return m.println();
    }

    real getLength() {
        return sqrt(x()*x() + y()*y() + z()*z());
    }

    Vec3f normalize() {
        Vec3f norm;
        real length = getLength();
        norm.setX(x()/length);
        norm.setY(y()/length);
        norm.setZ(z()/length);
        return norm;
    }
};

bool cmpYAscend(const Vec3f & a, const Vec3f & b) {
    return a.y() < b.y();
}

Vec3f barycentric(std::vector<Vec3f> & vertices, Vec3f & P) {
    Vec3f AB = vertices[1] - vertices[0];
    Vec3f AC = vertices[2] - vertices[0];
    Vec3f PA = vertices[0] - P;
    Vec3f v1(AB.x(), AC.x(), PA.x());
    Vec3f v2(AB.y(), AC.y(), PA.y());
    Vec3f v3 = v1.crossProduct(v2);
    Vec3f result;
    if (abs(v3.z()) < 1.0) {
        result = Vec3f(-1, 1, 1);
    }
    else {
        result.setX(1.0-(v3.x()+v3.y())/v3.z());
        result.setY(v3.x()/v3.z());
        result.setZ(v3.y()/v3.z());
    }
    return result;
}

/*
(v1.x, v1.y, v1.z)
(v2.x, v2.y, v2.z)

v1.y*v2.z-v1.z*v2.y
v1.z*v2.x-v1.x*v2.z
v1.x*v2.y-v1.y*v2.z
*/

#endif
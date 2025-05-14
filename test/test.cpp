#include <iostream>
using namespace std;

template <class T>
class SharedPointer {
private:
    T* _ptr;
    int *_pcount;
    
public:
    SharedPointer() {
        _ptr = nullptr;
        _pcount = nullptr;
    }

    SharedPointer(T *ptr) {
        _ptr = ptr;
        _pcount = new int(1);
    }

    SharedPointer(const SharedPointer<T> & sp) {
        _ptr = sp._ptr;
        _pcount = sp._pcount;
        ++(*_pcount);
    }

    int use_count() {
        return *_pcount;
    }

    T& operator*() {
        return *_ptr;
    }

    T* operator->() {
        return _ptr;
    }

    T& operator[](int pos) {
        return _ptr[pos];
    }

    SharedPointer<T> & operator=(const SharedPointer<T> &sp) {
        if (_ptr != sp._ptr) {
            Release();
            _ptr = sp._ptr;
            _pcount = sp._pcount;
            ++(*_pcount);
        }
        return *this;
    }

    void Release() {
        --(*_pcount);
        if (*_pcount == 0) {
            delete _pcount;
            delete []_ptr;
            _pcount = nullptr;
            _ptr = nullptr;
        }
    }

    ~SharedPointer() {
        Release();
    }
};

class Data {
public:
    int x;
    Data() {
        printf("creating %ld\n", this);
    }

    Data(int x) {
        this->x = x;
    }

    Data func() {
        printf("Data clone\n");
        Data clone(this->x);
        return clone;
    }

    virtual void hello() {
        printf("I am Data\n");
    }

    ~Data() {
        printf("deleting %ld\n", this);
    }
};

class DataDerived : public Data {
public:

    DataDerived func() {
        printf("DataDerived clone\n");
        DataDerived clone;
        clone.x = this->x;
        return clone;
    }

    void hello() override {
        printf("I am DataDerived\n");
    }

};

void invoke(Data & d) {
    d.func();
}

int main () {
    DataDerived d;
    d.func();
}
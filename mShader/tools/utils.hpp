#ifndef UTILS_H
#define UTILS_H

#include <sstream>
#include <string>

namespace mutils {

    std::vector<std::string> stringSplit(std::string s, char c) {
        std::vector<std::string> result;
        int start = 0, end = 0;
        while (start < s.size() && end < s.size()) {
            if (s[end] == c) {
                result.push_back(s.substr(start, end-start));
                start = end+1;
                end = start;
            }
            else end ++;
        }
        if (start < s.size()) {
            result.push_back(s.substr(start, s.size()-start));
        }
        else if (start == s.size()) {
            result.push_back(std::string(""));
        }
        return result;
    }

    std::string int2string(int x) {
        std::stringstream ss;
        ss << x;
        std::string result;
        ss >> result;
        return result;
    }

    std::string real2string(float x) {
        std::stringstream ss;
        ss << x;
        std::string result;
        ss >> result;
        return result;
    }

    std::string real2string(double x) {
        std::stringstream ss;
        ss << x;
        std::string result;
        ss >> result;
        return result;
    }


    int string2Int(std::string s) {
        std::stringstream ss;
        ss << s;
        int x;
        ss >> x;
        return x;
    }

}


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

    const int use_count() {
        return *_pcount;
    }

    T *get() {
        return this->_ptr;
    }

    const T& at(int pos) const {
        return _ptr[pos];
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
        if (_pcount == nullptr) return;
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

#endif
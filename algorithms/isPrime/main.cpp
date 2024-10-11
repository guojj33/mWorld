#include <iostream>
#include <string.h>
using namespace std;

class PrimeTable {
private:
    int *isPrime;

public:

    PrimeTable(int count) {
        isPrime = new int[count+1];
        isPrime[0] = isPrime[1] = 0;
        for (int i = 2; i <= count; ++i) { // 2 是质数
            isPrime[i] = 1;
        }
        for (int p = 2; p*p <= count; ++p) {    // 找还没访问的第一个质数 p，然后设置其倍数都为非质数
            if (isPrime[p]) {
                for (int j = p * p; j <= count; j+= p) {    // 倍数从第 p 倍开始，因为小与 p 的倍数已经设置过
                    isPrime[j] = 0;
                }
            }
        }
    }

    int isPrimeInt(int x) {
        return isPrime[x];
    }

    ~PrimeTable() {
        delete []isPrime;
    }
};

int main () {
    int count; // 包括 count 以内的素数
    cin >> count;
    PrimeTable pt(count);
    string s[2] = {"no", "yes"};
    for (int i = 1; i <= count; ++i) {
        printf("%d: %s\n", i, s[pt.isPrimeInt(i)].c_str());
    }
}
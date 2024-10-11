#include <iostream>
using namespace std;

void moveTo(int n, char start, char dest, char tmp) {
    if (n == 1) {
        printf("move 1 from %c to %c\n", start, dest);
        return;
    }
    moveTo(n-1, start, tmp, dest);
    printf("move %d from %c to %c\n", n, start, dest);
    moveTo(n-1, tmp, dest, start);
}

int main () {
    int n;
    cin >> n;
    moveTo(n, 'A', 'C', 'B');
}
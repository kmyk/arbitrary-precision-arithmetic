#include "natural.hpp"
using namespace std;

natural fact(int x) {
    natural y = natural(1);
    for (int i = 1; i <= x; ++i) {
        y = y * natural(i);
    }
    return y;
}

int main() {
    int n;
    cin >> n;
    cout << fact(n) << endl;
    return 0;
}

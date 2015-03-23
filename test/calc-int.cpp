#include "integer.hpp"
using namespace std;
int main() {
    integer a, b;
    char c;
    cin >> a >> c >> b;
    if (c == '+') {
        cout << a + b << endl;
    } else if (c == '-') {
        cout << a - b << endl;
    } else if (c == '*') {
        cout << a * b << endl;
    } else if (c == '/') {
        cout << a / b << endl;
    }
    return 0;
}

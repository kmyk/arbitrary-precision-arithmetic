#include "natural.hpp"
#include "natural.hpp"
#include "natural.hpp"
#include "natural.hpp"
#include <sstream>
#include <random>
using namespace std;

void test_ordering() {
    default_random_engine engine;
    uniform_int_distribution<natural::digit_t> digit_dist;
    uniform_int_distribution<int> padding_dist(0, 1000);
    for (int i = 0; i < 100; ++i) {
        natural::digit_t a = digit_dist(engine);
        natural::digit_t b = digit_dist(engine);
        int l = padding_dist(engine);
        natural an; {
            stringstream ss;
            ss << a << string(l, '0');
            an = natural(ss.str());
        }
        natural bn; {
            stringstream ss;
            ss << b << string(l, '0');
            bn = natural(ss.str());
        }
        assert ((a == b) == (an == bn));
        assert ((a != b) == (an != bn));
        assert ((a <= b) == (an <= bn));
        assert ((a <  b) == (an <  bn));
        assert ((a >= b) == (an >= bn));
        assert ((a >  b) == (an >  bn));
    }
}

void test_operate() {
    natural e16 = natural("65536"); // 2^16
    natural e31 = natural("2147483648"); // 2^31
    natural e32m1 = natural("4294967295"); // 2^32 - 1
    natural e32 = natural("4294967296"); // 2^32
    natural e33 = natural("8589934592"); // 2^33
    natural a;
    a = e32m1; assert (++a == e32);
    a = e32; assert (e32m1 == --a);
    assert (e31 + e31 == e32);
    assert (e32 + e32 == e33);
    assert (e32 - e31 == e31);
    assert (e33 - e32 == e32);
    assert (e31 * natural(2) == e32);
    assert (e32 * natural(2) == e33);
    assert (e31 * natural(4) == e33);
    assert (e16 * e16 == e32);
}

int main() {
    test_ordering();
    test_operate();
    return 0;
}

#define private public
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

void test_sub() {
    natural answer =  natural("100000000000000000000000000000000");
    assert (answer == natural("100000000000000000000000000000000") - natural("0"));
    assert (answer == natural("100000000000000000000000000000001") - natural("1"));
    assert (answer == natural("100000000000000000000000000000021") - natural("21"));
    assert (answer == natural("100000000000000000000000000000321") - natural("321"));
    assert (answer == natural("100000000000000000000000000004321") - natural("4321"));
    assert (answer == natural("100000000000000000000000000054321") - natural("54321"));
    assert (answer == natural("100000000000000000000000000654321") - natural("654321"));
    assert (answer == natural("100000000000000000000000007654321") - natural("7654321"));
    assert (answer == natural("100000000000000000000000087654321") - natural("87654321"));
    assert (answer == natural("100000000000001234567890123456789") - natural("1234567890123456789"));
    assert (answer == natural("100012345678901234567890123456789") - natural("12345678901234567890123456789"));
}

void test_mult_one() {
    natural  answer("100000000000000000000000000000000");
    assert (natural("100000000000000000000000000000000") * 1 == answer);
    assert (natural("1000000000000000000000000000000") * 100 == answer);
    assert (natural("10000000000000000000000000000") * 10000 == answer);
    assert (natural("100000000000000000000000000") * 1000000 == answer);
    assert (natural("1000000000000000000000000") * 100000000 == answer);
}

void test_mult_1() {
    natural a0 = natural(vector<natural::digit_t>({ 1 }));
    natural a1 = natural(vector<natural::digit_t>({ 0, 1 }));
    natural a2 = natural(vector<natural::digit_t>({ 0, 0, 1 }));
    natural a3 = natural(vector<natural::digit_t>({ 0, 0, 0, 1 }));
    assert (a0 * a0 == a0);
    assert (a1 * a1 == a2);
    assert (a1 * a2 == a3);
    assert ((a0 + a1) * a2 == a2 + a3);
}

void test_mult_2() {
    natural  answer("100000000000000000000000000000000");
    assert (natural("100000000000000000000000000000000") * natural("1") == answer);
    assert (natural("1000000000000000000000000000000") * natural("100") == answer);
    assert (natural("10000000000000000000000000000") * natural("10000") == answer);
    assert (natural("100000000000000000000000000") * natural("1000000") == answer);
    assert (natural("1000000000000000000000000") * natural("100000000") == answer);
    assert (natural("10000000000000000000000") * natural("10000000000") == answer);
    assert (natural("100000000000000000000") * natural("1000000000000") == answer);
    assert (natural("1000000000000000000") * natural("100000000000000") == answer);
    assert (natural("10000000000000000") * natural("10000000000000000") == answer);
    assert (natural("100000000000000") * natural("1000000000000000000") == answer);
    assert (natural("1000000000000") * natural("100000000000000000000") == answer);
    assert (natural("10000000000") * natural("10000000000000000000000") == answer);
    assert (natural("100000000") * natural("1000000000000000000000000") == answer);
    assert (natural("1000000") * natural("100000000000000000000000000") == answer);
    assert (natural("10000") * natural("10000000000000000000000000000") == answer);
    assert (natural("100") * natural("1000000000000000000000000000000") == answer);
    assert (natural("1") * natural("100000000000000000000000000000000") == answer);
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

void test_shift() {
    natural a, b; natural::digits_t v;
    v = { 1, 2, 3, 4, 5, 6, 7, 8 }; a = natural(v);
    v = { 3, 4, 5, 6, 7, 8 }; assert (natural::rshift_digit(a,2) == natural(v));
    v = { 0, 0, 1, 2, 3, 4, 5, 6, 7, 8 }; assert (natural::lshift_digit(a,2) == natural(v));
    natural e32 = natural("4294967296"); // 2^32
    assert (natural::rshift_digit(e32,1) == natural(1));
    assert (e32 == natural::lshift_digit(natural(1),1));
    a = natural("872346587326487287434732873677456478263487587361731672565438564387527344325");
    assert (natural::rshift_digit(natural::lshift_digit(a,37),37) == a);
    natural e384 = natural("39402006196394479212279040100143613805079739270465446667948293404245721771497210611414266254884915640806627990306816"); // 2(32*12)
    natural e512 = natural("13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006084096");
    assert (e384 == natural::rshift_digit(e512, 16-12));
    assert (e512 == natural::lshift_digit(e384, 16-12));
}

int main() {
    test_ordering();
    test_operate();
    test_sub();
    test_mult_one();
    test_mult_1();
    test_mult_2();
    test_shift();
    return 0;
}

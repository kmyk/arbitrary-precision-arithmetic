#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <cassert>
#include <limits>
#include <experimental/optional>

// thanks to:
// - http://idm.s9.xrea.com/factorization/multiprec/
// - http://fussy.web.fc2.com/algo/algo10-2.htm

class natural {
public:
    typedef uint32_t digit_t;
    typedef uint64_t double_digit_t;
    static const digit_t digit_max = std::numeric_limits<digit_t>::max();
    static const int  digit_digits = std::numeric_limits<digit_t>::digits;
    static const digit_t digit_highest_bit = 1 << (digit_digits-1);
    static const double_digit_t radix = (double_digit_t) digit_max + 1;
    static digit_t high_digit(double_digit_t a) { return a >> digit_digits; }
    static digit_t  low_digit(double_digit_t a) { return a; }
    static double_digit_t to_high_digit(digit_t a) { return (double_digit_t) a << digit_digits; }
    typedef std::vector<digit_t> digits_t;

public:
    natural() : digits(0) {}
    explicit natural(digit_t n) { if (n != 0) digits.push_back(n); }
    explicit natural(std::string const & s) {
        auto t = natural::from_string(s);
        *this = t ? *t : natural(0);
    }
#ifdef NDEBUG
    ~natural() = default; // non virtual
#else
    ~natural() { assert (valid()); }
#endif
private:
    explicit natural(digits_t const & _digits)
        : digits(_digits) {
        normalize();
    }
public:
    natural & operator ++ ();
    natural & operator -- ();
    natural   operator ++ (int);
    natural   operator -- (int);
    natural & operator += (natural const & n);
    natural & operator -= (natural const & n);
    natural & operator *= (natural const & n);
    friend natural operator + (natural const & a, natural const & b);
    friend natural operator - (natural const & a, natural const & b);
    friend natural operator * (natural const & a, natural const & b);
    static std::pair<natural,natural> divmod(natural const & a, natural const & b);
    friend natural operator / (natural const & a, natural const & b);
    friend natural operator % (natural const & a, natural const & b);
    friend bool operator == (natural const & a, natural const & b);
    friend bool operator != (natural const & a, natural const & b);
    friend bool operator <= (natural const & a, natural const & b);
    friend bool operator <  (natural const & a, natural const & b);
    friend bool operator >= (natural const & a, natural const & b);
    friend bool operator >  (natural const & a, natural const & b);
    explicit operator bool () const;
    long long int to_int() const;
    static std::experimental::optional<natural> from_string(std::string const & s);
    std::string to_string() const;
    friend std::istream & operator >> (std::istream & input, natural & n);
    friend std::ostream & operator << (std::ostream & output, natural const & n);
private:
    natural & operator *= (digit_t n); // for implementation
    friend natural operator * (natural const & a, digit_t b);
    bool valid() const {
        return digits.empty() or digits.back() != 0;
    }
    void normalize() {
        while (not digits.empty() and digits.back() == 0) digits.pop_back();
        assert (valid());
    }
    static natural rshift_digit(natural const & a, int b);
    static natural lshift_digit(natural const & a, int b);
    void rshift_digit(int n);
    void lshift_digit(int n);
    static std::pair<natural,natural> split_at(natural const & n, int p); // { upper, lower }
    natural drop(int n); // drop lower n digits and update the remaining upper digits
private:
    digits_t digits;
};

natural & natural::operator ++ () {
    natural::digits_t & a = digits;
    for (int i = 0; i < a.size(); ++i) {
        a[i] += 1;
        if (a[i] != 0) break;
    }
    if (a.empty() or a.back() == 0) a.resize(a.size()+1,1);
    return *this;
}
natural & natural::operator -- () {
#ifdef NDEBUG
    if (digits.empty()) return *this;
#else
    assert (not digits.empty()); // *this != 0
#endif
    natural::digits_t & a = digits;
    for (int i = 0; i < a.size(); ++i) {
        a[i] -= 1;
        if (a[i] != natural::digit_max) break;
    }
    normalize();
    return *this;
}
natural natural::operator ++ (int) {
    natural n = *this;
    ++ (*this);
    return n;
}
natural natural::operator -- (int) {
    natural n = *this;
    -- (*this);
    return n;
}

natural & natural::operator += (natural const & bn) {
    natural::digits_t & a = digits;
    natural::digits_t const & b = bn.digits;
    a.resize(std::max(a.size(), b.size()) + 1);
    bool overflow = false;
    for (int i = 0; i < b.size(); ++i) {
        natural::double_digit_t t = (natural::double_digit_t) a[i] + b[i] + (overflow ? 1 : 0);
        a[i] = t;
        assert (natural::high_digit(t) <= 1);
        overflow = natural::high_digit(t);
    }
    if (overflow) {
        for (int i = b.size(); i < a.size(); ++i) {
            a[i] += 1;
            if (a[i] != 0) break;
        }
    }
    normalize();
    return *this;
}

natural & natural::operator -= (natural const & bn) {
#ifdef NDEBUG
    if (*this <= bn) { digits.clear(); return *this; }
#else
    assert (*this >= bn);
#endif
    natural::digits_t & a = digits;
    natural::digits_t const & b = bn.digits;
    for (int i = 0; i < b.size(); ++i) {
        if (a[i] < b[i]) {
            a[i] = (natural::double_digit_t) natural::digit_max + 1 + a[i] - b[i];
            int j = i+1;
            for (; a[j] == 0; ++j) a[j] = natural::digit_max;
            assert (j < a.size());
            a[j] -= 1;
        } else {
            a[i] -= b[i];
        }
    }
    normalize();
    return *this;
}
natural & natural::operator *= (natural const & n) {
    return *this = *this * n;
}

natural operator + (natural const & a, natural const & b) {
    natural c = a;
    c += b;
    return c;
}

natural operator - (natural const & a, natural const & b) {
    natural c = a;
    c -= b;
    return c;
}

natural & natural::operator *= (digit_t b) {
    natural::digits_t & a = digits;
    natural::digit_t overflow = 0;
    for (int i = 0; i < a.size(); ++i) {
        natural::double_digit_t t = (natural::double_digit_t) a[i] * b + overflow;
        assert ((natural::double_digit_t) a[i] * b <= t); // t itself does not overflow
        a[i] = t;
        overflow = natural::high_digit(t);
    }
    if (overflow) {
        a.resize(a.size() + 1);
        a.back() = overflow;
    }
    assert (valid());
    return *this;
}
natural operator * (natural const & a, natural::digit_t b) {
    natural c = a;
    c *= b;
    return c;
}
std::pair<natural,natural> natural::split_at(natural const & n, int p) {
    natural a = n;
    natural b = a.drop(p);
    assert (natural::lshift_digit(a, p) + b == n);
    return std::make_pair(a, b);
}
natural natural::drop(int n) {
    natural::digits_t & a = digits;
    if (a.size() <= n) { natural::digits_t b; a.swap(b); return natural(b); }
    if (n == 0) { return natural(0); }
    assert (0 <= n and n <= a.size());
    natural::digits_t b(n);
    copy(a.begin(), a.begin()+n, b.begin());
    copy(a.begin()+n, a.end(),   a.begin());
    a.resize(a.size() - n);
    return natural(b);
}

natural operator * (natural const & a, natural const & b) {
    if (a.digits.empty() or b.digits.empty()) return natural(0);
    if (b.digits.size() == 1) return a * b.digits[0];
    if (a.digits.size() == 1) return b * a.digits[0];
    int p = std::max(a.digits.size(), b.digits.size()) / 2;
    assert (p != 0);
    natural a1 = a; natural a0 = a1.drop(p);
    natural b1 = b; natural b0 = b1.drop(p);
    // Karatsuba's algorithm
    const natural c2 = a1 * b1;
    const natural c0 = a0 * b0;
    const natural c1 = (a1 + a0) * (b1 + b0) - c2 - c0;
    return natural::lshift_digit(c2, 2*p) + natural::lshift_digit(c1, p) + c0;
}

// shift by sizeof(digit_t)
natural natural::rshift_digit(natural const & a, int b) {
    natural c = a;
    c.rshift_digit(b);
    return c;
}
natural natural::lshift_digit(natural const & a, int b) {
    natural c = a;
    c.lshift_digit(b);
    return c;
}

void natural::rshift_digit(int b) {
    if (0 < b) {
        natural::digits_t & a = digits;
        assert (b <= a.size());
        copy(a.begin()+b, a.end(), a.begin());
        a.resize(a.size() - b);
        assert (valid());
    } else if (b < 0) {
        lshift_digit(- b);
    }
}
void natural::lshift_digit(int b) {
    if (0 < b) {
        natural::digits_t & a = digits;
        if (a.empty()) return;
        a.resize(a.size() + b);
        rotate(a.begin(), a.end()-b, a.end());
        assert (valid());
    } else if (b < 0) {
        rshift_digit(- b);
    }
}

std::pair<natural,natural> natural::divmod(natural const & _an, natural const & bn) {
    assert (bn != natural(0));
    if (_an < bn) return std::make_pair(natural(0), _an);
    natural::digits_t a = _an.digits; // inplace
    natural::digits_t const & b = bn.digits;
    const int bl = b.size();
    // natural::digits_t q(a.size() - b.size() + 1);
    natural q;
    for (int i = a.size() - b.size(); 0 <= i; --i) {
        if (natural(a) < bn) break;
        if (1 <= i) {
            natural::double_digit_t t = (natural::to_high_digit(a[i+bl-1]) + a[i+bl-2]) / b[bl-1];
            natural x = natural(natural::digits_t({ natural::low_digit(t), natural::high_digit(t) }));
            natural y = natural::lshift_digit(natural(1), i-1);
            x *= y;
            while (natural(a) < bn * x) {
                x -= y;
            }
            q += x;
            a = (natural(a) - bn * x).digits;
        } else {
            assert (a.back() != 0);
            assert (a.size() == b.size());
            assert (1 <= a.size());
            int l = a.size();
            natural::digit_t t = l == 1
                ? a[0] / b[0]
                : (natural::to_high_digit(a[l-1]) + a[l-2]) / (natural::to_high_digit(b[l-1]) + b[l-2]) ;
            natural x = natural(t);
            while (natural(a) < bn * x) {
                -- x;
            }
            q += x;
            a = (natural(a) - bn * x).digits;
        }
    }
    assert (_an == q * bn + natural(a));
    assert (natural(a) < bn);
    return std::make_pair(q, natural(a));
}

natural operator / (natural const & a, natural const & b) {
    return natural::divmod(a,b).first;
}
natural operator % (natural const & a, natural const & b) {
    return natural::divmod(a,b).second;
}

bool operator == (natural const & a, natural const & b) {
    assert (a.valid());
    assert (b.valid());
    return a.digits == b.digits;
}
bool operator != (natural const & a, natural const & b) {
    assert (a.valid());
    assert (b.valid());
    return a.digits != b.digits;
}

bool operator <= (natural const & an, natural const & bn) {
    assert (an.valid());
    assert (bn.valid());
    natural::digits_t const & a = an.digits;
    natural::digits_t const & b = bn.digits;
    if (a.size() < b.size()) {
        return true;
    } else if (a.size() > b.size()) {
        return false;
    } else {
        for (int i = a.size()-1; 0 <= i; --i) {
            if (a[i] != b[i]) {
                return a[i] < b[i];
            }
        }
        return true; // equal
    }
}
bool operator <  (natural const & a, natural const & b) {
    return a != b and a <= b;
}
bool operator >= (natural const & a, natural const & b) {
    return not (a < b);
}
bool operator >  (natural const & a, natural const & b) {
    return not (a <= b);
}

natural::operator bool () const {
    return not digits.empty();
}
long long int natural::to_int() const {
    natural::double_digit_t t = 0;
    if (1 < digits.size()) t += natural::to_high_digit(digits[1]);
    if (0 < digits.size()) t += digits[0];
    return t;
}

std::experimental::optional<natural> natural::from_string(std::string const & s) {
    natural a = natural(0);
    for (int i = 0; i < s.length(); ++i) {
        if (not isdigit(s[i])) return std::experimental::optional<natural>();
        a *= 10;
        a += natural(s[i]-'0');
    }
    return std::experimental::optional<natural>(a);
}
std::string natural::to_string() const {
    std::string s;
    natural a = *this;
    while (a != natural(0)) {
        auto p = natural::divmod(a, natural(10));
        assert (0 <= p.second.to_int() and p.second.to_int() <= 9);
        s += p.second.to_int() + '0';
        a  = p.first;
    }
    if (s.empty()) s += '0';
    reverse(s.begin(), s.end());
    return s;
}

std::istream & operator >> (std::istream & input, natural & n) {
    std::string s;
    input >> s;
    if (not input.fail()) {
        auto t = natural::from_string(s);
        n = t ? *t : natural(0);
        if (not t) input.setstate(std::ios::failbit);
    }
    return input;
}
std::ostream & operator << (std::ostream & output, natural const & n) {
    return output << n.to_string();
}

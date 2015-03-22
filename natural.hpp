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
    bool valid() const {
        return digits.empty() or digits.back() != 0;
    }
    void normalize() {
        while (not digits.empty() and digits.back() == 0) digits.pop_back();
        assert (valid());
    }
    static natural rshift_digit(natural const & a, int b);
    static natural lshift_digit(natural const & a, int b);
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

natural & natural::operator -= (natural const & n) {
    return *this = *this - n;
}
natural & natural::operator *= (natural const & n) {
    return *this = *this * n;
}

natural operator + (natural const & a, natural const & b) {
    natural c = a;
    c += b;
    return c;
}

natural operator - (natural const & m, natural const & n) {
#ifdef NDEBUG
    if (m <= n) return natural(0);
#else
    assert (m >= n);
#endif
    natural::digits_t a = m.digits;
    natural::digits_t const & b = n.digits;
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
    return natural(a);
}

// O(length m * length n)
natural operator * (natural const & m, natural const & n) {
    natural::digits_t const & a = m.digits;
    natural::digits_t const & b = n.digits;
    natural::digits_t c(a.size() + b.size());
    for (int i = 0; i < a.size(); ++i) {
        natural::digit_t overflow = 0;
        for (int j = 0; j < b.size(); ++j) {
            natural::double_digit_t t = 0; // t itself does not overflow
            t += c[i+j];
            t += (natural::double_digit_t) a[i] * b[j];
            t += overflow;
            c[i+j] = t;
            overflow = natural::high_digit(t);
        }
        assert (natural::high_digit((natural::double_digit_t) c[i+b.size()] + overflow) == 0); // does not overflow
        c[i+b.size()] += overflow;
    }
    return natural(c);
}

// shift by sizeof(digit_t)
natural natural::rshift_digit(natural const & n, int b) {
    natural::digits_t const & a = n.digits;
    natural::digits_t c;
    if (0 < b) {
        c.resize(a.size() - b);
        for (int i = 0; i < c.size(); ++i) {
            assert (i+b < a.size());
            c[i] = a[i+b];
        }
    } else if (b < 0) {
        b = abs(b);
        c.resize(a.size() + b);
        for (int i = 0; i < a.size(); ++i) {
            assert (i+b < c.size());
            c[i+b] = a[i];
        }
    } else {
        c = a;
    }
    return natural(c);
}
natural natural::lshift_digit(natural const & a, int b) {
    return rshift_digit(a, -b);
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
        a *= natural(10);
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

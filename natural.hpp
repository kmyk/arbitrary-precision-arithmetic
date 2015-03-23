#pragma once
#include <iostream>
#include <vector>
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

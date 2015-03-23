#pragma once
#include "natural.hpp"

class integer {
public:
    integer() : sign(false), nat() {}
    /* implicit */ integer(natural const & n_) : sign(false), nat(n_) {}
    integer(bool sign_, natural const & n_) : sign(sign_), nat(n_) { normalize(); }
    explicit integer(natural::digit_t n_) : sign(false), nat(natural(n_)) {}
#ifdef NDEBUG
    ~integer() = default; // non virtual
#else
    ~integer() { assert (valid()); }
#endif
public:
    integer & operator ++ ();
    integer & operator -- ();
    integer   operator ++ (int);
    integer   operator -- (int);
    integer & operator += (integer const & n);
    integer & operator -= (integer const & n);
    integer & operator *= (integer const & n);
    friend integer operator + (integer const & a, integer const & b);
    friend integer operator - (integer const & a);
    friend integer operator - (integer const & a, integer const & b);
    friend integer operator * (integer const & a, integer const & b);
    friend integer operator / (integer const & a, integer const & b);
    friend bool operator == (integer const & a, integer const & b);
    friend bool operator != (integer const & a, integer const & b);
    friend bool operator <= (integer const & a, integer const & b);
    friend bool operator <  (integer const & a, integer const & b);
    friend bool operator >= (integer const & a, integer const & b);
    friend bool operator >  (integer const & a, integer const & b);
    explicit operator bool () const;
    long long int to_int() const;
    friend integer abs(integer const & n);
    natural to_natural() const;
    std::string to_string() const;
    static std::experimental::optional<integer> from_string(std::string const & s);
    friend std::istream & operator >> (std::istream & input, integer & n);
    friend std::ostream & operator << (std::ostream & output, integer const & n);
private:
    bool valid() const {
        return nat != natural(0) or sign == false;
    }
    void normalize() {
        if (nat == natural(0)) sign = false;
        assert (valid());
    }
private:
    bool sign; // is_negative
    natural nat;
};

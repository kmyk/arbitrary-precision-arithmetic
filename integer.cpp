#include "integer.hpp"

integer & integer::operator ++ () {
    if (not sign) {
        ++ nat;
    } else {
        -- nat;
    }
    normalize();
    return *this;
}
integer & integer::operator -- () {
    if (not sign) {
        if (nat == natural(0)) {
            sign = true;
            nat = natural(1);
        } else {
            -- nat;
        }
    } else {
        ++ nat;
    }
    return *this;
}
integer   integer::operator ++ (int) {
    integer n = *this;
    ++ (*this);
    return n;
}
integer   integer::operator -- (int) {
    integer n = *this;
    -- (*this);
    return n;
}
integer & integer::operator += (integer const & n) {
    if (sign == n.sign) {
        nat += n.nat;
    } else {
        if (nat >= n.nat) {
            nat -= n.nat;
        } else {
            sign = n.sign;
            nat = n.nat - nat;
        }
        normalize();
    }
    return *this;
}
integer & integer::operator -= (integer const & n) {
    return *this += integer(not n.sign, n.nat);
}
integer & integer::operator *= (integer const & n) {
    nat *= n.nat;
    sign = sign != n.sign;
    normalize();
    return *this;
}
integer operator + (integer const & a, integer const & b) {
    integer c = a;
    c += b;
    return c;
}
integer operator - (integer const & a) {
    integer n = integer(not a.sign, a.nat);
    n.normalize();
    return n;
}
integer operator - (integer const & a, integer const & b) {
    integer c = a;
    c -= b;
    return c;
}
integer operator * (integer const & a, integer const & b) {
    integer c = a;
    c *= b;
    return c;
}
integer operator / (integer const & a, integer const & b) {
    integer n = integer(a.sign != b.sign, a.nat / b.nat);
    n.normalize();
    return n;
}
bool operator == (integer const & a, integer const & b) {
    assert (a.valid());
    assert (b.valid());
    return a.sign == b.sign and a.nat == b.nat;
}
bool operator != (integer const & a, integer const & b) {
    assert (a.valid());
    assert (b.valid());
    return a.sign != b.sign or a.nat != b.nat;
}
bool operator <= (integer const & a, integer const & b) {
    assert (a.valid());
    assert (b.valid());
    if (a.sign == b.sign) {
        if (not a.sign) {
            return a.nat <= b.nat;
        } else {
            return b.nat <= a.nat;
        }
    } else {
        return a.sign;
    }
}
bool operator <  (integer const & a, integer const & b) {
    return a != b and a <= b;
}
bool operator >= (integer const & a, integer const & b) {
    return not (a < b);
}
bool operator >  (integer const & a, integer const & b) {
    return not (a <= b);
}
integer::operator bool () const {
    assert (valid());
    return not not nat;
}

long long int integer::to_int() const {
    return (sign ? -1 : 1) * nat.to_int();
}
integer abs(integer const & n) {
    return integer(n.nat);
}
natural integer::to_natural() const {
    return nat;
}
std::string integer::to_string() const {
    return (sign ? "-" : "") + nat.to_string();
}
std::experimental::optional<integer> integer::from_string(std::string const & s) {
    using namespace std::experimental;
    if (s.empty()) return optional<integer>();
    bool sign = false;
    optional<natural> n;
    if (s.front() == '-') {
        sign = true;
        n = natural::from_string(s.substr(1));
    } else {
        n = natural::from_string(s);
    }
    return n ? optional<integer>(integer(sign, *n)) : optional<integer>();
}
std::istream & operator >> (std::istream & input, integer & n) {
    std::string s;
    input >> s;
    if (not input.fail()) {
        auto t = integer::from_string(s);
        n = t ? *t : integer(0);
        if (not t) input.setstate(std::ios::failbit);
    }
    return input;
}
std::ostream & operator << (std::ostream & output, integer const & n) {
    return output << n.to_string();
}

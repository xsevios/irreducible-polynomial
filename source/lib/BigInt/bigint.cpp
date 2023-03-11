#include <bits/stdc++.h>

#include "fft.h"
#include "bigint.h"

using namespace std;

bigint::bigint(long long v) { *this = v; }

bigint &bigint::operator=(long long v) {
    sign = v < 0 ? -1 : 1;
    v *= sign;
    z.clear();
    for (; v > 0; v = v / base)
        z.push_back((int)(v % base));
    return *this;
}

bigint::bigint(const string &s) { read(s); }

bigint &bigint::operator+=(const bigint &other) {
    if (sign == other.sign) {
        for (int i = 0, carry = 0; i < other.z.size() || carry; ++i) {
            if (i == z.size())
                z.push_back(0);
            z[i] += carry + (i < other.z.size() ? other.z[i] : 0);
            carry = z[i] >= base;
            if (carry)
                z[i] -= base;
        }
    } else if (other != 0 /* prevent infinite loop */) {
        *this -= -other;
    }
    return *this;
}

bigint &bigint::operator-=(const bigint &other) {
    if (sign == other.sign) {
        if ((sign == 1 && *this >= other) || (sign == -1 && *this <= other)) {
            for (int i = 0, carry = 0; i < other.z.size() || carry; ++i) {
                z[i] -= carry + (i < other.z.size() ? other.z[i] : 0);
                carry = z[i] < 0;
                if (carry)
                    z[i] += base;
            }
            trim();
        } else {
            *this = other - *this;
            this->sign = -this->sign;
        }
    } else {
        *this += -other;
    }
    return *this;
}

bigint &bigint::operator*=(int v) {
    if (v < 0)
        sign = -sign, v = -v;
    for (int i = 0, carry = 0; i < z.size() || carry; ++i) {
        if (i == z.size())
            z.push_back(0);
        long long cur = (long long)z[i] * v + carry;
        carry = (int)(cur / base);
        z[i] = (int)(cur % base);
    }
    trim();
    return *this;
}

bigint bigint::operator*(int v) const { return bigint(*this) *= v; }

bigint bigint::operator/(const bigint &v) const { return divmod(*this, v).first; }

bigint bigint::operator%(const bigint &v) const { return divmod(*this, v).second; }

bigint &bigint::operator/=(int v) {
    if (v < 0)
        sign = -sign, v = -v;
    for (int i = (int)z.size() - 1, rem = 0; i >= 0; --i) {
        long long cur = z[i] + rem * (long long)base;
        z[i] = (int)(cur / v);
        rem = (int)(cur % v);
    }
    trim();
    return *this;
}

bigint bigint::operator/(int v) const { return bigint(*this) /= v; }

int bigint::operator%(int v) const {
    if (v < 0)
        v = -v;
    int m = 0;
    for (int i = (int)z.size() - 1; i >= 0; --i)
        m = (int)((z[i] + m * (long long)base) % v);
    return m * sign;
}

bigint &bigint::operator*=(const bigint &v) {
    *this = *this * v;
    return *this;
}

bigint &bigint::operator/=(const bigint &v) {
    *this = *this / v;
    return *this;
}

bigint &bigint::operator%=(const bigint &v) {
    *this = *this % v;
    return *this;
}

bool bigint::operator<(const bigint &v) const {
    if (sign != v.sign)
        return sign < v.sign;
    if (z.size() != v.z.size())
        return z.size() * sign < v.z.size() * v.sign;
    for (int i = (int)z.size() - 1; i >= 0; i--)
        if (z[i] != v.z[i])
            return z[i] * sign < v.z[i] * sign;
    return false;
}

bool bigint::operator>(const bigint &v) const { return v < *this; }
bool bigint::operator<=(const bigint &v) const { return !(v < *this); }
bool bigint::operator>=(const bigint &v) const { return !(*this < v); }
bool bigint::operator==(const bigint &v) const { return sign == v.sign && z == v.z; }
bool bigint::operator!=(const bigint &v) const { return !(*this == v); }

void bigint::trim() {
    while (!z.empty() && z.back() == 0)
        z.pop_back();
    if (z.empty())
        sign = 1;
}

bool bigint::isZero() const { return z.empty(); }

bigint bigint::abs() const { return sign == 1 ? *this : -*this; }

long long bigint::longValue() const {
    long long res = 0;
    for (int i = (int)z.size() - 1; i >= 0; i--)
        res = res * base + z[i];
    return res * sign;
}

void bigint::read(const string &s) {
    sign = 1;
    z.clear();
    int pos = 0;
    while (pos < s.size() && (s[pos] == '-' || s[pos] == '+')) {
        if (s[pos] == '-')
            sign = -sign;
        ++pos;
    }
    for (int i = (int)s.size() - 1; i >= pos; i -= base_digits) {
        int x = 0;
        for (int j = max(pos, i - base_digits + 1); j <= i; j++)
            x = x * 10 + s[j] - '0';
        z.push_back(x);
    }
    trim();
}

bigint bigint::operator*(const bigint &v) const {
    if (min(z.size(), v.z.size()) < 150)
        return mul_simple(v);
    bigint res;
    res.sign = sign * v.sign;
    res.z = multiply_bigint(convert_base(z, base_digits, fft_base_digits),
                            convert_base(v.z, base_digits, fft_base_digits), fft_base);
    res.z = convert_base(res.z, fft_base_digits, base_digits);
    res.trim();
    return res;
}

bigint bigint::mul_simple(const bigint &v) const {
    bigint res;
    res.sign = sign * v.sign;
    res.z.resize(z.size() + v.z.size());
    for (int i = 0; i < z.size(); ++i)
        if (z[i])
            for (int j = 0, carry = 0; j < v.z.size() || carry; ++j) {
                long long cur = res.z[i + j] + (long long)z[i] * (j < v.z.size() ? v.z[j] : 0) + carry;
                carry = (int)(cur / base);
                res.z[i + j] = (int)(cur % base);
            }
    res.trim();
    return res;
}

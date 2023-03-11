#ifndef _FFT_H
#define _FFT_H

#include <bits/stdc++.h>

using namespace std;

// Fast Fourier transform
// https://cp-algorithms.com/algebra/fft.html
// https://drive.google.com/file/d/1B9BIfATnI_qL6rYiE5hY9bh20SMVmHZ7/view

using cpx = complex<double>;

void ensure_capacity(int min_capacity);
void fft(vector<cpx> &z, bool inverse);
vector<int> multiply_bigint(const vector<int> &a, const vector<int> &b, int base);
vector<int> multiply_mod(const vector<int> &a, const vector<int> &b, int m);

#endif

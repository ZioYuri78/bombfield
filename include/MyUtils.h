#pragma once
#include <cmath>

namespace MyUtils {

	inline int Min(int _a, int _b) {
		return _a < _b ? _a : _b;
	}

	inline int Max(int _a, int _b) {
		return _a > _b ? _a : _b;
	}

	inline int Clamp(int _v, int _min, int _max) {		
		return Min(Max(_v, _min), _max);
	}

	inline float Clamp(float _v, float _min, float _max) {
		return fmin(fmax(_v, _min), _max);
	}

	// [0,m]->[0,M]
	inline int Remap(int v, int m, int M) {
		return (v/(float)m)*M;
	}

	// [a,b]->[A,B]
	inline int RemapRange(int n, int a, int A, int b, int B) {
		int delta_n = n-a;
		int delta_in = b-a;
		int delta_out = B-A;
		int result = ((delta_n/(float)delta_in) * delta_out) + A;
		return result;
	}
}

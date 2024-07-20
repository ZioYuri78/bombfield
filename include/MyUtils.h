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
}

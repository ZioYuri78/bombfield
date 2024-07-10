#pragma once
#include <cmath>

namespace MyUtils {

	inline int Min(int a, int b) {
		return a < b ? a : b;
	}

	inline int Max(int a, int b) {
		return a > b ? a : b;
	}

	inline int Clamp(int v, int min, int max) {
		return Min(Max(v, min), max);
	}

	inline float Clamp(float v, float min, float max) {
		return fmin(fmax(v, min), max);
	}
}

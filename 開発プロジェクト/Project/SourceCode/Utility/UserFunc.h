#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <numeric>

/* --- �錾 ---- */

class UserFunc
{
public:

	template <class T>
	static T Loop(const T& dst, const T& min, const T& max)
	{
		T tmp = dst;
		if (tmp < min) tmp = max;
		if (tmp > max) tmp = min;
		return tmp;
	}
};

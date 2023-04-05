#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <cstdint>

/* --- �錾 ---- */

/// <summary>
/// �O�i�����R���p�N�g�Ɉ����N���X�������ɔz�񐔂��w�肷��K�v������
/// </summary>
/// <typeparam name="s"></typeparam>
template<size_t s>
class Trits
{
public:

	Trits() = default;

	template<class...Args>
	Trits(Args... args)
		: blocks()
	{
		static_assert(sizeof...(args) == s, "The size of the array and the number of lements in the argument must be the same.");

		for (size_t ei = 0, bi = 0; uint8_t t : { args... })
		{
			blocks[bi] += t * static_cast<int>(pow(3, ei));
			// ��u���b�N�ɂT�̃f�[�^���i�[�����
			if (ei++ >= 4) { ei = 0; ++bi; }
		}
	}

	uint8_t operator[](size_t i) const
	{
		_ASSERT_EXPR(i < s, L"�z��O�ւ̃A�N�Z�X");
		return (blocks[i / 5ULL] / static_cast<uint8_t>(pow(3, i % 5ULL))) % 3ULL;
	}

	uint8_t Get(size_t i)
	{
		_ASSERT_EXPR(i < s, L"�z��O�ւ̃A�N�Z�X");
		return (blocks[i / 5ULL] / static_cast<uint8_t>(pow(3, i % 5ULL))) % 3ULL;
	}

	void Set(size_t i, uint8_t t)
	{
		_ASSERT_EXPR(i < s, L"�z��O�ւ̃A�N�Z�X");
		_ASSERT_EXPR(t < 3, L"�O�i���Ȃ̂�0~2�̒l");

		size_t b{ i / 5ULL };
		int p{ static_cast<int>(pow(3, i % 5ULL)) };
		blocks[b] -= Get(i) * p; // �t���O�����̒l���[����
		blocks[b] += t * p;   // �t���O�𗧂Ă�
	}

private:

	uint8_t blocks[(s / 5ULL) + 1ULL];
};

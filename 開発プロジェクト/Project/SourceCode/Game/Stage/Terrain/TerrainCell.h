#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <cstdint>

/* --- �錾 ---- */

class TerrainCell
{
public:

	enum State : uint8_t
	{
		None     = 0,	   // �Z���Ȃ�
		Traces   = 1 << 0, // �����ꂩ�̐���Ő����Z���ɂȂ��Ă����B
		Alive    = 1 << 1, // �������
		Dead	 = 1 << 2, // ���S���
		Obstacle = 1 << 3, // ����̃Z���̊����󂯂Ȃ����
	};

	TerrainCell()
		: state(Dead) { }

	State& operator=(uint8_t rhs) { return state = static_cast<State>(rhs); }
	State& operator|=(uint8_t rhs) { return state = static_cast<State>(state | rhs); }
	State& operator&=(uint8_t rhs) { return state = static_cast<State>(state & rhs); }
	State operator|(uint8_t rhs) const { return static_cast<State>(state | rhs); }
	State operator&(uint8_t rhs) const { return static_cast<State>(state & rhs); }
	State operator~() const { return static_cast<State>(~state); }

	/// <summary>
	/// �}�X�N�̕����̃r�b�g�̂ݏ���������
	/// </summary>
	void Set(State src, uint8_t mask)
	{
		state = static_cast<State>((state & ~mask) | src);
	}

	/// <summary>
	/// �}�X�N�̕����̃r�b�g�݂̂Ŕ�r���s��
	/// </summary>
	bool Compare(State src, uint8_t mask) const
	{
		return ((state & mask) == src);
	}

	State Get() const { return state; }

private:

	State state;
};

#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <cstdint>

/* --- �錾 ---- */

class TileMaterial
{
public:

	using Index = uint8_t;

	enum class Floor : Index
	{
		Default = 0,
		Warp,
		ElementCount, // �v�f��
	};

	enum class Wall : Index
	{
		Default = 0,
		ElementCount, // �v�f��
	};

	TileMaterial()
		: index() {}

	TileMaterial(Floor rhs)
		: index(static_cast<Index>(rhs)) {}
	TileMaterial(Wall rhs)
		: index(static_cast<Index>(rhs)) {}

	Index& operator=(Floor rhs) { return index = static_cast<Index>(rhs); }
	Index& operator=(Wall rhs) {  return index = static_cast<Index>(rhs); }

	bool operator==(Floor rhs) { return index == static_cast<Index>(rhs); }
	bool operator==(Wall rhs) {  return index == static_cast<Index>(rhs); }


	Index index;
};


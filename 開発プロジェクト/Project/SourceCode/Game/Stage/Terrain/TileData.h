#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <cstdint>

/* --- �O���錾 ---- */


/* --- �錾 ---- */

class TileData
{
public:

	TileData()
		: state(), mesh(), entity(), collide(), other(){}

	enum State : uint8_t
	{
		S_None = 0,
		S_PlayerSpawn, // �t���A�ړ����Ƀv���C���[���z�u�����ʒu
		S_EnemySpawn,  // �t���A�ړ����ɓG�����������ʒu
		S_ItemSpawn,   // �t���A�ړ����ɃA�C�e�������������ʒu
		S_NextFloor,   // ���̃^�C���̏�Ƀv���C���[������Ǝ��̃t���A�Ɉړ�����
	};

	// �`�掞�ɗp���郁�b�V��
	enum MeshID : uint8_t
	{
		M_None, 
		M_Floor,
		M_Wall,
		M_WallCeiling, // �㕔�݂̂̕ǁA�`�敉�׌y���̂��߂Ɏ��肪�ǂŕ`�悷��K�v�̂Ȃ������𖳂��������b�V��
	};

	// �o������G�E�A�C�e����ID
	enum EntityID : uint8_t
	{
		// �GID
		E_None,
		E_Random,   // ����ȓG�������ă����_���ŏo��
		E_Skeleton,
		E_Wolf,
		E_Spider,

		E_SeparateExtra, // ����ȓG�Ƃ̋�؂�
		E_Boss,
		E_End, 
		
		Separate = 127,// �G�ƃA�C�e���𕪂��鋫�E��

		// �A�C�e��ID
		I_Random,
		I_LifePot,
		I_FirePot,
		I_SparkPot,
		I_PoisonPot,
		I_End,

	};

	enum CollideFlag : uint8_t
	{
		C_None = 0,
		C_IsCollide    = 1 << 0,
		C_CollideUp    = 1 << 1,   // -Y
		C_CollideRight = 1 << 2,  // +X
		C_CollideDown  = 1 << 3,  // +Y
		C_CollideLeft  = 1 << 4,  // -X
	};
	
	enum OtherFlag : uint16_t
	{
		O_None = 0,
		O_EnemyHasWeapon = 1 << 0, // ���̃^�C������o������G�͕������������i��������Ă�G�̏ꍇ�̂݁j
	};

	void AddCollideFlag(uint8_t rhs) { collide = static_cast<CollideFlag>(collide | rhs); }
	void AddOtherFlag(uint16_t  rhs) { other = static_cast<OtherFlag>(other| rhs); }

	State       state;
	MeshID      mesh;
	EntityID    entity;
	CollideFlag collide;
	OtherFlag   other;
};

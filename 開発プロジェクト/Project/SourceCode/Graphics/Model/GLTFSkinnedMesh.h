#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "GLTFModel.h"

#include "../Resource/ConstantBuffer.h"

/* --- �錾 ---- */


class GLTFSkinnedMesh : public GLTFModel
{
public:

	GLTFSkinnedMesh(ID3D11Device* device, const char* filepath);
	virtual ~GLTFSkinnedMesh();

	/// <param name="shader">�V�F�[�_��ݒ肵�Ȃ��ꍇ�͔ėp�V�F�[�_�[�𗘗p����</param>
	void Render(ID3D11DeviceContext* dc, Shader* shader = nullptr) const override;

	/// <summary>
	/// �m�[�h�ɃA�j���V�������ʂ�K�p����B
	/// ���f���f�[�^�ɐݒ肳��Ă���A�j���[�V�������Q�Ƃ���B
	/// </summary>
	/// <param name="animation_index">�Q�Ƃ��郂�f���f�[�^�̃A�j���V�����ԍ�</param>
	/// <param name="time">�o�ߎ���</param>
	/// <param name="loop">���[�v�����邩</param>
	/// <returns>�A�j���[�V�������Đ��I�����Ă��邩�A�A�j���[�V���������݂��Ă��Ȃ��ꍇ��false��Ԃ�</returns>
	bool Animate(size_t animation_index, float time, bool loop);

	/// <summary>
	/// �m�[�h�ɃA�j���V�������ʂ�K�p����B
	/// �O���A�j���[�V�������Q�Ƃ���B
	/// </summary>
	/// <param name="animation"></param>
	/// <param name="time">�A�j���[�V�����̎��ԁB�A�j���[�V�������I�������炻�̏I�����Ԃ�ݒ肷��</param>
	/// <param name="has_played">�A�j���[�V�������I��������true��ݒ肷��</param>
	void Animate(const GLTFModelData::Animation& animation, float& time, bool& has_played);

	bool Animate(size_t animation_index, float& time);
	void AnimateLoop(size_t animation_index, float& time, bool& has_looped);

	/// <summary>
	/// �m�[�h�ɃA�j���V�������ʂ�K�p����
	/// �O���A�j���[�V�������Q�Ƃ���B
	/// </summary>
	/// <param name="animation"></param>
	/// <param name="time">�A�j���[�V�����̎��ԁB�A�j���[�V���������[�v������A�I�����ԂŊ������]��̎��Ԃ�ݒ肷��</param>
	/// <param name="has_looped">�A�j���[�V���������[�v������true��ݒ肷��</param>
	void AnimateLoop(const GLTFModelData::Animation& animation, float& time, bool& has_looped);

	void BlendAnimation(size_t index1, size_t index2, float time1, float time2, float interpolation);

	float GetAnimationLength(size_t index) const;

	/// <summary>
	/// �A�j���V�����m�[�h�̎擾
	/// </summary>
	/// <returns></returns>
	const std::vector<std::shared_ptr<GLTFModelData::Node>>& GetAnimatedNodes() const;

	/// <summary>
	/// �����̖��O�̃A�j���V�����m�[�h�̃��[���h���W��Ԃ�(�����蔻��̓z�p�Ƀ��[���h���s�ړ����v�Z�Ɋ܂܂�Ă��Ȃ�)
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	Vector3 GetAnimatedNodeWorldSR(const char* name) const;

	std::shared_ptr<GLTFModelData::Node> FindAnimatedNode(const char* node_name);

private:

	/// <summary>
	/// �m�[�h�C���f�b�N�X�̃��b�V����`�悷��ċA�֐�
	/// </summary>
	void DrawMeshRecursive(ID3D11DeviceContext* dc, int32_t node_index) const;
	
	// �A�j���[�V�������s���A���_�V�F�[�_�[�ɂăA�j���[�V�������ʂ𔽉f�����邽�߂̃m�[�h�z��
	std::vector<std::shared_ptr<GLTFModelData::Node>> animated_nodes; 

protected:

	struct CBufferPrimitive
	{
		static constexpr uint32_t slot{ 0 };

		Matrix world;
		int material{ -1 };
		int has_tangent{ 0 }; // ��������int�^
		int skin{ -1 };
		int pad;
	};

	static constexpr size_t PrimitiveMaxJoints{ 512 };

	/// <summary>
	/// �{�[���s��萔�o�b�t�@
	/// </summary>
	struct CBufferJoint
	{
		static constexpr uint32_t slot{ 2 };
		Matrix matrices[PrimitiveMaxJoints];
	};

	ConstantBuffer<CBufferPrimitive> primitive_cb; // �v���~�e�B�u�萔�o�b�t�@
	ConstantBuffer<CBufferJoint>	 joint_cb; // �{�[���s��萔�o�b�t�@
};

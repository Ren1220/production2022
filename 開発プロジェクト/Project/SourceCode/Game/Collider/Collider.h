#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <functional>
#include <unordered_map>

#include "Arithmetic/Arithmetic.h"

#include "../Component/Component.h"
#include "Graphics/Model/GLTFModelData.h"

/* --- �O���錾 ---- */

class ColliderAABB;
class ColliderSphere;
class ColliderCapsule;

/* --- �錾 ---- */

using TargetGroups = std::vector<std::string>;

class Collider: public Component
{
public:

	/// <summary>
	/// �R���C�_�[�̎��
	/// </summary>
	enum class Type
	{
		AABB, Sphere, Capsule
	};

	enum class Target
	{
		None = 0x00000000,

	};

	Collider(const char* name, const Vector3& offset, const Vector4& color, bool is_intersect_only)
		: name(name), standard_point(), offset(offset), color(color), reference_node(), show_debug_primitive(false), is_intersect_only(is_intersect_only), is_event_executed(false)
	{
	}

	~Collider() override = default;


	/// <summary>
	/// �J�n����
	/// </summary>
	void Start() override;
	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="elapsed_time">�o�ߎ���</param>
	void Update(float elapsed_time) override;
	/// <summary>
	/// ���̃A�N�^�̃R���C�_�[�ƌ��������Ƃ��Ɏ��s�����֐�
	/// </summary>
	/// <param name="opponent_actor"></param>
	void OnHitEvent(const std::shared_ptr<Actor>& opponent_actor) override;
	/// <summary>
	/// ImGui�`�揈��
	/// </summary>
	void ImGuiRender();

	/// <summary>
	/// ���̃R���C�_�[�ɐݒ肳�ꂽ�R�[���o�b�N�֐������s����B
	/// </summary>
	/// <param name="opponent"></param>
	void OnHitEventCallback(const std::shared_ptr<Actor>& opponent);

public:

	static Type GetColliderType(const size_t& hash_code);			// �n�b�V���l�̃R���C�_�[���擾����
	size_t GetHash() const { return typeid(*this).hash_code(); } 	// ���g�̃n�b�V���l���擾����
	const Vector3& GetStandardPoint() const { return standard_point; }				// ��̍��W
	const Vector3& GetOffset() const { return offset; }				// ��̍��W����̃I�t�Z�b�g

	void SetReferenceNode(const char* node_name);
	void SetOffset(const Vector3& o) { offset = o; }

	/// <summary>
	/// �n�b�V�����X�g����̂Ƃ�Intersect()�ŌĂ΂��
	/// </summary>
	static void InitializeHashList();
	/// <summary>
	/// �����̃R���C�_�[�Ƃ̌�������
	/// </summary>
	bool Intersect(const std::shared_ptr<Collider>& opponent_collider);
	/// <summary>
	/// �����̃R���C�_�[�Ƃ̌�������
	/// </summary>
	/// <param name="opponent_collider">������s���Ώ�</param>
	/// <param name="out_normal">�Փ˂����������i�[</param>
	/// <param name="out_penetration">�H�����ݗʂ��i�[</param>
	/// <returns></returns>
	bool Intersect(const std::shared_ptr<Collider>& opponent_collider, float& out_penetration);

	virtual bool IntersectSphere(const Vector3& sphere_position, float sphere_radius) const = 0;

	/// <summary>
	/// ���������݂̂ŏՓˏ������s��Ȃ�
	/// </summary>
	/// <returns></returns>
	bool IsIntersectOnly() const { return is_intersect_only; }
	void SetIntersectOnly(bool b) { is_intersect_only = b; }

	void SetShowDebugPrimitive(bool b) { show_debug_primitive = b; }

	/// <summary>
	/// OnHitEvent�ŌĂ΂��R�[���o�b�N�֐��B�ꎞ�I�ł�������A���̃R���C�_�[�݂̂̌����Ŋ֐������s�������Ƃ��Ɏg���B
	/// </summary>
	/// <param name="func_name"></param>
	/// <param name="callback">�����́@���g�̃A�N�^�@�Ɓ@���������R���C�_�[�����A�N�^</param>
	void AddOnHitEventCallBack(const char* func_name, const std::function<void(const std::shared_ptr<Actor>&, const std::shared_ptr<Actor>&)>& callback);
	void RemoveOnHitEventCallBack(const char* func_name);
	void ClearOnHitEventCallBack();

	bool IsEventExecuted() const { return is_event_executed; }
	void SetEventExecuted(bool b) { is_event_executed = b; }

private:

	/// <summary>
	/// AABB�Ƃ̌�������
	/// </summary>
	virtual bool IntersectAABB(const std::shared_ptr<ColliderAABB>& opponent, float& out_penetration) = 0;
	/// <summary>
	/// ���Ƃ̌�������
	/// </summary>
	virtual bool IntersectSphere(const std::shared_ptr<ColliderSphere>& opponent, float& out_penetration) = 0;
	///// <summary>
	///// �����~���Ƃ̌�������
	///// </summary>
	//virtual bool IntersectVCylinder(const std::shared_ptr<ColliderVCylinder>& opponent, float& out_penetration) = 0;
	/// <summary>
	/// �J�v�Z���Ƃ̌�������
	/// </summary>
	virtual bool IntersectCapsule(const std::shared_ptr<ColliderCapsule>& opponent, float& out_penetration) = 0;

private:

	static std::map<size_t, Type> collider_type_hash_list; // �e�R���C�_�[�̃n�b�V���l

protected:

	std::string name;

	Vector3 standard_point;						 // ��_
	Vector3 offset;								 // ��̍��W����̃I�t�Z�b�g
	Vector4 color;								 // �f�o�b�N�`�悷��ۂ̐F
	std::weak_ptr<GLTFModelData::Node> reference_node; // ���W���Q�Ƃ���m�[�h�̃A�h���X

	bool show_debug_primitive;					 // �f�o�b�O�p�v���~�e�B�u�̕`����s����
	bool is_intersect_only;						 // ��������̂�

	// OnHitEvent�ŌĂ΂��R�[���o�b�N�֐��B�ꎞ�I�ł�������A���̃R���C�_�[�݂̂̌����Ŋ֐������s�������Ƃ��Ɏg���B
	// �����́@���g�̃A�N�^�@�Ɓ@���������R���C�_�[�����A�N�^
	std::unordered_map<std::string, std::function<void(const std::shared_ptr<Actor>&, const std::shared_ptr<Actor>&)>> on_hit_event_call_back;

	bool is_event_executed; // ColliderManager�Ŏg�����
};

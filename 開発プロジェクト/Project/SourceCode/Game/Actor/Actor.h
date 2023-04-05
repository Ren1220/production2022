#pragma once

/* --- �T�v ---- */

// ��{�I�ȕ����̐���
// 
//   Actor�ɗl�X�ȋ@�\��ǉ�����Component�z����������AActorManager�ŊǗ����邱�ƂŁA
//   �Q�[�����ɓo�ꂷ��I�u�W�F�N�g�̑�����e�C�x���g���ƂɎ��s���邱�Ƃ��ł��A
//   ����ɂ��A�X�V���鏇�Ԃɂ��s������炷���Ƃ��ł���B
//   Component�͂�������N���X�Ƃ��Čp�����A�l�X�ȋ@�\��ǉ�����B
//   �ʂɎQ�Ƃ���ۂ�dynamic_cast���s���B
// 
// �ǉ����������̐���
// 
// �E�A�N�^���m�̐e�q�֌W
// �@�ċN�������s���A�e�A�N�^�̕ϊ��s�񂩂�q�A�N�^�̃��[���h���W���Z�o���Ă���B
// �@�������̏����}���邽�߁A�q�A�N�^���̃p�����[�^�͊O���N���X�ɂĊǗ�����Ă���B
// 
// �EBehavior/Animator/Collider/RigidBody�@�\
//�@ �����͕K�v�ɉ����ĔC�ӂŒǉ�����@�\
//	 Component�z��Ƃ͕ʂŎ��̂��������Ă���B
// �@�����̋@�\��Comopnent�̍X�V�Ƃ͕ʂōs��Ȃ��ƕs����������邽�ߕ����B
//   ��̓I�ɂ�Behavior�ł�������Component���̃p�����[�^�𗘗p���āA
//   ���̂ӂ�܂������肷��\�������铙�B
// 
//   Behavior : BehaviorTree��������StateMachine�ɂ��s���J�ڂ̊Ǘ��B
// 
//   Animator : �A�N�^�̂���SkinnedMesh�̃A�j���[�V������
// �@�@�@�@�@   �e�A�j���[�V�����ɐݒ肵���C�ӂ̃t���[���ŃR�[���o�b�N�֐����Ăяo��
//              AnimationEvent�̊Ǘ��B
// 
//   RigidBody : �A�N�^�̑��͏����B
// 
//   Collider : �A�N�^���m�̏Փˁi�����j����ƏՓˁi�����j���Ɏ��s����
//              Component���̊֐���OnHitEvent�̎��s�ƁA
//              RigidBody�𗘗p�����A�N�^���m�̉����o�����s���B
// 
//   �e�@�\�̏ڍׂ͂��̃N���X�ɂĐ����B
// 
// �Estd::hash�𗘗p�����n�b�V���l�ɂ�閼�O�����@�\�ɂ�鍂�����B
//

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>
#include <string>
#include <vector>
#include <set>
#include <optional>
#include <cassert>

#include "imgui.h"

#include "ActorManager.h"
#include "Arithmetic/Arithmetic.h"
#include "Game/General/GameSystem.h"
#include "General/MyImGuiHelper.h"
#include "Graphics/Model/GLTFModel.h"

/* --- �O���錾 ---- */

class ChildActorParam;
class Animator;
class Component;
class Behavior;
class Collider;
class RigidBody;

/* --- ��` ---- */

class Actor : public std::enable_shared_from_this<Actor>
{
public:

	Actor();
	virtual ~Actor();

	/// <summary>
	/// �q�A�N�^�̏ꍇ�́Alocal�̃p�����[�^��ݒ肷��B
	/// </summary>
	virtual void Initialize(
		const char* name,
		const Vector3& position,
		const Quaternion& rotation,
		const Vector3& scale,
		const char* group_name);

	/// <summary>
	/// �q�A�N�^�̏ꍇ�́Alocal�̃p�����[�^��ݒ肷��
	/// </summary>
	/// <typeparam name="T">�ݒ肷�郂�f���̃N���X</typeparam>
	template <class T>
	void Initialize(
		ID3D11Device* device,
		const char* name,
		const Vector3& position,
		const Quaternion& rotation,
		const Vector3& scale,
		const char* group_name,
		const char* model_file_path);

	/// <summary>
	/// ActorManager��StartActor�ŌĂяo����鏈��
	/// </summary>
	virtual void Start();

	/// <summary>
	/// ActorManager��UpdateActor�ŌĂяo����鏈���B
	/// �q�A�N�^�̏ꍇ�͐e�A�N�^��Update���ōċA�I�ɍX�V�������s���B
	/// </summary>
	/// <param name="elapsed_time"></param>
	virtual void Update(float elapsed_time);

	/// <summary>
	/// ActorManager��RemoveActor�ŌĂяo����鏈��
	/// </summary>
	virtual void End();

	/// <summary>
	/// ActorManager��UpdateTransform�ŌĂяo����鏈���B
	/// �q�A�N�^�̏ꍇ�͐e�A�N�^�̊֐����ōċA�I�ɍX�V�������s���B
	/// </summary>
	/// <param name="render_model_coordinate_space">���f���̍��W�n</param>
	virtual void UpdateTransform(const Matrix& render_model_coordinate_space = Matrix::RhsYup());

	/// <summary>
	/// ���t���[���ɂ����Ă̕ϊ��s��̐ݒ�B
	/// �q�A�N�^�̏ꍇ�̓��[�J����Ԃ̐ݒ���s���B(����ɐe�A�N�^�̃��[���h�ϊ��s��������ă��[���h�ϊ��s����v�Z����B)
	/// </summary>
	/// <param name="transform"></param>
	void SetNextTransform(const Matrix& transform)
	{
		next_transform = transform;
	}

	/// <summary>
	/// ���݂̃��[�J�����W�n�����[���h���W�n�ɒu��������B
	/// �e�q�֌W�̂���A�N�^�̊֌W��؂����ۂɃA�N�^�̍��W�����������Ȃ�Ȃ��悤�ɂ��邽�߂Ɏg���B
	/// </summary>
	void SetLocalTransformToWorld()
	{
		next_transform = world_transform;
	}

	/// <summary>
	/// �������Ɏ��s�����e�R���|�[�l���g�̊֐������s����
	/// </summary>
	virtual void OnHitEvent(const std::shared_ptr<Actor>& opponent_actor) const;

	/// <summary>
	/// ���݂̃A�j���[�V�������Ԃɉ������Đ����̃A�j���[�V�����̃C�x���g�����s����
	/// </summary>
	void ExecuteCurrentPlayAnimationEvent();

	void SetName(const char* name) { this->name = name; }
	void SetGroupName(const char* group_name) { this->group_name = group_name; }
	void SetWorldPosition(const Vector3& position) { this->world_position = position; }
	void SetWorldRotation(const Quaternion& rotation) { this->world_rotation = rotation; }
	void SetWorldScale(const Vector3& scale) { this->world_scale = scale; }

	const std::string& GetName()		   const { return name; }
	const std::string& GetGroupName()	   const { return group_name; }
	const Vector3& GetWorldPosition()  const { return world_position; }
	const Quaternion& GetWorldRotation()  const { return world_rotation; }
	const Vector3& GetWorldScale()     const { return world_scale; }
	const Matrix& GetWorldTransform() const { return world_transform; }

	/// <summary>
	/// �n�b�V���l�ɂ�閼�O�̔�r
	/// </summary>
	bool CompareName(const char* opponent) const;

	/// <summary>
	/// �n�b�V���l�ɂ�閼�O�̔�r
	/// </summary>
	bool CompareGroupName(const char* opponent) const;

protected:

	/// <summary>
	/// ���g���q�A�N�^�̏ꍇ�̍s��X�V�����B���̎q�A�N�^�̍s��X�V���ċA�I�ɍs���B
	/// </summary>
	/// <param name="parent_coordinate"></param>
	void UpdateTransformAsChild(const Matrix& parent_coordinate);

	std::string           name;					// ���O
	std::string           group_name;			// �O���[�v��������ނ̃A�N�^���ꊇ�ŏ�������ۂɎg��
	Vector3		          world_position;		// ���[���h���W
	Quaternion	          world_rotation;		// ���[���h��]
	Vector3		          world_scale;			// ���[���h�X�P�[��
	Matrix		          world_transform;		// ���[���h��ԕϊ��s��

	// ��L�̃p�����[�^�𗘗p�����ɍ��W��ݒ肷��ꍇ�ɗ��p����B
	// ��]�s�����x�N�H�[�^�j�I���ɕϊ����āA
	// �ēx��]�s��ɂ����ۂɉ�]�ʂŕs��������������߂��̑΍�Ŏg���ꍇ������B
	std::optional<Matrix> next_transform;		

public: // �`��֌W

	/// <summary>
	/// �`�揈��
	/// </summary>
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader);

	template <class T>
	void CreateModel(ID3D11Device* device, const char* filepath)
	{
		model = std::make_unique<T>(device, filepath);
	}

	template <class T>
	T* GetModel()
	{
		return dynamic_cast<T*>(model.get());
	}

	GLTFModel* GetModel() { return model.get(); }

protected:

	std::unique_ptr<GLTFModel> model; // ���f��

public:

	void UpdateBehavior(float elapsed_time) const;
	void UpdateAnimation(float elapsed_time) const;
	void UpdateRigidBody(float elapsed_time) const;
	void UpdateCollider(float elapsed_time) const;


	/// <summary>
	/// �R���|�[�l���g�̒ǉ�
	/// </summary>
	/// <typeparam name="T">�ǉ�����R���|�[�l���g</typeparam>
	/// <typeparam name="...Args">�R���|�[�l���g�N���X�R���X�g���N�^����</typeparam>
	/// <returns>�ǉ������R���|�[�l���g��Ԃ�</returns>
	template <class T, class... Args>
	std::shared_ptr<T> AddComponent(Args ... args)
	{
		std::shared_ptr<T> component = std::make_shared<T>(args...);
		component->SetActor(shared_from_this());
		components.emplace_back(component);
		return component;
	}

	/// <summary>
	/// �r�w�C�r�A�̍쐬
	/// </summary>
	/// <typeparam name="T">�ǉ�����r�w�C�r�A</typeparam>
	/// <typeparam name="...Args">�r�w�C�r�A�N���X�R���X�g���N�^����</typeparam>
	/// <returns>�ǉ������r�w�C�r�A��Ԃ�</returns>
	template <class T, class... Args>
	std::shared_ptr<T> CreateBehavior(Args ... args)
	{
		std::shared_ptr<T> ptr = std::make_shared<T>(args...);
		ptr->SetActor(shared_from_this());
		behavior = ptr;
		return ptr;
	}

	/// <summary>
	/// �A�j���[�V�����N���X�̍쐬�B�������郂�f����SkinnedMesh�łȂ��Ǝg���Ȃ��B
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<Animator> CreateAnimator();

	/// <summary>
	/// ���̏������s���I�u�W�F�N�g�𐶐�����B
	/// </summary>
	/// <typeparam name="mass">����</typeparam>
	/// <typeparam name="is_effect_gravity">�d�͂̉e�����ɂ��邩</typeparam>
	std::shared_ptr<RigidBody> CreateRigidBody(float mass, bool is_effect_gravity);

	/// <summary>
	/// ����������s���R���C�_�[���쐬���A�z��ɒǉ�����B
	/// �����ݒ肪�\�B
	/// </summary>
	/// <typeparam name="T">�ǉ�����R���C�_�[</typeparam>
	/// <typeparam name="...Args">�r�w�C�r�A�N���X�R���X�g���N�^����</typeparam>
	/// <param name="name">�ǉ������R���C�_�[�̖��O</param>
	/// <returns></returns>
	template <class T, class... Args>
	std::shared_ptr<T> CreateCollider(const char* name, Args... args)
	{
		std::shared_ptr<T> ptr = std::make_shared<T>(name, args...);
		ptr->SetActor(shared_from_this());
		RegisterCollider(ptr);
		colliders[name] = ptr;
		return ptr;
	}


	/// <summary>
	/// �R���|�[�l���g�z��̃R���|�[�l���g��T�Ɉ���_�E���L���X�g����
	/// ��v����ꍇ���̃|�C���^��Ԃ�
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <returns></returns>
	template <class T>
	std::shared_ptr<T> GetComponent()
	{
		// �ێ����Ă���R���|�[�l���g���w��̌^�ɃL���X�g���āA��v�����ꍇ�͂��̃R���|�[�l���g��Ԃ�
		for (std::shared_ptr<Component>& component : components)
		{
			// �R���|�[�l���g���������ꂽ
			if (std::shared_ptr<T> found_component = std::dynamic_pointer_cast<T>(component))
			{
				return found_component;
			}
		}

		return nullptr;
	}

	template <class T>
	std::shared_ptr<T> GetBehavior()
	{
		assert(behavior != nullptr);
		return std::dynamic_pointer_cast<T>(behavior);
	}

	std::shared_ptr<Collider> GetCollider(const char* name)
	{
		auto itr{ colliders.find(name) };
		if (itr != colliders.end())
		{
			return itr->second;
		}

		return nullptr;
	}

	template <class T>
	std::shared_ptr<T> GetCollider(const char* name)
	{
		if (std::shared_ptr<T> collider = std::dynamic_pointer_cast<T>(GetCollider(name)))
		{
			return collider;
		}

		return nullptr;
	}


	std::shared_ptr<Animator> GetAnimator();
	std::shared_ptr<RigidBody> GetRigidBody();
	const std::vector<std::shared_ptr<Component>>& GetComponents() const { return components; }
	const std::unordered_map<std::string, std::shared_ptr<Collider>>& GetColliders() const;

private:

	/// <summary>
	/// ���������R���C�_�[���R���C�_�[�Ǘ��N���X�ɓo�^����B
	/// �o�^���邱�ƂŊǗ��N���X������Փ˔���A�������s�����Ƃ��ł���B
	/// </summary>
	/// <param name="collider"></param>
	void RegisterCollider(const std::shared_ptr<Collider>& collider);

	std::vector<std::shared_ptr<Component>> components;
	std::shared_ptr<Behavior> behavior;
	std::shared_ptr<Animator> animator;
	std::shared_ptr<RigidBody> rigid_body;
	std::unordered_map<std::string, std::shared_ptr<Collider>> colliders;

public: // �A�N�^�̐e�q�֌W

	/// <summary>
	/// �Q�Ƃ��؂ꂽ�֌W�����A�N�^�����X�g����폜����
	/// </summary>
	void UpdateActorRelation();

	/// <summary>
	/// ���̃A�N�^�̎q�A�N�^�Ƃ��ĐV�����C���X�^���X���쐬����B
	/// �쐬����ɂ����āA�e�q���m�A�e�A�N�^���猩�Ă̌Z�퓯�m�̎�Q�Ƃ��o�^�����B
	/// </summary>
	/// <returns></returns>
	template <class T>
	std::shared_ptr<T> CreateChildActor()
	{
		auto created_actor{ GameSystem::GetInstance().GetActorManager()->Create<T>() };

		AddChildActor(created_actor);

		created_actor->SetParentActor(shared_from_this());

		return created_actor;
	}

	void RemoveChildActor(const std::shared_ptr<Actor>& actor);
	/// <summary>
	/// �����Ɋ��Ɏq�A�N�^�Ƃ��ēo�^���Ă���ق��̃A�N�^�̌Z��Ƃ��Ă��o�^����
	/// </summary>
	void AddChildActor(const std::shared_ptr<Actor>& actor);

	/// <summary>
	/// ������ null_ptr ��ݒ肵���ꍇ�A�e�A�N�^�����݂��Ȃ���ԂɂȂ�
	/// </summary>
	void SetParentActor(const std::shared_ptr<Actor>& actor);

	std::shared_ptr<Actor> GetChildActor(size_t index) const { return w_child_actors.at(index).lock(); }
	size_t CountOfChildActors() const { return w_child_actors.size(); }

	bool IsChildActor() const { return is_child; }
	void SetChildActorFlag(bool is_child) { this->is_child = is_child; }

	const std::unique_ptr<ChildActorParam>& GetChildActorParam() { return child_actor_param; }

private:

	bool is_child; // �q�A�N�^��
	// �q�A�N�^�Ƃ��ĐU�����ۂɗp����p�����[�^
	std::unique_ptr<ChildActorParam> child_actor_param;

	std::vector<std::weak_ptr<Actor>> w_child_actors;
	std::set<std::shared_ptr<Actor>> remove_child_actors;

public: // �f�o�b�N

	void ImGuiRender();

};

template<class T>
inline void Actor::Initialize(
	ID3D11Device* device,
	const char* name, 
	const Vector3& position, 
	const Quaternion& rotation, 
	const Vector3& scale, 
	const char* group_name, 
	const char* model_file_path)
{
	Initialize(name, position, rotation, scale, group_name);
	CreateModel<T>(device, model_file_path);
}
#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>
#include <vector>
#include <set>
#include <d3d11.h>

#include "Arithmetic/Arithmetic.h"
#include "Graphics/Shader/Shader.h"

/* --- �O���錾 ---- */

class Actor;

/* --- �錾 ---- */

/// <summary>
/// Actor�̊Ǘ��N���X�A��{�I�ɂ��̃N���X����ăA�N�^�̐������s���B
/// �������ꂽ�A�N�^�͂܂�StartActors�z��ɑ}������AStart()�����s������AUpdateActors�z��Ɉڂ����B
/// �q�A�N�^�Ɋւ��Ă͐e�A�N�^�̍X�V�������ōX�V���s���Ă���B
/// RemoveActors�ɓ���̃A�N�^�����݂���A�N�^�͏I���������s������UpdateActors���珜�O�����B
/// �`�揈���̓V�F�[�_�[�̎w�肪�Ȃ��ꍇ�W�I���g���o�b�t�@�ɏ������݃f�B�t�@�[�h�����_�����O���s���B
/// </summary>
class ActorManager
{
public:

	void Initialize(ID3D11Device* device);

	/// <summary>
	/// �S�A�N�^��������A���̃R���|�[�l���g�̏I�����������s����B
	/// </summary>
	void EndProcess();

	/// <summary>
	/// �A�N�^���쐬���A���X�g�ɓo�^����
	/// </summary>
	template <class T>
	std::shared_ptr<T> Create()
	{
		auto actor = std::make_shared<T>();
		{
			static int id = 0;
			char name[256];
			::sprintf_s(name, sizeof(name), "Actor%d", id++);
			actor->SetName(name);
		}
		start_actors.emplace_back(actor);
		return actor;
	}

	/// <summary>
	/// �}�l�[�W���[�̐����֐��𗘗p�����ɐ��������A�N�^��o�^����B
	/// �o�^�����A�N�^��StartActors�ɑ}�������B
	/// </summary>
	void Register(const std::shared_ptr<Actor>& actor);
	void Register(const std::vector<std::shared_ptr<Actor>>& actors);

	/// <summary>
	/// �A�N�^���폜���A�z�񂩂珜�O����B
	/// �X�V���ɂ��̃A�N�^�̏I�����������s���A�j������B
	/// </summary>
	void Remove(const std::shared_ptr<Actor>& actor);
	void Remove(const std::vector<std::shared_ptr<Actor>>& actors);

	/// <summary>
	/// �A�N�^�̍X�V����
	/// </summary>
	void UpdateActor(float elapsed_time);

	/// <summary>
	/// �A�N�^�̃A�j���[�V�����̍X�V����
	/// </summary>
	/// <param name="elapsed_time"></param>
	void UpdateAnimation(float elapsed_time);

	/// <summary>
	/// �A�N�^�̃r�w�C�r�A�̍X�V����
	/// </summary>
	/// <param name="elapsed_time"></param>
	void UpdateBehavior(float elapsed_time);

	/// <summary>
	/// �A�N�^�̍��̂̍X�V����
	/// </summary>
	/// <param name="elapsed_time"></param>
	void UpdateRigidBody(float elapsed_time);

	/// <summary>
	/// �A�N�^�̃R���C�_�[�̍X�V����
	/// </summary>
	/// <param name="elapsed_time"></param>
	void UpdateCollider(float elapsed_time);

	/// <summary>
	/// �A�N�^�̍s��̍X�V
	/// </summary>
	void UpdateTransform(const Matrix& render_model_coordinate_space = Matrix::RhsYup());

	/// <summary>
	/// ���݊Ǘ����ɂ���A�N�^�̍Đ����̃A�j���[�V�����̎��s�\���Ԃɂ���C�x���g�����s����
	/// </summary>
	void ExecuteActorCurrentPlayAnimationEvent();

	/// <summary>
	/// �A�N�^�̕`��B�O���̃V�F�[�_�[���g��Ȃ��ꍇGbuffer�Ƀ��f�������������ރV�F�[�_�[��ݒ肷��B
	/// </summary>
	void Render(ID3D11DeviceContext* dc, Shader* shader = nullptr);

	/// <summary>
	/// ImGui�`�揈��
	/// </summary>
	void ImGuiRender();

	void ImGuiRenderOnMainMenuBar();

	/// <summary>
	/// �n�b�V���l�𗘗p���Ĉ������̃A�N�^�����X�g���猟������
	/// </summary>
	std::shared_ptr<Actor> FindActor(const char* actor_name);

	/// <summary>
	/// �n�b�V���l�𗘗p���Ĉ������̃O���[�v�̃A�N�^�����X�g���猟������
	/// </summary>
	std::vector<std::shared_ptr<Actor>> FindActors(const char* group_name);

	/// <summary>
	/// �n�b�V���l�𗘗p���Ĉ������̃O���[�v�̃A�N�^�����X�g���猟������
	/// </summary>
	std::vector<std::shared_ptr<Actor>> FindActors(const std::vector<const char*>& group_names);

	/// <summary>
	/// UpdateActors���擾
	/// </summary>
	const std::vector<std::shared_ptr<Actor>>& GetUpdateActors();

	size_t CountUpdateActors() const;

	void ClearAllActor();

private:

	/// <summary>
	/// start_actors�̏����B���̏����̌�update_actors�Ƀ|�C���^���ڂ�B
	/// </summary>
	void UpdateStartActor();

	/// <summary>
	/// �e�A�N�^�̍X�V�����B�q�A�N�^�̏ꍇ�͐e�A�N�^�̍X�V���������ŏ������s���B
	/// </summary>
	/// <param name="elapsed_time"></param>
	void UpdateMainActor(float elapsed_time);

	/// <summary>
	/// Remove�����A�N�^�̏I���������s���A�e���X�g���珜�O����B
	/// </summary>
	void RemoveActor();

	/// <summary>
	/// �e�A�N�^�̐e�q�֌W�̍X�V
	/// </summary>
	void UpdateActorRelation();

	/// <summary>
	/// ���X�^�[
	/// </summary>
	void DrawLister();

	/// <summary>
	/// �ڍ�
	/// </summary>
	void DrawDetail();

private:

	std::vector<std::shared_ptr<Actor>> start_actors;	// Start()���Ăяo���A�N�^, Start()�͈�񂾂��Ă΂��
	std::vector<std::shared_ptr<Actor>> update_actors;	// Update()���Ăяo���A�N�^
	std::set<std::shared_ptr<Actor>> selection_actors;	// 
	std::set<std::shared_ptr<Actor>> remove_actors;		// 

	bool show_lister = false;
	bool show_detail = false;
	bool show_animation_view = false;

	Shader gbuffer_shader;	// �W�I���g���o�b�t�@�Ɋe�����������ރV�F�[�_�[
};

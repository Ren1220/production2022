#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <d3d11.h>

#include "imgui.h"
#include "General/MyImGuiHelper.h"

/* --- �錾 ---- */

class Scene
{
public:

	Scene()
		: is_ready(false), scene_timer() {}
	virtual ~Scene() = default;

	/// <summary>
	/// ����������
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// �I������
	/// </summary>
	virtual void EndProcess() = 0;

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="elapsed_time"></param>
	virtual void Update(float elapsed_time) = 0;

	/// <summary>
	/// �`�揈��
	/// </summary>
	virtual void Render(ID3D11DeviceContext* dc) = 0;


	/// <summary>
	/// ImGui�`�揈��
	/// </summary>
	virtual void ImGuiRender() {};

public:

	// �����������Ă��邩
	bool IsReady() const { return is_ready; };

	// ���������ݒ�
	void SetReady() { is_ready = true; }

protected:

	float scene_timer; // �V�[�����o�ߎ���

	bool is_ready; // ������������������
};
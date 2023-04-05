#include "SceneLoading.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include <thread>

#include "Include/misc.h"

#include "General/AtaiEngine.h"
#include "Graphics/General/ResourceFileloader.h"
#include "Utility/ChronoTimer.h"
#include "SceneManager.h"

/* --- ��` ---- */

SceneLoading::SceneLoading(std::unique_ptr<Scene>&& next_scene)
	: next_scene(std::move(next_scene))
{
	// �A�j���[�V�����̑��݂���t�H���_�̃p�X�����
	loading = std::make_unique<TextureAnimation>("./Data/Texture/General/Loadings");

	// �A�j���[�V�����t���[����ݒ�
	loading->SetAnimationFrame(loading->CalculateAnimationFramePerTime(1.0f));
}

void SceneLoading::Initialize()
{
	/*
		�X���b�h�𗧂��グ����thread�֐����j�������܂łɁA
		�i�����Initialize()�֐����I������Ƃ��j
		Join()�֐��ŃX���b�h�̏I����҂��A
		Detouch()�ŃX���b�h�̊Ǘ���������Ȃ���΃G���[�ɂȂ�
	*/

	// ���[�h����fps�����Ȃ�
	MySystem.SetSwapChainPresentFlag(DXGI_PRESENT_ALLOW_TEARING);

	// �X���b�h�J�n
	std::thread thread(LoadingThread, this);

	// �X���b�h�̊Ǘ������
	thread.detach();
}

void SceneLoading::EndProcess()
{
	ResourceFileLoader::ReleaseAllTexture();
}

void SceneLoading::Update(float elapsed_time)
{
	loading->Update(elapsed_time);

	// ���̃V�[���̏���������������V�[����؂�ւ���
	if (next_scene->IsReady())
	{
		SceneManager::GetInstance().ChangeScene(std::move(next_scene));
		MySystem.SetSwapChainPresentFlag(0);
		return;
	}
}

void SceneLoading::Render(ID3D11DeviceContext* dc)
{
	auto [width, height] { MySystem.GetScreenSize<float>() };

	MySystem.ActivateSwapChainBuffer();

	loading->Render(dc, width - loading->GetTextureWidth() + 120, height - loading->GetTextureHeight() + 50, loading->GetTextureWidth(), loading->GetTextureHeight());
}

void SceneLoading::LoadingThread(SceneLoading* scene)
{
	HRESULT hr{ S_OK };
	// COM�֘A�̏������ŃX���b�h���ɌĂԕK�v������
	hr = CoInitialize(nullptr);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ChronoTimer time;
	time.Start();
	// ���̃V�[���̏��������s��
	scene->next_scene->Initialize();
	time.Stop(true, L"�V�[������������");

	// �X���b�h���I���O��COM�֘A�̏I������
	CoUninitialize();

	// ���̃V�[���̏��������ݒ�
	scene->next_scene->SetReady();
}

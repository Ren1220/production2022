#include "SceneManager.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Debug/DebugGUI.h"

/* --- ��` ---- */

void SceneManager::EndProcess()
{	
	if (current_scene)
	{
		current_scene->EndProcess();
	}

	current_scene.reset();
}

void SceneManager::Update(float elapsed_time)
{
	auto& m{ DebugGUI::GetInstance().GetPerformanceGUI().StartMeasure("SceneUpdate") };

	if (current_scene != nullptr)
	{
		current_scene->Update(elapsed_time);
	}

	m.StopMeasure();
}

void SceneManager::Render(ID3D11DeviceContext* dc)
{
	auto& m{ DebugGUI::GetInstance().GetPerformanceGUI().StartMeasure("SceneRender") };

	if (current_scene != nullptr)
	{
		current_scene->Render(dc);
	}

	m.StopMeasure();
}

void SceneManager::ClearCurrentScene()
{
	if (current_scene)
	{
		current_scene->EndProcess();
	}

	current_scene.reset();
}

void SceneManager::ImGuiRender()
{
#ifdef _DEBUG
	current_scene->ImGuiRender();
#endif // _DEBUG
}

void SceneManager::ChangeScene(std::unique_ptr<Scene> scene)
{
	// �Â��V�[���̏I������
	ClearCurrentScene();

	// �V�����V�[����ݒ�
	current_scene = std::move(scene);

	// �V�[������������
	if (!current_scene->IsReady())
	{
		current_scene->Initialize();
	}
}

void SceneManager::ChangeScene(std::unique_ptr<Scene>& scene)
{

	// �Â��V�[���̏I������
	ClearCurrentScene();

	// �V�����V�[����ݒ�
	current_scene = std::move(scene);

	// �V�[������������
	if (!current_scene->IsReady())
	{
		current_scene->Initialize();
	}
}

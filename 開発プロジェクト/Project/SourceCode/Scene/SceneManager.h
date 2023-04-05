#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>

#include "Utility/Singleton.h"
#include "Scene.h"

/* --- �O���錾 ---- */

class SceneRogueLike2D_AreaDivision;
class SceneRogueLike2D_BSPTree;
class SceneRogueLike2D_Ver2;
class SceneRogueLike2D_Ver3;
class SceneGame;
class SceneTitle;
class SceneCellularAutomata;
class SceneCellularAutomata2;
class SceneCellularAutomata3;
class ScenePerlinNoise;
class ScenePerlinNoise2D;
class SceneGUITest;

/* --- �錾 ---- */

//using InitialScene = SceneCellularAutomata2;

#ifdef _DEBUG
using InitialScene = SceneTitle;
#else
using InitialScene = SceneGame;
#endif // _DEBUG


class SceneManager : public Singleton<SceneManager>
{
public:

	void EndProcess();
	/// <summary>
	/// ���݂̃V�[���̍X�V����
	/// </summary>
	void Update(float elapsed_time);
	/// <summary>
	/// ���݂̃V�[���̕`�揈��
	/// </summary>
	void Render(ID3D11DeviceContext* dc);
	/// <summary>
	/// ���݂̃V�[���̏I���������Ăт����������
	/// </summary>
	void ClearCurrentScene();
	void ImGuiRender();
	void ChangeScene(std::unique_ptr<Scene> scene);
	void ChangeScene(std::unique_ptr<Scene>& scene);

private:

	std::unique_ptr<Scene> current_scene{ nullptr };
	std::unique_ptr<Scene> next_scene{ nullptr };
};


#include "framework.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Audio/AudioManager.h"
#include "Debug/DebugPrimitive.h"
#include "Graphics/General/Graphics.h"
#include "Graphics/Debug/DebugPrimitive2D.h"

#include "Scene/SceneLoading.h"
#include "Scene/SceneGame.h"
#include "Scene/SceneTitle.h"

/* --- ��` ---- */

Framework::~Framework()
{
    SceneManager::GetInstance().EndProcess();
    Graphics::GetInstance().EndProcess(MySystem.GetDeviceContext());

    AudioManager::GetInstance().EndProcess();
}

bool Framework::Initialize()
{
    // �L�[���͂̏���������
	Input::GetInstance().Initialize(hwnd);

    // DirectX�̏�����
    MySystem.Initialize(hwnd, false);
    MySystem.SetSwapChainPresentFlag(0);
    auto [width, height] { MySystem.GetScreenSize<UINT>() };

    AudioManager::GetInstance().Initialize();

    ID3D11Device* device{ MySystem.GetDevice() };
    Graphics::GetInstance().Initialize(device, width, height);

    // �f�o�b�N�v���~�e�B�u�`��N���X�̏�����
    DebugPrimitive::GetInstance().Initialize(device);
    DebugPrimitive2D::GetInstance().Initialize(device);

    // �V�����V�[���̐ݒ�
    SceneManager::GetInstance().ChangeScene(std::make_unique<SceneLoading>(std::make_unique<InitialScene>()));

    return true;
}

void Framework::Update(float elapsed_time)
{
    // �L�[���͂̍X�V����
    Input::GetInstance().Update(elapsed_time);
    
    // �I�[�f�B�I�̍X�V
    AudioManager::GetInstance().Update();

    // ���݂̃V�[���̍X�V����
    SceneManager::GetInstance().Update(elapsed_time);

#ifdef USE_IMGUI
    // ImGui�X�V����
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
#endif
}

void Framework::Render(float elapsed_time)
{
    // �ʃX���b�h���Ƀf�o�C�X�R���e�L�X�g���g���Ă����ꍇ��
    // �����A�N�Z�X���Ȃ��悤�ɔr�����䂷��
    std::lock_guard<std::mutex> lock(MySystem.GetMutex());

    ID3D11DeviceContext* dc{ MySystem.GetDeviceContext() };

    // ���݂̃V�[���̕`�揈��
    SceneManager::GetInstance().Render(dc);

    // ImGui�`�揈��
#ifdef USE_IMGUI
    SceneManager::GetInstance().ImGuiRender();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif // USE_IMGUI

    // ��ʂɕ`�悷�� �����̑������ŉσt���[�����[�g�ɂȂ�
    MySystem.Present();
}

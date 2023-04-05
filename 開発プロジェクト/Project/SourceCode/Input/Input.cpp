#include "Input.h"

/* --- ��` ---- */

Input::Input()
	: hwnd(), cursor(), gamepad(), keyboard(), vertical_wheel_state()
{
}

void Input::Initialize(HWND hwnd)
{
	this->hwnd = hwnd;
}

void Input::Update(float elapsed_time)
{
	cursor.Update(hwnd);
	keyboard.Update();
	gamepad.Update();

	UpdateWheel(elapsed_time);
}

void Input::UpdateWheel(float elapsed_time)
{
	vertical_wheel_state = std::lerp(vertical_wheel_state, 0.0f, elapsed_time * 2.0f);
}

//void Input::GetRayFromCursorPosition(DirectX::XMFLOAT3& start, DirectX::XMFLOAT3& end, ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection) const
//{
//	using namespace DirectX;
//
//	// �r���[�|�[�g
//	D3D11_VIEWPORT viewport;
//	UINT num_viewport{ 1 };
//	dc->RSGetViewports(&num_viewport, &viewport);
//
//	// �s��̒�`
//	DirectX::XMMATRIX view_matrix{ XMLoadFloat4x4(&view) };
//	DirectX::XMMATRIX projection_matrix{ XMLoadFloat4x4(&projection) };
//	DirectX::XMMATRIX world_matrix{ XMMatrixIdentity() };
//
//	// �J�[�\���̈ʒu���擾����
//	DirectX::XMFLOAT3 screen_position {};
//
//	screen_position.x = static_cast<float>(this->GetCurrentCursorPositionX());
//	screen_position.y = static_cast<float>(this->GetCurrentCursorPositionY());
//	screen_position.z = 0.0f;
//
//	// �}�E�X�̃X�N���[�����W�����[���h���W�ɕϊ�����
//
//	// �n�_�̌v�Z
//	DirectX::XMVECTOR Start{};
//	Start = XMVector3Unproject(
//		DirectX::XMLoadFloat3(&screen_position),
//		viewport.TopLeftX, viewport.TopLeftY,
//		viewport.Width, viewport.Height,
//		viewport.MinDepth, viewport.MaxDepth,
//		projection_matrix,
//		view_matrix,
//		world_matrix
//	);
//
//	DirectX::XMStoreFloat3(&start, Start);
//
//	// �I�_�̌v�Z
//	screen_position.z = 1.0f;
//
//	DirectX::XMVECTOR End{};
//	End = XMVector3Unproject(
//		DirectX::XMLoadFloat3(&screen_position),
//		viewport.TopLeftX, viewport.TopLeftY,
//		viewport.Width, viewport.Height,
//		viewport.MinDepth, viewport.MaxDepth,
//		projection_matrix,
//		view_matrix,
//		world_matrix
//	);
//
//	DirectX::XMStoreFloat3(&end, End);
//}

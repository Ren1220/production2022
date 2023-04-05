#include "EffekseerHandle.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <numbers>

#include "imgui.h"

#include "Graphics/General/Graphics.h"

/* --- 定義 ---- */

Effekseer::Manager* GetManager()
{
	return Graphics::GetInstance().GetEffekseerManager()->GetEffekseerManager();
}

EffekseerHandle::EffekseerHandle(const char* filepath, const Vector3& position)
{
	Initialize(filepath, position);
}

EffekseerHandle::~EffekseerHandle()
{
}

void EffekseerHandle::Initialize(const char* filepath, const Vector3& position)
{
	this->position = position;
	this->angle = {};
	this->scale = { 1,1,1 };

	handle = Graphics::GetInstance().GetEffekseerManager()->PlayEffect(filepath, position);
}

void EffekseerHandle::SetTransform(const Vector3& position, const Vector3& rotation, const Vector3& scale)
{
	GetManager()->SetLocation(handle, position.x, position.y, position.z);
	GetManager()->SetRotation(handle, rotation.x, rotation.y, rotation.z);
	GetManager()->SetScale(handle, scale.x, scale.y, scale.z);

	//Effekseer::Matrix43 m, r;
	//r.RotationXYZ(angle.x, angle.y, angle.z);
	//m.SetSRT(*reinterpret_cast<const Effekseer::Vector3D*>(&scale),
	//	r,
	//	*reinterpret_cast<const Effekseer::Vector3D*>(&position));

	//GetManager()->SetMatrix(handle, m);
}

void EffekseerHandle::SetTransform(const Matrix& transform)
{
	GetManager()->SetMatrix(handle, 
		{
			transform.m[0][1], transform.m[1][1], transform.m[2][1], transform.m[3][1],
			transform.m[0][2], transform.m[1][2], transform.m[2][2], transform.m[3][2],
			transform.m[0][3], transform.m[1][3], transform.m[2][3], transform.m[3][3]
		});
}

void EffekseerHandle::SetPosition(const Vector3& position)
{
	GetManager()->SetLocation(handle, position.x, position.y, position.z);
}

void EffekseerHandle::SetRotation(const Vector3& axis, float angle)
{
	GetManager()->SetRotation(handle, (*reinterpret_cast<const Effekseer::Vector3D*>(&axis)), angle);
}

void EffekseerHandle::SetRotation(const Quaternion& rotation)
{
	// クォータニオンを回転軸と回転量に分解
	float c{ acosf(rotation.w) };

	float angle{ 2.0f * c  };
	angle -= 3.14f; 
	Vector3 axis;

	axis.x = rotation.x / sinf(c);
	axis.y = rotation.y / sinf(c);
	axis.z = rotation.z / sinf(c);

	SetRotation(axis, angle);

	// 投射方向から角度を算出
	//Vector3 a{ 0.0f, 0.0f, -1.0f };
	//Vector3 b{ direction };
	//float angle{ a.Dot(b) };

	//Vector3 cross(a.Cross(b));
	//if (cross.y < 0)
	//{
	//	angle += 1.57f;
	//}

	//EffekseerHandle handle("Poison/poison_launch.efk", pos);
	//handle.SetRotation(Vector3(0, 1, 0), angle);
	//handle.SetScale({ 0.1f, 0.1f, 0.1f });
}

void EffekseerHandle::SetRotation(const Vector3& radian)
{
	GetManager()->SetRotation(handle, radian.x, radian.y, radian.z);
}

void EffekseerHandle::SetScale(const Vector3& scale)
{
	GetManager()->SetScale(handle, scale.x, scale.y, scale.z);
}

void EffekseerHandle::SetColor(const Vector4& color)
{
	Effekseer::Color c{};
	c.R = static_cast<uint8_t>(0xff * color.x);
	c.G = static_cast<uint8_t>(0xff * color.y);
	c.B = static_cast<uint8_t>(0xff * color.z);
	c.A = static_cast<uint8_t>(0xff * color.w);

	GetManager()->SetAllColor(handle, c);
}

void EffekseerHandle::Destroy()
{
	GetManager()->StopEffect(handle);
}

void EffekseerHandle::SetShown(bool is_shown)
{
	GetManager()->SetShown(handle, is_shown);
}

bool EffekseerHandle::IsExist()
{
	return GetManager()->Exists(handle);
}

void EffekseerHandle::ImGuiRender()
{
	ImGui::SliderFloat3("Position", &position.x, -100.0f, 100.0f);
	ImGui::SliderFloat3("Angle", &angle.x, -3.14f, 3.14f);
	ImGui::SliderFloat3("Scale", &scale.x, 0.0f, 100.0f);


	Effekseer::Matrix43 m, r;
	r.RotationXYZ(angle.x, angle.y, angle.z);
	m.SetSRT(*reinterpret_cast<const Effekseer::Vector3D*>(&scale),
			r,
			*reinterpret_cast<const Effekseer::Vector3D*>(&position));

	GetManager()->SetMatrix(handle, m);

}

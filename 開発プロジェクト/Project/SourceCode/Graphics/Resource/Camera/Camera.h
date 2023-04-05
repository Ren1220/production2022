#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>

#include "Arithmetic/Arithmetic.h"

/* --- �O���錾 ---- */

class CameraShakeBase;

/* --- �錾 ---- */

class Camera
{
public:

	Camera(
		const Vector3& eye,
		const Vector3& focus,
		float near_z,
		float far_z,
		bool is_fix_up);
	virtual ~Camera() = default;

	virtual void Update(float elapsed_time);
	virtual void ImGuiRender();

	const Matrix&  GetView()       const { return view;       }
	const Matrix&  GetProjection() const { return projection; }
	const Vector3& GetEye()        const { return eye;        }
	const Vector3& GetFocus()      const { return focus;      }
	const Vector3& GetRight()      const { return right;      }
	const Vector3& GetUp()         const { return up;         }
	const Vector3& GetForward()    const { return forward;    }

	float GetNearZ() const { return near_z; }
	float GetFarZ() const { return far_z; }

	void SetEye(const Vector3& eye)		{ this->eye    = eye;    }
	void SetFocus(const Vector3& focus)	{ this->focus  = focus;  }
	void SetNearZ(float near_z)			{ this->near_z = near_z; }
	void SetFarZ(float far_z)			{ this->far_z  = far_z;  }

	void SetCameraShake(std::unique_ptr<CameraShakeBase>&& camera_shake);

	/// <summary>
	/// �J��������
	/// </summary>
	/// <param name="elapsed_time"></param>
	virtual void Control(float elapsed_time) {};

protected:

	/// <summary>
	/// �r���[�s��A�v���W�F�N�V�����s��̌v�Z
	/// </summary>
	virtual void CalcViewProj(
		const Vector3& eye,
		const Vector3& focus,
		Matrix& out_view,
		Matrix& out_projection) = 0;

	/// <summary>
	/// �r���[�s��̋t�s�񂩂�J�����̃��[���h��Ԃɂ���������x�N�g�����Z�o����
	/// </summary>
	void CalcDirection();

	/// <summary>
	/// �J�����V�F�C�N��ݒ肵�Ă���ꍇ�A�J�����V�F�C�N���s���A�ω��������_�E�����_���擾����B
	/// �V�F�C�N���鎞�Ԃ��o�߂����J�����V�F�C�N�I�u�W�F�N�g�͔j�������B
	/// </summary>
	/// <param name="elapsed_time"></param>
	/// <param name="out_eye"></param>
	/// <param name="out_focus"></param>
	void CameraShake(
		float elapsed_time, 
		Vector3& out_eye, 
		Vector3& out_focus);

	Matrix view;		
	Matrix projection;	

	Vector3 eye;   
	Vector3 focus; 

	Vector3 right;	 // �r���[�s��̋t�s�񂩂�擾���郏�[���h��Ԃɂ���������x�N�g��
	Vector3 up;		 // �r���[�s��̋t�s�񂩂�擾���郏�[���h��Ԃɂ���������x�N�g��
	Vector3 forward; // �r���[�s��̋t�s�񂩂�擾���郏�[���h��Ԃɂ���������x�N�g��

	float near_z;
	float far_z;

	bool is_fix_up; // �J�����̏�����x�N�g�����Œ肷�邩�H

	// �J�����V�F�C�N�p�I�u�W�F�N�g
	std::unique_ptr<CameraShakeBase> camera_shake;
};

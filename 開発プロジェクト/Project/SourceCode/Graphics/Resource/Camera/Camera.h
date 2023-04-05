#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>

#include "Arithmetic/Arithmetic.h"

/* --- 前方宣言 ---- */

class CameraShakeBase;

/* --- 宣言 ---- */

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
	/// カメラ操作
	/// </summary>
	/// <param name="elapsed_time"></param>
	virtual void Control(float elapsed_time) {};

protected:

	/// <summary>
	/// ビュー行列、プロジェクション行列の計算
	/// </summary>
	virtual void CalcViewProj(
		const Vector3& eye,
		const Vector3& focus,
		Matrix& out_view,
		Matrix& out_projection) = 0;

	/// <summary>
	/// ビュー行列の逆行列からカメラのワールド空間における向きベクトルを算出する
	/// </summary>
	void CalcDirection();

	/// <summary>
	/// カメラシェイクを設定している場合、カメラシェイクを行い、変化した視点・注視点を取得する。
	/// シェイクする時間が経過したカメラシェイクオブジェクトは破棄される。
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

	Vector3 right;	 // ビュー行列の逆行列から取得するワールド空間における向きベクトル
	Vector3 up;		 // ビュー行列の逆行列から取得するワールド空間における向きベクトル
	Vector3 forward; // ビュー行列の逆行列から取得するワールド空間における向きベクトル

	float near_z;
	float far_z;

	bool is_fix_up; // カメラの上向きベクトルを固定するか？

	// カメラシェイク用オブジェクト
	std::unique_ptr<CameraShakeBase> camera_shake;
};

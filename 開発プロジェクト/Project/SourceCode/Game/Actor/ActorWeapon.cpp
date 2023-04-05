#include "ActorWeapon.h"

/* --- 追加のヘッダファイルのインクルード ---- */	

#include "Graphics/Model/GLTFStaticMesh.h"

#include "../Component/Weapon/Weapon.h"
#include "../Collider/ColliderCapsule.h"
#include "../Collider/ColliderSphere.h"

/* --- 定義 ---- */

void ActorWeapon::Initialize(
	ID3D11Device* device, 
	const char* name, 
	int32_t attack_power,
	int32_t durability,
	const Vector3& position,
	const Quaternion& rotation, 
	const Vector3& scale)
{
	Actor::Initialize<GLTFStaticMesh>(
		device,
		name,
		position,
		rotation,
		scale,
		"Weapon",
		"Data/Model/GLTF/Item/Weapon/Sword/sword1.gltf");

	CreateRigidBody(0.3f, true);
	CreateCollider<ColliderCapsule>("Capsule", Vector3(0, 0, 0), Vector3(0, 1, 0), 0.2f, Vector4(0, 1, 0, 1), true);
	CreateCollider<ColliderSphere>("Sphere",   Vector3(0, 0, 0), 0.5f, Vector4(0, 1, 0, 1), true);
	AddComponent<Weapon>(attack_power, durability);
}

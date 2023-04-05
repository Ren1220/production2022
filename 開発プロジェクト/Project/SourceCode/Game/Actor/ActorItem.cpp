#include "ActorItem.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "../Collider/ColliderSphere.h"
#include "Graphics/Model/GLTFStaticMesh.h"

/* --- 定義 ---- */

void ActorItem::Initialize(ID3D11Device* device, const char* name, const Vector3& position, const Quaternion& rotation, const Vector3& scale)
{
	Actor::Initialize<GLTFStaticMesh>(
			device,
			name,
			position,
			rotation,
			scale,
			"Item",
			"Data/Model/GLTF/Item/Bottle/bottle.gltf");

	CreateRigidBody(0.3f, true);
	CreateCollider<ColliderSphere>("Body", Vector3(0, 0.5f, 0), 0.25f, Vector4(0, 1, 0, 1));

}

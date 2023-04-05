#include "ItemDataBase.h"

/* --- 定義 ---- */

void ItemDataBase::Initialize(ID3D11Device* device)
{
	Create(device, List::LifePotion,   L"回復ポーション", L"Data/Texture/Item/life_potion2.png", {1.0f, 1.0f, 1.0f, 1.0f}, "Data/Model/GLTF/GLTF/Item/Bottle/scene.gltf");
	Create(device, List::FirePotion,   L"火炎ポーション", L"Data/Texture/Item/fire_potion2.png", {1.0f, 1.0f, 1.0f, 1.0f}, "Data/Model/GLTF/GLTF/Item/Bottle/scene.gltf");
	Create(device, List::SparkPotion,  L"電気ポーション", L"Data/Texture/Item/spark_potion2.png", {1.0f, 1.0f, 1.0f, 1.0f}, "Data/Model/GLTF/GLTF/Item/Bottle/scene.gltf");
	Create(device, List::PoisonPotion, L"毒ポーション", L"Data/Texture/Item/poison_potion.png",   {1.0f, 1.0f, 1.0f, 1.0f}, "Data/Model/GLTF/GLTF/Item/Bottle/scene.gltf");
}

void ItemDataBase::Create(
	ID3D11Device* device,
	List i,
	const wchar_t* name, 
	const wchar_t* image_path, 
	const Vector4& color, 
	const char* field_model_path)
{
	data_list[static_cast<size_t>(i)].Create(device, i, name, image_path, color, field_model_path);
}

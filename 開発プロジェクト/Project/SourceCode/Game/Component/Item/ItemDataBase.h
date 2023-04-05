#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>
#include <array>

#include "Arithmetic/Arithmetic.h"
#include "Graphics/Resource/Texture/Texture.h"

/* --- �錾 ---- */

class ItemDataBase
{
public:

	enum class List
	{
		LifePotion,
		FirePotion,
		PoisonPotion,
		SparkPotion,

		ElementCount // �A�C�e���̐�
	};

	struct Data
	{
		void Create(
			ID3D11Device* device,
			List id,
			const wchar_t* name,
			const wchar_t* image_path,
			const Vector4& color,
			const char* field_model_path)
		{
			this->id = id;
			this->name = name;
			this->field_model_path = field_model_path;
			image.Create(device, image_path);
			this->color = color;
		}

		List id;
		std::wstring name;
		std::string field_model_path;
		Texture image;
		Vector4 color;
	};

	void Initialize(ID3D11Device* device);

	const Data& GetData(List i)
	{
		return data_list[static_cast<size_t>(i)];
	}

private:

	void Create(
		ID3D11Device* device,
		List i,
		const wchar_t* name,
		const wchar_t* image_path,
		const Vector4& color,
		const char* field_model_path);

	std::array<Data, static_cast<size_t>(List::ElementCount)> data_list;

};

#include "HUDDamage.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Arithmetic/MyMath.h"
#include "Game/Actor/Actor.h"
#include "Game/General/GameText.h"
#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/Camera/Camera.h"


/* --- 定義 ---- */

HUDDamage::HUDDamage()
{
}

HUDDamage::~HUDDamage()
{
}

void HUDDamage::Initialize()
{
	datas.clear();
}

void HUDDamage::AddDatas(
	const std::shared_ptr<Actor>& actor, 
	float display_time, 
	int32_t damage_val)
{
	auto result{ datas.try_emplace(actor->GetName(), Data(actor, display_time)) };
	
	// 挿入成功（新しいデータ）
	if (result.second)
	{
		result.first->second.damage_value += damage_val;
	}
	// 挿入失敗（すでにデータが存在している）
	else
	{
		result.first->second.damage_value += damage_val;
	}
}

void HUDDamage::Update(float elapsed_time)
{
	for (auto& data : datas)
	{
		//data.second.display_value += static_cast<double>(elapsed_time);

		data.second.display_value = std::lerp(data.second.display_value, static_cast<double>(data.second.damage_value), elapsed_time);

		data.second.display_value = (std::max)(data.second.display_value, static_cast<double>(data.second.damage_value));

		data.second.timer -= elapsed_time;
	}

	auto ptr{ std::ranges::find_if(datas, [](const auto& data) { return data.second.timer < 0.0f || data.second.w_actor.expired(); }) };
	if (ptr != datas.end())
	{
		datas.erase(ptr, datas.end());
	}
}

void HUDDamage::Render(const Camera* camera, float screen_width, float screen_height)
{

	TextRenderer::Format format
	{
		FontFamily::Makinas4,
		17.5f,
		DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
		DWRITE_FONT_WEIGHT_EXTRA_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL
	};

	for (const auto& data : datas)
	{
		auto actor{ data.second.w_actor.lock() };
		std::wstring disp_str{ std::to_wstring(static_cast<int32_t>(data.second.display_value)) };

		constexpr Vector3 world_offset{ 0.0f, 1.0f, 0.0f };
		const Vector3 world_pos{ actor->GetWorldPosition() + world_offset };
		const Vector2 screen_pos{ Math::WorldToScreen(world_pos, camera->GetView(), camera->GetProjection(), screen_width, screen_height) };

		GameSys.GetGameText()->SetText(
				GameText::Layer::HUD,
				disp_str.c_str(), format, { screen_pos.x - 60.0f, screen_pos.y - 7.5f + data.second.timer  * 2.0f}, { 120.0f, 15.0f }, { 1.0f, 1.0f, 1.0f, data.second.timer });
		//Graphics::GetInstance().GetTextRenderer().SetText(disp_str.c_str(), FontFamily::Kokumincho, 15.0f, { screen_pos.x - 60.0f, screen_pos.y - 7.5f + data.second.timer  * 2.0f}, { 120.0f, 15.0f }, { 1.0f, 0.0f, 0.0f, data.second.timer }, DWRITE_TEXT_ALIGNMENT_CENTER);
	}
}

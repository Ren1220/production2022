#include "EffekseerEffect.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "General/AtaiEngine.h"

#include "Graphics/General/Graphics.h"
#include "EffekseerManager.h"

/* --- 定義 ---- */

void EffekseerEffect::Create(Effekseer::Manager* manager, const char* filepath)
{
	//// エフェクトを読み込みする前にロックする
	//// マルチスレッドでEffectを作成するとDeviceContextを同時アクセスして
	//// フリーズする可能性があるので排他制御する
	//std::lock_guard<std::mutex> lock(DXSystem.GetMutex());

	// Effekseerのリソースを読み込む
	// EffekseerはUTF-16のファイルパス以外は対応していないため文字コード変換が必要
	char16_t utf_16_filename[256];
	Effekseer::ConvertUtf8ToUtf16(utf_16_filename, 256, filepath);

	// Effekseerエフェクト読み込み
	effect = Effekseer::Effect::Create(manager, (EFK_CHAR*)utf_16_filename);
}

Effekseer::Handle EffekseerEffect::Play(
	const Vector3& position)
{
	const auto& manager{ Graphics::GetInstance().GetEffekseerManager()->GetEffekseerManager() };

	auto handle = manager->Play(effect, position.x, position.y, position.z);
	manager->SetScale(handle, 1.0f, 1.0f, 1.0f);
	manager->SetRotation(handle, 0, 0, 0);

	return handle;
}

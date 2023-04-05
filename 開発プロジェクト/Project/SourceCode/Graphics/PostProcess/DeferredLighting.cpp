#include "DeferredLighting.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "imgui.h"

#include "Include/misc.h"

#include "Graphics/General/DXHelperUsers.h"
#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/ConstantBufferManager.h"
#include "Graphics/Resource/FrameBuffer.h"
#include "Graphics/Resource/Texture/TextureRenderer.h"

/* --- 定義 ---- */

void DeferredLighting::Initialize(ID3D11Device* device, uint32_t width, uint32_t height)
{
	HRESULT hr{ S_OK };

	this->width = width;
	this->height = height;

	// ジオメトリバッファの作成
	geometry_buffer.Create(device, width, height);

	// ライトバッファHDRに対応させている
	lighting_rt = std::make_shared<RenderTarget>();
	lighting_rt->Create(device, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);

	// 光源定数バッファの作成
	light_cb.Create(device);

	// シェーダー
	{
		lighting_shader.CreateVSFromCSO(device, "Shader/Sprite2DVS.cso");
		lighting_shader.CreatePSFromCSO(device, "Shader/DeferredLightingPS.cso");
		lighting_shader.CreateBlendState(device, BSMode::Alpha);
		lighting_shader.CreateDepthStencilState(device, DSMode::Depth_Stencil);
		lighting_shader.CreateRasterizerState(device, D3D11_FILL_SOLID, D3D11_CULL_NONE, false);
		lighting_shader.CreateSamplerState(device, 0, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
		lighting_shader.CreateSamplerState(device, 1, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
		lighting_shader.CreateSamplerState(device, 2, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP);
		lighting_shader.CreateSamplerState(device, 3, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_BORDER);
	}

	// 全方位シャドウテクスチャの品質は1024*1024で行う
	shadow_caster.Create(device, 1024, 1024);
}

void DeferredLighting::EndProcess(ID3D11DeviceContext* dc)
{
}

void DeferredLighting::Update()
{
	// 参照先が存在しない場合は配列から除外
	auto remove_itr{ std::ranges::remove_if(w_lights, [](const auto& w_light) {return w_light.expired(); }) };
	// eraseしないとデータは配列から除外されていない
	w_lights.erase(remove_itr.begin(), w_lights.end());
}

void DeferredLighting::ActivateGeometryBuffer(ID3D11DeviceContext* dc)
{
	geometry_buffer.ActivateGeometry(dc);
}

void DeferredLighting::ActivateBufferForEffekseer(ID3D11DeviceContext* dc)
{
	Graphics::GetInstance().GetFrameBuffer()->Set(dc,
		static_cast<float>(width),
		static_cast<float>(height),
		geometry_buffer.depth_stencil,
		0U,							 // 深度ステンシルビューの初期化は行わない
		geometry_buffer.emmisive_rt);// 自己発光			    
}

void DeferredLighting::ShadowCast(
	ID3D11DeviceContext* dc)
{
	// 書き込み開始
	shadow_caster.Activate(dc);

	for(size_t li = 0; li < w_lights.size(); ++li)
	{
		auto light{ w_lights[li].lock() };

		if (li > shadow_caster.GetMaxLightCount()) break; // とりあえずライトは１０個まで

		// 点光源のみ(減衰パラメータが０以上の光源を点光源とみなす)
		if (light->light_parameter.w >= 0.1f)
		{
			constexpr float near_z{ 0.1f }; // シャドウイング用のプロジェクション行列のnear_z少し離す

			shadow_caster.ClearRTV(dc);

			Vector3 eye{ &light->light_parameter.x };
			// 光源情報を与えて定数バッファを更新
			shadow_caster.UpdateCBuffer(dc, eye, near_z, light->light_parameter.w);
			shadow_caster.SetCBuffer(dc, CBufferLight::SLOT);
			// 深度情報を書き込む
			shadow_caster.RenderModel(dc);
			// 書き込んだ深度情報をキューブマップ配列の対応したインデックスにコピーする
			shadow_caster.WriteTextureCubeArray(dc, static_cast<uint32_t>(li));
		}
	}

	// 書き込み終了
	shadow_caster.Deactivate(dc);
}

void DeferredLighting::Lighting(ID3D11DeviceContext* dc)
{
	auto& frame_buffer{ Graphics::GetInstance().GetFrameBuffer() };
	auto& constant_buffer_manager{ Graphics::GetInstance().GetConstantBufferManager() };

	// ライトバッファを設定する
	frame_buffer->Set(
		dc,
		static_cast<float>(width),
		static_cast<float>(height),
		nullptr,
		0,
		lighting_rt);

	ID3D11ShaderResourceView* srvs[]
	{
		geometry_buffer.color_rt->GetTexture().GetView().Get(),				 // 0 : 色
		geometry_buffer.roughness_metallic_rt->GetTexture().GetView().Get(), // 1 : ラフネス・メタリック
		geometry_buffer.emmisive_rt->GetTexture().GetView().Get(),			 // 2 : 自己発光
		geometry_buffer.occlusion_rt->GetTexture().GetView().Get(),			 // 3 : 陰影
		geometry_buffer.normal_rt->GetTexture().GetView().Get(),			 // 4 : 法線
		geometry_buffer.world_position_rt->GetTexture().GetView().Get(),	 // 5 : 3D座標
		shadow_caster.GetShadowCastTextureCubeArray().srv.Get()
	};

	// バッファにライティング結果を書き込む
	
	{
		dc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// 描画開始
		Graphics::GetInstance().GetTextureRenderer()->BeginDraw(dc, &lighting_shader);

		// 定数バッファの更新
		{
			CBufferLight cb{};

			// 設定しているライトの情報をバッファに書き込んでいく
			for (size_t li = 0; li < w_lights.size(); ++li)
			{
				if (li > shadow_caster.GetMaxLightCount()) break; // とりあえずライトは１０個まで

				memcpy_s(&cb.light[li], sizeof(Light), w_lights[li].lock().get(), sizeof(Light));
			}

			light_cb.UpdateSubresouce(dc, cb);
			constant_buffer_manager->SetBuffer(dc, CBufferLight::SLOT, &light_cb);
		}


		// 描画実行
		Graphics::GetInstance().GetTextureRenderer()->Draw(
			dc,
			srvs,
			_countof(srvs),
			static_cast<float>(width),
			static_cast<float>(height));

		Graphics::GetInstance().GetTextureRenderer()->EndDraw(dc);
	}
}

void DeferredLighting::ImGuiRender()
{
	ImGui::Begin("Deferred");

	geometry_buffer.ImGuiRender();

	ImGui::End();

	shadow_caster.ImGuiRender();
}

void DeferredLighting::RegisterLight(const std::shared_ptr<Light>& light_data)
{
	_ASSERT_EXPR(static_cast<uint32_t>(w_lights.size()) < shadow_caster.GetMaxLightCount(), L"設定したシャドウイングを行う点光源の数を超えています。");
	w_lights.emplace_back(light_data);
}

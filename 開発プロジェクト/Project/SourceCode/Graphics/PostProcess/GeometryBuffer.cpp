#include "GeometryBuffer.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "imgui.h"

#include "Graphics/Shader/Shader.h"
#include "Graphics/General/DXHelperUsers.h"
#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/FrameBuffer.h"

/* --- 定義 ---- */

GeometryBuffer::GeometryBuffer()
{

}

void GeometryBuffer::Create(ID3D11Device* device, uint32_t width, uint32_t height)
{
	this->width = width;
	this->height = height;

	// 各バッファの作成
	depth_stencil			= std::make_shared<DepthStencil>();
	color_rt				= std::make_shared<RenderTarget>();
	roughness_metallic_rt	= std::make_shared<RenderTarget>();
	normal_rt				= std::make_shared<RenderTarget>();
	emmisive_rt				= std::make_shared<RenderTarget>();
	occlusion_rt			= std::make_shared<RenderTarget>();
	world_position_rt		= std::make_shared<RenderTarget>();
	depth_rt				= std::make_shared<RenderTarget>();


	depth_stencil->Create(		  device, width, height, DXGI_FORMAT_R32_FLOAT);
	color_rt->Create(			  device, width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
	roughness_metallic_rt->Create(device, width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
	normal_rt->Create(			  device, width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
	emmisive_rt->Create(		  device, width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
	occlusion_rt->Create(		  device, width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
	world_position_rt->Create(	  device, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT); // 今回は１６で大丈夫
	//world_position_rt->Create(	  device, width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);
	depth_rt->Create(			  device, width, height, DXGI_FORMAT_R32_FLOAT);
}

void GeometryBuffer::ActivateGeometry(ID3D11DeviceContext* dc)
{
	auto& frame_buffer{ Graphics::GetInstance().GetFrameBuffer() };

	frame_buffer->Set(dc,
		static_cast<float>(width),
		static_cast<float>(height),
		depth_stencil,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		color_rt,				// 0 : 色				    
		roughness_metallic_rt,	// 1 : ラフネス・メタリック  
		emmisive_rt,			// 2 : 自己発光			    
		occlusion_rt,			// 3 : 陰影				  
		normal_rt,				// 4 : 法線				  
		world_position_rt,		// 5 : 3D位置				 
		depth_rt);				// 6 : 深度				  
}

void GeometryBuffer::ImGuiRender()
{
	auto show_buffer_texture = [](const char* name, const std::shared_ptr<RenderTarget>& buffer_data)->void
	{
		ImGui::Text(name);
		ImGui::Image(buffer_data->GetTexture().GetView().Get(), {256, 144});
		ImGui::Spacing();
	};

	show_buffer_texture("Color", color_rt);
	show_buffer_texture("RoughnessMetallic", roughness_metallic_rt);
	show_buffer_texture("Emmisive", emmisive_rt);
	show_buffer_texture("Occlusion", occlusion_rt);
	show_buffer_texture("Normal", normal_rt);
	show_buffer_texture("WorldPosition", world_position_rt);
	show_buffer_texture("Depth", depth_rt);
}

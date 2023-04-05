#include "ConstantBufferManager.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <format>

/* --- 定義 ---- */

void ConstantBufferManager::EndProcess(ID3D11DeviceContext* dc)
{
	// 定数バッファリストをすべて空にする。
	std::ranges::fill(constant_buffers, nullptr);
}

void ConstantBufferManager::PrepareBuffer(
	uint32_t index, 
	const ConstantBufferBase* buffer, 
	bool enable_caution)
{
#ifdef _DEBUG
	static std::array<bool, D3D11_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT> caution{};

	if (constant_buffers[index] != nullptr && buffer != nullptr)
	{
		if (constant_buffers[index]->GetHash() != buffer->GetHash())
		{
			if (caution[index])
			{
				auto sub1{ constant_buffers[index]->name };
				auto sub2{ std::string(buffer->GetName()) };

				auto log{ std::format(
					L"監視対象の定数バッファ{}が{}に上書きされました。",
					std::wstring(sub1.begin(), sub1.end()),
					std::wstring(sub2.begin(), sub2.end())) };
				OutputDebugStringW(log.c_str());
			}
		}
	}
#endif // !

	_ASSERT(buffer != nullptr);
	constant_buffers[index] = std::make_unique<CBufferData>(buffer->GetName(), buffer->Get());


#ifdef _DEBUG
	caution[index] = enable_caution;
#endif // !
}

void ConstantBufferManager::ActivateBuffer(ID3D11DeviceContext* dc) const
{
	ID3D11Buffer* cbs[D3D11_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT]{ };
	for (uint32_t ci = 0; const auto& cb : constant_buffers)
	{
		cbs[ci++] = cb ? cb->constant_buffer.Get() : nullptr;
	}

	dc->CSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, cbs);

	dc->VSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, cbs);
	dc->GSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, cbs);
	dc->HSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, cbs);
	dc->DSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, cbs);
	dc->PSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, cbs);
}

void ConstantBufferManager::SetBuffer(
	ID3D11DeviceContext* dc, 
	uint32_t index, 
	const ConstantBufferBase* buffer, 
	bool enable_caution)
{
	// リストに登録
	PrepareBuffer(index, buffer, enable_caution);

	// デバイスコンテキストに設定
	dc->CSSetConstantBuffers(index, 1, buffer->GetAddressOf());
	dc->VSSetConstantBuffers(index, 1, buffer->GetAddressOf());
	dc->GSSetConstantBuffers(index, 1, buffer->GetAddressOf());
	dc->HSSetConstantBuffers(index, 1, buffer->GetAddressOf());
	dc->DSSetConstantBuffers(index, 1, buffer->GetAddressOf());
	dc->PSSetConstantBuffers(index, 1, buffer->GetAddressOf());
}

void ConstantBufferManager::OutputCurrentConstantBufferLog() const
{
#ifdef _DEBUG
	std::stringstream str;

	for (uint32_t ci = 0; const auto & cb : constant_buffers)
	{
		str << ci++ << " " << (cb ? cb->name : "nullptr") << std::endl;
	}

	OutputDebugStringA(str.str().c_str());
#endif // _DEBUG
}

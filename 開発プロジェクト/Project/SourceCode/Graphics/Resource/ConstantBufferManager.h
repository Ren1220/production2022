#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <array>

#include "ConstantBuffer.h"

/* --- 宣言 ---- */

/// <summary>
/// 定数バッファの設定の管理を行うクラス。主にデバッグの為にこのクラスを介して設定している。
/// OutputCurrentConstantBufferLog()で何番にどんな名前の定数バッファが設定されているか分かる為便利。
/// </summary>
class ConstantBufferManager
{
public:

	ConstantBufferManager() :
		constant_buffers() {}
	virtual ~ConstantBufferManager() = default;

	void EndProcess(ID3D11DeviceContext* dc);
	/// <summary>
	/// リストの引数のインデックスに定数バッファを登録する。
	/// デバイスコンテキストに設定されるわけではない。
	/// </summary>
	/// <param name="index"></param>
	/// <param name="buffer"></param>
	/// <param name="enable_caution">登録するバッファと異なる種類のデータで再登録された際にログに警告を出す。</param>
	void PrepareBuffer(
		uint32_t index, 
		const ConstantBufferBase* buffer,
		bool enable_caution = false);
	/// <summary>
	/// リストに登録されたすべてのバッファをまとめてデバイスコンテキストに設定する。
	/// </summary>
	void ActivateBuffer(ID3D11DeviceContext* dc) const;
	/// <summary>
	/// リストの引数のインデックスに定数バッファを登録する。
	/// デバイスコンテキストの引数のスロットに定数バッファを設定する。
	/// </summary>
	void SetBuffer(
		ID3D11DeviceContext* dc, 
		uint32_t index, 
		const ConstantBufferBase* buffer,
		bool enable_caution = false);

	void OutputCurrentConstantBufferLog() const;

private:

	struct CBufferData
	{
		CBufferData(const char* name, ID3D11Buffer* cbuffer)
			: name(name), constant_buffer(cbuffer) {}
		~CBufferData(){};

		const size_t GetHash() const { return std::hash<std::string>()(name); }

		const std::string name;
		const Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
	};

	std::array<std::unique_ptr<CBufferData>, D3D11_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT> constant_buffers;

};

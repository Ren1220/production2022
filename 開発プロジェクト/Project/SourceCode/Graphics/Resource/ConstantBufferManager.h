#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>
#include <array>

#include "ConstantBuffer.h"

/* --- �錾 ---- */

/// <summary>
/// �萔�o�b�t�@�̐ݒ�̊Ǘ����s���N���X�B��Ƀf�o�b�O�ׂ̈ɂ��̃N���X����Đݒ肵�Ă���B
/// OutputCurrentConstantBufferLog()�ŉ��Ԃɂǂ�Ȗ��O�̒萔�o�b�t�@���ݒ肳��Ă��邩������ו֗��B
/// </summary>
class ConstantBufferManager
{
public:

	ConstantBufferManager() :
		constant_buffers() {}
	virtual ~ConstantBufferManager() = default;

	void EndProcess(ID3D11DeviceContext* dc);
	/// <summary>
	/// ���X�g�̈����̃C���f�b�N�X�ɒ萔�o�b�t�@��o�^����B
	/// �f�o�C�X�R���e�L�X�g�ɐݒ肳���킯�ł͂Ȃ��B
	/// </summary>
	/// <param name="index"></param>
	/// <param name="buffer"></param>
	/// <param name="enable_caution">�o�^����o�b�t�@�ƈقȂ��ނ̃f�[�^�ōēo�^���ꂽ�ۂɃ��O�Ɍx�����o���B</param>
	void PrepareBuffer(
		uint32_t index, 
		const ConstantBufferBase* buffer,
		bool enable_caution = false);
	/// <summary>
	/// ���X�g�ɓo�^���ꂽ���ׂẴo�b�t�@���܂Ƃ߂ăf�o�C�X�R���e�L�X�g�ɐݒ肷��B
	/// </summary>
	void ActivateBuffer(ID3D11DeviceContext* dc) const;
	/// <summary>
	/// ���X�g�̈����̃C���f�b�N�X�ɒ萔�o�b�t�@��o�^����B
	/// �f�o�C�X�R���e�L�X�g�̈����̃X���b�g�ɒ萔�o�b�t�@��ݒ肷��B
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

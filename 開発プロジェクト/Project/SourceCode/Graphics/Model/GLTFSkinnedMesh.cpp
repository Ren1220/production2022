#include "GLTFSkinnedMesh.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include <functional>

#include "imgui.h"

#include "Include/misc.h"

#include "Graphics/General/Graphics.h"
#include "Graphics/General/DXHelperUsers.h"
#include "Graphics/General/ResourceFileloader.h"
#include "Graphics/Shader/Shader.h"
#include "Graphics/Resource/ConstantBufferManager.h"

#include "GLTFModelResource.h"

/* --- ��` ---- */

GLTFSkinnedMesh::GLTFSkinnedMesh(ID3D11Device* device, const char* filepath)
	: GLTFModel(device, filepath)
{

	// �A�j���V�����m�[�h�̐ݒ�
	const auto& nodes{ GetModelData()->nodes };

	for (size_t i = 0; i < nodes.size(); ++i)
	{
		std::shared_ptr<GLTFModelData::Node>& animated_node{ animated_nodes.emplace_back() };
		// �l�̃R�s�[�����A�V�������������m�ۂ���
		animated_node = std::make_shared<GLTFModelData::Node>(*nodes.at(i));
	}

	// �v���~�e�B�u�萔�o�b�t�@
	primitive_cb.Create(device);
	// �{�[���s��萔�o�b�t�@
	joint_cb.Create(device);
}

GLTFSkinnedMesh::~GLTFSkinnedMesh()
{
}

void GLTFSkinnedMesh::Render(ID3D11DeviceContext* dc, Shader* shader) const
{
	auto model_data{ GetModelData() };

	if (shader)
	{
		shader->Activate(dc);
	}
	else
	{
		// �V�F�[�_��ݒ肵�Ȃ��ꍇ�͔ėp�V�F�[�_�[�𗘗p����
		model_data->ActivateDefaultShader(dc);
	}

	// ���̓��C�A�E�g�͂��ׂẴ��f���`��V�F�[�_�[�ŋ���
	dc->IASetInputLayout(model_data->GetInputLayout().Get());

	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �}�e���A���f�[�^���Q�Ƃ���V�F�[�_���\�[�X�r���[
	dc->PSSetShaderResources(0, 1, model_data->material_resource_view.GetAddressOf());
	
	for (const auto& node_index : model_data->scenes.at(0).nodes)
	{
		// �m�[�h���ƂɃm�[�h�̃��b�V����`�悷��ċA�֐�
		DrawMeshRecursive(dc, node_index);
	}
}

bool GLTFSkinnedMesh::Animate(size_t animation_index, float time, bool loop)
{
	using namespace std;

	// ���f���f�[�^�̎Q�Ƃ̎擾
	auto model_data{ GetModelData() };

	// �L�[�t���[���̏ꏊ�ƕ�Ԓl��Ԃ��֐�
	std::function<size_t(const vector<float>&, float&, float&, bool, bool&)> index_of{
		[](const vector<float>& timelines, float &time, float& interpolation_factor, bool loop, bool& end)-> size_t
		{
			const size_t keyframe_count{ timelines.size() };

			// �A�j���[�V�������I�������
			if (time > timelines.at(keyframe_count - 1))
			{
				// ���[�v����ꍇ
				if (loop)
				{
					// ���݂̃L�[�t���[����S�̂̃L�[�t���[�����Ŋ������l��V�����L�[�t���[���ɐݒ肷��
					time = fmodf(time, timelines.at(keyframe_count - 1));
				}
				// ���[�v���Ȃ��ꍇ
				else
				{
					// �A�j���[�V���������m�[�h�̈ʒu������Ȃ��悤�ɕ�Ԓl���ő�ɂ��A
					// �Ōォ��ЂƂO�̃L�[�t���[����Ԃ�
					interpolation_factor = 1.0f;
					end = false; // �Đ��I���t���O
					return keyframe_count - 2;
				}
			}
			// �A�j���[�V�����J�n�O
			else if (time < timelines.at(0))
			{
				interpolation_factor = 0.0f;
				return 0;
			}
			// �A�j���V������

			// �L�[�t���[���̏ꏊ��T��
			size_t keyframe_index{ 0 };
			for (size_t time_index = 1; time_index < keyframe_count; ++time_index)
			{
				// ���݂̎��Ԃ��^�C�����C���̎��Ԃ𒴂����n�_�̃L�[�t���[����ݒ肷��
				if (time < timelines.at(time_index))
				{
					keyframe_index = max<size_t>(0LL, time_index - 1);
					break;
				}
			}
			// ��Ԓl���v�Z����
			interpolation_factor = (time - timelines.at(keyframe_index)) /
					(timelines.at(keyframe_index + 1)) - timelines.at(keyframe_index);

			// �L�[�t���[���ԍ���Ԃ�
			return keyframe_index;
		}
	};

	// �Đ��t���O
	bool is_play{ true };

	// �A�j���V������񂪑��݂���ꍇ
	if (model_data->animations.size() > 0)
	{
		const GLTFModelData::Animation& animation{ model_data->animations.at(animation_index) };

		for (vector<GLTFModelData::Animation::Channel>::const_reference channel : animation.channels)
		{
			const GLTFModelData::Animation::Sampler& sampler{ animation.samplers.at(channel.sampler) };
			const vector<float>& timeline{ animation.timelines.at(sampler.input) };

			if (timeline.size() == 0)
			{
				continue;
			}

			float interpolation_factor{ };
			// ���݂̃L�[�t���[���̏�Ԃ��擾����A�Đ��I�����Ă��邩�ǂ�����
			size_t keyframe_index{ index_of(timeline, time, interpolation_factor, loop, is_play) };
			// �A�j���V�����̑Ώۂ��X�P�[���̏ꍇ
			if (channel.target_path == "scale")
			{
				const vector<Vector3>& scales{ animation.scales.at(sampler.output) };
				animated_nodes.at(channel.target_node)->scale = scales.at(keyframe_index).Lerp(scales.at(keyframe_index + 1), interpolation_factor);
			}
			else if (channel.target_path == "rotation")
			{
				const vector<Quaternion>& rotations{ animation.rotations.at(sampler.output) };

				animated_nodes.at(channel.target_node)->rotation = rotations.at(keyframe_index).Slerp(rotations.at(keyframe_index + 1), interpolation_factor).Normalize();

				//XMFLOAT4 q0{ &rotations.at(keyframe_index).x };
				//XMFLOAT4 q1{ &rotations.at(keyframe_index + 1).x };

				//XMFLOAT4 q;
				//XMStoreFloat4(&q, XMQuaternionSlerp(XMLoadFloat4(&q0), XMLoadFloat4(&q1), interpolation_factor));

				//animated_nodes.at(channel.target_node).rotation = &q.x;

			}
			else if (channel.target_path == "translation")
			{
				const vector<Vector3>& translations{ animation.translations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->translation = translations.at(keyframe_index).Lerp(translations.at(keyframe_index + 1), interpolation_factor);
			}
		}

		// �O���[�o���s����X�V����
		model_data->CumulateTransforms(animated_nodes);
	}
	else
	{
		animated_nodes = model_data->nodes;
	}

	return is_play;
}


bool GLTFSkinnedMesh::Animate(size_t animation_index, float& time)
{
	using namespace std;
	// ���f���f�[�^�̎Q�Ƃ̎擾
	auto model_data{ GetModelData() };

	// �L�[�t���[���̏ꏊ�ƕ�Ԓl��Ԃ��֐�
	function<size_t(const vector<float>&, float&, float&)> index_of =
		[](const vector<float>& timelines, float& time, float& interpolation_factor)-> size_t {
			const size_t keyframe_count{ timelines.size() };

			float end_time { timelines.at(keyframe_count - 1) };

			// �A�j���[�V�������I�������
			if (time > end_time)
			{
				// �A�j���[�V���������m�[�h�̈ʒu������Ȃ��悤�ɕ�Ԓl���ő�ɂ��A
				// �Ōォ��ЂƂO�̃L�[�t���[����Ԃ�
				interpolation_factor = 1.0f;
				return keyframe_count - 2;
			}
			// �A�j���[�V�����J�n�O
			else if (time < timelines.at(0))
			{
				interpolation_factor = 0.0f;
				return 0;
			}
			// �A�j���V������

			// �L�[�t���[���̏ꏊ��T��
			size_t keyframe_index{ 0 };
			for (size_t time_index = 1; time_index < keyframe_count; ++time_index)
			{
				// ���݂̎��Ԃ��^�C�����C���̎��Ԃ𒴂����n�_�̃L�[�t���[����ݒ肷��
				if (time < timelines.at(time_index))
				{
					keyframe_index = max<size_t>(0LL, time_index - 1);
					break;
				}
			}

			// ��Ԓl���v�Z����
			interpolation_factor = (time - timelines.at(keyframe_index)) /
					(timelines.at(keyframe_index + 1)) - timelines.at(keyframe_index);

			// �L�[�t���[���ԍ���Ԃ�
			return keyframe_index;
	};

	if(model_data->animations.size() > 0)
	{
		const GLTFModelData::Animation& animation{ model_data->animations.at(animation_index) };

		for (vector<GLTFModelData::Animation::Channel>::const_reference channel : animation.channels)
		{
			const GLTFModelData::Animation::Sampler& sampler{ animation.samplers.at(channel.sampler) };
			const vector<float>& timeline{ animation.timelines.at(sampler.input) };

			if (timeline.size() == 0)
			{
				continue;
			}

			float interpolation_factor{ };
			// ���݂̃L�[�t���[���̏�Ԃ��擾����
			size_t keyframe_index{ index_of(timeline, time, interpolation_factor) };
			// �A�j���V�����̑Ώۂ��X�P�[���̏ꍇ
			if (channel.target_path == "scale")
			{
				const vector<Vector3>& scales{ animation.scales.at(sampler.output) };
				animated_nodes.at(channel.target_node)->scale = scales.at(keyframe_index).Lerp(scales.at(keyframe_index + 1), interpolation_factor);
			}
			else if (channel.target_path == "rotation")
			{
				const vector<Quaternion>& rotations{ animation.rotations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->rotation = rotations.at(keyframe_index).Slerp(rotations.at(keyframe_index + 1), interpolation_factor).Normalize();
			}
			else if (channel.target_path == "translation")
			{
				const vector<Vector3>& translations{ animation.translations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->translation = translations.at(keyframe_index).Lerp(translations.at(keyframe_index + 1), interpolation_factor);
			}
		}

		// �O���[�o���s����X�V����
		model_data->CumulateTransforms(animated_nodes);

		// �A�j���[�V�������I��������
		float end{ animation.MeasureAnimationTimeEnd() };
		if (time > end)
		{
			time = end;
			return true;
		}
	}

	return false;
}

void GLTFSkinnedMesh::AnimateLoop(size_t animation_index, float& time, bool& has_looped)
{
	has_looped = false;

	using namespace std;
	// ���f���f�[�^�̎Q�Ƃ̎擾
	auto model_data{ GetModelData() };

	if (model_data->animations.size() <= animation_index) return;

	// �L�[�t���[���̏ꏊ�ƕ�Ԓl��Ԃ��֐�
	function<size_t(const vector<float>&, float&, float&)> index_of{
		[](const vector<float>& timelines, float& time, float& interpolation_factor)-> size_t
		{
			const size_t keyframe_count{ timelines.size() };

			float end_time { timelines.at(keyframe_count - 1) };

			// �A�j���[�V�������I�������
			if (time > end_time)
			{
				// ���݂̃L�[�t���[����S�̂̃L�[�t���[�����Ŋ������l��V�����L�[�t���[���ɐݒ肷��
				time = fmodf(time, end_time);
			}
			// �A�j���[�V�����J�n�O
			else if (time < timelines.at(0))
			{
				interpolation_factor = 0.0f;
				return 0;
			}
			// �A�j���V������

			// �L�[�t���[���̏ꏊ��T��
			size_t keyframe_index{ 0 };
			for (size_t time_index = 1; time_index < keyframe_count; ++time_index)
			{
				// ���݂̎��Ԃ��^�C�����C���̎��Ԃ𒴂����n�_�̃L�[�t���[����ݒ肷��
				if (time < timelines.at(time_index))
				{
					keyframe_index = max<size_t>(0LL, time_index - 1);
					break;
				}
			}

			// ��Ԓl���v�Z����
			interpolation_factor = (time - timelines.at(keyframe_index)) /
					(timelines.at(keyframe_index + 1)) - timelines.at(keyframe_index);

			// �L�[�t���[���ԍ���Ԃ�
			return keyframe_index;
		}
	};

	if(model_data->animations.size() > 0)
	{
		const auto& animation{ model_data->animations.at(animation_index) };

		for (vector<GLTFModelData::Animation::Channel>::const_reference channel : animation.channels)
		{
			const GLTFModelData::Animation::Sampler& sampler{ animation.samplers.at(channel.sampler) };
			const vector<float>& timeline{ animation.timelines.at(sampler.input) };

			if (timeline.size() == 0)
			{
				continue;
			}

			float interpolation_factor{ };
			// ���݂̃L�[�t���[���̏�Ԃ��擾����
			size_t keyframe_index{ index_of(timeline, time, interpolation_factor) };
			// �A�j���V�����̑Ώۂ��X�P�[���̏ꍇ
			if (channel.target_path == "scale")
			{
				const vector<Vector3>& scales{ animation.scales.at(sampler.output) };
				animated_nodes.at(channel.target_node)->scale = scales.at(keyframe_index).Lerp(scales.at(keyframe_index + 1), interpolation_factor);
			}
			else if (channel.target_path == "rotation")
			{
				const vector<Quaternion>& rotations{ animation.rotations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->rotation = rotations.at(keyframe_index).Slerp(rotations.at(keyframe_index + 1), interpolation_factor).Normalize();
			}
			else if (channel.target_path == "translation")
			{
				const vector<Vector3>& translations{ animation.translations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->translation = translations.at(keyframe_index).Lerp(translations.at(keyframe_index + 1), interpolation_factor);
			}
		}

		// �O���[�o���s����X�V����
		model_data->CumulateTransforms(animated_nodes);

		// �A�j���[�V���������[�v������
		float end{ animation.MeasureAnimationTimeEnd() };
		if (time > end)
		{
			time = fmodf(time, end);
			has_looped = true;
		}
	}


	return;
}

#if 0
bool GLTFModel::Animate(const GLTFModelData::Animation& animation, float& time, bool loop)
{
	using namespace std;
	// ���f���f�[�^�̎Q�Ƃ̎擾
	std::shared_ptr<GLTFModelData> model_data{ GetModelData() };

	// �L�[�t���[���̏ꏊ�ƕ�Ԓl��Ԃ��֐�
	function<size_t(const vector<float>&, float&, float&, bool, bool&)> index_of{
		[](const vector<float>& timelines, float& time, float& interpolation_factor, bool loop, bool& end)-> size_t
		{
			const size_t keyframe_count{ timelines.size() };

			// �A�j���[�V�������I�������
			if (time > timelines.at(keyframe_count - 1))
			{
				// ���[�v����ꍇ
				if (loop)
				{
					// ���݂̃L�[�t���[����S�̂̃L�[�t���[�����Ŋ������l��V�����L�[�t���[���ɐݒ肷��
					time = fmodf(time, timelines.at(keyframe_count - 1));
				}
				// ���[�v���Ȃ��ꍇ
				else
				{
					// �A�j���[�V���������m�[�h�̈ʒu������Ȃ��悤�ɕ�Ԓl���ő�ɂ��A
					// �Ōォ��ЂƂO�̃L�[�t���[����Ԃ�
					interpolation_factor = 1.0f;
					end = false; // �Đ��I���t���O
					return keyframe_count - 2;
				}
			}
			// �A�j���[�V�����J�n�O
			else if (time < timelines.at(0))
			{
				interpolation_factor = 0.0f;
				return 0;
			}
			// �A�j���V������

			// �L�[�t���[���̏ꏊ��T��
			size_t keyframe_index{ 0 };
			for (size_t time_index = 1; time_index < keyframe_count; ++time_index)
			{
				// ���݂̎��Ԃ��^�C�����C���̎��Ԃ𒴂����n�_�̃L�[�t���[����ݒ肷��
				if (time < timelines.at(time_index))
				{
					keyframe_index = max<size_t>(0LL, time_index - 1);
					break;
				}
			}
			// ��Ԓl���v�Z����
			interpolation_factor = (time - timelines.at(keyframe_index)) /
					(timelines.at(keyframe_index + 1)) - timelines.at(keyframe_index);

			// �L�[�t���[���ԍ���Ԃ�
			return keyframe_index;
		}
	};

	// �Đ��t���O
	bool is_play{ true };

	{
		for (vector<GLTFModelData::Animation::Channel>::const_reference channel : animation.channels)
		{
			const GLTFModelData::Animation::Sampler& sampler{ animation.samplers.at(channel.sampler) };
			const vector<float>& timeline{ animation.timelines.at(sampler.input) };

			if (timeline.size() == 0)
			{
				continue;
			}

			float interpolation_factor{ };
			// ���݂̃L�[�t���[���̏�Ԃ��擾����A�Đ��I�����Ă��邩�ǂ�����
			size_t keyframe_index{ index_of(timeline, time, interpolation_factor, loop, is_play) };
			// �A�j���V�����̑Ώۂ��X�P�[���̏ꍇ
			if (channel.target_path == "scale")
			{
				const vector<Vector3>& scales{ animation.scales.at(sampler.output) };
				animated_nodes.at(channel.target_node)->scale = scales.at(keyframe_index).Lerp(scales.at(keyframe_index + 1), interpolation_factor);
			}
			else if (channel.target_path == "rotation")
			{
				const vector<Quaternion>& rotations{ animation.rotations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->rotation = rotations.at(keyframe_index).Slerp(rotations.at(keyframe_index + 1), interpolation_factor).Normalize();
			}
			else if (channel.target_path == "translation")
			{
				const vector<Vector3>& translations{ animation.translations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->translation = translations.at(keyframe_index).Lerp(translations.at(keyframe_index + 1), interpolation_factor);
			}
		}

		// �O���[�o���s����X�V����
		model_data->CumulateTransforms(animated_nodes);
	}

	return is_play;
}
#endif

void GLTFSkinnedMesh::AnimateLoop(const GLTFModelData::Animation& animation, float& time, bool& has_looped)
{
	has_looped = false;

	using namespace std;
	// ���f���f�[�^�̎Q�Ƃ̎擾
	auto model_data{ GetModelData() };

	// �L�[�t���[���̏ꏊ�ƕ�Ԓl��Ԃ��֐�
	function<size_t(const vector<float>&, float&, float&)> index_of{
		[](const vector<float>& timelines, float& time, float& interpolation_factor)-> size_t
		{
			const size_t keyframe_count{ timelines.size() };

			float end_time { timelines.at(keyframe_count - 1) };

			// �A�j���[�V�������I�������
			if (time > end_time)
			{
				// ���݂̃L�[�t���[����S�̂̃L�[�t���[�����Ŋ������l��V�����L�[�t���[���ɐݒ肷��
				time = fmodf(time, end_time);
			}
			// �A�j���[�V�����J�n�O
			else if (time < timelines.at(0))
			{
				interpolation_factor = 0.0f;
				return 0;
			}
			// �A�j���V������

			// �L�[�t���[���̏ꏊ��T��
			size_t keyframe_index{ 0 };
			for (size_t time_index = 1; time_index < keyframe_count; ++time_index)
			{
				// ���݂̎��Ԃ��^�C�����C���̎��Ԃ𒴂����n�_�̃L�[�t���[����ݒ肷��
				if (time < timelines.at(time_index))
				{
					keyframe_index = max<size_t>(0LL, time_index - 1);
					break;
				}
			}

			// ��Ԓl���v�Z����
			interpolation_factor = (time - timelines.at(keyframe_index)) /
					(timelines.at(keyframe_index + 1)) - timelines.at(keyframe_index);

			// �L�[�t���[���ԍ���Ԃ�
			return keyframe_index;
		}
	};


	{
		for (vector<GLTFModelData::Animation::Channel>::const_reference channel : animation.channels)
		{
			const GLTFModelData::Animation::Sampler& sampler{ animation.samplers.at(channel.sampler) };
			const vector<float>& timeline{ animation.timelines.at(sampler.input) };

			if (timeline.size() == 0)
			{
				continue;
			}

			float interpolation_factor{ };
			// ���݂̃L�[�t���[���̏�Ԃ��擾����
			size_t keyframe_index{ index_of(timeline, time, interpolation_factor) };
			// �A�j���V�����̑Ώۂ��X�P�[���̏ꍇ
			if (channel.target_path == "scale")
			{
				const vector<Vector3>& scales{ animation.scales.at(sampler.output) };
				animated_nodes.at(channel.target_node)->scale = scales.at(keyframe_index).Lerp(scales.at(keyframe_index + 1), interpolation_factor);
			}
			else if (channel.target_path == "rotation")
			{
				const vector<Quaternion>& rotations{ animation.rotations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->rotation = rotations.at(keyframe_index).Slerp(rotations.at(keyframe_index + 1), interpolation_factor).Normalize();
			}
			else if (channel.target_path == "translation")
			{
				const vector<Vector3>& translations{ animation.translations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->translation = translations.at(keyframe_index).Lerp(translations.at(keyframe_index + 1), interpolation_factor);
			}
		}

		// �O���[�o���s����X�V����
		model_data->CumulateTransforms(animated_nodes);
	}

	// �A�j���[�V���������[�v������
	float end{ animation.MeasureAnimationTimeEnd() };
	if (time > end)
	{
		time = fmodf(time, end);
		has_looped = true;
	}

	return;
}

void GLTFSkinnedMesh::BlendAnimation(size_t index1, size_t index2, float time1, float time2, float interpolation)
{
	using namespace std;
	// ���f���f�[�^�̎Q�Ƃ̎擾
	auto model_data{ GetModelData() };

	if (index1 < 0 || index2 < 0 || index1 >= model_data->animations.size() || index2 >= model_data->animations.size()) return;

	// �L�[�t���[���̏ꏊ�ƕ�Ԓl��Ԃ��֐�
	function<size_t(const vector<float>&, float&, float&)> index_of{
		[](const vector<float>& timelines, float& time, float& interpolation_factor)-> size_t
		{
			const size_t keyframe_count{ timelines.size() };

			float end_time { timelines.at(keyframe_count - 1) };

			// �A�j���[�V�������I�������
			if (time > end_time)
			{
				// ���݂̃L�[�t���[����S�̂̃L�[�t���[�����Ŋ������l��V�����L�[�t���[���ɐݒ肷��
				time = fmodf(time, end_time);
			}
			// �A�j���[�V�����J�n�O
			else if (time < timelines.at(0))
			{
				interpolation_factor = 0.0f;
				return 0;
			}
			// �A�j���V������

			// �L�[�t���[���̏ꏊ��T��
			size_t keyframe_index{ 0 };
			for (size_t time_index = 1; time_index < keyframe_count; ++time_index)
			{
				// ���݂̎��Ԃ��^�C�����C���̎��Ԃ𒴂����n�_�̃L�[�t���[����ݒ肷��
				if (time < timelines.at(time_index))
				{
					keyframe_index = max<size_t>(0LL, time_index - 1);
					break;
				}
			}

			// ��Ԓl���v�Z����
			interpolation_factor = (time - timelines.at(keyframe_index)) /
					(timelines.at(keyframe_index + 1)) - timelines.at(keyframe_index);

			// �L�[�t���[���ԍ���Ԃ�
			return keyframe_index;
		}
	};

	std::vector<GLTFModelData::Node> node1(animated_nodes.size());
	for (size_t i = 0ULL; i < animated_nodes.size(); ++i)
	{
		node1[i] = *animated_nodes[i];
	}
	std::vector<GLTFModelData::Node>  node2 = node1;

	if (model_data->animations.size() > 0)
	{
		
		const auto& animation{ model_data->animations.at(index1) };

		for (vector<GLTFModelData::Animation::Channel>::const_reference channel : animation.channels)
		{
			const GLTFModelData::Animation::Sampler& sampler{ animation.samplers.at(channel.sampler) };
			const vector<float>& timeline{ animation.timelines.at(sampler.input) };

			if (timeline.size() == 0)
			{
				continue;
			}

			float interpolation_factor{ };
			// ���݂̃L�[�t���[���̏�Ԃ��擾����
			size_t keyframe_index{ index_of(timeline, time1, interpolation_factor) };
			// �A�j���V�����̑Ώۂ��X�P�[���̏ꍇ
			if (channel.target_path == "scale")
			{
				const vector<Vector3>& scales{ animation.scales.at(sampler.output) };
				node1.at(channel.target_node).scale = scales.at(keyframe_index).Lerp(scales.at(keyframe_index + 1), interpolation_factor);
			}
			else if (channel.target_path == "rotation")
			{
				const vector<Quaternion>& rotations{ animation.rotations.at(sampler.output) };
				node1.at(channel.target_node).rotation = rotations.at(keyframe_index).Slerp(rotations.at(keyframe_index + 1), interpolation_factor).Normalize();
			}
			else if (channel.target_path == "translation")
			{
				const vector<Vector3>& translations{ animation.translations.at(sampler.output) };
				node1.at(channel.target_node).translation = translations.at(keyframe_index).Lerp(translations.at(keyframe_index + 1), interpolation_factor);
			}
		}
	}

	if (model_data->animations.size() > 0)
	{
		const auto& animation{ model_data->animations.at(index2) };

		for (vector<GLTFModelData::Animation::Channel>::const_reference channel : animation.channels)
		{
			const GLTFModelData::Animation::Sampler& sampler{ animation.samplers.at(channel.sampler) };
			const vector<float>& timeline{ animation.timelines.at(sampler.input) };

			if (timeline.size() == 0)
			{
				continue;
			}

			float interpolation_factor{ };
			// ���݂̃L�[�t���[���̏�Ԃ��擾����
			size_t keyframe_index{ index_of(timeline, time2, interpolation_factor) };
			// �A�j���V�����̑Ώۂ��X�P�[���̏ꍇ
			if (channel.target_path == "scale")
			{
				const vector<Vector3>& scales{ animation.scales.at(sampler.output) };
				node2.at(channel.target_node).scale = scales.at(keyframe_index).Lerp(scales.at(keyframe_index + 1), interpolation_factor);
			}
			else if (channel.target_path == "rotation")
			{
				const vector<Quaternion>& rotations{ animation.rotations.at(sampler.output) };
				node2.at(channel.target_node).rotation = rotations.at(keyframe_index).Slerp(rotations.at(keyframe_index + 1), interpolation_factor).Normalize();
			}
			else if (channel.target_path == "translation")
			{
				const vector<Vector3>& translations{ animation.translations.at(sampler.output) };
				node2.at(channel.target_node).translation = translations.at(keyframe_index).Lerp(translations.at(keyframe_index + 1), interpolation_factor);
			}
		}
	}

	for (size_t i = 0ULL; i < animated_nodes.size(); ++i)
	{
		auto& n{ animated_nodes[i] };

		const auto& n1{ node1[i] };
		const auto& n2{ node2[i] };

		n->rotation    = n1.rotation.Slerp(n2.rotation, interpolation);
		n->scale       = n1.scale.Lerp(n2.scale, interpolation);
		n->translation = n1.translation.Lerp(n2.translation, interpolation);
	}

	// �O���[�o���s����X�V����
	model_data->CumulateTransforms(animated_nodes);
}

float GLTFSkinnedMesh::GetAnimationLength(size_t index) const
{
	return GetModelData()->animations[index].MeasureAnimationTimeEnd();
}

void GLTFSkinnedMesh::Animate(const GLTFModelData::Animation& animation, float& time, bool& has_played)
{
	has_played = false;

	using namespace std;
	// ���f���f�[�^�̎Q�Ƃ̎擾
	auto model_data{ GetModelData() };

	// �L�[�t���[���̏ꏊ�ƕ�Ԓl��Ԃ��֐�
	function<size_t(const vector<float>&, float&, float&)> index_of{
		[](const vector<float>& timelines, float& time, float& interpolation_factor)-> size_t
		{
			const size_t keyframe_count{ timelines.size() };

			float end_time { timelines.at(keyframe_count - 1) };

			// �A�j���[�V�������I�������
			if (time > end_time)
			{
				// �A�j���[�V���������m�[�h�̈ʒu������Ȃ��悤�ɕ�Ԓl���ő�ɂ��A
				// �Ōォ��ЂƂO�̃L�[�t���[����Ԃ�
				interpolation_factor = 1.0f;
				return keyframe_count - 2;
			}
			// �A�j���[�V�����J�n�O
			else if (time < timelines.at(0))
			{
				interpolation_factor = 0.0f;
				return 0;
			}
			// �A�j���V������

			// �L�[�t���[���̏ꏊ��T��
			size_t keyframe_index{ 0 };
			for (size_t time_index = 1; time_index < keyframe_count; ++time_index)
			{
				// ���݂̎��Ԃ��^�C�����C���̎��Ԃ𒴂����n�_�̃L�[�t���[����ݒ肷��
				if (time < timelines.at(time_index))
				{
					keyframe_index = max<size_t>(0LL, time_index - 1);
					break;
				}
			}

			// ��Ԓl���v�Z����
			interpolation_factor = (time - timelines.at(keyframe_index)) /
					(timelines.at(keyframe_index + 1)) - timelines.at(keyframe_index);

			// �L�[�t���[���ԍ���Ԃ�
			return keyframe_index;
		}
	};


	{
		for (vector<GLTFModelData::Animation::Channel>::const_reference channel : animation.channels)
		{
			const GLTFModelData::Animation::Sampler& sampler{ animation.samplers.at(channel.sampler) };
			const vector<float>& timeline{ animation.timelines.at(sampler.input) };

			if (timeline.size() == 0)
			{
				continue;
			}

			float interpolation_factor{ };
			// ���݂̃L�[�t���[���̏�Ԃ��擾����
			size_t keyframe_index{ index_of(timeline, time, interpolation_factor) };
			// �A�j���V�����̑Ώۂ��X�P�[���̏ꍇ
			if (channel.target_path == "scale")
			{
				const vector<Vector3>& scales{ animation.scales.at(sampler.output) };
				animated_nodes.at(channel.target_node)->scale = scales.at(keyframe_index).Lerp(scales.at(keyframe_index + 1), interpolation_factor);
			}
			else if (channel.target_path == "rotation")
			{
				const vector<Quaternion>& rotations{ animation.rotations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->rotation = rotations.at(keyframe_index).Slerp(rotations.at(keyframe_index + 1), interpolation_factor).Normalize();
			}
			else if (channel.target_path == "translation")
			{
				const vector<Vector3>& translations{ animation.translations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->translation = translations.at(keyframe_index).Lerp(translations.at(keyframe_index + 1), interpolation_factor);
			}
		}

		// �O���[�o���s����X�V����
		model_data->CumulateTransforms(animated_nodes);
	}

	// �A�j���[�V�������I��������
	float end{ animation.MeasureAnimationTimeEnd() };
	if (time > end)
	{
		time = end;
		has_played = true;
	}

	return;
}

void GLTFSkinnedMesh::DrawMeshRecursive(ID3D11DeviceContext* dc, int32_t node_index) const
{
	// ���f���f�[�^�̎Q�Ƃ̎擾
	auto model_data{ GetModelData() };
	// �C���f�b�N�X�̃m�[�h�̎Q��
	const std::shared_ptr<GLTFModelData::Node>& node{ model_data->nodes.at(node_index) };

	// �X�L��(�A�j���V����)��񂪑��݂���ꍇ
	if (node->skin > -1)
	{
		const GLTFModelData::Skin& skin{ model_data->skins.at(node->skin) };

		// �{�[���s��萔�o�b�t�@�̐ݒ�
		{
			CBufferJoint cb{ };

			for (size_t joint_index = 0; joint_index < skin.joints.size(); ++joint_index)
			{
				cb.matrices[joint_index] =
					skin.inverse_bind_matrices.at(joint_index) *
					animated_nodes.at(skin.joints.at(joint_index))->global_transform *
					node->global_transform.Inverse();
			}

			joint_cb.UpdateSubresouce(dc, cb);
			Graphics::GetInstance().GetConstantBufferManager()->SetBuffer(dc, CBufferJoint::slot, &joint_cb);
		}
	}

	// ���b�V�������݂���ꍇ
	if (node->mesh > -1)
	{
		const GLTFModelData::Mesh& mesh{ model_data->meshes.at(node->mesh) };

		// �v���~�e�B�u�P�ʂł̕`��
		for (std::vector<GLTFModelData::Mesh::Primitive>::const_reference primitive : mesh.primitives)
		{
			// ���_�o�b�t�@�̐ݒ�
			{
				// ���_�V�F�[�_�[��SoA�Ȃ̂ŁA����ɍ��킹���`�Œ��_�o�b�t�@���쐬����
				ID3D11Buffer* vertex_buffers[]
				{
					primitive.vertex_buffer_views.at("POSITION").buffer.Get(),
					primitive.vertex_buffer_views.at("NORMAL").buffer.Get(),
					primitive.vertex_buffer_views.at("TANGENT").buffer.Get(),
					primitive.vertex_buffer_views.at("TEXCOORD_0").buffer.Get(),
					primitive.vertex_buffer_views.at("JOINTS_0").buffer.Get(),
					primitive.vertex_buffer_views.at("WEIGHTS_0").buffer.Get(),
				};

				UINT strides[]
				{
					static_cast<UINT>(primitive.vertex_buffer_views.at("POSITION").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("NORMAL").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("TANGENT").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("TEXCOORD_0").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("JOINTS_0").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("WEIGHTS_0").stride_in_bytes),
				};

				UINT offsets[_countof(vertex_buffers)]{ 0 };

				dc->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
				dc->IASetIndexBuffer(primitive.index_buffer_view.buffer.Get(),
					primitive.index_buffer_view.format, 0);
			}

			// �v���~�e�B�u�萔�o�b�t�@�̐ݒ�
			{
				CBufferPrimitive cb{ };
				const auto& world_transform{ GetWorldTransform() };

				cb.material = primitive.material;
				cb.has_tangent = primitive.vertex_buffer_views.at("TANGENT").buffer != nullptr;
				cb.skin = node->skin;

				cb.world = node->global_transform * world_transform;

				primitive_cb.UpdateSubresouce(dc, cb);
				Graphics::GetInstance().GetConstantBufferManager()->SetBuffer(dc, CBufferPrimitive::slot, &primitive_cb);
			}

			// �e�N�X�`�����̐ݒ�
			{
				if (primitive.material > -1)
				{
					const GLTFModelData::Material& material{ model_data->materials.at(primitive.material) };

					// �e�N�X�`���C���f�b�N�X�z��
					const int texture_indices[]
					{
						material.data.pbr_metallic_roughness.basecolor_texture.index,         // �J���[
						material.data.pbr_metallic_roughness.metallic_roughness_texture.index,// �e���Ɣ��˗�
						material.data.normal_texture.index,									  // �@��
						material.data.emissive_texture.index,								  // ���g����̔���
						material.data.occlusion_texture.index,								  // �A�e
					};

					ID3D11ShaderResourceView* null_shader_resource_view{ };
					// �e�N�X�`���C���f�b�N�X�z��̑傫���̔z����쐬����
					std::vector<ID3D11ShaderResourceView*> shader_resource_views(_countof(texture_indices));

					for (int32_t texture_index = 0; texture_index < shader_resource_views.size(); ++
						texture_index)
					{
						// �v���~�e�B�u�ɔԍ��̎�ނ̃e�N�X�`�����ݒ肳��Ă���ꍇ�͏���n���A�ݒ肳��Ă��Ȃ��ꍇ�͋�̏���n���B
						shader_resource_views.at(texture_index) = texture_indices[texture_index] > -1 ?
							model_data->texture_resource_views.
							at(model_data->textures.
								at(texture_indices[texture_index]).
								source).Get() :
							null_shader_resource_view;
					}

					// �s�N�Z���V�F�[�_�[�Ƀe�N�X�`������n��
					dc->PSSetShaderResources(1, static_cast<UINT>(shader_resource_views.size()),
						shader_resource_views.data());
				}
			}

			// �`����s
			dc->DrawIndexed(static_cast<UINT>(primitive.index_buffer_view.Count()), 0, 0);
		}
	}

	// �q�m�[�h�����݂���ꍇ
	for (std::vector<int32_t>::value_type child_index : node->children)
	{
		DrawMeshRecursive(dc, child_index);
	}
}

const std::vector<std::shared_ptr<GLTFModelData::Node>>& GLTFSkinnedMesh::GetAnimatedNodes() const
{
	return animated_nodes;
}

Vector3 GLTFSkinnedMesh::GetAnimatedNodeWorldSR(const char* name) const
{
	using namespace DirectX;

	// �����̖��O�̃m�[�h����������
	const auto itr{ std::ranges::find_if(animated_nodes,
										 [&](const auto& node) { return node->name == name; }) };

	// �m�[�h�����������ꍇ���̃��[���h���W��Ԃ�
	if (itr != animated_nodes.end())
	{
		Vector3 out{};
		const auto& world_transform{ GetWorldTransform() };

		Matrix w
		{
			world_transform._11, world_transform._12, world_transform._13, world_transform._14,
			world_transform._21, world_transform._22, world_transform._23, world_transform._24,
			world_transform._31, world_transform._32, world_transform._33, world_transform._34,
			0, 0, 0, 1
		};

		return out.Transform((*itr)->global_transform * w);
	}

	_ASSERT_EXPR(itr != animated_nodes.end(), L"�m�[�h��������܂���ł���");
	return {};
}

std::shared_ptr<GLTFModelData::Node> GLTFSkinnedMesh::FindAnimatedNode(const char* node_name)
{
	auto find_node_iterator{ std::ranges::find_if(animated_nodes, [&](const auto& node) { return node->name == node_name; }) };

	if(find_node_iterator != animated_nodes.end())
	{
		return (*find_node_iterator);
	}

	return nullptr;
}


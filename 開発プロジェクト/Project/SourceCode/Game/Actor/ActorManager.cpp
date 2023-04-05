#include "ActorManager.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "imgui.h"

#include "General/AtaiEngine.h"
#include "Game/Animation/Animator.h"
#include "Actor.h"

/* --- ��` ---- */

void ActorManager::Initialize(ID3D11Device* device)
{
	// �W�I���g���o�b�t�@�ɏ������݂��s���V�F�[�_�[
	gbuffer_shader.CreateVSFromCSO(device, "Shader/GeometryBufferVS.cso");
	gbuffer_shader.CreatePSFromCSO(device, "Shader/GeometryBufferPS.cso");
	gbuffer_shader.CreateRasterizerState(device, D3D11_FILL_SOLID, D3D11_CULL_BACK, true);
}

void ActorManager::EndProcess()
{
	for (auto& s : start_actors)
	{
		s->End();
	}
	start_actors.clear();

	for (auto& u : update_actors)
	{
		u->End();
	}
	update_actors.clear();

	for (auto& r : remove_actors)
	{
		r->End();
	}
	remove_actors.clear();
}

void ActorManager::Register(const std::vector<std::shared_ptr<Actor>>& actors)
{
	// actors�̃f�[�^��start_actors�ɑ}������
	start_actors.insert(start_actors.end(), actors.begin(), actors.end());
}

void ActorManager::Register(const std::shared_ptr<Actor>& actor)
{
	start_actors.emplace_back(actor);
}

void ActorManager::Remove(const std::shared_ptr<Actor>& actor)
{
	remove_actors.emplace(actor);
}

void ActorManager::Remove(const std::vector<std::shared_ptr<Actor>>& actors)
{
	// actors�̃f�[�^��remove_actors��inserter�֐����g����push_back���Ă���
	std::ranges::copy(actors, std::inserter(remove_actors, remove_actors.end()));
}

void ActorManager::UpdateActor(float elapsed_time)
{
	// �������ꂽ�A�N�^�̏���
	UpdateStartActor();
	// �A�N�^�̍X�V
	UpdateMainActor(elapsed_time);
	// �폜����A�N�^�̏���
	RemoveActor();
	// �A�N�^�̐e�q�֌W�̍X�V
	UpdateActorRelation();
}

void ActorManager::UpdateAnimation(float elapsed_time)
{
	for (const auto& actor : update_actors)
	{
		actor->UpdateAnimation(elapsed_time);
	}
}

void ActorManager::UpdateBehavior(float elapsed_time)
{
	for (const auto& actor : update_actors)
	{
		actor->UpdateBehavior(elapsed_time);
	}
}

void ActorManager::UpdateRigidBody(float elapsed_time)
{
	for (const auto& actor : update_actors)
	{
		actor->UpdateRigidBody(elapsed_time);
	}
}

void ActorManager::UpdateCollider(float elapsed_time)
{
	for (const auto& actor : update_actors)
	{
		actor->UpdateCollider(elapsed_time);
	}
}

void ActorManager::UpdateTransform(const Matrix& render_model_coordinate_space)
{
	for (const auto& a : update_actors)
	{
		if (a->IsChildActor()) continue; // �q�A�N�^�̏����͐e�A�N�^���ōs����B

		a->UpdateTransform(Matrix::RhsYup());
	}
}

void ActorManager::ExecuteActorCurrentPlayAnimationEvent()
{
	for (const auto& actor : update_actors)
	{
		actor->ExecuteCurrentPlayAnimationEvent();
	}
}

void ActorManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (const auto& a : update_actors)
	{
		a->Render(dc, shader ? shader : &gbuffer_shader);
	}
}

void ActorManager::ImGuiRender()
{
	if (show_lister)
	{
		// ���X�^�[�`��
		DrawLister();
	}

	if (show_detail)
	{
		// �ڍו`��
		DrawDetail();
	}
}

void ActorManager::ImGuiRenderOnMainMenuBar()
{
	bool dummy{ false };

	if (ImGui::MenuItem("List", NULL, &dummy, true))
	{
		show_detail = !show_detail;
		show_lister = !show_lister;
	}
}

std::shared_ptr<Actor> ActorManager::FindActor(const char* actor_name)
{
	for (const auto& a : update_actors)
	{
		// ���O�̔�r
		if (a->CompareName(actor_name))
		{
			return a;
		}
	}

	return nullptr;
}

std::vector<std::shared_ptr<Actor>> ActorManager::FindActors(const char* group_name)
{
	std::vector<std::shared_ptr<Actor>> found_actors;

	for (const auto& a : update_actors)
	{
		// ���O�̔�r
		if (a->CompareGroupName(group_name))
		{
			found_actors.emplace_back(a);
		}
	}

	return found_actors;
}

std::vector<std::shared_ptr<Actor>> ActorManager::FindActors(const std::vector<const char*>& group_names)
{
	std::vector<std::shared_ptr<Actor>> found_actors;

	for (const auto& a : update_actors)
	{
		for (const auto& n : group_names)
		{
			// ���O�̔�r
			if (a->CompareGroupName(n))
			{
				found_actors.emplace_back(a);
			}
		}
	}

	return found_actors;
}

const std::vector<std::shared_ptr<Actor>>& ActorManager::GetUpdateActors()
{
	return update_actors;
}

size_t ActorManager::CountUpdateActors() const
{
	return update_actors.size();
}

void ActorManager::ClearAllActor()
{
	start_actors.clear();
	update_actors.clear();
}

void ActorManager::UpdateStartActor()
{
	for (auto& a : start_actors)
	{
		a->Start();

		update_actors.emplace_back(a);
	}

	start_actors.clear();
}

void ActorManager::UpdateMainActor(float elapsed_time)
{
	for (auto& a : update_actors)
	{
		// �q�A�N�^�̏ꍇ�͐e�A�N�^�̓����ōX�V�������s���B
		if (a->IsChildActor()) continue;

		a->Update(elapsed_time);
	}
}

void ActorManager::RemoveActor()
{
	// �폜�t���O�̗������A�N�^�����X�g����폜����
	for (const auto& a : remove_actors)
	{
		if (!a) continue;

		// �폜����A�N�^�̏I������
		a->End();

		// �e���X�g������폜����A�N�^�����O����B

		auto itr_start = std::ranges::find(start_actors, a);
		if (itr_start != start_actors.end())
		{
			start_actors.erase(itr_start);
		}

		auto itr_update = std::ranges::find(update_actors, a);
		if (itr_update != update_actors.end())
		{
			update_actors.erase(itr_update);
		}

		auto itr_selection = selection_actors.find(a);
		if (itr_selection != selection_actors.end())
		{
			selection_actors.erase(itr_selection);
		}
	}

	remove_actors.clear();
}

void ActorManager::UpdateActorRelation()
{
	// �A�N�^���m�̎Q�Ƃ̊֌W�����X�V����
	for (auto& a: update_actors)
	{
		a->UpdateActorRelation();
	}
}

void ActorManager::DrawLister()
{
	std::function<void(const std::shared_ptr<Actor>&)> draw_lister_of_child_actor =
		[&](const std::shared_ptr<Actor>& child_actor) -> void
	{
		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

		// �q�m�[�h�����݂��Ă���ꍇ�\�����@��ύX����
		if (child_actor->CountOfChildActors() > 0)
		{
			nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		}

		// �w�肵���L�[�̗v�f���܂܂�Ă��邩�𔻒肷��
		if (selection_actors.contains(child_actor))
		{
			nodeFlags |= ImGuiTreeNodeFlags_Selected;
		}

		if (ImGui::TreeNodeEx(child_actor.get(), nodeFlags, child_actor->GetName().c_str()))
		{
			if (ImGui::IsItemClicked())
			{
				// �P��I�������Ή����Ă���
				//ImGuiIO& io = ImGui::GetIO();
				selection_actors.clear();
				selection_actors.insert(child_actor);
			}

			size_t count_of_child_actors{ child_actor->CountOfChildActors() };
			for (size_t i = 0; i < count_of_child_actors; ++i)
			{
				draw_lister_of_child_actor(child_actor->GetChildActor(i));
			}

			ImGui::TreePop();
		}
	};

	ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	ImGui::Begin("Actor Lister", nullptr, ImGuiWindowFlags_None);
	{
		for (std::shared_ptr<Actor>& actor : update_actors)
		{
			if (!actor->IsChildActor())
			{
				ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

				// �q�m�[�h�����݂��Ă���ꍇ�\�����@��ύX����
				if (actor->CountOfChildActors() > 0)
				{
					nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
				}

				// �w�肵���L�[�̗v�f���܂܂�Ă��邩�𔻒肷��
				if (selection_actors.contains(actor))
				{
					nodeFlags |= ImGuiTreeNodeFlags_Selected;
				}

				if (ImGui::TreeNodeEx(actor.get(), nodeFlags, actor->GetName().c_str()))
				{
					if (ImGui::IsItemClicked())
					{
						// �P��I�������Ή����Ă���
						//ImGuiIO& io = ImGui::GetIO();
						selection_actors.clear();
						selection_actors.insert(actor);
					}

					size_t count_of_child_actors{ actor->CountOfChildActors() };
					for (size_t i = 0; i < count_of_child_actors; ++i)
					{
						draw_lister_of_child_actor(actor->GetChildActor(i));
					}

					ImGui::TreePop();
				}
			}
		}
	}
	ImGui::End();
}

void ActorManager::DrawDetail()
{
	ImGui::SetNextWindowPos(ImVec2(950, 50), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	//show_detail = !ImGui::Begin("Actor Detail", nullptr, ImGuiWindowFlags_None);
	//if (!show_detail)
	ImGui::Begin("Actor Detail", nullptr, ImGuiWindowFlags_None);
	{
		std::shared_ptr<Actor> lastSelected = selection_actors.empty() ? nullptr : *selection_actors.rbegin();
		if (lastSelected != nullptr)
		{
			lastSelected->ImGuiRender();

			if (auto animator = lastSelected->GetAnimator())
			{
				animator->ImGuiRender();
			}

		}
	}
	ImGui::End();
}

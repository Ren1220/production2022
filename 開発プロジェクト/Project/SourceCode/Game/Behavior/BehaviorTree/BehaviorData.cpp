#include "BehaviorData.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "BehaviorNode.h"

/* --- 定義 ---- */

BehaviorData::BehaviorData()
{
    Initialize();
}

BehaviorData::~BehaviorData()
{
}

std::shared_ptr<BehaviorNode> BehaviorData::PopSequenceNode()
{
    // データが存在しない場合
    if (w_sequence_stack.empty())
    {
        return nullptr;
    }

    std::shared_ptr<BehaviorNode> pop_node{ nullptr };

    // データが存在する場合
    if (pop_node = w_sequence_stack.top().lock())
    {
        // 取り出したデータを削除
        w_sequence_stack.pop();
    }

    return pop_node;
}

int32_t BehaviorData::GetSequenceStep(const char* name)
{
    // 引数のキーのステップが見つからなかった場合
    if (run_sequence_step_map.find(name) == run_sequence_step_map.end())
    {
        // ステップ０で新しく配列に登録する
        run_sequence_step_map.emplace(std::make_pair(name, 0));
    }

    // 引数のキーの値を返す
    return run_sequence_step_map.at(name);
}

void BehaviorData::SetSequenceStep(const char* name, int32_t step)
{
    run_sequence_step_map[name] = step;
}

void BehaviorData::Initialize()
{
    run_sequence_step_map.clear();

    w_sequence_stack = std::stack<std::weak_ptr<BehaviorNode>>();
    _ASSERT_EXPR(w_sequence_stack.empty(), L"コンテナが正常に初期化されていません");
}

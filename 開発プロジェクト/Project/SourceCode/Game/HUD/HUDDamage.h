#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <unordered_map>
#include <memory>
#include <string>

/* --- 前方宣言 ---- */

class Actor;
class Camera;

/* --- 宣言 ---- */

class HUDDamage
{
public:

	HUDDamage();
	~HUDDamage();

	void Initialize();
	void AddDatas(const std::shared_ptr<Actor>& actor, float display_time, int32_t damage_val);
	void Update(float elapsed_time);
	void Render(const Camera* camera, float screen_width, float screen_height);

private:

	class Data
	{
	public:

		Data(const std::shared_ptr<Actor>& actor, float display_time)
			: w_actor(actor), timer(display_time), display_value(0), damage_value(0) {}

		std::weak_ptr<Actor> w_actor;
		double display_value;
		int32_t damage_value;
		float timer;
	};

	std::unordered_map<std::string, Data> datas;

};

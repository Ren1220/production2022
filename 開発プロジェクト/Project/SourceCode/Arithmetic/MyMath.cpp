#include "MyMath.h"

/* --- 定義 ---- */

float Math::ToRadian(float degree)
{
    return degree * std::numbers::pi_v<float> / 180.0f;
}

float Math::ToDegree(float radian)
{
    return radian * 180.0f / std::numbers::pi_v<float>;
}

Vector2 Math::ScreenToClip(const Vector2& position, float screen_width, float screen_height)
{
    Vector2 clip_pos{};
    clip_pos.x = 2.0f * position.x / screen_width - 1.0f;
    clip_pos.y = 1.0f - 2.0f * position.y / screen_height;
    return clip_pos;
}

Vector2 Math::WorldToScreen(
    const Vector3& position, const Matrix& view, const Matrix& projection, float screen_width, float screen_height)
{
    Vector3 scr_pos{ position.Transform(view * projection) };
    // スクリーン座標変換
    scr_pos.x = (scr_pos.x + 1.0f) * screen_width * 0.5f;
    scr_pos.y = (-scr_pos.y + 1.0f) * screen_height * 0.5f;

    return { scr_pos.x, scr_pos.y };
}

std::pair<Vector3, Vector3> Math::ScreenToWorld(
    const Vector2& position, 
    const Matrix& view,
    const Matrix& projection,
    float screen_width, 
    float screen_height)
{
    // -1.0f ~ 1.0f
    Vector2 clip_pos{ ScreenToClip(position, screen_width, screen_height) };

    Vector3 near{ clip_pos.x, clip_pos.y, 0.0f }; 
    Vector3 far { clip_pos.x, clip_pos.y, 1.0f };  

    // 順番を間違えない
    Matrix i{ projection.Inverse() * view.Inverse() };

    near = near.TransformCoordinate(i);
    far = far.TransformCoordinate(i);

    return { near, far };
}

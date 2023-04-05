#include "Intersect2D.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <cmath>

/* --- 定義 ---- */

bool IntersectUser2D::IntersectAABBVSCircle(
    float box_mx, float box_Mx, 
    float box_my, float box_My, 
    float sphere_pos_x, float sphere_pos_y, 
    float sphere_radius)
{
    // 領域A（x方向に半径分拡張したAABB内に球の座標があるか）
    {
        float mx{ box_mx - sphere_radius }, Mx{ box_Mx + sphere_radius };

        if (sphere_pos_x >= mx && sphere_pos_x <= Mx &&
            sphere_pos_y >= box_my && sphere_pos_y <= box_My)
        {
            return true;
        }
    }

    // 領域B（y方向に半径分拡張したAABB内に球の座標があるか）
    {
        float my{ box_my - sphere_radius }, My{ box_My + sphere_radius };

        if (sphere_pos_x >= box_mx && sphere_pos_x <= box_Mx &&
            sphere_pos_y >= my && sphere_pos_y <= My)
        {
            return true;
        }
    }

    // 角（矩形の各頂点と球の座標の距離と球の半径の比較（平方））
    if     ((box_mx - sphere_pos_x) * (box_mx - sphere_pos_x) + (box_my - sphere_pos_y) * (box_my - sphere_pos_y)
        < sphere_radius * sphere_radius) return true;
    else if ((box_Mx - sphere_pos_x) * (box_Mx - sphere_pos_x) + (box_my - sphere_pos_y) * (box_my - sphere_pos_y)
        < sphere_radius * sphere_radius) return true;
    else if ((box_mx - sphere_pos_x) * (box_mx - sphere_pos_x) + (box_My - sphere_pos_y) * (box_My - sphere_pos_y)
        < sphere_radius * sphere_radius) return true;
    else if ((box_Mx - sphere_pos_x) * (box_Mx - sphere_pos_x) + (box_My - sphere_pos_y) * (box_My - sphere_pos_y)
        < sphere_radius * sphere_radius) return true;

    return false;
}

bool IntersectUser2D::IntersectRayVSAABB(
    float ray_pos_x, float ray_pos_y,
    float ray_dir_x, float ray_dir_y,
    float ray_length,
    float box_pos_x, float box_pos_y,
    float box_width, float box_height,
    float& out_near_len,
    float& out_norm_x, float& out_norm_y)
{
    out_near_len = -1.0f;

    // レイが交差する長さを求める
    if (ray_length < 0) return false;

    float t0x{ (box_pos_x - ray_pos_x) / ray_dir_x };
    float t1x{ (box_width + box_pos_x - ray_pos_x) / ray_dir_x };

    if (t0x > t1x)
    {
        std::swap(t0x, t1x);
    }

    if (ray_length < t0x || ray_length > t1x)
    {
        return false;
    }

    float t0y{ (box_pos_y - ray_pos_y) / ray_dir_y };
    float t1y{ (box_height  + box_pos_y - ray_pos_y) / ray_dir_y };

    if (t0y > t1y)
    {
        std::swap(t0y, t1y);
    }

    if (ray_length < t0y || ray_length > t1y)
    {
        return false;
    }

    if (t0x > t0y)
    {
        out_near_len = t0x;
        out_norm_x = (ray_dir_x > 0.0f) ? -1.0f : 1.0f;
        out_norm_y = 0.0f;
    }
    else
    {
        out_near_len = t0y;
        out_norm_x = 0.0f;
        out_norm_y = (ray_dir_y > 0.0f) ? -1.0f : 1.0f;
    }

    if (out_near_len < ray_length)
    {
        return true;
    }

    return false;
}

bool IntersectUser2D::IntersectRayVSAABB(
    float ray_pos_x, float ray_pos_y,
    float ray_dir_x, float ray_dir_y,
    float ray_length,
    float box_pos_x, float box_pos_y,
    float box_width, float box_height, float& out_near_len)
{
    out_near_len = -1.0f;

    // レイが交差する長さを求める
    if (ray_length < 0) return false;

    float t0x{ (-box_width + box_pos_x - ray_pos_x) / ray_dir_x };
    float t1x{ ( box_width + box_pos_x - ray_pos_x) / ray_dir_x };

    if (t0x > t1x) { std::swap(t0x, t1x); }

    if (ray_length < t0x || ray_length > t1x)
    {
        return false;
    }

    float t0y{ (-box_height + box_pos_y - ray_pos_y) / ray_dir_y };
    float t1y{ ( box_height + box_pos_y - ray_pos_y) / ray_dir_y };

    if (t0y > t1y) { std::swap(t0y, t1y); }

    if (ray_length < t0y || ray_length > t1y)
    {
        return false;
    }

    out_near_len = (std::max)(t0x, t0y);
    if (out_near_len < ray_length)
    {
        return true;
    }

    return false;
}

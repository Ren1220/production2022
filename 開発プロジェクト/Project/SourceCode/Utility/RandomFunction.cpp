#include "RandomFunction.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

/* --- ��` ---- */

std::random_device Random::seed_generator;
std::mt19937 Random::engine(Random::seed_generator());

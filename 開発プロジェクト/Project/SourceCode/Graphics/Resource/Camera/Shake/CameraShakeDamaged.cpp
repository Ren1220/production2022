#include "CameraShakeDamaged.h"

/* --- ’è‹` ---- */

void CameraShakeDamaged::CameraShake(
	const Camera* camera, 
	Vector3& out_eye,
	Vector3& out_focus)
{
	out_focus += camera->GetRight() * sinf(timer * frequency) * amplitude;
}

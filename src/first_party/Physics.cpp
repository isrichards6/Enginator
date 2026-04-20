//#include "Physics.h"
//#include <SDL_rect.h>
//
//bool Physics::CollisionCheck(const SDL_FRect& collider_a, const SDL_FRect& collider_b) {
//	bool left_gap = collider_a.x >= collider_b.x + collider_b.w;
//	bool right_gap = collider_a.x + collider_a.w <= collider_b.x;
//	bool top_gap = collider_a.y >= collider_b.y + collider_b.h;
//	bool bottom_gap = collider_a.y + collider_a.h <= collider_b.y;
//
//	return !(left_gap || right_gap || top_gap || bottom_gap);
//}
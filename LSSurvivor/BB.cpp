#include "BB.h"


BB::BB(void)
{
}


BB::~BB(void)
{
}
//implementar
bool BB::Intersect(BB box2){
	BB box1 = *this;

	if(box1.max.x<box2.min.x || box1.min.x > box2.max.x || box1.max.y<box2.min.y || box1.min.y > box2.max.y){
	return false;
	}
	return true;
}
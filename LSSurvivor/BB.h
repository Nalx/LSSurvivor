#pragma once

#ifndef __BB_H__
#define __BB_H__
class BB
{
public:
	BB(void);
	~BB(void);

	struct Vector3{
		float x;
		float y;
		float z;
	};

	Vector3 max;
	Vector3 min;

	bool Intersect(BB box2);

};
#endif


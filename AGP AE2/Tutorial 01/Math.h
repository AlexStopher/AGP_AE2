#pragma once

#include <math.h>

struct xyz
{
	float x, y, z;

	xyz operator+(const xyz& b)
	{
		xyz temp;

		temp.x = this->x + b.x;
		temp.y = this->y + b.y;
		temp.z = this->z + b.z;

		return temp;
	}

	xyz operator-(const xyz& b)
	{
		xyz temp;

		temp.x = this->x - b.x;
		temp.y = this->y - b.y;
		temp.z = this->z - b.z;

		return temp;
	}

	xyz()
	{

	}
	xyz(float x1, float y1, float z1)
	{
		x = x1;
		y = y1;
		z = z1;
	}
};

struct Plane 
{ 
	xyz normal;
	float d; 
};

class Math
{

public:

	Math();
	~Math();

	static float DotProduct(xyz* u, xyz* v); //returns the dot product of two vectors
	static xyz CrossProduct(xyz* u, xyz* v);
	static xyz Normal(xyz* a, xyz* b, xyz* c);
	static int Sign(float number);
	static bool PointInTriangle(xyz* triangle1, xyz* triangle2, xyz* triangle3, xyz* point);

	static Plane PlaneCreation(xyz* a, xyz* b, xyz* c);
	static float PlaneEquation(Plane* plane, xyz* position);
	static xyz PlaneIntersection(Plane* plane, xyz* point1, xyz* point2);
};


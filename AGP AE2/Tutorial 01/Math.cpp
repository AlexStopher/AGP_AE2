#include "Math.h"



Math::Math()
{
	srand((unsigned)time(NULL));
}


Math::~Math()
{
}



float Math::DotProduct(xyz* u, xyz* v)
{
	float temp = (u->x * v->x) + (u->y * v->y) + (u->z * v->z);
	return temp;
}

xyz Math::CrossProduct(xyz* u, xyz* v)
{
	xyz CrossProd;

	CrossProd.x = (u->y * v->z) - (u->z * v->y);
	CrossProd.y = (u->z * v->x) - (u->x * v->z);
	CrossProd.z = (u->x * v->y) - (u->y * v->x);

	return CrossProd;
}

//Returns a 1 or a -1
int Math::Sign(float number) 
{
	return (number < 0.0f ? -1 : (number > 0.0f ? 1 : 0));
}

xyz Math::Normal(xyz* a, xyz* b, xyz* c)
{
	xyz Temp1, Temp2;

	Temp1.x = b->x - a->x;
	Temp1.y = b->y - a->y;
	Temp1.z = b->z - a->z;

	Temp2.x = c->x - a->x;
	Temp2.y = c->y - a->y;
	Temp2.z = c->z - a->z;

	xyz tempCross = CrossProduct(&Temp1, &Temp2);

	float length = sqrt((tempCross.x * tempCross.x) + (tempCross.y * tempCross.y) + (tempCross.z * tempCross.z));

	xyz normalVector;

	normalVector.x = tempCross.x / length;
	normalVector.y = tempCross.y / length;
	normalVector.z = tempCross.z / length;

	return normalVector;
}

Plane Math::PlaneCreation(xyz* a, xyz* b, xyz* c)
{
	Plane plane;

	//Fetches the normal of the plane
	xyz NormalVector = Normal(a, b, c);

	plane.normal = NormalVector;

	//Calculates the distance offset of the plane from the origin
	plane.d = -DotProduct(&NormalVector, a);

	return plane;
}

float Math::PlaneEquation(Plane* plane, xyz* position)
{
	return ((plane->normal.x * position->x) + (plane->normal.y * position->y) + (plane->normal.z * position->z) + plane->d);
}

xyz Math::PlaneIntersection(Plane* plane, xyz* point1, xyz* point2)
{
	//Creates a ray from two points
	xyz Ray, RayPoint;
	float t;

	Ray.x = point2->x - point1->x;
	Ray.y = point2->y - point1->y;
	Ray.z = point2->z - point1->z;

	t = (-plane->d - DotProduct(&plane->normal, point1)) / DotProduct(&plane->normal, &Ray);


	if (t < 0.0f || t > 1.0f)
	{
		RayPoint.x = 999999999;
		RayPoint.y = 999999999;
		RayPoint.z = 999999999;
	}
	else
	{
		RayPoint.x = point1->x + (Ray.x * t);
		RayPoint.y = point1->y + (Ray.y * t);
		RayPoint.z = point1->z + (Ray.z * t);
	}

	return RayPoint;
}

bool Math::PointInTriangle(xyz* triangle1, xyz* triangle2, xyz* triangle3, xyz* point)
{


	xyz ap, ab, bp, bc, cp, ca;

	ap = (*point - *triangle1);
	ab = (*triangle2 - *triangle1);
	bp = (*point - *triangle2);
	bc = (*triangle3 - *triangle2);
	cp = (*point - *triangle3);
	ca = (*triangle1 - *triangle3);

	float AB, BC, CA;

	AB = DotProduct((&CrossProduct(&ap, &ab)),(&CrossProduct(&bp, &bc)));

	BC = DotProduct((&CrossProduct(&bp, &bc)), (&CrossProduct(&cp, &ca)));

	CA = DotProduct((&CrossProduct(&cp, &ca)), (&CrossProduct(&ap, &ab)));
	


	if (AB >= 0 &&
		BC >= 0 &&
		CA >= 0)
	{
		return true;
	}
	if (AB <= 0 &&
		BC <= 0 &&
		CA <= 0)
	{
		return true;
	}


	
		
	return false;
	
}

float Math::GetRandomNumber(int randomMax, int randomMin)
{
	return (double)rand() / (RAND_MAX) * (randomMax - randomMin) + randomMin;
}
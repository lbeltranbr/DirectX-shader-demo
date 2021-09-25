#include "pch.h"
#include "Interactable_ball.h"

Interactable_Ball::Interactable_Ball()
{
	center = DirectX::SimpleMath::Vector3(0, 0, 0);
	rad = 1;
	render = false;
}

Interactable_Ball::~Interactable_Ball()
{
}

DirectX::SimpleMath::Vector3 Interactable_Ball::GetCenter()
{
	return center;
}

float Interactable_Ball::GetRad()
{
	return rad;
}

void Interactable_Ball::SetCenter(DirectX::SimpleMath::Vector3 c)
{
	center = c;
}

void Interactable_Ball::SetRad(float r)
{
	rad = r;
}

bool Interactable_Ball::SphereCollision(DirectX::SimpleMath::Vector3 o, DirectX::SimpleMath::Vector3 d)
{
	float r_2 = rad * rad;
	//vector from the ray origin to the center of the sphere
	DirectX::SimpleMath::Vector3 l = center - o;
	//projection of l onto the ray direction
	float s = l.Dot(d);
	float s_2 = s * s;

	float l_2 = l.Dot(l);

	//if inside sphere
	if (s<0 && l_2>r_2)
		return false; //there's no collision

	float m_2 = l_2 - s_2;

	if (m_2 > r_2)
		return false; //there's no collision

	return true;

	/*float a, b, c, discriminant;


	// Calculate the a, b, and c coefficients.
	a = (d.x * d.x) + (d.y * d.y) + (d.z * d.z);
	b = ((d.x * o.x) + (d.y * o.y) + (d.z * o.z)) * 2.0f;
	c = ((o.x * o.x) + (o.y * o.y) + (o.z * o.z)) - (rad * rad);

	// Find the discriminant.
	discriminant = (b * b) - (4 * a * c);

	// if discriminant is negative the picking ray missed the sphere, otherwise it intersected the sphere.
	if (discriminant < 0.0f)
	{
		return false;
	}

	return true;*/
	
}

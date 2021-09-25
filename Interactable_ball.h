#pragma once
#include "modelclass.h"

class Interactable_Ball
{
public:

	Interactable_Ball();
	~Interactable_Ball();

	ModelClass ball;
	bool render;

	DirectX::SimpleMath::Vector3 GetCenter();
	float GetRad();

	void SetCenter(DirectX::SimpleMath::Vector3 c);
	void SetRad(float r);

	bool SphereCollision(DirectX::SimpleMath::Vector3 o, DirectX::SimpleMath::Vector3 d);



private:

	DirectX::SimpleMath::Vector3 center;
	float rad;


};


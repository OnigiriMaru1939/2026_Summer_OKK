#pragma once


#include "BulletBase.h"

class Bullet : public BulletBase
{
public:
	Bullet(FileManager& fileMng, Stage& stage, float x, float y, float vx, float vy, float scale);
	~Bullet() override;
	void Update() override;

};


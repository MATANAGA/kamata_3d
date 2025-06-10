#pragma once
#include "KamataEngine.h"

class Player {
public:
	~Player();
	void Initialize();
	void Update();
	void Draw(KamataEngine::Camera& camera);

private:
	KamataEngine::Model* player_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
};

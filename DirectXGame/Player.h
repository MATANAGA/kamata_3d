#pragma once
#include "KamataEngine.h"

class Player {
public:
	~Player();
	void Initialize();
	void Update();
	void Draw(KamataEngine::Camera& camera);

private:
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	uint32_t textureHandle_ = 0;
};

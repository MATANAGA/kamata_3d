#pragma once
#include "KamataEngine.h"

class Player {
public:
	~Player();
	void Initialize();
	void Update();
	void Draw(KamataEngine::Camera& camera);
	void Initialize(Model* model, Camera* camera, const Vector3& position);

private:
	KamataEngine::Model* player_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
};

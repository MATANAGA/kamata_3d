#pragma once
#include "KamataEngine.h"

class Player {

public:
	

	~Player();
	KamataEngine::Vector3 velocity_{};
	static inline const float kAcceleration = 0.03f;
	static inline const float kAttenuation = 0.3f;

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

private:
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Camera* camera_;
};

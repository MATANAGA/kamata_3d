#pragma once
#include "KamataEngine.h"

class Skydome {
public:
	void Initialize();
	void Update(const KamataEngine::Vector3& cameraPosition);
	void Draw(KamataEngine::Camera& camera); 

private:
	KamataEngine::Model* skydome_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
};

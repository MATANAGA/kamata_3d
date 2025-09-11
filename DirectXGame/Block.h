#pragma once
#include "KamataEngine.h"

class BlockModel {
public:
	~BlockModel();
	void Initialize();
	void Update();
	void Draw(KamataEngine::Camera& camera);
	void Draw(const KamataEngine::WorldTransform& worldTransform, KamataEngine::Camera& camera);

private:
	KamataEngine::Model* block_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
};

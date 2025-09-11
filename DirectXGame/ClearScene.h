#pragma once
#include "Fade.h"
#include "KamataEngine.h"

class ClearScene {
public:
	ClearScene() = default;
	~ClearScene();

	void Initialize();
	void Update();
	void Draw();
	bool IsFinished() const { return finished_; }

private:
	bool finished_ = false;
	float timer_ = 0.0f;
	static constexpr float kWaitTime = 2.0f;

	KamataEngine::Camera camera_;
	KamataEngine::Model* clearTextModel_ = nullptr;
	KamataEngine::WorldTransform clearTextTransform_;
	Fade* fade_ = nullptr;

	// BGM
	uint32_t bgmHandle_ = 0;
	bool bgmPlaying_ = false;
};

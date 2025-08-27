#pragma once
#include "Fade.h"
#include "KamataEngine.h"

class DeathScene {
public:
	DeathScene() = default;
	~DeathScene();

	void Initialize();
	void Update();
	void Draw();
	bool IsFinished() const { return finished_; }

private:
	bool finished_ = false;
	float timer_ = 0.0f;
	static constexpr float kWaitTime = 2.0f;

	KamataEngine::Camera camera_;
	KamataEngine::Model* deathTextModel_ = nullptr;
	KamataEngine::WorldTransform deathTextTransform_;
	Fade* fade_ = nullptr;

	// BGM
	uint32_t bgmHandle_ = 0;
	bool bgmPlaying_ = false;
};

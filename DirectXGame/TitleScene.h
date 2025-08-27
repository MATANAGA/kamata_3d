#pragma once
#include "Fade.h"
#include "KamataEngine.h"
#include <cassert>
#include <iostream>

class TitleScene {
public:
	enum class Phase {
		kFadeIn,
		kMain,
		kFadeOut,
	};

	Phase phase_ = Phase::kFadeIn;
	bool finished_ = false;

	~TitleScene();
	void Initialize();
	void Update();
	void Draw();
	bool IsFinished() const { return finished_; }

private:
	int frameCount_ = 0;
	Fade* fade_ = nullptr;
	static constexpr float kFadeTime = 1.0f; // フェード時間（秒）
	KamataEngine::Camera camera_;
	KamataEngine::Model* modelTitle_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::WorldTransform playerTransform_;
	KamataEngine::ObjectColor objectColor_;
	uint32_t bgmHandle_ = 0;
	bool bgmPlaying_ = false;
};

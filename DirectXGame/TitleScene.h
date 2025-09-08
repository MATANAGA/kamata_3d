#pragma once
#include "Fade.h"
#include "KamataEngine.h"

using namespace KamataEngine;

class TitleScene {
public:
	TitleScene() = default;
	~TitleScene();

	void Initialize();
	void Update();
	void Draw();
	void StopBGM();

	bool IsFinished() const { return finished_; }

private:
	enum class Phase { kFadeIn, kMain, kFadeOut };
	Phase phase_ = Phase::kFadeIn;

	// Fade
	Fade* fade_ = nullptr;
	static constexpr float kFadeTime = 0.5f;

	// Camera
	Camera camera_;

	// BGM
	uint32_t bgmHandle_ = 0;
	bool bgmPlaying_ = false;

	// 标题模型
	Model* modelTitle_ = nullptr;
	Model* title_ = nullptr;
	WorldTransform titleTransform_;
	WorldTransform worldTransform_;
	ObjectColor objectColor_;

	// 玩家模型
	Model* modelPlayer_ = nullptr;
	WorldTransform playerTransform_;

	// 四个角落模型
	static constexpr int kCornerCount = 4;
	Model* cornerModels_[kCornerCount] = {nullptr};
	WorldTransform cornerTransforms_[kCornerCount];

	// 说明页Sprite
	uint32_t instructionTextureHandle_ = 0;
	Sprite* instructionSprite_ = nullptr;
	bool showInstruction_ = false;

	// 帧计数
	int frameCount_ = 0;

	// 是否结束
	bool finished_ = false;
};

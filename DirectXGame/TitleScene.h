#pragma once
#include "KamataEngine.h"
#include <cassert>
#include <iostream>

class TitleScene {
public:
	~TitleScene();
	void Initialize();
	void Update();
	void Draw();
	bool IsFinished() const { return finished_; }

private:
	bool finished_ = false;
	int frameCount_ = 0;

	KamataEngine::Camera camera_;
	KamataEngine::Model* modelTitle_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::WorldTransform playerTransform_; // 玩家变换

	KamataEngine::ObjectColor objectColor_;
};

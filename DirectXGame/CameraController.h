#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Player;

class CameraController {

public:
	struct Rect {
		float left = 0.0f;
		float right = 0.0f;
		float bottom = 0.0f;
		float top = 0.0f;
	};

	void Initialize();
	void Update();
	void Reset();

	void SetTarget(Player* target) { target_ = target; }
	void SetMovableArea(Rect area) { movableArea_ = area; }

	const KamataEngine::Camera& GetViewProjection() const { return camera_; }

private:
	Camera camera_;
	Player* target_ = nullptr;
	Vector3 targetOffset_ = {0.0f, 0.0f, -15.0f};
	Rect movableArea_ = {0, 100, 0, 100};

	Vector3 targetPosition_; // ← 追従目標座標
	static inline const float kInterpolationRate = 0.1f;
	static inline const float kVelocityBias = 30.0f;

	// ←★ 追従対象が画面内に収まるようにするためのマージン
	static inline const Rect targetMargin_ = {-9.0f, 9.0f, -5.0f, 5.0f};
};

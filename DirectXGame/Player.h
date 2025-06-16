#pragma once
#include "KamataEngine.h"
#include <algorithm>

class Player {

	enum class LRDirection {
		kRight,
		kLeft
	};
	LRDirection lrdirection_ = LRDirection::kRight;
	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;

	static inline const float kTimeTurn = 0.3f;


	
	// ジャンプ
	bool onGround_ = true;
	//
	static inline const float kGravityAccleration = 0.1f; // 重力加速度（每帧加的速度）
	static inline const float kLimitFallSpeed = 1.0f;     // 最大落下速度（终端速度）
	static inline const float kJumpAcceleration = 1.0f;  


public:
	~Player();
	KamataEngine::Vector3 velocity_{};
	static inline const float kAcceleration = 0.02f;
	static inline const float kAttenuation = 0.3f;

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

private:
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Camera* camera_;
};

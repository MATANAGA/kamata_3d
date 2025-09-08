#pragma once
#include "KamataEngine.h"

class DeathParticles {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();
	bool IsFinished() const { return isFinished_; }

private:
	void UpdateMatrix();

	static inline const float kInitialSpeed = 15.0f;    // 初始向上速度
	static inline const float kGravity = 19.8f;         // 重力加速度
	static inline const float kRotationSpeed = 360.0f; // 翻滚角速度（度/秒）
	static inline const float kZSpeed = -5.0f;         // Z轴靠近画面速度
	static inline const float kScaleSpeed = 1.5f;      // 放大速度，每秒放大倍数

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Vector3 velocity_;
	KamataEngine::Vector3 rotationVelocity_;

	bool isFinished_ = false;
};

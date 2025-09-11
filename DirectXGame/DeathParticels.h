#pragma once
#include "KamataEngine.h"
#include <array>
#include <numbers>

class DeathParticles {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();
	bool IsFinished() const { return isFinished_; }

private:
	void UpdateMatrix();

	static inline const uint32_t kNumParticles = 8;
	static inline const float kDuration = 1.0f;                                                                  // 存続時間（秒）
	static inline const float kSpeed = 0.1f;                                                                     // 移動速度
	static inline const float kAngleUnit = std::numbers::pi_v<float> * 2.0f / static_cast<float>(kNumParticles); // 1つ分の角度

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	std::array<KamataEngine::WorldTransform, kNumParticles> worldTransforms_;
	std::array<KamataEngine::Vector3, kNumParticles> velocities_; // 各パーティクルの速度

	KamataEngine::ObjectColor objectColor_; // 色変更用
	KamataEngine::Vector4 color_{};         // 現在の色（RGBA）

	bool isFinished_ = false;
	float counter_ = 0.0f;
};

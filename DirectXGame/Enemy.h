#pragma once
#include "KamataEngine.h"
#include <numbers>

class Enemy {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

private:
	// モデル・カメラ
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	// ワールド変換
	KamataEngine::WorldTransform worldTransform_;

	// 移動用
	KamataEngine::Vector3 velocity_ = {};

	// 歩行速度
	static inline const float kWalkSpeed = 0.05f;

	// 歩行アニメーション（首振り）パラメータ
	float walkTimer_ = 0.0f;
	static inline const float kWalkMotionAngleStart = -30.0f; // [度]
	static inline const float kWalkMotionAngleEnd = 30.0f;    // [度]
	static inline const float kWalkMotionTime = 1.0f;         // 周期[秒]

	void UpdateMatrix(); // 行列更新
};

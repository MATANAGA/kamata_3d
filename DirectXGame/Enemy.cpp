#include "Enemy.h"
#include "MyMath.h" // MakeAffineMatrix用
#include <cassert>
#include <cmath> // std::sin
#include <numbers>

using namespace KamataEngine;
using namespace MathUtility;

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	assert(camera);

	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = -std::numbers::pi_v<float> / 2; // 左向き

	velocity_ = {-kWalkSpeed, 0.0f, 0.0f}; // 左に移動

	walkTimer_ = 0.0f;

	UpdateMatrix();
}

void Enemy::Update() {
	// 移動
	worldTransform_.translation_ += velocity_;

	// ▼ 向きの切り替え：X方向の速度に応じてY軸回転を設定
	if (velocity_.x > 0.0f) {
		worldTransform_.rotation_.y = 0.0f; // 右向き
	} else if (velocity_.x < 0.0f) {
		worldTransform_.rotation_.x = std::numbers::pi_v<float>; // 左向き
	}

	// ▼ タイマ一加算
	walkTimer_ += 1.0f / 60.0f;

	float param = std::sin(2.0f * std::numbers::pi_v<float> * walkTimer_ / kWalkMotionTime);
	float degree = kWalkMotionAngleStart + (kWalkMotionAngleEnd - kWalkMotionAngleStart) * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.x = degree * (std::numbers::pi_v<float> / 180.0f); // 度→ラジアン

	UpdateMatrix();
}

void Enemy::Draw() {
	if (model_ && camera_) {
		model_->Draw(worldTransform_, *camera_);
	}
}

void Enemy::UpdateMatrix() {
	worldTransform_.matWorld_ = MakeAffineMatrrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

#include "Enemy.h"
#include "MyMath.h" // MakeAffineMatrix用
#include <cassert>
#include <cmath> // std::sin
#include <numbers>

using namespace KamataEngine;
using namespace MathUtility;

// 巡逻范围X轴限制（例）


void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	assert(camera);

	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = -std::numbers::pi_v<float>/2; // 左向き

	velocity_ = {-kWalkSpeed, 0.0f, 0.0f}; // 左に移動

	walkTimer_ = 0.0f;

	UpdateMatrix();
}

void Enemy::Update() {
	// 移動
	worldTransform_.translation_ += velocity_;

	if (velocity_.x > 0.0f) {
		worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2; // 右向き（+90度）
	} else if (velocity_.x < 0.0f) {
		worldTransform_.rotation_.y = -std::numbers::pi_v<float> / 2; // 左向き（-90度）
	}

	// 範囲判定：超えたら速度反転
	if (worldTransform_.translation_.x < kPatrolMinX) {
		worldTransform_.translation_.x = kPatrolMinX; // 抑える
		velocity_.x = kWalkSpeed;                     // 右へ移動
	} else if (worldTransform_.translation_.x > kPatrolMaxX) {
		worldTransform_.translation_.x = kPatrolMaxX;
		velocity_.x = -kWalkSpeed; // 左へ移動
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
bool Enemy::CheckCollisionWithPlayer(const Player& player) const {
	const auto& enemyPos = worldTransform_.translation_;
	const auto& playerPos = player.GetWorldTransform().translation_;

	float enemyHalfW = kWidth / 2.0f;
	float enemyHalfH = kHeight / 2.0f;
	float playerHalfW = Player::kCollisionWidth / 2.0f;
	float playerHalfH = Player::kCollisionHeight / 2.0f;

	bool overlapX = std::abs(enemyPos.x - playerPos.x) < (enemyHalfW + playerHalfW);
	bool overlapY = std::abs(enemyPos.y - playerPos.y) < (enemyHalfH + playerHalfH);
	bool overlapZ = std::abs(enemyPos.z - playerPos.z) < (enemyHalfW + playerHalfW); // Z轴近似用宽度

	return overlapX && overlapY && overlapZ;
}
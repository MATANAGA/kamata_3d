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
	// 移动
	worldTransform_.translation_ += velocity_;

	// 巡逻边界判定
	if (worldTransform_.translation_.x < kPatrolMinX) {
		worldTransform_.translation_.x = kPatrolMinX;
		velocity_.x = kWalkSpeed;
	} else if (worldTransform_.translation_.x > kPatrolMaxX) {
		worldTransform_.translation_.x = kPatrolMaxX;
		velocity_.x = -kWalkSpeed;
	}

	// 朝向（左右翻转用）
	float facingY = (velocity_.x > 0.0f) ? std::numbers::pi_v<float> / 2 : -std::numbers::pi_v<float> / 2;

	// --- 自转角度累加 ---
	const float kRotateSpeedY = 2.0f * std::numbers::pi_v<float> / 120.0f; // Y 轴自转速度
	const float kRotateSpeedX = 2.0f * std::numbers::pi_v<float> / 150.0f; // X 轴自转速度

	selfRotateY_ += kRotateSpeedY;
	selfRotateX_ += kRotateSpeedX;

	// 应用旋转
	worldTransform_.rotation_.y = facingY + selfRotateY_;
	worldTransform_.rotation_.x = selfRotateX_;

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
#include "EnemyB.h"
#include "MapChipField.h"
#include "MyMath.h"
#include <cassert>
#include <cmath>
#include <numbers>

using namespace KamataEngine;
using namespace MathUtility;

void EnemyB::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model && camera);

	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	velocity_ = {0.0f, 0.0f, 0.0f};
	targetVelocity_ = {0.0f, 0.0f, 0.0f};
	walkTimer_ = 0.0f;
	selfRotateY_ = 0.0f;

	UpdateMatrix();
}

void EnemyB::Update() {
	// 漂浮计时器
	walkTimer_ += 1.0f / 60.0f;

	// 每隔约2秒随机一次目标方向（只在XY平面）
	if (static_cast<int>(walkTimer_ * 60) % 120 == 0) {
		float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * std::numbers::pi_v<float>;
		float vy = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f; // 上下漂浮

		targetVelocity_.x = std::cos(angle) * kWalkSpeed;
		targetVelocity_.y = vy;
		targetVelocity_.z = 0.0f; // 禁止Z轴漂移
	}

	// --- 软约束：地图边界检测 ---
	float mapWidth = MapChipField::kNumBlockHorizontal * MapChipField::kBlockWidth;
	float mapHeight = MapChipField::kNumBlockVirtical * MapChipField::kBlockHeight;

	const float margin = 1.0f; // 边界安全距离

	if (worldTransform_.translation_.x < margin) {
		targetVelocity_.x = std::abs(targetVelocity_.x); // 往右
	} else if (worldTransform_.translation_.x > mapWidth - margin) {
		targetVelocity_.x = -std::abs(targetVelocity_.x); // 往左
	}

	if (worldTransform_.translation_.y < margin) {
		targetVelocity_.y = std::abs(targetVelocity_.y); // 往上
	} else if (worldTransform_.translation_.y > mapHeight - margin) {
		targetVelocity_.y = -std::abs(targetVelocity_.y); // 往下
	}

	// --- 平滑插值到目标速度 ---
	float t = 0.02f; // 插值系数
	velocity_.x += (targetVelocity_.x - velocity_.x) * t;
	velocity_.y += (targetVelocity_.y - velocity_.y) * t;

	// 更新位置（Z保持不动）
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;

	// 敌人保持轻微自转
	const float kRotateSpeedY = 2.0f * std::numbers::pi_v<float> / 240.0f;
	selfRotateY_ += kRotateSpeedY;
	worldTransform_.rotation_.y = selfRotateY_;

	UpdateMatrix();
}

void EnemyB::Draw() {
	if (model_ && camera_) {
		model_->Draw(worldTransform_, *camera_);
	}
}

void EnemyB::UpdateMatrix() {
	worldTransform_.matWorld_ = MakeAffineMatrrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

bool EnemyB::CheckCollisionWithPlayer(const Player& player) const {
	const auto& enemyPos = worldTransform_.translation_;
	const auto& playerPos = player.GetWorldTransform().translation_;

	float enemyHalfW = kWidth / 2.0f;
	float enemyHalfH = kHeight / 2.0f;
	float playerHalfW = Player::kCollisionWidth / 2.0f;
	float playerHalfH = Player::kCollisionHeight / 2.0f;

	bool overlapX = std::abs(enemyPos.x - playerPos.x) < (enemyHalfW + playerHalfW);
	bool overlapY = std::abs(enemyPos.y - playerPos.y) < (enemyHalfH + playerHalfH);
	bool overlapZ = std::abs(enemyPos.z - playerPos.z) < (enemyHalfW + playerHalfW);

	return overlapX && overlapY && overlapZ;
}

#include "EnemyB.h"
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
	worldTransform_.rotation_.y = -std::numbers::pi_v<float> / 2;
	velocity_ = {-kWalkSpeed, 0.0f, 0.0f};
	walkTimer_ = 0.0f;
	UpdateMatrix();
}

void EnemyB::Update() {
	// 移动计时器
	walkTimer_ += 1.0f / 60.0f;

	// 随机巡逻逻辑：当到达巡逻边界或随机触发时改变方向
	if (worldTransform_.translation_.x <= kPatrolMinX) {
		velocity_.x = kWalkSpeed; // 往右
	} else if (worldTransform_.translation_.x >= kPatrolMaxX) {
		velocity_.x = -kWalkSpeed; // 往左
	} else {
		// 小概率随机改变方向
		if ((rand() % 200) == 0) { // 约每 200 帧随机一次
			velocity_.x *= -1.0f;
		}
	}

	// 更新位置
	worldTransform_.translation_.x += velocity_.x;

	// 朝向
	worldTransform_.rotation_.y = (velocity_.x > 0.0f) ? std::numbers::pi_v<float> / 2 : -std::numbers::pi_v<float> / 2;

	// 自转动画
	const float kRotateSpeedY = 2.0f * std::numbers::pi_v<float> / 120.0f;
	selfRotateY_ += kRotateSpeedY;
	worldTransform_.rotation_.y += selfRotateY_;

	UpdateMatrix();
}

void EnemyB::Draw() {
	if (model_ && camera_)
		model_->Draw(worldTransform_, *camera_);
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

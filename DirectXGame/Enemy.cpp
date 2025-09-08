#include "Enemy.h"
#include "MapChipField.h"
#include "MyMath.h"
#include <cassert>
#include <cmath>
#include <numbers>

using namespace KamataEngine;
using namespace MathUtility;

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
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

void Enemy::Update() {
	// 漂浮计时器
	walkTimer_ += 1.0f / 60.0f;

	// --- 敌人状态机 ---
	switch (state_) {
	case State::Normal: {
		// 每隔约2秒随机一次目标方向（只在XY平面）
		if (static_cast<int>(walkTimer_ * 60) % 120 == 0) {
			float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * std::numbers::pi_v<float>;
			float vy = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f; // 上下漂浮

			targetVelocity_.x = std::cos(angle) * kWalkSpeed;
			targetVelocity_.y = vy;
			targetVelocity_.z = 0.0f; // 禁止Z轴漂移
		}

		// 随机进入冲刺预备（大约每5～8秒一次）
		if (rand() % (60 * 6) == 0) { // ~6秒概率一次
			state_ = State::PreDash;
			stateTimer_ = 0.0f;
		}
		break;
	}
	case State::PreDash: {
		// 限制Y分量，不要太大
		targetVelocity_.y *= 0.5f; // Y轴影响减半

		// 上下剧烈抖动
		float shake = std::sin(stateTimer_ * 60.0f * 0.5f) * 0.2f;
		worldTransform_.translation_.y += shake;

		// 1秒后进入冲刺
		if (stateTimer_ > 2.0f) {
			state_ = State::Dashing;
			stateTimer_ = 0.0f;

			 // 随机冲刺方向（XY平面）
			float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * std::numbers::pi_v<float>;

			targetVelocity_.x = std::cos(angle) * kDashSpeed;
			targetVelocity_.y = std::sin(angle) * kDashSpeed;
		}
		break;
	}
	case State::Dashing: {
		// 冲刺 0.5秒
		if (stateTimer_ > 0.05f) {
			state_ = State::Normal;
			stateTimer_ = 0.0f;
		}
		break;
	}
	}

	// --- 边界检测（固定25*10） ---
	const float mapWidth = 25.0f;
	const float mapHeight = 10.0f;
	const float margin = 1.0f;

	if (worldTransform_.translation_.x <= margin || worldTransform_.translation_.x >= mapWidth - margin) {
		velocity_.x = -velocity_.x; // 直接反转速度
	}
	if (worldTransform_.translation_.y <= margin || worldTransform_.translation_.y >= mapHeight - margin) {
		velocity_.y = -velocity_.y;
	}

	// --- 平滑速度 ---
	float t = (state_ == State::Dashing) ? 0.2f : 0.02f; // 冲刺时插值更快
	velocity_.x += (targetVelocity_.x - velocity_.x) * t;
	velocity_.y += (targetVelocity_.y - velocity_.y) * t;

	// 更新位置
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;

	// 敌人保持轻微自转
	const float kRotateSpeedY = 2.0f * std::numbers::pi_v<float> / 240.0f;
	selfRotateY_ += kRotateSpeedY;
	worldTransform_.rotation_.y = selfRotateY_;

	// 更新时间
	stateTimer_ += 1.0f / 60.0f;

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
	bool overlapZ = std::abs(enemyPos.z - playerPos.z) < (enemyHalfW + playerHalfW);

	return overlapX && overlapY && overlapZ;
}

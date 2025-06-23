#define NOMINMAX
#include "Player.h"
#include "MyMath.h"
#include <algorithm>
#include <numbers>

using namespace KamataEngine;
using namespace MathUtility;

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Update() {
	// ①移動入力
	InputMove();

	// ②移動量を加味して衝突判定する
	CheckMapCollision();

	// ③判定結果を反映して移動させる
	CheckMapMove();

	// --- 移動 ---
	worldTransform_.translation_ += velocity_;

	// ④天井に接触している場合の処理
	CheckMapCeiling();

	// ⑤壁に接触している場合の処理
	CheckMapWall();

	// ⑥接地状態の切り替え
	CheckMapLanding();

	// ⑦旋回制御
	AnimateTurn();

	// ⑧行列計算
	UpdateMatrix();
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }

void Player::InputMove() {
	if (onGround_) {
		// キー入力で移動量を設定する処理
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				if (lrdirection_ != LRDirection::kRight) {
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
					lrdirection_ = LRDirection::kRight;
				}
				acceleration.x += kAcceleration;
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				if (lrdirection_ != LRDirection::kLeft) {
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
					lrdirection_ = LRDirection::kLeft;
				}
				acceleration.x -= kAcceleration;
			}
			velocity_ += acceleration;
		} else {
			velocity_.x *= (1.0f - kAttenuation);
		}

		// ジャンプ
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_ += Vector3(0, kJumpAcceleration, 0);

		}
	} else {
		// 空中：落下処理
		velocity_ += Vector3(0, -kGravityAccleration, 0);
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
	worldTransform_.translation_ += velocity_;

	bool landing = false;

	if (velocity_.y < 0) {
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}
	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}
	} else {
		if (landing) {
			worldTransform_.translation_.y = 1.0f;
			velocity_.x *= (1.0f - kAttenuation);
			velocity_.y = 0.0f;
			onGround_ = true;
		}
	}
}

void Player::AnimateTurn() {
	if (turnTimer_ > 0.0f) {
		// タイマーを1/60秒分カウントダウン
		turnTimer_ -= 1.0f / 60.0f;

		float destinationRotationYTable[] = {
		    std::numbers::pi_v<float> / 2.0f,       // 左（+90°）
		    std::numbers::pi_v<float> * 3.0f / 2.0f // 右（+270°）
		};

		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrdirection_)];
		worldTransform_.rotation_.y = ElseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}
}

void Player::UpdateMatrix() {
	worldTransform_.matWorld_ = MakeAffineMatrrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

// --- 以下の関数はまだ中身未実装なので、仮置き ---

void Player::CheckMapCollision() {
	// TODO: マップチップとの衝突判定を実装する
}

void Player::CheckMapMove() {
	// TODO: 衝突結果に応じた移動反映処理を実装する
}

void Player::CheckMapCeiling() {
	// TODO: 天井との接触を確認して補正
}

void Player::CheckMapWall() {
	// TODO: 左右の壁に当たっている場合の処理
}

void Player::CheckMapLanding() {
	// TODO: 地面との接触を判定して onGround_ を切り替える
}

Player::~Player() {}



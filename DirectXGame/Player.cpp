#include "Player.h"
#include "MyMath.h"
#include "numbers"
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
	worldTransform_.matWorld_ = MakeAffineMatrrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
	worldTransform_.translation_ += velocity_;
	if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
		Vector3 acceleration = {};
		if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
			acceleration.x += kAcceleration;
		} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
			acceleration.x -= kAcceleration;
		}
		velocity_ += acceleration;
	} else {
		velocity_.x *= (1.0f - kAttenuation);
	}
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }

Player::~Player() {
	// 不 delete model_，因为是外部传入的共享资源
}

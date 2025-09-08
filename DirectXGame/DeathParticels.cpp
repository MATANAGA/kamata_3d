#include "DeathParticels.h"
#include "MyMath.h"
#include <cassert>

using namespace KamataEngine;
using namespace MathUtility;

void DeathParticles::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	assert(camera);

	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f}; // 初始缩放
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};

	velocity_ = {0.0f, kInitialSpeed, kZSpeed};
	rotationVelocity_ = {0.0f, 0.0f, 0.0f}; // 上升阶段不旋转

	isFinished_ = false;
}

void DeathParticles::Update() {
	if (isFinished_)
		return;

	float deltaTime = 1.0f / 60.0f;

	// 重力
	velocity_.y -= kGravity * deltaTime;

	// 下降阶段开始翻滚
	if (velocity_.y < 0.0f && rotationVelocity_.x == 0.0f) {
		rotationVelocity_.x = kRotationSpeed;

	}

	// 更新位置
	worldTransform_.translation_ += velocity_ * deltaTime;

	// 更新旋转
	worldTransform_.rotation_ += rotationVelocity_ * deltaTime;

	// 靠近摄像机时放大 scale
	worldTransform_.scale_ += Vector3{kScaleSpeed * deltaTime, kScaleSpeed * deltaTime, kScaleSpeed * deltaTime};

	// 到地面停止
	if (worldTransform_.translation_.y <= 0.0f) {
		worldTransform_.translation_.y = 0.0f;
		velocity_ = {0.0f, 0.0f, 0.0f};
		rotationVelocity_ = {0.0f, 0.0f, 0.0f};
		isFinished_ = true;
	}

	UpdateMatrix();
}

void DeathParticles::Draw() {
	if (!model_)
		return;

	model_->Draw(worldTransform_, *camera_);
}

void DeathParticles::UpdateMatrix() {
	worldTransform_.matWorld_ = MakeAffineMatrrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

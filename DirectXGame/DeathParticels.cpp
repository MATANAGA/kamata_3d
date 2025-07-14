#include "DeathParticels.h"
#include "MyMath.h"
#include <algorithm>
#include <cassert>
using namespace KamataEngine;
using namespace MathUtility;


void DeathParticles::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	assert(camera);

	model_ = model;
	camera_ = camera;
	// 色初期化（白、不透明）
	objectColor_.Initialize();
	color_ = {1.0f, 1.0f, 1.0f, 1.0f};
	objectColor_.SetColor(color_);

	// パーティクル8個分、ワールドトランスフォームと速度を初期化
	for (uint32_t i = 0; i < kNumParticles; ++i) {
		worldTransforms_[i].Initialize();
		worldTransforms_[i].translation_ = position;
		worldTransforms_[i].scale_ = {1.0f, 1.0f, 1.0f};

		Vector3 baseVelocity = {kSpeed, 0.0f, 0.0f};
		float angle = kAngleUnit * static_cast<float>(i);
		Matrix4x4 rot = MakeRotateZMatrix(angle);
		velocities_[i] = Transform(baseVelocity, rot);
	}

	isFinished_ = false;
	counter_ = 0.0f;
}

void DeathParticles::Update() {
	if (isFinished_)
		return;

	counter_ += 1.0f / 60.0f;

	// フェードアウト処理（0秒→1.0、kDuration秒→0.0）
	color_.w = std::clamp(1.0f - (counter_ / kDuration), 0.0f, 1.0f);
	objectColor_.SetColor(color_);

	for (uint32_t i = 0; i < kNumParticles; ++i) {
		worldTransforms_[i].translation_ += velocities_[i];
	}

	if (counter_ >= kDuration) {
		counter_ = kDuration;
		isFinished_ = true;
	}
	UpdateMatrix();
}

void DeathParticles::Draw() {
	if (isFinished_)
		return;

	for (auto& worldTransform : worldTransforms_) {
		model_->Draw(worldTransform, *camera_, &objectColor_);
	}
}

void DeathParticles::UpdateMatrix() {
	for (auto& worldTransform : worldTransforms_) {
		worldTransform.matWorld_ = MakeAffineMatrrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
		worldTransform.TransferMatrix();
	}
}

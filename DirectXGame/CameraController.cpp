#define NOMINMAX
#include "CameraController.h"
#include "MyMath.h" // Vector3 演算・Lerpなど
#include "Player.h"
#include <algorithm> // clamp に必要

using namespace KamataEngine;
using namespace MathUtility;

void CameraController::Initialize() { camera_.Initialize(); }

void CameraController::Reset() {
	if (target_ == nullptr)
		return;

	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	targetPosition_ = targetWorldTransform.translation_ + targetOffset_;
	camera_.translation_ = targetPosition_;

	camera_.TransferMatrix();
}

void CameraController::Update() {
    if (target_ == nullptr)
        return;

    const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
    const Vector3& targetVelocity = target_->GetVelocity();

    // 追従対象の目標座標計算（オフセット + 速度影響）
    targetPosition_ = targetWorldTransform.translation_ + targetOffset_ + targetVelocity * kVelocityBias;

    // 補間
    camera_.translation_.x = Lerp(camera_.translation_.x, targetPosition_.x, kInterpolationRate);
    camera_.translation_.y = Lerp(camera_.translation_.y, targetPosition_.y, kInterpolationRate);
    //camera_.translation_.z = Lerp(camera_.translation_.z, targetPosition_.z, kInterpolationRate);

    // 追従対象を画面内に収める補正
    camera_.translation_.x = std::max(camera_.translation_.x, targetWorldTransform.translation_.x + targetMargin_.left);
    camera_.translation_.x = std::min(camera_.translation_.x, targetWorldTransform.translation_.x + targetMargin_.right);

    camera_.translation_.y = std::max(camera_.translation_.y, targetWorldTransform.translation_.y + targetMargin_.bottom);
    camera_.translation_.y = std::min(camera_.translation_.y, targetWorldTransform.translation_.y + targetMargin_.top);

    // 移動範囲制限
    camera_.translation_.x = std::clamp(camera_.translation_.x, movableArea_.left, 28.0f);
    camera_.translation_.y = std::clamp(camera_.translation_.y, movableArea_.bottom, 12.0f);

    // 行列更新
    camera_.UpdateMatrix();
}


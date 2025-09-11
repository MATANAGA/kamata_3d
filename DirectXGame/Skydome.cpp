#include "Skydome.h"
#include "MyMath.h"
using namespace KamataEngine;

void Skydome::Initialize() {
	skydome_ = KamataEngine::Model::CreateFromOBJ("SkyDome");
	worldTransform_.Initialize();
}	


void Skydome::Update(const Vector3& cameraPosition) {
	worldTransform_.translation_ = cameraPosition;
	worldTransform_.matWorld_ = MakeAffineMatrrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Skydome::Draw(KamataEngine::Camera& camera_) {
	skydome_->Draw(worldTransform_, camera_);
}

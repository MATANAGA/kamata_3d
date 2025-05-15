#include "Block.h"

using namespace KamataEngine;

void BlockModel::Initialize() {
	textureHandle_ = TextureManager::Load("cube/cube.jpg");
	model_ = Model::Create();
	worldTransform_.Initialize();
}

void BlockModel::Update() {
	// 玩家更新逻辑（如果后续需要）
}

void BlockModel::Draw(Camera& camera) { model_->Draw(worldTransform_, camera, textureHandle_); }

void BlockModel::Draw(const WorldTransform& worldTransform, Camera& camera) { model_->Draw(worldTransform, camera, textureHandle_); }

BlockModel::~BlockModel() { delete model_; }

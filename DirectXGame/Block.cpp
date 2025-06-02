#include "Block.h"

using namespace KamataEngine;

void BlockModel::Initialize() {
	block_ = Model::CreateFromOBJ("block", true);

	worldTransform_.Initialize();
}

void BlockModel::Update() {
	// 玩家更新逻辑（如果后续需要）
}

void BlockModel::Draw(Camera& camera) { block_->Draw(worldTransform_, camera); }

void BlockModel::Draw(const WorldTransform& worldTransform, Camera& camera) { block_->Draw(worldTransform, camera); }

BlockModel::~BlockModel() { delete block_; }

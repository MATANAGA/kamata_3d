#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"
#include "MyMath.h"
#include <algorithm>
#include <numbers>
#include <cassert>

using namespace KamataEngine;
using namespace MathUtility;

void Player::Reset(const KamataEngine::Vector3& pos) {
	// 把玩家位置放到复活点
	worldTransform_.translation_ = pos;
	// 清空速度
	velocity_ = {0.0f, 0.0f, 0.0f};
	// 确保玩家是活着的
	SetAlive(true);
}
void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Update() {
	// ① 移動入力（velocity_ のみ更新、座標は加えない）
	InputMove();

	// ② 移動量から当たり判定（info に補正移動量が返ってくる）
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;
	CheckMapCollision(collisionMapInfo);

	// ③ 判定結果を反映して実際に移動
	worldTransform_.translation_ += collisionMapInfo.move;

	// velocity_ を修正後の移動量に合わせて更新
	velocity_ = collisionMapInfo.move;

	// ④ 天井との接触処理
	CheckMapCeiling(collisionMapInfo);

	// ⑤ 壁との接触処理（横方向）
	CheckMapWall();

	// ✅⑥ 接地状態の切り替え（地面に立っているかどうか）
	onGround_ = collisionMapInfo.landing;

	// ⑦ 向きアニメーション（左右回転）
	AnimateTurn();

	// ⑧ 行列更新（描画に必要）
	UpdateMatrix();
}


void Player::Draw() { model_->Draw(worldTransform_, *camera_); }

void Player::InputMove() {
	Vector3 acceleration = {};

	// 左右移动：无论空中还是地面都接受输入，区别在于加速度大小
	float currentAcceleration = onGround_ ? kAcceleration : kAcceleration * 2.0f; // 空中加速减半例子

	if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
		if (lrdirection_ != LRDirection::kRight) {
			turnFirstRotationY_ = worldTransform_.rotation_.y;
			turnTimer_ = kTimeTurn;
			lrdirection_ = LRDirection::kRight;
		}
		acceleration.x += currentAcceleration;
	}
	if (Input::GetInstance()->PushKey(DIK_LEFT)) {
		if (lrdirection_ != LRDirection::kLeft) {
			turnFirstRotationY_ = worldTransform_.rotation_.y;
			turnTimer_ = kTimeTurn;
			lrdirection_ = LRDirection::kLeft;
		}
		acceleration.x -= currentAcceleration;
	}

	velocity_ += acceleration;

	// 地面时没有输入则衰减速度，空中不衰减（或者你想要的话可以加点空气阻力）
	if (onGround_ && !Input::GetInstance()->PushKey(DIK_RIGHT) && !Input::GetInstance()->PushKey(DIK_LEFT)) {
		velocity_.x *= (1.0f - kAttenuation);
	}

	// 限制最大速度
	constexpr float kMaxSpeed = 0.2f; // 根据你需要调整
	velocity_.x = std::clamp(velocity_.x, -kMaxSpeed, kMaxSpeed);

	// 跳跃输入（只有地面时可以跳）

	if (onGround_ && Input::GetInstance()->PushKey(DIK_UP)) {
		velocity_ += Vector3(0, kJumpAcceleration, 0);
		onGround_ = false; // 跳跃中切换为非接地状态
	}

	// 空中重力处理
	if (!onGround_) {
		velocity_ += Vector3(0, -kGravityAccleration, 0);
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}



void Player::UpdateMatrix() {
	worldTransform_.matWorld_ = MakeAffineMatrrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}
// --- 以下の関数はまだ中身未実装なので、仮置き ---

void Player::CheckMapMove(CollisionMapInfo& info) {
	worldTransform_.translation_ += info.move;

	// TODO: 衝突結果に応じた移動反映処理を実装する
}

void Player::CheckMapCeiling(CollisionMapInfo& info) {
	// TODO: 天井との接触を確認して補正
	if (info.ceiling) {

		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0;
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

void Player::CheckMapLanding() {
	bool landing = false;

	// 当前是在下降
	if (velocity_.y < 0) {
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}

	if (onGround_) {
		// 之前是接地，现在上升了 → 切换为空中
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}
	} else {
		// 之前是空中，若刚才撞到了地面 → 落地处理
		if (landing) {
			worldTransform_.translation_.y = 1.0f;
			velocity_.x *= (1.0f - kAttenuation); // 横方向速度衰减
			velocity_.y = 0.0f;                   // 停止下落
			onGround_ = true;
		}
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Player::Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+Player::kWidth / 2.0f, -Player::kHeight / 2.0f, 0},
	    {-Player::kWidth / 2.0f, -Player::kHeight / 2.0f, 0},
	    {+Player::kWidth / 2.0f, +Player::kHeight / 2.0f, 0},
	    {-Player::kWidth / 2.0f, +Player::kHeight / 2.0f, 0}
    };
	return center + offsetTable[static_cast<uint32_t>(corner)];
}
void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}
void Player::CheckMapWall() {
	// 右方向に進んでいるとき
	if (velocity_.x > 0) {
		for (int i : {kRightTop, kRightBottom}) {
			Vector3 pos = CornerPosition(worldTransform_.translation_, static_cast<Corner>(i));
			auto indexSet = mapChipField_->GetMapChipIndexSetByPosition(pos);
			if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
				auto rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
				worldTransform_.translation_.x = rect.left - kWidth / 2;
				velocity_.x = 0;
				break;
			}
		}
	}
	// 左方向に進んでいるとき
	else if (velocity_.x < 0) {
		for (int i : {kLeftTop, kLeftBottom}) {
			Vector3 pos = CornerPosition(worldTransform_.translation_, static_cast<Corner>(i));
			auto indexSet = mapChipField_->GetMapChipIndexSetByPosition(pos);
			if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
				auto rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
				worldTransform_.translation_.x = rect.right + kWidth / 2;
				velocity_.x = 0;
				break;
			}
		}
	}
}

void Player::CheckMapCollisionUp(CollisionMapInfo& info) {
	// 上昇あり？
	if (info.move.y <= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;

	// 真上の当たり判定を行う
	bool hit = false;

	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, +kHeight / 2.0f, 0));
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));

		// 天井に当たったことを記録する
		info.ceiling = true;
	}
}
void Player::CheckMapCollisionDown(CollisionMapInfo& info) {
	// ① 移动方向不是向下，就跳过判定
	if (info.move.y >= 0) {
		return;
	}

	// ② 移动后的角点坐标
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	// ③ 左下 & 右下角是否打到方块
	bool hit = false;
	MapChipField::IndexSet indexSet;
	MapChipType mapChipType;

	// 左下角
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右下角
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ④ 如果撞到了，就限制移动量，防止“めり込み”
	if (hit) {
		// 再次获取“移动后的下边中心点”位置对应方块
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, -kHeight / 2.0f, 0));
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		// 限制最大可移动距离 = 从角色中心到底部 + 微小空隙
		info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + kHeight / 2.0f + kBlank);

		// 标记“着地”状态，用于下一步切换
		info.landing = true;
	}
}

void Player::CheckMapCollisionRight(CollisionMapInfo& info) {
	// 右方向移動なし？
	if (info.move.x <= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;

	// 右方向の当たり判定を行う
	bool hit = false;

	// 右上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0, 0));
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.x = std::min(info.move.x, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));

		// 壁に当たったことを記録する
		info.hitWall = true;
	}
}
void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {
	// 左方向移動なし？
	if (info.move.x >= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;

	// 左方向の当たり判定を行う
	bool hit = false;

	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0, 0));
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.x = std::max(info.move.x, rect.right - worldTransform_.translation_.x + kWidth / 2.0f + kBlank);

		// 壁に当たったことを記録する
		info.hitWall = true;
	}
}

Player::~Player() {}

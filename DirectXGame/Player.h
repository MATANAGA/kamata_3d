#pragma once

#include "KamataEngine.h"
#include <algorithm>
// 角の位置を示す列挙型

class MapChipField;

class Player {
	static inline const float kBlank = 0.01f;

	enum LRDirection { kRight, kLeft };
	LRDirection lrdirection_ = LRDirection::kRight;
	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;

	static inline const float kTimeTurn = 0.3f;
	// 当たり判定サイズ（幅と高さ）

	// ジャンプ
	bool onGround_ = true;
	bool canJump_ = true;

	//
	static inline const float kGravityAccleration = 0.1f; // 重力加速度（每帧加的速度）
	static inline const float kLimitFallSpeed = 0.3f;     // 最大落下速度（终端速度）
	static inline const float kJumpAcceleration = 0.9f;

public:
	~Player();
	void SetAlive(bool alive) { alive_ = alive; }
	bool IsAlive() const { return alive_; }

	KamataEngine::Vector3 velocity_{};
	static inline const float kAcceleration = 0.01f;
	static inline const float kAttenuation = 0.1f;
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

	static inline const float kCollisionWidth = 0.8f;
	static inline const float kCollisionHeight = 0.8f;
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; } // ← Setter 実装
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();
	enum Corner { kRightBottom, kLeftBottom, kRightTop, kLeftTop, kNumCorner };
	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

private:
	bool alive_ = true; // 初始存活

	static inline const float kAttenuationLanding = 0.3f; // 30% 衰减
	                                                      // 壁衝突時の減速係数（X方向）
	static inline const float kAttenuationWall = 0.3f;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	// マップ衝突情報構造体
	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool hitWall = false;
		KamataEngine::Vector3 move;
	};
	// 各ステップに対応する関数
	void InputMove();                               // ①移動入力
	void CheckMapCollision(CollisionMapInfo& info); // ②移動量から仮の衝突チェック
	void CheckMapMove(CollisionMapInfo& info);      // ③移動処理（実際の座標更新）
	void CheckMapCeiling(CollisionMapInfo& info);   // ④天井当たり判定
	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);

	void ChangeOnGroundState(const CollisionMapInfo& info);

	void CheckMapWall();    // ⑤横方向の壁判定
	void CheckMapLanding(); // ⑥接地判定
	void AnimateTurn();     // ⑦旋回
	void UpdateMatrix();    // ⑧行列更新

	KamataEngine::Model* model_ = nullptr;
	MapChipField* mapChipField_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Camera* camera_;
};

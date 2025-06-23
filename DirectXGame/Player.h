#pragma once

#include "KamataEngine.h"
#include <algorithm>
// 角の位置を示す列挙型

class MapChipField;

class Player {

	// マップ衝突情報構造体
	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool hitWall = false;
		Vector3 move;
	};
	enum Corner { kRightBottom, kLeftBottom, kRightTop, kLeftTop, kNumCorner };
	enum LRDirection { kRight, kLeft };
	LRDirection lrdirection_ = LRDirection::kRight;
	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;

	static inline const float kTimeTurn = 0.3f;
	// 当たり判定サイズ（幅と高さ）
	
	// ジャンプ
	bool onGround_ = true;
	//
	static inline const float kGravityAccleration = 0.1f; // 重力加速度（每帧加的速度）
	static inline const float kLimitFallSpeed = 1.0f;     // 最大落下速度（终端速度）
	static inline const float kJumpAcceleration = 1.0f;

	// 当たり判定の処理
	void CheckMapCollision(CollisionMapInfo& info);
	void CheckMapCollisionUp(CollisionMapInfo& info);

public:
	~Player();
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	KamataEngine::Vector3 velocity_{};
	static inline const float kAcceleration = 0.005f;
	static inline const float kAttenuation = 0.3f;
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }
	
	Vector3 CornerPosition(const Vector3& center, Corner corner);

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; } // ← Setter 実装
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

private:
	// 各ステップに対応する関数
	void InputMove();         // ①移動入力
	void CheckMapCollision(); // ②移動量から仮の衝突チェック
	void CheckMapMove();      // ③移動処理（実際の座標更新）
	void CheckMapCeiling();   // ④天井当たり判定
	void CheckMapWall();      // ⑤横方向の壁判定
	void CheckMapLanding();   // ⑥接地判定
	void AnimateTurn();       // ⑦旋回
	void UpdateMatrix();      // ⑧行列更新

	KamataEngine::Model* model_ = nullptr;
	MapChipField* mapChipField_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Camera* camera_;
};

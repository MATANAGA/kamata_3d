#pragma once
#include <KamataEngine.h>
#include <cstdint>
#include <string> // std::string を使うので必要
#include <vector>

// マップチップの種類
enum class MapChipType {
	KBlank,
	KBlock,
};

// マップデータ本体（2次元配列）
struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {
private:
	// 定数
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	static inline const uint32_t kNumBlockVirtical = 100;
	static inline const uint32_t kNumBlockHorizontal = 100;
	float GetBlockWidth() const { return kBlockWidth; }
	float GetBlockHeight() const { return kBlockHeight; }
	MapChipData mapChipData_;

public:
	// マップの初期化（空データ生成）
	void ResetMapChipData();

	// CSV 読み込み
	void LoadMapChipCsv(const std::string& filePath);

	// マップ情報取得系
	uint32_t GetNumBlockVirticcal() const { return kNumBlockVirtical; }
	uint32_t GetNumBlockHorizontal() const { return kNumBlockHorizontal; }
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
};

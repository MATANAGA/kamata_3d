#include "MapChipField.h"
#include "map"
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>

using namespace KamataEngine;
namespace {

std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::KBlank},
    {"1", MapChipType::KBlock},
};
}


void MapChipField::ResetMapChipData() {

	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);
	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockHorizontal);
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filePath) {

	ResetMapChipData();

	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	std::stringstream mapChipCsv;
	mapChipCsv << file.rdbuf();
	file.close();

	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		std::string line;
		getline(mapChipCsv, line);

		std::istringstream line_stream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			std::string word;
			getline(line_stream, word, ',');
			if (mapChipTable.contains(word)) {
				mapChipData_.data[i][j] = mapChipTable[word];
			}
		}
	}
}

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (xIndex < 0 || kNumBlockHorizontal - 1 < xIndex) {
		return MapChipType::KBlank;
	}
	if (yIndex < 0 || kNumBlockVirtical - 1 < yIndex) {
		return MapChipType::KBlank;
	}

	return mapChipData_.data[yIndex][xIndex];
}

KamataEngine::Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) {
    // 左上角为原点，地图向右下展开
    return KamataEngine::Vector3(
        kBlockWidth * xIndex-36,
        kBlockHeight * yIndex-20,
        0.0f
    );
}


#pragma once
#include <cstdint>
namespace Gconstants
{
	static constexpr int32_t  widthMask = 127;
	static constexpr int32_t  heightMask = 63;
	static constexpr uint8_t  chunkShift = 5;
	static constexpr uint8_t  chunkMask = (1 << chunkShift) - 1;
	static constexpr uint16_t chunkTileNum = 1024;
	static constexpr uint8_t  tileItemLimit = 255;
	static constexpr uint8_t  maxSignalStrength = 16;
	static constexpr uint8_t tileSize = 32;
	static constexpr uint8_t halfTileSize = 16;
	static constexpr uint8_t tileShift = 5;
	static constexpr uint8_t tileMask = 31;
	static constexpr uint8_t memoryLimit = 16;
	static constexpr uint16_t robotMoveFrequency = 7;
	static constexpr uint8_t cameraSpeed = 32;
	static constexpr uint8_t halfItemSprite = 8;
	static constexpr uint8_t tooltipPadding = 6;
	static constexpr uint8_t hotbarSlotSize = 50;
	static constexpr uint8_t hotbarPadding = 4;
}
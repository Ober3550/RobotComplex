#pragma once
#include <cstdint>
namespace GC
{
	static constexpr int32_t  FRAMERATE = 60;
	static constexpr int32_t  UPDATERATE = 20;
	static constexpr uint8_t cameraSpeed = 10;
	static constexpr float MIN_ZOOM = 0.5;
	static constexpr float MAX_ZOOM = 5.0;
	static constexpr float ZOOM_VELOCITY = 0.5;

	// World Constants
	static constexpr uint8_t  chunkShift = 5;
	static constexpr uint8_t  chunkMask = 31;
	static constexpr uint8_t  chunkSize = 32;
	static constexpr uint16_t chunkTileNum = 1024;

	static constexpr uint8_t reservedGround = 63;
	static constexpr uint8_t collideGround = 31;

	static constexpr uint8_t tileSize = 32;
	static constexpr uint8_t halfTileSize = 16;
	static constexpr uint8_t tileShift = 5;
	static constexpr uint8_t tileMask = 31;
	
	static constexpr uint8_t halfItemSprite = 8;

	// GUI constants
	static constexpr float tooltipPadding = 6;
	static constexpr float hotbarSlotSize = 50;
	static constexpr float hotbarPadding = 4;
	static constexpr float hotbarTotalSize = hotbarSlotSize + hotbarPadding;

	// Game constants
	static constexpr uint8_t robotStrength = 6;
	static constexpr uint8_t maxSignalStrength = 32;
	static constexpr uint8_t startSignalStrength = 16;
	static constexpr uint8_t colorClassA = 1;
	static constexpr uint8_t colorClassB = 4;
	static constexpr uint8_t MAXLOGIC = 255;
}
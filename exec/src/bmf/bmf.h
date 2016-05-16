#pragma once

#include <stdarg.h>
#include <pthread.h>
#include <semaphore.h>

#include "boost_cmdline_parse_bp.h"
#include "ansi_colors.h"

#include <string>
#include <vector>
#include <thread>

struct GlyphEntry {

	uint16_t mGlyph;
	uint16_t mBLx;
	uint16_t mBLy;
	uint16_t mTRx;
	uint16_t mTRy;

} __attribute__((packed));

struct FontCacheHeader {

	uint32_t magic;
	uint16_t mMajorVersion;
	uint16_t mMinorVersion;
	uint32_t mID;
	uint64_t mFlags;
	uint16_t mNumberOfGlyphs;
	uint16_t mBitmapWidth;
	uint16_t mBitmapHeight;
	uint8_t mBytesPerPixel;
	uint16_t mGlyphFontSize;
	uint16_t mMaxAdvance;
	uint16_t mDpi;

} __attribute__((packed));

#define FONTCACHEFILE_MAGIC 0x6366610A
#define FONTCACHEFILE_MAJOR_VERSION 0x10
#define FONTCACHEFILE_MINOR_VERSION 0x03

#define BMF_REGULAR 0
#define BMF_ITALIC 1
#define BMF_BOLD 2

/**
 *
 * Using `djb2` (Dan Bernstein) 
 * algorithm 
 *
 * @param  pString [description]
 *
 * @return         [description]
 */
static uint32_t hash_name(std::string& pString) {

	uint32_t hash = 5381;
	int32_t c;

	for (char& c : pString) {
		hash = ((hash << 5) + hash) + static_cast<int32_t>(c); // hash * 33 + c
	}

	return hash;

}
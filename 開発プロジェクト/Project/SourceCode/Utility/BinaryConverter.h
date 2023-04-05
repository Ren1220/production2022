#pragma once

#include <vector>
#include <bitset>

// https://ameblo.jp/baginko1005/entry-10840077511.html

namespace BinaryConvertor
{

	// Convert the 32-bit binary encoding into hexadecimal
	extern int Binary2Hex(std::string Binary);
	// Convert the 32-bit binary into the decimal
	extern float GetFloat32(std::string Binary);
	extern int16_t ConvertBinaryToUShort(uint8_t dec[2]);
	extern int32_t ConvertBinaryToInt(uint8_t dec[4]);
	extern int64_t ConvertBinaryToLongLong(uint8_t dec[8]);
	extern float ConvertBinaryToFloat(uint8_t dec[4]);

};
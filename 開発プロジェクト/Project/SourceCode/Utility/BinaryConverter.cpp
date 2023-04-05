#include "BinaryConverter.h"


int16_t BinaryConvertor::ConvertBinaryToUShort(uint8_t dec[2])
{
	std::bitset<16> bit16{};

	for (int i = 0; i < 2; i++)
	{
		bit16 |= (dec[i] << (size_t(8) * i));
	}

	return static_cast<short>(bit16.to_ulong());
}

int32_t BinaryConvertor::ConvertBinaryToInt(uint8_t dec[4])
{
	std::bitset<32> bit32{};

	for (int i = 0; i < 4; i++)
	{
		bit32 |= (dec[i] << (size_t)(8 * i));
	}

	return static_cast<int32_t>(bit32.to_ulong());
}

int64_t BinaryConvertor::ConvertBinaryToLongLong(uint8_t dec[8])
{
	std::bitset<64> bit64{};

	for (int i = 0; i < 8; i++)
	{
		bit64 |= (dec[i] << (size_t)(8 * i));
	}

	return bit64.to_ullong();
}

float BinaryConvertor::ConvertBinaryToFloat(uint8_t dec[4])
{
	std::bitset<32> bit32{};

	for (int i = 0; i < 4; i++)
	{
		bit32 |= (size_t(dec[i]) << (size_t(8) * i));
	}

	return GetFloat32(bit32.to_string());
}

int32_t BinaryConvertor::Binary2Hex(std::string Binary)
{
	std::bitset<32> set(Binary);

	int32_t hex = set.to_ulong();

	return hex;
}

float BinaryConvertor::GetFloat32(std::string Binary)
{
	if (Binary == "10000000000000000000000000000000")
	{
		return 0.0f;
	}

	int32_t HexNumber = Binary2Hex(Binary);

	if (!HexNumber)
	{
		return 0.0f;
	}

	bool negative = !(HexNumber & 0x80000000);
	int  exponent = (HexNumber & 0x7f800000) >> 23;
	int sign = negative ? -1 : 1;

	// Subtract 127 from the exponent
	exponent -= 127;

	// Convert the mantissa into decimal using the
	// last 23 bits
	int power = -1;
	float total = 0.0;
	for (int i = 0; i < 23; i++)
	{
		int c = Binary[i + 9] - '0';
		total += (float)c * (float)pow(2.0, power);
		power--;
	}
	total += 1.0;

	float value = sign * (float)pow(2.0, exponent) * total;

	return value;
}
	


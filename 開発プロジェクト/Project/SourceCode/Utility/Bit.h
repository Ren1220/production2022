#pragma once

#include <vector>
#include <string>
#include <bitset>

using Byte1 = uint8_t;
using Byte2 = uint16_t;
using Byte4 = uint32_t;

using ByteArray = std::vector<Byte1>;

/// <summary>
/// Byte1２つをByte2に変換する
/// </summary>
/// <param name="_0">|>00000000| 00000000</param>
/// <param name="_1">| 00000000|>00000000</param>
/// <returns></returns>
inline Byte2 BitComposition(Byte1 _0, Byte1 _1)
{
	Byte2 tmp{};
	tmp |= (_0 << 8);
	tmp |= _1;
	return tmp;
}

/// <summary>
/// Byte1４つをByte4に変換する
/// </summary>
/// <param name="_0">|>00000000| 00000000| 00000000| 00000000</param>
/// <param name="_1">| 00000000|>00000000| 00000000| 00000000</param>
/// <param name="_2">| 00000000| 00000000|>00000000| 00000000</param>
/// <param name="_3">| 00000000| 00000000| 00000000|>00000000</param>
/// <returns></returns>
inline Byte4 BitComposition(Byte1 _0, Byte1 _1, Byte1 _2, Byte1 _3)
{
	Byte4 tmp{};
	tmp |= (_0 << 24);
	tmp |= (_1 << 16);
	tmp |= (_2 << 8);
	tmp |= (_3);
	return tmp;
}

inline float GetFloat(Byte1 _0, Byte1 _1, Byte1 _2, Byte1 _3)
{
	std::vector<char> cmd(4);
	cmd[0] = _0;
	cmd[1] = _1;
	cmd[2] = _2;
	cmd[3] = _3;

	return *reinterpret_cast<float*>(&cmd[0]);
}

inline float GetFloat(Byte1 _0, Byte1 _1)
{
	std::vector<char> cmd(4);
	cmd[0] = _0;
	cmd[1] = _1;

	return *reinterpret_cast<float*>(&cmd[0]);
}

inline float GetFloat(const std::bitset<32>& bit)
{
	std::string Binary = bit.to_string();

	int32_t HexNumber = bit.to_ulong();

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

inline float GetFloat(Byte4 byte)
{
	std::bitset<32> bit(byte);

	std::string Binary = bit.to_string();

	int32_t HexNumber = bit.to_ulong();

	if (!HexNumber)
	{
		return 0.0f;
	}

	if (Binary == "10000000000000000000000000000000")
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


//class Bit
//{
//private:
//
//	std::vector<bool> bits;
//
//
//public:
//
//	const std::vector<bool>& GetBits() const { return bits; }
//
//	/// <summary>
//	/// 文字列型のビット列を取得する
//	/// </summary>
//	/// <returns></returns>
//	std::string GetString() const
//	{
//		std::string tmp{};
//		for (int32_t i = bits.size() - 1; i > -1; i--)
//		{
//			tmp += (bits[i]) ? "1" : "0";
//		}
//		return tmp;
//	}
//
//	/// <summary>
//	/// ビット列をuint64_tで返す
//	/// </summary>
//	/// <returns></returns>
//	uint64_t GetU64() const
//	{
//		uint64_t tmp{};
//		for (int32_t i = bits.size() - 1; i > -1; i--)
//		{
//			tmp += (bits[i]) ? (1 << i) : 0;
//		}
//		return tmp;
//	}
//
//	/// <summary>
//	/// ビット列をuint32_tで返す
//	/// </summary>
//	/// <returns></returns>
//	uint32_t GetU32() const
//	{
//		uint32_t tmp{};
//		//for (int32_t i = 0; i < bits.size();i++)
//		//{
//		//	tmp += (bits[i]) ? (1 >> i) : 0;
//		//}
//		for (int32_t i = bits.size() - 1; i > -1; i--)
//		{
//			tmp |= (bits[i]) ? (1 << i) : 0;
//		}
//		return tmp;
//	}
//
//
//public:
//
//	/// <summary>
//	/// 値をビット列で格納する
//	/// </summary>
//	/// <typeparam name="T">[Byte1][Byte2][Byte4]</typeparam>
//	/// <param name="dst">[Byte1][Byte2][Byte4]</param>
//	template <class T>
//	void BitSet(T dst)
//	{
//		bits.clear();
//
//		int t = sizeof(dst);
//
//		for (int8_t i = 0; i < sizeof(dst) * 8; i++)
//		{
//			bits.push_back((dst >> i) & 1);
//		}
//	}
//
//	template <class T>
//	void BitAdd(T dst)
//	{
//		for (int8_t i = 0; i < sizeof(dst) * 8; i++)
//		{
//			bits.push_back((dst >> i) & 1);
//		}
//	}
//
//
//};

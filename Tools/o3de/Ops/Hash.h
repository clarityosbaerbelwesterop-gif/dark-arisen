#pragma once

// MD5 (to verify provider ETags) and SHA-256 (provenance identity), RFC 1321 / FIPS 180-4.

#include <array>
#include <cstdint>
#include <cstdio>
#include <string>
#include <string_view>

namespace DarkArisen::Tools
{
    inline std::string ToHex(const std::uint8_t* Bytes, const std::size_t Count)
    {
        static constexpr char Digits[] = "0123456789abcdef";
        std::string Result;
        Result.reserve(Count * 2);
        for (std::size_t Index = 0; Index < Count; ++Index)
        {
            Result += Digits[Bytes[Index] >> 4];
            Result += Digits[Bytes[Index] & 0x0F];
        }
        return Result;
    }

    inline std::string Sha256Hex(const std::string_view Data)
    {
        static constexpr std::uint32_t K[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
            0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7,
            0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
            0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85,
            0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
            0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c,
            0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
            0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
        std::uint32_t H[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
        const auto Rotr = [](const std::uint32_t Value, const int Bits) { return (Value >> Bits) | (Value << (32 - Bits)); };

        std::string Message(Data);
        const std::uint64_t BitLength = static_cast<std::uint64_t>(Data.size()) * 8u;
        Message += static_cast<char>(0x80);
        while (Message.size() % 64 != 56) Message += '\0';
        for (int Shift = 56; Shift >= 0; Shift -= 8) Message += static_cast<char>((BitLength >> Shift) & 0xFF);

        for (std::size_t Block = 0; Block < Message.size(); Block += 64)
        {
            std::uint32_t W[64];
            for (int Index = 0; Index < 16; ++Index)
            {
                const auto* Bytes = reinterpret_cast<const unsigned char*>(Message.data() + Block + Index * 4);
                W[Index] = (std::uint32_t(Bytes[0]) << 24) | (std::uint32_t(Bytes[1]) << 16) | (std::uint32_t(Bytes[2]) << 8) | Bytes[3];
            }
            for (int Index = 16; Index < 64; ++Index)
            {
                const std::uint32_t S0 = Rotr(W[Index - 15], 7) ^ Rotr(W[Index - 15], 18) ^ (W[Index - 15] >> 3);
                const std::uint32_t S1 = Rotr(W[Index - 2], 17) ^ Rotr(W[Index - 2], 19) ^ (W[Index - 2] >> 10);
                W[Index] = W[Index - 16] + S0 + W[Index - 7] + S1;
            }
            std::uint32_t A = H[0], B = H[1], C = H[2], D = H[3], E = H[4], F = H[5], G = H[6], Hh = H[7];
            for (int Index = 0; Index < 64; ++Index)
            {
                const std::uint32_t S1 = Rotr(E, 6) ^ Rotr(E, 11) ^ Rotr(E, 25);
                const std::uint32_t Choice = (E & F) ^ (~E & G);
                const std::uint32_t Temp1 = Hh + S1 + Choice + K[Index] + W[Index];
                const std::uint32_t S0 = Rotr(A, 2) ^ Rotr(A, 13) ^ Rotr(A, 22);
                const std::uint32_t Majority = (A & B) ^ (A & C) ^ (B & C);
                const std::uint32_t Temp2 = S0 + Majority;
                Hh = G; G = F; F = E; E = D + Temp1; D = C; C = B; B = A; A = Temp1 + Temp2;
            }
            H[0] += A; H[1] += B; H[2] += C; H[3] += D; H[4] += E; H[5] += F; H[6] += G; H[7] += Hh;
        }
        std::array<std::uint8_t, 32> Digest{};
        for (int Index = 0; Index < 8; ++Index)
        {
            Digest[static_cast<std::size_t>(Index * 4 + 0)] = static_cast<std::uint8_t>(H[Index] >> 24);
            Digest[static_cast<std::size_t>(Index * 4 + 1)] = static_cast<std::uint8_t>(H[Index] >> 16);
            Digest[static_cast<std::size_t>(Index * 4 + 2)] = static_cast<std::uint8_t>(H[Index] >> 8);
            Digest[static_cast<std::size_t>(Index * 4 + 3)] = static_cast<std::uint8_t>(H[Index]);
        }
        return ToHex(Digest.data(), Digest.size());
    }

    inline std::string Md5Hex(const std::string_view Data)
    {
        static constexpr std::uint32_t Shifts[64] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 5, 9, 14,
            20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 6, 10,
            15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};
        static constexpr std::uint32_t K[64] = {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a,
            0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e,
            0x49b40821, 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
            0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a, 0xfffa3942,
            0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa,
            0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 0xf4292244, 0x432aff97, 0xab9423a7,
            0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
            0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};
        std::uint32_t A0 = 0x67452301, B0 = 0xefcdab89, C0 = 0x98badcfe, D0 = 0x10325476;

        std::string Message(Data);
        const std::uint64_t BitLength = static_cast<std::uint64_t>(Data.size()) * 8u;
        Message += static_cast<char>(0x80);
        while (Message.size() % 64 != 56) Message += '\0';
        for (int Shift = 0; Shift < 64; Shift += 8) Message += static_cast<char>((BitLength >> Shift) & 0xFF);

        for (std::size_t Block = 0; Block < Message.size(); Block += 64)
        {
            std::uint32_t M[16];
            for (int Index = 0; Index < 16; ++Index)
            {
                const auto* Bytes = reinterpret_cast<const unsigned char*>(Message.data() + Block + Index * 4);
                M[Index] = Bytes[0] | (std::uint32_t(Bytes[1]) << 8) | (std::uint32_t(Bytes[2]) << 16) | (std::uint32_t(Bytes[3]) << 24);
            }
            std::uint32_t A = A0, B = B0, C = C0, D = D0;
            for (int Index = 0; Index < 64; ++Index)
            {
                std::uint32_t F = 0;
                int G = 0;
                if (Index < 16) { F = (B & C) | (~B & D); G = Index; }
                else if (Index < 32) { F = (D & B) | (~D & C); G = (5 * Index + 1) % 16; }
                else if (Index < 48) { F = B ^ C ^ D; G = (3 * Index + 5) % 16; }
                else { F = C ^ (B | ~D); G = (7 * Index) % 16; }
                F = F + A + K[Index] + M[G];
                A = D; D = C; C = B;
                B = B + ((F << Shifts[Index]) | (F >> (32 - Shifts[Index])));
            }
            A0 += A; B0 += B; C0 += C; D0 += D;
        }
        std::array<std::uint8_t, 16> Digest{};
        const std::uint32_t Words[4] = {A0, B0, C0, D0};
        for (int Word = 0; Word < 4; ++Word)
            for (int Byte = 0; Byte < 4; ++Byte)
                Digest[static_cast<std::size_t>(Word * 4 + Byte)] = static_cast<std::uint8_t>(Words[Word] >> (8 * Byte));
        return ToHex(Digest.data(), Digest.size());
    }
}

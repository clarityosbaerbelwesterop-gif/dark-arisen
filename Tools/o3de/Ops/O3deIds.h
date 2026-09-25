#pragma once

// Identifiers O3DE derives from source paths, reproduced so generated prefabs can reference assets
// before the Asset Processor has run. Verified against O3DE 2605.0:
//   AssetProcessor  AssetUtilities::CreateSafeSourceUUIDFromName  (lowercase, '/' separators)
//   AzCore          AZ::Uuid::CreateName                           (SHA-1, RFC 4122 variant, version 5)
//   Atom RPI        ModelAssetBuilderComponent::CreateAssetId      (0x10000000 | CRC32(group) & 0xFFFFFF)
// A scene's default mesh group is named after the source file stem.

#include "Hash.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <string_view>

namespace DarkArisen::Tools
{
    /** AZ::Uuid::CreateName(Name).ToString<AZStd::string>(): "{XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}". */
    inline std::string AzUuidFromName(const std::string_view Name)
    {
        std::array<std::uint8_t, 20> Digest = Sha1(Name);
        Digest[8] = static_cast<std::uint8_t>((Digest[8] & 0xBF) | 0x80);
        Digest[6] = static_cast<std::uint8_t>((Digest[6] & 0x5F) | 0x50);
        static constexpr char Digits[] = "0123456789ABCDEF";
        std::string Result = "{";
        for (std::size_t Index = 0; Index < 16; ++Index)
        {
            if (Index == 4 || Index == 6 || Index == 8 || Index == 10) Result += '-';
            Result += Digits[Digest[Index] >> 4];
            Result += Digits[Digest[Index] & 0x0F];
        }
        return Result + "}";
    }

    /** Source asset UUID for a path relative to its scan folder. */
    inline std::string SourceAssetUuid(std::string_view ScanRelativePath)
    {
        std::string Normalised(ScanRelativePath);
        std::transform(Normalised.begin(), Normalised.end(), Normalised.begin(), [](const char Character)
        {
            if (Character == '\\') return '/';
            return (Character >= 'A' && Character <= 'Z') ? static_cast<char>(Character + ('a' - 'A')) : Character;
        });
        return AzUuidFromName(Normalised);
    }

    /** Product sub-id of the Atom model built from a mesh group. */
    inline std::uint32_t AtomModelSubId(const std::string_view MeshGroupName)
    {
        return 0x10000000u | (Crc32(MeshGroupName, true) & 0x00FFFFFFu);
    }
}

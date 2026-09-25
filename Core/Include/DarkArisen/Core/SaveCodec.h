#pragma once

#include "DarkArisen/Core/CampaignState.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace DarkArisen::Core
{
    /**
     * Deterministic, engine-neutral save file format.
     *
     *   DARKARISEN-SAVE <format>\n
     *   checksum <fnv1a64 of body, 16 hex digits>\n
     *   <body: one tab-separated record per line>
     *
     * Floats use shortest round-trip text, so encode(decode(x)) is byte-identical.
     * Decoding is strict: unknown records, duplicate scalars, bad numbers, a wrong
     * checksum or a truncated file are all rejected. Decoding does not validate
     * campaign rules; CampaignRuntime::LoadState migrates and validates afterwards.
     */
    class SaveCodec
    {
    public:
        static constexpr int FormatVersion = 1;
        static constexpr std::string_view Magic = "DARKARISEN-SAVE";

        static std::string Encode(const CampaignState& State);
        static bool Decode(std::string_view Text, CampaignState& OutState, std::vector<std::string>& OutErrors);
        static std::uint64_t Checksum(std::string_view Body);
    };
}

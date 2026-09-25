#pragma once

#include <AzCore/std/string/string.h>

#include <string>

namespace DarkArisen
{
    /**
     * Save slots under the O3DE user folder (@user@/Saves/<slot>.dasave).
     * Writes go to a temporary file first so a crash never leaves a half-written slot.
     * Content is produced/parsed by Core::SaveCodec; this class only moves bytes.
     */
    class SaveSlotStore
    {
    public:
        static constexpr const char* AutosaveSlot = "Autosave";

        static bool IsValidSlotName(const AZStd::string& slotName);
        static AZStd::string SlotPath(const AZStd::string& slotName);
        static bool Write(const AZStd::string& slotName, const std::string& contents, AZStd::string& outError);
        static bool Read(const AZStd::string& slotName, std::string& outContents, AZStd::string& outError);
    };
}

#pragma once

#include <AzCore/base.h>
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
        /** UE AAlphaMenuPlayerController save slot: quick save, pause-menu save and load. */
        static constexpr const char* ManualSlot = "DarkArisenAlpha";

        static bool IsValidSlotName(const AZStd::string& slotName);
        static AZStd::string SlotPath(const AZStd::string& slotName);
        static bool Write(const AZStd::string& slotName, const std::string& contents, AZStd::string& outError);
        static bool Read(const AZStd::string& slotName, std::string& outContents, AZStd::string& outError);
        static bool Exists(const AZStd::string& slotName);
        /** File modification time, 0 when the slot does not exist. */
        static AZ::u64 ModifiedTime(const AZStd::string& slotName);
    };
}

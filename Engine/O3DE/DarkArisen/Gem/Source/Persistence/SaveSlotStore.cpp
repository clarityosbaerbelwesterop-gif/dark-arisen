#include "SaveSlotStore.h"

#include <AzCore/IO/FileIO.h>

namespace DarkArisen
{
    namespace
    {
        constexpr AZ::u64 MaximumSaveBytes = 16ull * 1024ull * 1024ull;
    }

    bool SaveSlotStore::IsValidSlotName(const AZStd::string& slotName)
    {
        if (slotName.empty() || slotName.size() > 32)
        {
            return false;
        }
        for (const char character : slotName)
        {
            const bool allowed = (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') ||
                (character >= '0' && character <= '9') || character == '_' || character == '-';
            if (!allowed)
            {
                return false;
            }
        }
        return true;
    }

    AZStd::string SaveSlotStore::SlotPath(const AZStd::string& slotName)
    {
        return AZStd::string::format("@user@/Saves/%s.dasave", slotName.c_str());
    }

    bool SaveSlotStore::Write(const AZStd::string& slotName, const std::string& contents, AZStd::string& outError)
    {
        AZ::IO::FileIOBase* fileIO = AZ::IO::FileIOBase::GetInstance();
        if (!fileIO || !IsValidSlotName(slotName))
        {
            outError = "Invalid save slot or file IO unavailable.";
            return false;
        }
        fileIO->CreatePath("@user@/Saves");
        const AZStd::string finalPath = SlotPath(slotName);
        const AZStd::string tempPath = finalPath + ".tmp";

        AZ::IO::HandleType handle = AZ::IO::InvalidHandle;
        if (!fileIO->Open(tempPath.c_str(), AZ::IO::OpenMode::ModeWrite | AZ::IO::OpenMode::ModeBinary, handle))
        {
            outError = "Cannot open " + tempPath + " for writing.";
            return false;
        }
        AZ::u64 written = 0;
        const bool writeOk = fileIO->Write(handle, contents.data(), contents.size(), &written) && written == contents.size();
        fileIO->Close(handle);
        if (!writeOk)
        {
            fileIO->Remove(tempPath.c_str());
            outError = "Short write to " + tempPath + ".";
            return false;
        }
        if (fileIO->Exists(finalPath.c_str()) && !fileIO->Remove(finalPath.c_str()))
        {
            outError = "Cannot replace " + finalPath + ".";
            return false;
        }
        if (!fileIO->Rename(tempPath.c_str(), finalPath.c_str()))
        {
            outError = "Cannot move " + tempPath + " into place.";
            return false;
        }
        return true;
    }

    bool SaveSlotStore::Read(const AZStd::string& slotName, std::string& outContents, AZStd::string& outError)
    {
        AZ::IO::FileIOBase* fileIO = AZ::IO::FileIOBase::GetInstance();
        if (!fileIO || !IsValidSlotName(slotName))
        {
            outError = "Invalid save slot or file IO unavailable.";
            return false;
        }
        const AZStd::string path = SlotPath(slotName);
        AZ::u64 size = 0;
        if (!fileIO->Size(path.c_str(), size) || size == 0 || size > MaximumSaveBytes)
        {
            outError = "Save slot " + slotName + " is missing, empty or oversized.";
            return false;
        }
        AZ::IO::HandleType handle = AZ::IO::InvalidHandle;
        if (!fileIO->Open(path.c_str(), AZ::IO::OpenMode::ModeRead | AZ::IO::OpenMode::ModeBinary, handle))
        {
            outError = "Cannot open " + path + ".";
            return false;
        }
        outContents.resize(static_cast<size_t>(size));
        const bool readOk = fileIO->Read(handle, outContents.data(), size, true);
        fileIO->Close(handle);
        if (!readOk)
        {
            outError = "Short read from " + path + ".";
            return false;
        }
        return true;
    }
}

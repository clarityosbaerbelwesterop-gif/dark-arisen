#include "DarkArisen/Core/WorldStreaming.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace DarkArisen::Core
{
    WorldStreaming::WorldStreaming(StreamingSettings InSettings, std::vector<StreamedPlace> InPlaces)
        : Settings(InSettings), Places(std::move(InPlaces))
    {
        const double Tile = std::max(Settings.TileSize, 1.0);
        ColumnCount = static_cast<int>(std::ceil(Settings.WorldWidth / Tile));
        RowCount = static_cast<int>(std::ceil(Settings.WorldHeight / Tile));
        const auto Count = static_cast<std::size_t>(ColumnCount * RowCount);
        Land.assign(Count, 1);
        Resident.assign(Count, 0);
        PlaceResident.assign(Places.size(), 0);
        Settings.UnloadRadius = std::max(Settings.UnloadRadius, Settings.LoadRadius);
        Settings.LoadsPerUpdate = std::max(Settings.LoadsPerUpdate, 1);
    }

    void WorldStreaming::SetLandTiles(const std::vector<int>& Tiles)
    {
        std::fill(Land.begin(), Land.end(), std::uint8_t{0});
        for (const int T : Tiles)
            if (T >= 0 && static_cast<std::size_t>(T) < Land.size()) Land[static_cast<std::size_t>(T)] = 1;
    }

    double WorldStreaming::TileDistanceSquared(const int Tile, const double X, const double Y) const
    {
        const int Column = Tile % ColumnCount, Row = Tile / ColumnCount;
        const double X0 = Column * Settings.TileSize, Y0 = Row * Settings.TileSize;
        const double Dx = std::max({X0 - X, 0.0, X - (X0 + Settings.TileSize)});
        const double Dy = std::max({Y0 - Y, 0.0, Y - (Y0 + Settings.TileSize)});
        return Dx * Dx + Dy * Dy;
    }

    StreamingChange WorldStreaming::Update(const double PlayerX, const double PlayerY)
    {
        StreamingChange Change;
        const double Load2 = Settings.LoadRadius * Settings.LoadRadius, Unload2 = Settings.UnloadRadius * Settings.UnloadRadius;

        // leaving: only beyond the unload radius
        for (std::size_t T = 0; T < Resident.size(); ++T)
        {
            if (Resident[T] && TileDistanceSquared(static_cast<int>(T), PlayerX, PlayerY) > Unload2)
            {
                Resident[T] = 0;
                Change.UnloadTiles.push_back(static_cast<int>(T));
            }
        }
        for (std::size_t P = 0; P < Places.size(); ++P)
        {
            const double Dx = Places[P].X - PlayerX, Dy = Places[P].Y - PlayerY;
            const double Leave = Places[P].Radius + (Settings.UnloadRadius - Settings.LoadRadius);
            if (PlaceResident[P] && Dx * Dx + Dy * Dy > Leave * Leave)
            {
                PlaceResident[P] = 0;
                Change.UnloadPlaces.push_back(Places[P].Id);
            }
        }

        // arriving: within the load radius, nearest first, places before tiles at equal distance, within the budget
        struct Want
        {
            double Distance2;
            bool Place;
            std::size_t Index;
        };
        std::vector<Want> Wanted;
        const int C0 = std::max(0, static_cast<int>(std::floor((PlayerX - Settings.LoadRadius) / Settings.TileSize)));
        const int C1 = std::min(ColumnCount - 1, static_cast<int>(std::floor((PlayerX + Settings.LoadRadius) / Settings.TileSize)));
        const int R0 = std::max(0, static_cast<int>(std::floor((PlayerY - Settings.LoadRadius) / Settings.TileSize)));
        const int R1 = std::min(RowCount - 1, static_cast<int>(std::floor((PlayerY + Settings.LoadRadius) / Settings.TileSize)));
        for (int Row = R0; Row <= R1; ++Row)
            for (int Column = C0; Column <= C1; ++Column)
            {
                const int T = Row * ColumnCount + Column;
                if (!Land[static_cast<std::size_t>(T)] || Resident[static_cast<std::size_t>(T)]) continue;
                const double D2 = TileDistanceSquared(T, PlayerX, PlayerY);
                if (D2 <= Load2) Wanted.push_back({D2, false, static_cast<std::size_t>(T)});
            }
        for (std::size_t P = 0; P < Places.size(); ++P)
        {
            if (PlaceResident[P]) continue;
            const double Dx = Places[P].X - PlayerX, Dy = Places[P].Y - PlayerY;
            const double D2 = Dx * Dx + Dy * Dy;
            if (D2 <= Places[P].Radius * Places[P].Radius) Wanted.push_back({D2, true, P});
        }
        std::sort(Wanted.begin(), Wanted.end(), [](const Want& A, const Want& B)
            { return A.Distance2 != B.Distance2 ? A.Distance2 < B.Distance2 : (A.Place != B.Place ? A.Place : A.Index < B.Index); });
        const std::size_t Budget = static_cast<std::size_t>(Settings.LoadsPerUpdate);
        for (std::size_t K = 0; K < Wanted.size() && K < Budget; ++K)
        {
            const Want& W = Wanted[K];
            if (W.Place)
            {
                PlaceResident[W.Index] = 1;
                Change.LoadPlaces.push_back(Places[W.Index].Id);
            }
            else
            {
                Resident[W.Index] = 1;
                Change.LoadTiles.push_back(static_cast<int>(W.Index));
            }
        }
        return Change;
    }

    bool WorldStreaming::IsTileResident(const int Tile) const
    {
        return Tile >= 0 && static_cast<std::size_t>(Tile) < Resident.size() && Resident[static_cast<std::size_t>(Tile)];
    }

    bool WorldStreaming::IsPlaceResident(const std::string& Id) const
    {
        for (std::size_t P = 0; P < Places.size(); ++P)
            if (Places[P].Id == Id) return PlaceResident[P] != 0;
        return false;
    }

    std::size_t WorldStreaming::ResidentTileCount() const
    {
        return static_cast<std::size_t>(std::count(Resident.begin(), Resident.end(), std::uint8_t{1}));
    }
}

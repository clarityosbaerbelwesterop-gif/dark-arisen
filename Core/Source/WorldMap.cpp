#include "DarkArisen/Core/WorldMap.h"

#include <algorithm>
#include <cmath>

namespace DarkArisen::Core
{
    WorldMap::WorldMap(MapSettings InSettings, std::vector<MapPlace> InPlaces, RegionAt InRegion)
        : Settings(InSettings), Places(std::move(InPlaces)), Region(std::move(InRegion))
    {
        Revealed.assign(static_cast<std::size_t>(Columns()) * static_cast<std::size_t>(Rows()), false);
    }

    int WorldMap::Columns() const { return std::max(1, static_cast<int>(std::ceil(Settings.Width / Settings.Cell))); }
    int WorldMap::Rows() const { return std::max(1, static_cast<int>(std::ceil(Settings.Height / Settings.Cell))); }

    void WorldMap::Reveal(const double X, const double Y, const double Radius)
    {
        const int C0 = std::max(0, static_cast<int>((X - Radius) / Settings.Cell)), C1 = std::min(Columns() - 1, static_cast<int>((X + Radius) / Settings.Cell));
        const int R0 = std::max(0, static_cast<int>((Y - Radius) / Settings.Cell)), R1 = std::min(Rows() - 1, static_cast<int>((Y + Radius) / Settings.Cell));
        for (int R = R0; R <= R1; ++R)
            for (int C = C0; C <= C1; ++C)
            {
                const double Cx = (C + 0.5) * Settings.Cell - X, Cy = (R + 0.5) * Settings.Cell - Y;
                if (Cx * Cx + Cy * Cy <= Radius * Radius) Revealed[static_cast<std::size_t>(R) * static_cast<std::size_t>(Columns()) + static_cast<std::size_t>(C)] = true;
            }
        // what Jake can see from here goes on the map
        for (const MapPlace& P : Places)
        {
            const double Dx = P.X - X, Dy = P.Y - Y;
            if (Dx * Dx + Dy * Dy <= std::max(Radius, P.SeenFrom) * std::max(Radius, P.SeenFrom)) Known.insert(P.Id);
        }
    }

    void WorldMap::Travel(const double X, const double Y, const bool AtSea, const bool Overcast)
    {
        const double Moved = Started ? std::sqrt((X - TrueX) * (X - TrueX) + (Y - TrueY) * (Y - TrueY)) : 0.0;
        TrueX = X;
        TrueY = Y;
        if (AtSea && Overcast && Started)
        {
            // dead reckoning: the marked position follows the heading and speed, but the doubt grows with each mile
            EstimateX = X;
            EstimateY = Y;
            Doubt += Moved * Settings.DriftPerMetre;
        }
        else
        {
            // a fix: land underfoot or the sky to read
            EstimateX = X;
            EstimateY = Y;
            Doubt = 0.0;
        }
        Started = true;
        Reveal(X, Y, AtSea ? Settings.RevealAtSea : Settings.RevealOnLand);
    }

    void WorldMap::ReachVista(const double X, const double Y)
    {
        Reveal(X, Y, Settings.RevealFromVista);
        Doubt = 0.0;
    }

    bool WorldMap::Learn(const std::string_view PlaceId)
    {
        for (const MapPlace& P : Places)
            if (P.Id == PlaceId)
            {
                Known.insert(P.Id);
                return true;
            }
        return false;
    }

    bool WorldMap::IsRevealed(const double X, const double Y) const
    {
        if (X < 0.0 || Y < 0.0 || X >= Settings.Width || Y >= Settings.Height) return false;
        return Revealed[static_cast<std::size_t>(static_cast<int>(Y / Settings.Cell)) * static_cast<std::size_t>(Columns()) +
                        static_cast<std::size_t>(static_cast<int>(X / Settings.Cell))];
    }

    std::vector<const MapPlace*> WorldMap::MinimapPlaces() const
    {
        std::vector<const MapPlace*> Out;
        for (const MapPlace& P : Places)
        {
            const double Dx = P.X - EstimateX, Dy = P.Y - EstimateY;
            if (Known.count(P.Id) && Dx * Dx + Dy * Dy <= Settings.MinimapRadius * Settings.MinimapRadius) Out.push_back(&P);
        }
        return Out;
    }

    double WorldMap::Explored(const std::string_view RegionId) const
    {
        if (!Region) return 0.0;
        std::size_t Total = 0, Seen = 0;
        for (int R = 0; R < Rows(); ++R)
            for (int C = 0; C < Columns(); ++C)
            {
                if (Region((C + 0.5) * Settings.Cell, (R + 0.5) * Settings.Cell) != RegionId) continue;
                ++Total;
                Seen += Revealed[static_cast<std::size_t>(R) * static_cast<std::size_t>(Columns()) + static_cast<std::size_t>(C)] ? 1u : 0u;
            }
        return Total ? static_cast<double>(Seen) / static_cast<double>(Total) : 0.0;
    }

    std::vector<std::uint8_t> WorldMap::SaveRevealed() const
    {
        std::vector<std::uint8_t> Bytes((Revealed.size() + 7) / 8, 0);
        for (std::size_t K = 0; K < Revealed.size(); ++K)
            if (Revealed[K]) Bytes[K / 8] = static_cast<std::uint8_t>(Bytes[K / 8] | (1u << (K % 8)));
        return Bytes;
    }

    bool WorldMap::LoadRevealed(const std::vector<std::uint8_t>& Bytes)
    {
        if (Bytes.size() != (Revealed.size() + 7) / 8) return false;
        for (std::size_t K = 0; K < Revealed.size(); ++K) Revealed[K] = ((Bytes[K / 8] >> (K % 8)) & 1u) != 0;
        return true;
    }
}

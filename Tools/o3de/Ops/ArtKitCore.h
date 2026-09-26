#pragma once

// Internal building blocks of the art kit. Only IEEE basic operations (+ - * / sqrt) are used for
// anything that reaches a file; sine and cosine are evaluated by our own series, so the output is
// the same on GCC, Clang and MSVC.

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace DarkArisen::Tools::Art
{
    constexpr double Pi = 3.14159265358979323846;

    double Sin(double X);
    double Cos(double X);
    double Clamp(double V, double Lo, double Hi);
    double Lerp(double A, double B, double T);
    double SmoothStep(double E0, double E1, double X);

    struct V3
    {
        double X = 0, Y = 0, Z = 0;
        V3 operator+(const V3& O) const { return {X + O.X, Y + O.Y, Z + O.Z}; }
        V3 operator-(const V3& O) const { return {X - O.X, Y - O.Y, Z - O.Z}; }
        V3 operator*(double S) const { return {X * S, Y * S, Z * S}; }
    };
    double Dot(const V3& A, const V3& B);
    V3 Cross(const V3& A, const V3& B);
    double Length(const V3& A);
    V3 Normalize(const V3& A);

    /** Rows of a 3x4 affine transform (rotation/scale in 3x3, translation in the last column). */
    struct Xf
    {
        std::array<double, 12> M{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0};
        static Xf Translate(const V3& T);
        static Xf RotateX(double Radians);
        static Xf RotateY(double Radians);
        static Xf RotateZ(double Radians);
        static Xf Scale(const V3& S);
        Xf operator*(const Xf& O) const;
        V3 Point(const V3& P) const;
        V3 Direction(const V3& D) const;
    };

    /** Deterministic pseudo random stream (integer only). */
    class Rng
    {
    public:
        explicit Rng(std::uint32_t Seed) : State(Seed * 747796405u + 2891336453u) {}
        std::uint32_t Next();
        double Unit();                        // [0, 1)
        double Range(double Lo, double Hi);
    private:
        std::uint32_t State;
    };
    std::uint32_t HashString(std::string_view Text);

    // ------------------------------------------------------------------ textures

    struct Image
    {
        int Width = 0, Height = 0, Channels = 3;
        std::vector<std::uint8_t> Pixels;  // row-major, Channels per pixel
    };

    /** Tileable value noise in [0, 65535] with the given lattice period (cells per tile). */
    int Noise(int X, int Y, int Size, int Period, std::uint32_t Seed);
    /** Octave sum of Noise, periods Base, 2*Base, ... ; result in [0, 65535]. */
    int Fbm(int X, int Y, int Size, int BasePeriod, int Octaves, std::uint32_t Seed);
    /** Height (0..65535) to a tangent-space normal map. */
    Image NormalFromHeight(const std::vector<int>& Height, int Size, int StrengthPercent);
    std::string EncodePng(const Image& Img);

    // ------------------------------------------------------------------ meshes

    struct Vertex
    {
        V3 P, N;
        double U = 0, V = 0;
    };

    struct Material
    {
        std::string Name;
        std::string BaseColorTexture;   // file name under Assets/Art/Textures, empty for none
        std::string NormalTexture;
        std::array<double, 4> BaseColor{1, 1, 1, 1};
        double Metallic = 0.0;
        double Roughness = 0.8;
        bool Mask = false;              // alpha-tested (fronds)
        bool DoubleSided = false;
        double MetresPerTile = 2.0;     // world-space UV scale used by the builders
    };

    class Mesh
    {
    public:
        explicit Mesh(std::string Name) : MeshName(std::move(Name)) {}

        void Push(const Xf& T);
        void Pop();
        const Xf& Top() const { return Stack.back(); }

        /** Axis-aligned box (in the current transform), world-scaled UVs per face. */
        void Box(const Material& M, const V3& Min, const V3& Max);
        /** Tapered cylinder along the segment, optional caps. */
        void Tube(const Material& M, const V3& A, const V3& B, double RadiusA, double RadiusB, int Sides, bool Caps = true);
        /**
         * Surface through rings of equal point count (smooth normals). Each ring runs around the
         * section; Closed joins the last point of a ring to its first.
         */
        void Loft(const Material& M, const std::vector<std::vector<V3>>& Rings, bool Closed, bool FlipWinding = false);
        void Quad(const Material& M, const V3& A, const V3& B, const V3& C, const V3& D, bool TwoSided = false);
        /** Card with explicit UVs (0..1), for alpha-tested leaves. */
        void Card(const Material& M, const V3& A, const V3& B, const V3& C, const V3& D);
        /** Planar polygon cap (convex, counter-clockwise seen along +Normal). */
        void Fan(const Material& M, const std::vector<V3>& Points, const V3& Normal);
        /** Raw triangle with its own normals and UVs. */
        void Triangle(const Material& M, const Vertex& A, const Vertex& B, const Vertex& C);
        /** Indexed triangles (shared vertices), transformed by the current stack. */
        void Indexed(const Material& M, const std::vector<Vertex>& Vertices, const std::vector<std::uint32_t>& Indices);

        struct Part
        {
            Material Mat;
            std::vector<Vertex> Vertices;
            std::vector<std::uint32_t> Indices;
        };
        const std::vector<Part>& Parts() const { return PartList; }
        const std::string& Name() const { return MeshName; }
        void Append(const Mesh& Other);

    private:
        std::string MeshName;
        std::vector<Part> PartList;
        std::vector<Xf> Stack{Xf{}};
        Part& PartFor(const Material& M);
    };

    /**
     * glTF 2.0 (embedded buffer, external PNG textures under ../Textures/). Meshes are built in the
     * O3DE world frame (X forward, Y left, Z up, metres); the writer stores g = (-x, z, y), which the
     * project's glTF import (Gltf.h) maps back to world. The map is a rotation: winding is kept.
     */
    std::string WriteGltf(const Mesh& Model, const std::string& Generator);
    /** Positions/normals only, one primitive, for a PhysX collision copy. */
    std::string WriteCollisionGltf(const Mesh& Model);
}

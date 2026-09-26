// Dark Arisen O3DE operations owner.
//
// Native C++ replacement for bootstrap/configure/build/test/package scripts
// (Docs/NATIVE_CPP_POLICY.md forbids shell/PowerShell/Python sources in Tools/).
// Every step is deterministic and fails closed with a stable exit code:
//   0 ok, 2 usage, 3 lock file, 4 missing prerequisite, 5 engine checkout, 6 command failed.

#include "AssetPipeline.h"
#include "Hash.h"
#include "Materializer.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace
{
    enum Exit : int
    {
        Ok = 0,
        Usage = 2,
        LockError = 3,
        MissingPrerequisite = 4,
        EngineCheckout = 5,
        CommandFailed = 6
    };

#if defined(_WIN32)
    constexpr const char* NullDevice = "NUL";
    constexpr const char* DefaultEngineRoot = "C:\\DarkArisenEngine\\o3de";
    constexpr const char* PlatformName = "windows";
    constexpr const char* O3deScript = "scripts\\o3de.bat";
    constexpr const char* GetPython = "python\\get_python.bat";
#else
    constexpr const char* NullDevice = "/dev/null";
    constexpr const char* DefaultEngineRoot = "/opt/dark-arisen/o3de";
    constexpr const char* PlatformName = "linux";
    constexpr const char* O3deScript = "scripts/o3de.sh";
    constexpr const char* GetPython = "python/get_python.sh";
#endif

    struct Options
    {
        std::string Command;
        fs::path RepoRoot;
        fs::path EngineRoot;
        fs::path BuildDir;
        std::string Config = "profile";
        fs::path Manifest;
        fs::path Glb;
        long long MinimumVertices = 0;
        bool DryRun = false;
        bool Check = false;
    };

    std::string Quote(const std::string& Value)
    {
        return "\"" + Value + "\"";
    }

    int Run(const std::string& CommandLine, const Options& Opts)
    {
        std::cout << "+ " << CommandLine << "\n" << std::flush;
        if (Opts.DryRun)
        {
            return 0;
        }
        const int Status = std::system(CommandLine.c_str());
        return Status == 0 ? 0 : (Status == -1 ? 127 : Status);
    }

    bool Succeeds(const std::string& CommandLine)
    {
        return std::system((CommandLine + " > " + NullDevice + " 2>&1").c_str()) == 0;
    }

    std::string Capture(const std::string& CommandLine)
    {
#if defined(_WIN32)
        FILE* Pipe = _popen(CommandLine.c_str(), "r");
#else
        FILE* Pipe = popen(CommandLine.c_str(), "r");
#endif
        if (Pipe == nullptr)
        {
            return {};
        }
        std::string Output;
        std::array<char, 256> Buffer{};
        while (std::fgets(Buffer.data(), static_cast<int>(Buffer.size()), Pipe) != nullptr)
        {
            Output += Buffer.data();
        }
#if defined(_WIN32)
        _pclose(Pipe);
#else
        pclose(Pipe);
#endif
        while (!Output.empty() && (Output.back() == '\n' || Output.back() == '\r'))
        {
            Output.pop_back();
        }
        return Output;
    }

    /** Reads Tools/o3de/O3DE_VERSION.lock and refuses floating refs. */
    bool ReadLock(const fs::path& RepoRoot, std::map<std::string, std::string>& Lock)
    {
        std::ifstream Stream(RepoRoot / "Tools" / "o3de" / "O3DE_VERSION.lock");
        if (!Stream)
        {
            std::cerr << "error: Tools/o3de/O3DE_VERSION.lock not found under " << RepoRoot << "\n";
            return false;
        }
        std::string Line;
        while (std::getline(Stream, Line))
        {
            if (Line.empty() || Line[0] == '#')
            {
                continue;
            }
            const auto Split = Line.find('=');
            if (Split == std::string::npos)
            {
                std::cerr << "error: malformed lock line: " << Line << "\n";
                return false;
            }
            Lock[Line.substr(0, Split)] = Line.substr(Split + 1);
        }
        for (const char* Key : {"repository", "tag", "commit", "license", "package_server"})
        {
            if (Lock[Key].empty())
            {
                std::cerr << "error: lock is missing '" << Key << "'\n";
                return false;
            }
        }
        const std::string& Commit = Lock["commit"];
        const bool IsSha = Commit.size() == 40 && Commit.find_first_not_of("0123456789abcdef") == std::string::npos;
        const std::string& Tag = Lock["tag"];
        if (!IsSha || Tag == "main" || Tag == "development" || Tag == "HEAD" || Tag == "latest")
        {
            std::cerr << "error: the lock must pin a release tag and a full 40-character commit SHA\n";
            return false;
        }
        return true;
    }

    struct Check
    {
        std::string Name;
        bool Passed = false;
        std::string Detail;
        bool Blocking = true;
    };

    std::vector<Check> Doctor(const Options& Opts, const std::map<std::string, std::string>& Lock)
    {
        std::vector<Check> Checks;
        const auto Tool = [&](const std::string& Name, const std::string& Probe, const std::string& Why, bool Blocking = true)
        {
            const bool Found = Succeeds(Probe);
            std::string Version = Found ? Capture(Probe + " 2>&1") : Why;
            Version = Version.substr(0, Version.find('\n'));
            Checks.push_back({Name, Found, Version, Blocking});
        };
        Tool("git", "git --version", "install git");
        Tool("git-lfs", "git lfs version", "install git-lfs; O3DE stores binary assets in LFS");
        Tool("cmake", "cmake --version", "install CMake >= 3.24 (O3DE 2605.0 minimum)");
#if defined(_WIN32)
        Tool("msvc", "where cl", "run from a Visual Studio 2022 Developer prompt");
#else
        Tool("ninja", "ninja --version", "install ninja-build");
        Tool("clang", "clang++ --version", "install clang (O3DE Linux builds use clang)");
        Tool("vulkan", "vulkaninfo --summary", "no Vulkan device: Editor/GameLauncher cannot render here", false);
#endif
        Tool("curl", "curl --version", "install curl (used for the package-server probe)");

        const std::string Server = Lock.at("package_server");
        const bool ServerReachable = Succeeds("curl -sSfI --max-time 15 " + Server + "/");
        // The bucket answers 403/404 for its root when reachable; only a transport failure is fatal.
        const bool TransportOk = ServerReachable ||
            Capture("curl -s -o " + std::string(NullDevice) + " -w %{http_code} --max-time 15 " + Server + "/") != "000";
        Checks.push_back({"o3de-package-server", TransportOk,
            TransportOk ? Server + " reachable"
                        : Server + " unreachable: O3DE cannot download its third-party packages. "
                                   "Allow this host in the environment network policy.",
            true});

        std::error_code Error;
        const fs::path Probe = fs::exists(Opts.EngineRoot.parent_path(), Error) ? Opts.EngineRoot.parent_path() : fs::current_path();
        const auto Space = fs::space(Probe, Error);
        const double FreeGiB = Error ? 0.0 : static_cast<double>(Space.available) / (1024.0 * 1024.0 * 1024.0);
        std::ostringstream Detail;
        Detail.precision(1);
        Detail << std::fixed << FreeGiB << " GiB free at " << Probe.string() << " (O3DE source builds need 100+ GB: docs.o3de.org/docs/welcome-guide/requirements)";
        Checks.push_back({"disk", FreeGiB >= 100.0, Detail.str(), true});
        return Checks;
    }

    int CommandDoctor(const Options& Opts, const std::map<std::string, std::string>& Lock)
    {
        bool Blocked = false;
        for (const Check& Item : Doctor(Opts, Lock))
        {
            const char* Status = Item.Passed ? "ok  " : (Item.Blocking ? "FAIL" : "warn");
            std::cout << "[" << Status << "] " << Item.Name << ": " << Item.Detail << "\n";
            Blocked = Blocked || (!Item.Passed && Item.Blocking);
        }
        return Blocked ? MissingPrerequisite : Ok;
    }

    int VerifyEngine(const Options& Opts, const std::map<std::string, std::string>& Lock)
    {
        if (!fs::exists(Opts.EngineRoot / "engine.json"))
        {
            std::cerr << "error: no O3DE checkout at " << Opts.EngineRoot << " (run bootstrap)\n";
            return EngineCheckout;
        }
        const std::string Head = Capture("git -C " + Quote(Opts.EngineRoot.string()) + " rev-parse HEAD");
        if (Head != Lock.at("commit"))
        {
            std::cerr << "error: engine checkout is at " << (Head.empty() ? "<unknown>" : Head) << ", lock requires "
                      << Lock.at("commit") << "\n";
            return EngineCheckout;
        }
        std::cout << "engine " << Opts.EngineRoot.string() << " @ " << Head << " (" << Lock.at("tag") << ")\n";
        return Ok;
    }

    int CommandBootstrap(const Options& Opts, const std::map<std::string, std::string>& Lock)
    {
        const std::string Root = Quote(Opts.EngineRoot.string());
        if (!fs::exists(Opts.EngineRoot / ".git"))
        {
            fs::create_directories(Opts.EngineRoot.parent_path());
            if (Run("git clone --branch " + Lock.at("tag") + " --single-branch " + Lock.at("repository") + " " + Root, Opts) != 0)
            {
                return CommandFailed;
            }
        }
        if (Run("git -C " + Root + " fetch --depth 1 origin " + Lock.at("commit"), Opts) != 0 ||
            Run("git -C " + Root + " checkout --detach " + Lock.at("commit"), Opts) != 0 ||
            Run("git -C " + Root + " lfs install --local", Opts) != 0 ||
            Run("git -C " + Root + " lfs pull", Opts) != 0)
        {
            return CommandFailed;
        }
        if (!Opts.DryRun && VerifyEngine(Opts, Lock) != Ok)
        {
            return EngineCheckout;
        }
        // O3DE's own tooling: its pinned Python runtime and engine registration.
        if (Run(Quote((Opts.EngineRoot / GetPython).string()), Opts) != 0 ||
            Run(Quote((Opts.EngineRoot / O3deScript).string()) + " register --this-engine", Opts) != 0)
        {
            return CommandFailed;
        }
        std::cout << "O3DE_ENGINE_ROOT=" << Opts.EngineRoot.string() << "\n";
        return Ok;
    }

    fs::path ProjectPath(const Options& Opts)
    {
        return Opts.RepoRoot / "Engine" / "O3DE" / "DarkArisen";
    }

    int CommandConfigure(const Options& Opts, const std::map<std::string, std::string>& Lock)
    {
        if (!Opts.DryRun && VerifyEngine(Opts, Lock) != Ok)
        {
            return EngineCheckout;
        }
#if defined(_WIN32)
        const std::string Generator = "-G \"Visual Studio 17 2022\"";
#else
        const std::string Generator = "-G \"Ninja Multi-Config\" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++";
#endif
        const std::string CommandLine = "cmake -S " + Quote(ProjectPath(Opts).string()) + " -B " +
            Quote(Opts.BuildDir.string()) + " " + Generator + " -DLY_UNITY_BUILD=ON -DLY_DISABLE_TEST_MODULES=OFF" +
            " -DCMAKE_MODULE_PATH=" + Quote((Opts.EngineRoot / "cmake").string()) +
            " -DLY_PACKAGE_SERVER_URLS=" + Lock.at("package_server");
        return Run(CommandLine, Opts) == 0 ? Ok : CommandFailed;
    }

    int CommandBuild(const Options& Opts)
    {
        const std::string Build = "cmake --build " + Quote(Opts.BuildDir.string()) + " --config " + Opts.Config;
        for (const char* Target : {"DarkArisen.GameLauncher", "Editor", "AssetProcessor", "DarkArisen.Assets"})
        {
            if (Run(Build + " --target " + Target, Opts) != 0)
            {
                std::cerr << "error: build target " << Target << " failed\n";
                return CommandFailed;
            }
        }
        return Ok;
    }

    int CommandTest(const Options& Opts)
    {
        // The engine-neutral core is always testable, with or without an engine.
        const fs::path CoreBuild = Opts.BuildDir / "core";
        if (Run("cmake -S " + Quote((Opts.RepoRoot / "Core").string()) + " -B " + Quote(CoreBuild.string()), Opts) != 0 ||
            Run("cmake --build " + Quote(CoreBuild.string()) + " --config Release", Opts) != 0 ||
            Run("ctest --test-dir " + Quote(CoreBuild.string()) + " -C Release --output-on-failure", Opts) != 0)
        {
            return CommandFailed;
        }
        if (!fs::exists(Opts.BuildDir / "CMakeCache.txt"))
        {
            std::cout << "O3DE build tree not configured; engine integration tests skipped (NOT verified).\n";
            return Ok;
        }
        return Run("ctest --test-dir " + Quote(Opts.BuildDir.string()) + " -C " + Opts.Config +
                       " -L SUITE_main --output-on-failure -R DarkArisen",
                   Opts) == 0
            ? Ok
            : CommandFailed;
    }

    int CommandPackage(const Options& Opts, const std::map<std::string, std::string>& Lock)
    {
        if (!Opts.DryRun && VerifyEngine(Opts, Lock) != Ok)
        {
            return EngineCheckout;
        }
        const fs::path Output = Opts.RepoRoot / "Build" / "Package" / PlatformName;
        const std::string CommandLine = Quote((Opts.EngineRoot / O3deScript).string()) +
            " export-project -es ExportSourceBuiltProject -pp " + Quote(ProjectPath(Opts).string()) +
            " -out " + Quote(Output.string()) + " -cfg release -a --no-unified-launcher --seedlist " +
            Quote((ProjectPath(Opts) / "AssetBundling" / "SeedLists" / "DefaultLevel.seed").string());
        return Run(CommandLine, Opts) == 0 ? Ok : CommandFailed;
    }

    std::string UtcNow()
    {
        const std::time_t Now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm Utc{};
#if defined(_WIN32)
        gmtime_s(&Utc, &Now);
#else
        gmtime_r(&Now, &Utc);
#endif
        char Buffer[32];
        std::strftime(Buffer, sizeof(Buffer), "%Y-%m-%dT%H:%M:%SZ", &Utc);
        return Buffer;
    }

    int CommandImportGlb(const Options& Opts)
    {
        if (Opts.Manifest.empty() || Opts.Glb.empty())
        {
            std::cerr << "error: import-glb needs --manifest=<Higgsfield3DSource.json> --glb=<exact downloaded GLB>\n";
            return Usage;
        }
        DarkArisen::Tools::ImportRequest Request;
        Request.RepoRoot = Opts.RepoRoot;
        Request.ManifestPath = Opts.Manifest.is_absolute() ? Opts.Manifest : Opts.RepoRoot / Opts.Manifest;
        Request.GlbPath = Opts.Glb;
        Request.MinimumVertices = Opts.MinimumVertices;
        Request.ImportDateUtc = UtcNow();
        DarkArisen::Tools::ImportResult Result;
        std::string Error;
        if (!DarkArisen::Tools::ImportProviderGlb(Request, Result, Error))
        {
            std::cerr << "import rejected: " << Error << "\n";
            return CommandFailed;
        }
        std::cout << "imported " << Result.Destination.string() << "\nsha256 " << Result.Sha256 << "\nprovenance "
                  << Result.ProvenancePath.string() << "\n";
        return Ok;
    }

    /** Fetches every "name=url <40-hex commit>" line of a lock file into Destination/name, exactly. */
    int FetchPinnedDependencies(const fs::path& LockPath, const fs::path& Destination, const Options& Opts)
    {
        std::ifstream Stream(LockPath);
        if (!Stream)
        {
            std::cerr << "error: missing dependency lock " << LockPath << "\n";
            return LockError;
        }
        std::string Line;
        while (std::getline(Stream, Line))
        {
            if (Line.empty() || Line[0] == '#') continue;
            std::istringstream Fields(Line);
            std::string NameAndUrl, Commit;
            Fields >> NameAndUrl >> Commit;
            const auto Split = NameAndUrl.find('=');
            if (Split == std::string::npos || Commit.size() != 40 ||
                Commit.find_first_not_of("0123456789abcdef") != std::string::npos)
            {
                std::cerr << "error: malformed dependency line: " << Line << "\n";
                return LockError;
            }
            const std::string Name = NameAndUrl.substr(0, Split);
            const std::string Url = NameAndUrl.substr(Split + 1);
            const std::string Target = Quote((Destination / Name).string());
            if (!fs::exists(Destination / Name / ".git") &&
                Run("git init -q " + Target + " && git -C " + Target + " remote add origin " + Url, Opts) != 0)
            {
                return CommandFailed;
            }
            if (Run("git -C " + Target + " fetch -q --depth 1 origin " + Commit, Opts) != 0 ||
                Run("git -C " + Target + " checkout -q --detach " + Commit, Opts) != 0)
            {
                return CommandFailed;
            }
        }
        return Ok;
    }

    /**
     * Downloads an archive pinned by SHA-256 (fail closed on any mismatch) and extracts it once.
     * Lock line: "<name>=<https url> sha256:<64 hex>".
     */
    int FetchVerifiedArchive(const fs::path& LockPath, const std::string& Name, const fs::path& Destination, const Options& Opts)
    {
        std::ifstream Stream(LockPath);
        std::string Line, Url, Expected;
        while (std::getline(Stream, Line))
        {
            if (Line.rfind(Name + "=", 0) != 0) continue;
            std::istringstream Fields(Line.substr(Name.size() + 1));
            std::string Hash;
            Fields >> Url >> Hash;
            if (Hash.rfind("sha256:", 0) == 0) Expected = Hash.substr(7);
        }
        if (Url.empty() || Expected.size() != 64 || Expected.find_first_not_of("0123456789abcdef") != std::string::npos)
        {
            std::cerr << "error: " << LockPath << " has no valid pinned entry for " << Name << "\n";
            return LockError;
        }
        const fs::path Archive = Destination / (Name + ".tar.gz");
        const fs::path Extracted = Destination / Name;
        fs::create_directories(Destination);
        if (Opts.DryRun)
        {
            std::cout << "[dry-run] fetch " << Url << " (sha256 " << Expected << ")\n";
            return Ok;
        }
        const auto Digest = [&Archive]()
        {
            std::ifstream In(Archive, std::ios::binary);
            const std::string Bytes((std::istreambuf_iterator<char>(In)), std::istreambuf_iterator<char>());
            return DarkArisen::Tools::Sha256Hex(Bytes);
        };
        if (!fs::exists(Archive) || Digest() != Expected)
        {
            if (Run("curl -fsSL -o " + Quote(Archive.string()) + " " + Quote(Url), Opts) != 0)
            {
                return CommandFailed;
            }
            if (Digest() != Expected)
            {
                std::cerr << "error: " << Name << " archive does not match its pinned sha256\n";
                return LockError;
            }
            std::error_code Ignored;
            fs::remove_all(Extracted, Ignored);
        }
        if (!fs::exists(Extracted))
        {
            fs::create_directories(Extracted);
            if (Run("tar -xzf " + Quote(Archive.string()) + " -C " + Quote(Extracted.string()), Opts) != 0)
            {
                return CommandFailed;
            }
        }
        return Ok;
    }

    /**
     * The Atom shader path without the Asset Processor: preprocess with the engine's shader
     * include roots, AZSL -> HLSL with O3DE's azslc 1.8.22, then HLSL -> SPIR-V (Vulkan) and DXIL
     * (DX12) with DXC 1.8.2505.1, the compiler O3DE 2605.0 pins. Every .azsl under the project's
     * Assets/Shaders and Tools/o3de/ShaderCheck is compiled for each "check: <profile> <entry>"
     * line it carries; O3DE's own Unlit shader is compiled first as the calibration reference.
     */
    int CommandShaderCheck(const Options& Opts)
    {
#if defined(_WIN32)
        std::cerr << "error: shader-check runs on Linux (the pinned DXC archive is the Linux build)\n";
        return MissingPrerequisite;
#else
        const fs::path CheckDir = Opts.RepoRoot / "Tools" / "o3de" / "ShaderCheck";
        const fs::path Deps = Opts.BuildDir / "shader-deps";
        if (const int Status = FetchPinnedDependencies(CheckDir / "DEPENDENCIES.lock", Deps, Opts); Status != Ok)
        {
            return Status;
        }
        if (const int Status = FetchVerifiedArchive(CheckDir / "ARCHIVES.lock", "dxc", Deps, Opts); Status != Ok)
        {
            return Status;
        }
        const fs::path AzslcBuild = Opts.BuildDir / "azslc";
        const fs::path Azslc = AzslcBuild / "azslc";
        const std::string CxxFlags = " \"-DCMAKE_CXX_FLAGS=-include cstdint\""; // upstream misses <cstdint> on GCC 13
        if (Run("cmake -S " + Quote((Deps / "azslc" / "src").string()) + " -B " + Quote(AzslcBuild.string()) +
                    " -DCMAKE_BUILD_TYPE=Release -DANTLR_BUILD_CPP_TESTS=OFF" + CxxFlags, Opts) != 0 ||
            Run("cmake --build " + Quote(AzslcBuild.string()) + " --config Release", Opts) != 0)
        {
            return CommandFailed;
        }
        const fs::path DxcRoot = Deps / "dxc";
        const std::string Dxc = "LD_LIBRARY_PATH=" + Quote((DxcRoot / "lib").string()) + " " + Quote((DxcRoot / "bin" / "dxc").string());

        const fs::path Project = Opts.RepoRoot / "Engine" / "O3DE" / "DarkArisen";
        const fs::path ShaderRoot = Project / "Assets" / "Shaders";
        const fs::path& Engine = Opts.EngineRoot;
        if (!Opts.DryRun && !fs::exists(Engine / "Gems" / "Atom" / "Feature" / "Common" / "Assets" / "ShaderLib"))
        {
            std::cerr << "error: Atom shader libraries not found under " << Engine << " (pass --engine=PATH)\n";
            return MissingPrerequisite;
        }
        std::string Includes = " -I " + Quote((Project / "ShaderLib").string());
        for (const fs::path& Root : {Engine / "Gems" / "Atom" / "Feature" / "Common" / "Assets" / "ShaderLib",
                 Engine / "Gems" / "Atom" / "RPI" / "Assets" / "ShaderLib", Engine / "Gems" / "Atom" / "Feature" / "Common" / "Assets" / "Shaders",
                 Engine / "Gems"})
        {
            Includes += " -I " + Quote(Root.string());
        }
        std::vector<std::pair<fs::path, std::vector<std::pair<std::string, std::string>>>> Jobs;
        Jobs.push_back({Engine / "Gems" / "Atom" / "Feature" / "Common" / "Assets" / "Shaders" / "Unlit" / "Unlit.azsl",
            {{"vs_6_2", "MainVS"}, {"ps_6_2", "MainPS"}}});
        std::vector<fs::path> Sources;
        for (const fs::path& Root : {CheckDir, ShaderRoot})
        {
            if (!fs::exists(Root)) continue;
            for (const auto& Entry : fs::recursive_directory_iterator(Root))
            {
                if (Entry.is_directory())
                {
                    Includes += " -I " + Quote(Entry.path().string());
                }
                else if (Entry.path().extension() == ".azsl")
                {
                    Sources.push_back(Entry.path());
                }
            }
        }
        std::sort(Sources.begin(), Sources.end());
        for (const fs::path& Source : Sources)
        {
            std::ifstream In(Source);
            std::string Line;
            std::vector<std::pair<std::string, std::string>> Entries;
            while (std::getline(In, Line))
            {
                const auto Marker = Line.find("check:");
                if (Marker == std::string::npos) continue;
                std::istringstream Fields(Line.substr(Marker + 6));
                std::string Profile, EntryPoint;
                Fields >> Profile >> EntryPoint;
                if (Profile.size() == 6 && Profile[2] == '_' && !EntryPoint.empty()) Entries.push_back({Profile, EntryPoint});
            }
            if (Entries.empty())
            {
                std::cerr << "error: " << Source.filename() << " lacks a 'check: <profile> <entry>' line (e.g. vs_6_2 MainVS)\n";
                return LockError;
            }
            Jobs.push_back({Source, Entries});
        }

        const fs::path Out = Opts.BuildDir / "shader-check";
        fs::create_directories(Out);
        int Compiled = 0;
        for (const auto& [Source, Entries] : Jobs)
        {
            const std::string Stem = Source.stem().string();
            const fs::path Pre = Out / (Stem + ".pre.azsl");
            const fs::path Hlsl = Out / (Stem + ".hlsl");
            if (Run("clang -E -P -x c -Wno-pragma-once-outside-header" + Includes + " " + Quote(Source.string()) + " -o " + Quote(Pre.string()), Opts) != 0 ||
                Run(Quote(Azslc.string()) + " " + Quote(Pre.string()) + " -o " + Quote(Hlsl.string()), Opts) != 0)
            {
                std::cerr << "error: " << Source << " failed AZSL -> HLSL\n";
                return CommandFailed;
            }
            for (const auto& [Profile, EntryPoint] : Entries)
            {
                const std::string Base = (Out / (Stem + "." + EntryPoint)).string();
                if (Run(Dxc + " -T " + Profile + " -E " + EntryPoint + " -spirv -fspv-target-env=vulkan1.1 " + Quote(Hlsl.string()) +
                            " -Fo " + Quote(Base + ".spv"), Opts) != 0 ||
                    Run(Dxc + " -T " + Profile + " -E " + EntryPoint + " " + Quote(Hlsl.string()) + " -Fo " + Quote(Base + ".dxil"), Opts) != 0)
                {
                    std::cerr << "error: " << Source << " " << EntryPoint << " failed HLSL -> SPIR-V/DXIL\n";
                    return CommandFailed;
                }
                ++Compiled;
            }
        }
        std::cout << Compiled << " entry points compiled AZSL -> HLSL -> SPIR-V + DXIL (" << Jobs.size() << " shaders, O3DE Unlit as calibration)\n";
        return Compiled > 0 ? Ok : LockError;
#endif
    }

    /**
     * Builds O3DE 2605.0 AzCore from the pinned engine sources plus the dependencies pinned in
     * AzCoreProbe/DEPENDENCIES.lock, then runs the campaign adapter inside AZ::ComponentApplication.
     * Needs no O3DE package server; proves framework-level runtime, not Editor/Atom.
     */
    int CommandProbe(const Options& Opts, const std::map<std::string, std::string>& Lock)
    {
        if (!Opts.DryRun && VerifyEngine(Opts, Lock) != Ok)
        {
            return EngineCheckout;
        }
        const fs::path ProbeDir = Opts.RepoRoot / "Tools" / "o3de" / "AzCoreProbe";
        const fs::path Deps = Opts.BuildDir / "probe-deps";
        if (const int Status = FetchPinnedDependencies(ProbeDir / "DEPENDENCIES.lock", Deps, Opts); Status != Ok)
        {
            return Status;
        }
        const fs::path Build = Opts.BuildDir / "azcore-probe";
        if (Run("cmake -S " + Quote(ProbeDir.string()) + " -B " + Quote(Build.string()) +
                    " -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release" +
                    " -DO3DE_ENGINE_ROOT=" + Quote(Opts.EngineRoot.string()) + " -DDARKARISEN_PROBE_DEPS=" + Quote(Deps.string()),
                Opts) != 0 ||
            Run("cmake --build " + Quote(Build.string()), Opts) != 0 ||
            Run("ctest --test-dir " + Quote(Build.string()) + " --output-on-failure", Opts) != 0)
        {
            return CommandFailed;
        }
        return Ok;
    }

    /** Writes (or with --check verifies) the O3DE vertical-slice levels generated from ContentSource. */
    int CommandMaterialize(const Options& Opts)
    {
        DarkArisen::Tools::MaterializeResult Result;
        if (!DarkArisen::Tools::MaterializeCampaign(Opts.RepoRoot, Result))
        {
            for (const std::string& Error : Result.Errors)
            {
                std::cerr << "materialize: " << Error << "\n";
            }
            return CommandFailed;
        }
        std::vector<std::string> Stale;
        const bool Ok_ = DarkArisen::Tools::ApplyMaterialization(Opts.RepoRoot, Result, Opts.Check, Stale);
        for (const std::string& Path : Stale)
        {
            std::cout << (Opts.Check ? "stale: " : "wrote: ") << Path << "\n";
        }
        std::cout << Result.Files.size() << " files, " << Stale.size() << (Opts.Check ? " stale" : " updated") << "\n";
        if (!Ok_)
        {
            std::cerr << (Opts.Check ? "materialized content is out of date; run DarkArisenO3DE materialize\n" : "write failed\n");
            return CommandFailed;
        }
        return Ok;
    }

    void PrintUsage()
    {
        std::cout << "usage: DarkArisenO3DE <doctor|lock|bootstrap|configure|build|test|package|import-glb|probe|shader-check|materialize>\n"
                     "       [--repo=PATH]\n"
                     "       [--engine=PATH] [--build-dir=PATH] [--config=profile|debug|release] [--dry-run]\n"
                     "       import-glb --manifest=PATH --glb=PATH [--min-vertices=N]\n"
                     "       materialize [--check]\n"
                     "O3DE_ENGINE_ROOT overrides the default engine location ("
                  << DefaultEngineRoot << ").\n";
    }
}

int main(int ArgumentCount, char** Arguments)
{
    Options Opts;
    const char* EnvironmentEngine = std::getenv("O3DE_ENGINE_ROOT");
    Opts.EngineRoot = EnvironmentEngine != nullptr && *EnvironmentEngine != '\0' ? fs::path(EnvironmentEngine) : fs::path(DefaultEngineRoot);
    Opts.RepoRoot = fs::current_path();
    for (int Index = 1; Index < ArgumentCount; ++Index)
    {
        const std::string Argument = Arguments[Index];
        const auto Value = [&Argument](const char* Prefix) { return Argument.substr(std::string(Prefix).size()); };
        if (Argument.rfind("--repo=", 0) == 0) Opts.RepoRoot = Value("--repo=");
        else if (Argument.rfind("--engine=", 0) == 0) Opts.EngineRoot = Value("--engine=");
        else if (Argument.rfind("--build-dir=", 0) == 0) Opts.BuildDir = Value("--build-dir=");
        else if (Argument.rfind("--config=", 0) == 0) Opts.Config = Value("--config=");
        else if (Argument.rfind("--manifest=", 0) == 0) Opts.Manifest = Value("--manifest=");
        else if (Argument.rfind("--glb=", 0) == 0) Opts.Glb = Value("--glb=");
        else if (Argument.rfind("--min-vertices=", 0) == 0) Opts.MinimumVertices = std::atoll(Value("--min-vertices=").c_str());
        else if (Argument == "--dry-run") Opts.DryRun = true;
        else if (Argument == "--check") Opts.Check = true;
        else if (Opts.Command.empty() && Argument.rfind("--", 0) != 0) Opts.Command = Argument;
        else
        {
            PrintUsage();
            return Usage;
        }
    }
    if (Opts.BuildDir.empty())
    {
        Opts.BuildDir = Opts.RepoRoot / "Build" / "O3DE" / PlatformName;
    }

    std::map<std::string, std::string> Lock;
    if (Opts.Command.empty() || Opts.Command == "help")
    {
        PrintUsage();
        return Opts.Command.empty() ? Usage : Ok;
    }
    if (!ReadLock(Opts.RepoRoot, Lock))
    {
        return LockError;
    }
    if (Opts.Command == "lock")
    {
        for (const auto& [Key, Value] : Lock) std::cout << Key << "=" << Value << "\n";
        return Ok;
    }
    if (Opts.Command == "doctor") return CommandDoctor(Opts, Lock);
    if (Opts.Command == "bootstrap") return CommandBootstrap(Opts, Lock);
    if (Opts.Command == "configure") return CommandConfigure(Opts, Lock);
    if (Opts.Command == "build") return CommandBuild(Opts);
    if (Opts.Command == "test") return CommandTest(Opts);
    if (Opts.Command == "package") return CommandPackage(Opts, Lock);
    if (Opts.Command == "import-glb") return CommandImportGlb(Opts);
    if (Opts.Command == "probe") return CommandProbe(Opts, Lock);
    if (Opts.Command == "shader-check") return CommandShaderCheck(Opts);
    if (Opts.Command == "materialize") return CommandMaterialize(Opts);
    PrintUsage();
    return Usage;
}

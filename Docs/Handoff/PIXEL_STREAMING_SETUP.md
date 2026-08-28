# PIXEL STREAMING SETUP — MILESTONE 0

**Status:** Engineering task specification
**Phase:** Handoff — Document 2 of 2
**Owner:** the engineering agent (GPT 5.6 Sol / Claude Code)
**Related:** PROMPT_FOR_GPT5_SOL.md §8, technical_targets.md
**Repo:** `https://github.com/EpicGames/PixelStreamingInfrastructure` — **UE 5.5 branch**

---

## 1. WHY THIS IS FIRST

Flo works from an iPad. **He cannot run Unreal Engine.** Until this is standing, every judgement about feel — the 6-frame deflection window, the 4-second camera hold, whether a battle line about to break actually sounds quieter — is being made from written descriptions.

**Nothing else in the project is worth this much per hour of work.**

---

## 2. WHAT IT IS

Unreal's Pixel Streaming renders the game on a server GPU and streams the frames to a browser over WebRTC, sending input back. **The client needs only a browser.** An iPad Pro is a fully adequate client.

**Three components:**
1. **The UE application** with the Pixel Streaming plugin, launched with streaming flags
2. **The Signalling Server** (Node.js) — WebRTC handshake and web player
3. **A TURN server** — required in practice, because most networks will not allow a direct peer connection

---

## 3. SERVER SPECIFICATION

**Development (M0-M4):**

| | Spec |
|---|---|
| GPU | **NVIDIA RTX A4000 / L4 / RTX 4070 or better** — NVENC required |
| vCPU | 8+ |
| RAM | 32GB |
| Disk | 500GB NVMe (UE builds are large) |
| OS | **Windows Server 2022** (Linux works but adds friction with 5.5) |
| Bandwidth | 50 Mbps up per concurrent stream |

**Providers, in order of preference:** Azure NV-series (best Windows GPU support) · AWS G5 · Paperspace · a colocated box if cost matters more than convenience.

**Cost expectation:** roughly **$0.80-1.60/hour** while running. **Auto-shutdown after 30 minutes idle is mandatory** — see §7.

**From M5 onward** the Belos benchmark (300 combatants, fire, collapse) will need a stronger GPU. Plan an upgrade path, do not over-provision now.

---

## 4. IMPLEMENTATION STEPS

### ☐ 4.1 — Plugin and launch config

- ☐ Enable **Pixel Streaming** and **Pixel Streaming Players** plugins in the UE 5.5 project
- ☐ Add a `Tools/streaming/launch-stream.ps1` that starts the packaged build with:
  - `-PixelStreamingURL=ws://localhost:8888`
  - `-RenderOffscreen -Unattended -NoTextureStreaming`
  - `-ResX=1920 -ResY=1080` *(iPad-appropriate; do not stream 4K)*
- ☐ Verify **NVENC hardware encoding** is active. Software encode will not hold 60fps and will invalidate every feel judgement.

### ☐ 4.2 — Signalling server

- ☐ Clone `PixelStreamingInfrastructure`, **UE 5.5 branch**
- ☐ `SignallingWebServer` running as a Windows service (auto-restart on crash)
- ☐ HTTPS with a real certificate — **iOS Safari will refuse microphone/gamepad APIs over plain HTTP**

### ☐ 4.3 — TURN

- ☐ Deploy **coturn** on the same host or adjacent
- ☐ Static credentials, rotated per milestone
- ☐ **Test from a mobile network, not just wifi.** This is the step people skip and it is the step that breaks.

### ☐ 4.4 — The web player

- ☐ Strip the default Epic UI down to: the video surface, a fullscreen button, and a latency readout
- ☐ **Touch input mapped for iPad:** virtual stick left, look-drag right, and — critically — **a settings-free way to attach a Bluetooth controller.** Flo should be able to pair a DualSense and have it just work.
- ☐ **Latency readout visible by default.** He needs to know whether input lag is the build or the network before he judges combat feel.

### ☐ 4.5 — Access

- ☐ Password-gated. No public URL, ever.
- ☐ **One branch build at a time**, at a stable URL, so the link never changes
- ☐ A `Tools/streaming/deploy.ps1` that pulls a branch, packages, and swaps the running build in one command

---

## 5. THE LATENCY TARGET

| Segment | Budget |
|---|---|
| Render | 16.6ms *(the game's own frame)* |
| Encode (NVENC) | 8-12ms |
| Network RTT | 20-60ms *(depends on server region)* |
| Decode + display | 15-25ms |
| **Total glass-to-glass** | **60-115ms** |

### 5.1 The Honest Caveat You Must Give Flo

**This is playable and it is not the shipping experience.**

At 60-115ms, Flo can judge: world, tone, pacing, camera holds, dialogue, sound, whether a place feels right, whether a scene lands.

**He cannot reliably judge the 6-frame deflection window**, which is 100ms of total tolerance — **the same order as the streaming latency itself.**

**Design consequence:** combat feel must be validated on a local machine. Budget for **one Windows dev box that a contractor or Flo himself can eventually use**, or for a periodic session on a rented physical machine. **Say this to Flo explicitly rather than letting him conclude the combat feels wrong when it is the wire.**

### 5.2 Server Region

Flo is in **Germany.** Use **Azure West Europe / Germany West Central** or equivalent. A US-East server adds 90ms of RTT and makes the above worse than useless for anything but sightseeing.

---

## 6. WHAT TO STREAM AND WHEN

| Milestone | What Flo should be judging |
|---|---|
| M0 | that it works at all |
| M1 | camera holds, locomotion weight, the wounded set — **not deflection timing** |
| M2 | **the vertical slice.** Dungeon silence, the boss's three phases, whether sparing Cruz feels like it should |
| M3 | the ship as a place; going below while sailing |
| M4 | the four-second Marks line; whether the journal reads like a person |
| M5 | **whether a line about to break sounds quieter** |
| M6 | the emergence frame; the arrow; the thirty seconds |

---

## 7. OPERATIONS

- ☐ **Auto-shutdown after 30 minutes idle.** A forgotten GPU instance is €500/month of nothing.
- ☐ A one-tap start from a bookmark or shortcut — **Flo should not be running cloud CLI commands from an iPad**
- ☐ Session recording optional, off by default, **on for milestone reviews** so feedback can reference a timestamp
- ☐ Frame-time telemetry (`PROMPT_FOR_GPT5_SOL.md` §4, M0) logged server-side and reviewable

---

## 8. THE MERGE GATE

**M0 is complete when, and only when:**

Flo opens a bookmarked URL on an iPad, enters a password, sees a grey-box level, moves a capsule with touch controls **and** with a paired DualSense, reads a latency number under 120ms, and the whole thing shuts itself down half an hour after he closes the tab.

**Report with the URL and the password. Not with a description.**

---

## 9. ONE MORE THING

When this works, Flo will play Dark Arisen for the first time.

It will be a capsule in a grey box. **Tell him what it is before he opens it**, so that the moment is what it actually is — the first time anyone has played this — rather than a disappointment about box geometry.

**Then get to M1.**

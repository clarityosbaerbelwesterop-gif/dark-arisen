#!/usr/bin/env python3
"""Promote already-tested candidate bytes to final private Alpha ZIP names."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import shutil
import sys
import zipfile
from pathlib import Path


FINAL_NAMES = {
    "Win64": "DarkArisen-Alpha-Windows-Arcware.zip",
    "Linux": "DarkArisen-Alpha-Linux-x86_64.zip",
}

REQUIRED_GATES = (
    "full_content_m0_m8",
    "windows_ue55_build_and_automation",
    "linux_ue55_build_and_automation",
    "windows_start_to_credits_playthrough",
    "linux_start_to_credits_playthrough",
    "content_manifest_reconciled",
    "performance_floor_60_fps",
    "privacy_access_denied",
    "credits_music_and_licences_approved",
    "flo_alpha_approval",
)


class PromotionError(RuntimeError):
    pass


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def read_candidate_manifest(path: Path) -> dict[str, object]:
    try:
        with zipfile.ZipFile(path) as archive:
            matches = [
                name
                for name in archive.namelist()
                if name.rstrip("/").endswith("DarkArisen-build-manifest.json")
            ]
            if len(matches) != 1:
                raise PromotionError(
                    f"{path.name} must contain exactly one DarkArisen build manifest"
                )
            with archive.open(matches[0]) as handle:
                manifest = json.load(handle)
                if not isinstance(manifest, dict):
                    raise PromotionError(f"{path.name} build manifest must be a JSON object")
                return manifest
    except (OSError, zipfile.BadZipFile, json.JSONDecodeError) as error:
        raise PromotionError(f"cannot verify candidate {path}: {error}") from error


def validate_candidate(
    path: Path,
    expected_platform: str,
    expected_commit: str,
    expected_sha256: str,
) -> None:
    if not path.is_file():
        raise PromotionError(f"candidate ZIP is missing: {path}")
    actual_hash = sha256(path)
    if actual_hash != expected_sha256.lower():
        raise PromotionError(
            f"candidate checksum mismatch for {path.name}: {actual_hash}"
        )
    manifest = read_candidate_manifest(path)
    requirements = {
        "schema_version": 1,
        "artifact_kind": "candidate",
        "alpha_status": "not-accepted",
        "project": "DarkArisen",
        "platform": expected_platform,
        "configuration": "Shipping",
        "candidate_commit": expected_commit,
        "pixel_streaming": True,
    }
    for key, expected in requirements.items():
        if manifest.get(key) != expected:
            raise PromotionError(
                f"{path.name} manifest has invalid {key!r}: {manifest.get(key)!r}"
            )
    if not str(manifest.get("engine_version", "")).startswith("5.5."):
        raise PromotionError(f"{path.name} was not packaged with Unreal Engine 5.5")


def validate_evidence(evidence: dict[str, object]) -> tuple[str, dict[str, str]]:
    if not isinstance(evidence, dict):
        raise PromotionError("evidence must be a JSON object")
    if evidence.get("schema_version") != 1 or evidence.get("status") != "accepted-alpha":
        raise PromotionError("evidence must use schema 1 and status 'accepted-alpha'")
    commit = str(evidence.get("candidate_commit", "")).lower()
    if len(commit) != 40 or any(character not in "0123456789abcdef" for character in commit):
        raise PromotionError("evidence candidate_commit must be a complete commit SHA")
    gates = evidence.get("gates")
    if not isinstance(gates, dict):
        raise PromotionError("evidence gates object is missing")
    missing = [gate for gate in REQUIRED_GATES if gates.get(gate) is not True]
    if missing:
        raise PromotionError("Alpha acceptance gates are not green: " + ", ".join(missing))
    artifacts = evidence.get("artifacts")
    if not isinstance(artifacts, dict):
        raise PromotionError("evidence artifacts object is missing")
    hashes: dict[str, str] = {}
    for platform_key in ("windows", "linux"):
        item = artifacts.get(platform_key)
        if not isinstance(item, dict):
            raise PromotionError(f"evidence artifact is missing: {platform_key}")
        value = str(item.get("sha256", "")).lower()
        if len(value) != 64 or any(character not in "0123456789abcdef" for character in value):
            raise PromotionError(f"invalid {platform_key} candidate SHA-256 in evidence")
        hashes[platform_key] = value
    return commit, hashes


def promote(
    evidence_path: Path,
    windows_path: Path,
    linux_path: Path,
    output_directory: Path,
) -> list[Path]:
    try:
        evidence = json.loads(evidence_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        raise PromotionError(f"cannot read Alpha evidence: {error}") from error
    commit, hashes = validate_evidence(evidence)
    validate_candidate(windows_path, "Win64", commit, hashes["windows"])
    validate_candidate(linux_path, "Linux", commit, hashes["linux"])

    output_directory = output_directory.resolve()
    if output_directory == Path(output_directory.anchor):
        raise PromotionError("output directory may not be a filesystem root")
    final_paths = [output_directory / name for name in FINAL_NAMES.values()]
    for path in final_paths:
        if path.exists() or Path(f"{path}.sha256").exists():
            raise PromotionError(f"refusing to overwrite existing Alpha artifact: {path}")

    output_directory.mkdir(parents=True, exist_ok=True)
    inputs = ((windows_path, hashes["windows"]), (linux_path, hashes["linux"]))
    partials: list[Path] = []
    try:
        for (source, expected_hash), destination in zip(inputs, final_paths):
            partial = destination.with_suffix(destination.suffix + ".partial")
            partials.append(partial)
            shutil.copyfile(source, partial)
            if sha256(partial) != expected_hash:
                raise PromotionError(f"copy verification failed for {destination.name}")
        for partial, destination, (_, expected_hash) in zip(
            partials, final_paths, inputs
        ):
            os.replace(partial, destination)
            Path(f"{destination}.sha256").write_text(
                f"{expected_hash}  {destination.name}\n", encoding="ascii"
            )
    finally:
        for partial in partials:
            if partial.exists():
                partial.unlink()
    return final_paths


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--evidence", type=Path, required=True)
    parser.add_argument("--windows", type=Path, required=True)
    parser.add_argument("--linux", type=Path, required=True)
    parser.add_argument("--output-directory", type=Path, required=True)
    args = parser.parse_args()
    try:
        outputs = promote(
            args.evidence.resolve(),
            args.windows.resolve(),
            args.linux.resolve(),
            args.output_directory,
        )
    except PromotionError as error:
        print(f"Alpha promotion refused: {error}", file=sys.stderr)
        return 1
    for output in outputs:
        print(f"Accepted private Alpha artifact: {output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

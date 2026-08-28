#!/usr/bin/env python3

from __future__ import annotations

import hashlib
import importlib.util
import json
import sys
import tempfile
import unittest
import zipfile
from pathlib import Path

MODULE_PATH = Path(__file__).with_name("promote-alpha-candidates.py")
SPEC = importlib.util.spec_from_file_location("promote_alpha_candidates", MODULE_PATH)
assert SPEC and SPEC.loader
MODULE = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = MODULE
SPEC.loader.exec_module(MODULE)

FINAL_NAMES = MODULE.FINAL_NAMES
REQUIRED_GATES = MODULE.REQUIRED_GATES
PromotionError = MODULE.PromotionError
promote = MODULE.promote


class AlphaCandidatePromotionTests(unittest.TestCase):
    commit = "1" * 40

    def _candidate(self, root: Path, platform: str) -> tuple[Path, str]:
        candidate = root / f"candidate-{platform}.zip"
        manifest = {
            "schema_version": 1,
            "artifact_kind": "candidate",
            "alpha_status": "not-accepted",
            "project": "DarkArisen",
            "platform": platform,
            "configuration": "Shipping",
            "candidate_commit": self.commit,
            "engine_version": "5.5.4",
            "pixel_streaming": True,
        }
        with zipfile.ZipFile(candidate, "w") as archive:
            archive.writestr("DarkArisen-build-manifest.json", json.dumps(manifest))
            archive.writestr("DarkArisen/Content/Paks/DarkArisen.pak", platform)
        digest = hashlib.sha256(candidate.read_bytes()).hexdigest()
        return candidate, digest

    def _evidence(
        self,
        root: Path,
        windows_hash: str,
        linux_hash: str,
        *,
        missing_gate: str | None = None,
    ) -> Path:
        gates = {gate: gate != missing_gate for gate in REQUIRED_GATES}
        evidence = {
            "schema_version": 1,
            "status": "accepted-alpha",
            "candidate_commit": self.commit,
            "artifacts": {
                "windows": {"sha256": windows_hash},
                "linux": {"sha256": linux_hash},
            },
            "gates": gates,
        }
        path = root / "evidence.json"
        path.write_text(json.dumps(evidence), encoding="utf-8")
        return path

    def test_valid_candidates_are_copied_without_byte_changes(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            windows, windows_hash = self._candidate(root, "Win64")
            linux, linux_hash = self._candidate(root, "Linux")
            evidence = self._evidence(root, windows_hash, linux_hash)
            output = root / "accepted"

            results = promote(evidence, windows, linux, output)

            self.assertEqual({path.name for path in results}, set(FINAL_NAMES.values()))
            self.assertEqual(hashlib.sha256(results[0].read_bytes()).hexdigest(), windows_hash)
            self.assertEqual(hashlib.sha256(results[1].read_bytes()).hexdigest(), linux_hash)
            self.assertTrue(Path(f"{results[0]}.sha256").is_file())
            self.assertTrue(Path(f"{results[1]}.sha256").is_file())

    def test_missing_acceptance_gate_refuses_promotion(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            windows, windows_hash = self._candidate(root, "Win64")
            linux, linux_hash = self._candidate(root, "Linux")
            evidence = self._evidence(
                root,
                windows_hash,
                linux_hash,
                missing_gate="flo_alpha_approval",
            )
            with self.assertRaisesRegex(PromotionError, "flo_alpha_approval"):
                promote(evidence, windows, linux, root / "accepted")

    def test_checksum_mismatch_refuses_promotion(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            windows, windows_hash = self._candidate(root, "Win64")
            linux, linux_hash = self._candidate(root, "Linux")
            evidence = self._evidence(root, "0" * 64, linux_hash)
            with self.assertRaisesRegex(PromotionError, "checksum mismatch"):
                promote(evidence, windows, linux, root / "accepted")

    def test_platform_mismatch_refuses_promotion(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            windows, windows_hash = self._candidate(root, "Linux")
            linux, linux_hash = self._candidate(root, "Win64")
            evidence = self._evidence(root, windows_hash, linux_hash)
            with self.assertRaisesRegex(PromotionError, "platform"):
                promote(evidence, windows, linux, root / "accepted")

    def test_existing_final_artifact_is_not_overwritten(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            windows, windows_hash = self._candidate(root, "Win64")
            linux, linux_hash = self._candidate(root, "Linux")
            evidence = self._evidence(root, windows_hash, linux_hash)
            output = root / "accepted"
            output.mkdir()
            existing = output / FINAL_NAMES["Win64"]
            existing.write_bytes(b"owner data")
            with self.assertRaisesRegex(PromotionError, "overwrite"):
                promote(evidence, windows, linux, output)
            self.assertEqual(existing.read_bytes(), b"owner data")


if __name__ == "__main__":
    unittest.main()

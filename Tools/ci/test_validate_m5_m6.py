#!/usr/bin/env python3
"""Regression coverage for validate_m5_m6.py."""

from __future__ import annotations

import shutil
import tempfile
import unittest
from pathlib import Path

import validate_m5_m6

ROOT = Path(__file__).resolve().parents[2]


class M5M6ValidatorTests(unittest.TestCase):
    def _copy_contract(self, destination: Path) -> None:
        for relative in validate_m5_m6.REQUIRED_FILES:
            source = ROOT / relative
            target = destination / relative
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source, target)

    def test_repository_contract_passes(self) -> None:
        self.assertEqual(validate_m5_m6.validate(ROOT), [])

    def test_fast_travel_api_fails_closed(self) -> None:
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)
            self._copy_contract(root)
            header = root / "Source/DarkArisen/Highmoore/HighmooreHorseComponent.h"
            header.write_text(header.read_text(encoding="utf-8") + "\nvoid TeleportToWaypoint();\n", encoding="utf-8")
            errors = validate_m5_m6.validate(root)
            self.assertTrue(any("fast travel" in error.lower() for error in errors))

    def test_magic_system_api_fails_closed(self) -> None:
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)
            self._copy_contract(root)
            header = root / "Source/DarkArisen/Highmoore/CrystalGuardianComponent.h"
            header.write_text(header.read_text(encoding="utf-8") + "\nvoid CastSpell();\n", encoding="utf-8")
            errors = validate_m5_m6.validate(root)
            self.assertTrue(any("magic" in error.lower() for error in errors))

    def test_crew_hostage_exclusion_fails_closed(self) -> None:
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)
            self._copy_contract(root)
            source = root / "Source/DarkArisen/ColonialWar/RetaliationSubsystem.cpp"
            text = source.read_text(encoding="utf-8").replace(
                "|| Candidate.bChild\n            || Candidate.bNarrativeProtected",
                "|| Candidate.bChild\n            || Candidate.bCrew\n            || Candidate.bNarrativeProtected",
            )
            source.write_text(text, encoding="utf-8")
            errors = validate_m5_m6.validate(root)
            self.assertTrue(any("adult crew" in error.lower() for error in errors))

    def test_illegal_arrow_helper_fails_closed(self) -> None:
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)
            self._copy_contract(root)
            header = root / "Source/DarkArisen/Highmoore/PrincessQuestStateComponent.h"
            header.write_text(header.read_text(encoding="utf-8") + "\nvoid SlowMotionArrow();\n", encoding="utf-8")
            errors = validate_m5_m6.validate(root)
            self.assertTrue(any("slow-motion" in error.lower() for error in errors))


if __name__ == "__main__":
    unittest.main()

#!/usr/bin/env python3
"""Regression coverage for validate_m3_m4.py."""

from __future__ import annotations

import shutil
import tempfile
import unittest
from pathlib import Path

import validate_m3_m4


ROOT = Path(__file__).resolve().parents[2]


class M3M4ValidatorTests(unittest.TestCase):
    def test_repository_contract_passes(self) -> None:
        self.assertEqual(validate_m3_m4.validate(ROOT), [])

    def _copy_contract(self, destination: Path) -> None:
        for relative in validate_m3_m4.REQUIRED_FILES:
            source = ROOT / relative
            target = destination / relative
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source, target)

    def test_generic_currency_conversion_fails_closed(self) -> None:
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)
            self._copy_contract(root)
            header = root / "Source/DarkArisen/Systems/ProgressionEconomyComponent.h"
            header.write_text(
                header.read_text(encoding="utf-8")
                + "\nvoid ConvertCurrency(EDarkArisenCurrency From, EDarkArisenCurrency To);\n",
                encoding="utf-8",
            )
            errors = validate_m3_m4.validate(root)
            self.assertTrue(any("conversion" in error.lower() for error in errors))

    def test_sixth_named_crew_entry_fails_closed(self) -> None:
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)
            self._copy_contract(root)
            source = root / "Source/DarkArisen/Ship/ShipVoyageComponent.cpp"
            source.write_text(
                source.read_text(encoding="utf-8")
                + '\n// AddCrew(TEXT("crew.invalid"), TEXT("Invalid"), TEXT("Invalid"), ECrewWatch::Dawn);\n',
                encoding="utf-8",
            )
            errors = validate_m3_m4.validate(root)
            self.assertTrue(any("exactly five" in error.lower() for error in errors))

    def test_autosave_before_decision_api_fails_closed(self) -> None:
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)
            self._copy_contract(root)
            header = root / "Source/DarkArisen/World/DarkArisenWorldRulesSubsystem.h"
            header.write_text(
                header.read_text(encoding="utf-8") + "\nvoid AutosaveBeforeDecision();\n",
                encoding="utf-8",
            )
            errors = validate_m3_m4.validate(root)
            self.assertTrue(any("autosave-before-choice" in error.lower() for error in errors))


if __name__ == "__main__":
    unittest.main()

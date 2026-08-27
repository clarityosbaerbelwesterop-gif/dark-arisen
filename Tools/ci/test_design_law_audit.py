#!/usr/bin/env python3

from __future__ import annotations

import tempfile
import unittest
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from design_law_audit import RACHE_IMPLEMENTATION, audit


class DesignLawAuditTests(unittest.TestCase):
    def test_clean_source_passes(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            path = root / "Source/DarkArisen/Plain.cpp"
            path.parent.mkdir(parents=True)
            path.write_text("void Move() {}\n", encoding="utf-8")
            self.assertEqual(audit(root), [])

    def test_forbidden_interface_symbol_fails(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            path = root / "Source/DarkArisen/Bad.cpp"
            path.parent.mkdir(parents=True)
            path.write_text("void Build() { HealthBarWidget(); }\n", encoding="utf-8")
            self.assertTrue(audit(root))

    def test_general_widget_construction_is_not_overblocked(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            path = root / "Source/DarkArisen/PauseMenu.cpp"
            path.parent.mkdir(parents=True)
            path.write_text("void Build() { CreateWidget(); }\n", encoding="utf-8")
            self.assertEqual(audit(root), [])

    def test_time_dilation_outside_rache_fails(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            bad = root / "Source/DarkArisen/Cutscene.cpp"
            bad.parent.mkdir(parents=True)
            bad.write_text("void Arrow() { SetGlobalTimeDilation(); }\n", encoding="utf-8")
            self.assertTrue(audit(root))

    def test_custom_actor_time_dilation_fails(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            bad = root / "Source/DarkArisen/WeaponWheel.cpp"
            bad.parent.mkdir(parents=True)
            bad.write_text("void Open() { CustomTimeDilation = 0.3f; }\n", encoding="utf-8")
            self.assertTrue(audit(root))

    def test_rache_requires_locked_constant_and_restore(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            good = root / RACHE_IMPLEMENTATION
            good.parent.mkdir(parents=True)
            good.write_text(
                "void Enter() { SetGlobalTimeDilation(DesignLaws::RacheTimeScale); }\n"
                "void Exit() { SetGlobalTimeDilation(1.0f); }\n",
                encoding="utf-8",
            )
            self.assertEqual(audit(root), [])


if __name__ == "__main__":
    unittest.main()

#!/usr/bin/env python3

from __future__ import annotations

import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from validate_m1 import validate


class M1ValidationTests(unittest.TestCase):
    def test_checked_in_project_satisfies_m1_contract(self) -> None:
        root = Path(__file__).resolve().parents[2]
        self.assertEqual(validate(root), [])

    def test_empty_repository_fails_closed(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            findings = validate(Path(temporary))
            self.assertTrue(findings)
            self.assertTrue(any("CoreLoopTuning.h" in finding for finding in findings))

    def test_superseded_interaction_helper_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            source = root / "Source/DarkArisen"
            source.mkdir(parents=True)
            (source / "Bad.cpp").write_text("void HighlightNearbyLoot();", encoding="utf-8")
            findings = validate(root)
            self.assertTrue(any("HighlightNearbyLoot" in finding for finding in findings))


if __name__ == "__main__":
    unittest.main()

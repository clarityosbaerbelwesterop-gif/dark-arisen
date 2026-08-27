#!/usr/bin/env python3

from __future__ import annotations

import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from validate_m2 import validate


class M2QuestFoundationValidationTests(unittest.TestCase):
    def test_checked_in_project_satisfies_m2_quest_contract(self) -> None:
        root = Path(__file__).resolve().parents[2]
        self.assertEqual(validate(root), [])

    def test_empty_repository_fails_closed(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            findings = validate(Path(temporary))
            self.assertTrue(findings)
            self.assertTrue(any("QuestJournalComponent.h" in finding for finding in findings))

    def test_player_facing_activation_signal_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            component = root / "Source/DarkArisen/Components"
            component.mkdir(parents=True)
            (component / "QuestJournalComponent.h").write_text(
                "void OnQuestActivated();", encoding="utf-8")
            (component / "QuestJournalComponent.cpp").write_text("", encoding="utf-8")
            findings = validate(root)
            self.assertTrue(any("OnQuestActivated" in finding for finding in findings))


if __name__ == "__main__":
    unittest.main()

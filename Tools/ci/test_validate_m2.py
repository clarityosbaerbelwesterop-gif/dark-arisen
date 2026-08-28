#!/usr/bin/env python3

from __future__ import annotations

import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from validate_m2 import validate


class M2SourceContractValidationTests(unittest.TestCase):
    def test_checked_in_project_satisfies_m2_source_contract(self) -> None:
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

    def test_boss_lifespan_cleanup_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            bosses = root / "Source/DarkArisen/Bosses"
            bosses.mkdir(parents=True)
            (bosses / "IsabelCruzCharacter.h").write_text("", encoding="utf-8")
            (bosses / "IsabelCruzCharacter.cpp").write_text(
                "SetLifeSpan(4.0f);", encoding="utf-8")
            findings = validate(root)
            self.assertTrue(any("SetLifeSpan" in finding for finding in findings))

    def test_heat_widget_signal_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            components = root / "Source/DarkArisen/Components"
            components.mkdir(parents=True)
            (components / "HeatExposureComponent.h").write_text(
                "void OnHeatChanged();", encoding="utf-8")
            (components / "HeatExposureComponent.cpp").write_text("", encoding="utf-8")
            findings = validate(root)
            self.assertTrue(any("OnHeatChanged" in finding for finding in findings))

    def test_cenote_music_cue_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            dungeons = root / "Source/DarkArisen/Dungeons"
            dungeons.mkdir(parents=True)
            (dungeons / "CenoteFirstMotherComponent.h").write_text(
                "class UMusicCue;", encoding="utf-8")
            (dungeons / "CenoteFirstMotherComponent.cpp").write_text("", encoding="utf-8")
            findings = validate(root)
            self.assertTrue(any("MusicCue" in finding for finding in findings))

    def test_radiant_rexa_mission_generator_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            missions = root / "Source/DarkArisen/Missions"
            missions.mkdir(parents=True)
            (missions / "RexaM2MissionCatalog.h").write_text(
                "void GenerateRadiantQuest();", encoding="utf-8")
            (missions / "RexaM2MissionCatalog.cpp").write_text("", encoding="utf-8")
            findings = validate(root)
            self.assertTrue(any("GenerateRadiantQuest" in finding for finding in findings))

    def test_physical_journal_objective_checkbox_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            interaction = root / "Source/DarkArisen/Interaction"
            interaction.mkdir(parents=True)
            (interaction / "PhysicalJournalActor.h").write_text(
                "bool ObjectiveCheckbox;", encoding="utf-8")
            (interaction / "PhysicalJournalActor.cpp").write_text("", encoding="utf-8")
            findings = validate(root)
            self.assertTrue(any("ObjectiveCheckbox" in finding for finding in findings))


if __name__ == "__main__":
    unittest.main()

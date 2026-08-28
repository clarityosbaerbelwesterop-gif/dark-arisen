#!/usr/bin/env python3

from __future__ import annotations

import json
import shutil
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from validate_alpha_platforms import REQUIRED_FILES, validate


class AlphaPlatformValidationTests(unittest.TestCase):
    def setUp(self) -> None:
        self.root = Path(__file__).resolve().parents[2]

    def _copy_contract(self, destination: Path) -> None:
        for relative in REQUIRED_FILES:
            source = self.root / relative
            target = destination / relative
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source, target)

    def test_checked_in_project_satisfies_contract(self) -> None:
        self.assertEqual(validate(self.root), [])

    def test_missing_linux_packager_fails_closed(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            copy = Path(temporary)
            self._copy_contract(copy)
            (copy / "Tools/ci/package-alpha-linux.sh").unlink()
            findings = validate(copy)
            self.assertTrue(any("package-alpha-linux.sh" in item for item in findings))

    def test_public_actions_artifact_upload_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            copy = Path(temporary)
            self._copy_contract(copy)
            workflow = copy / ".github/workflows/ci.yml"
            workflow.write_text(
                workflow.read_text(encoding="utf-8")
                + "\n# forbidden\n# uses: actions/upload-artifact@v4\n",
                encoding="utf-8",
            )
            findings = validate(copy)
            self.assertTrue(any("uploaded" in item for item in findings))

    def test_pixel_streaming_ps5_alpha_target_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            copy = Path(temporary)
            self._copy_contract(copy)
            project_path = copy / "DarkArisen.uproject"
            project = json.loads(project_path.read_text(encoding="utf-8"))
            project["Plugins"][0]["TargetAllowList"].append("PS5")
            project_path.write_text(json.dumps(project), encoding="utf-8")
            findings = validate(copy)
            self.assertTrue(any("exactly Win64 and Linux" in item for item in findings))

    def test_ps5_gate_inside_alpha_m8_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            copy = Path(temporary)
            self._copy_contract(copy)
            checklist = copy / "Docs/ALPHA_DELIVERY_CHECKLIST.md"
            text = checklist.read_text(encoding="utf-8")
            text = text.replace(
                "## Post-Alpha / Beta — native PS5",
                "- [ ] PS5 dev-kit Alpha gate.\n\n## Post-Alpha / Beta — native PS5",
                1,
            )
            checklist.write_text(text, encoding="utf-8")
            findings = validate(copy)
            self.assertTrue(any("forbidden inside Alpha M8" in item for item in findings))


if __name__ == "__main__":
    unittest.main()

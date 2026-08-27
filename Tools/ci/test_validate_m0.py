#!/usr/bin/env python3

from __future__ import annotations

import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from validate_m0 import validate


class M0ValidationTests(unittest.TestCase):
    def test_checked_in_project_satisfies_contract(self) -> None:
        root = Path(__file__).resolve().parents[2]
        self.assertEqual(validate(root), [])

    def test_empty_repository_fails_closed(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            findings = validate(Path(temporary))
            self.assertTrue(findings)
            self.assertTrue(any("DarkArisen.uproject" in finding for finding in findings))


if __name__ == "__main__":
    unittest.main()

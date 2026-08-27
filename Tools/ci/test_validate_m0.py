#!/usr/bin/env python3

from __future__ import annotations

import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from validate_m0 import validate, validate_private_aws_text


class M0ValidationTests(unittest.TestCase):
    def test_checked_in_project_satisfies_contract(self) -> None:
        root = Path(__file__).resolve().parents[2]
        self.assertEqual(validate(root), [])

    def test_empty_repository_fails_closed(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            findings = validate(Path(temporary))
            self.assertTrue(findings)
            self.assertTrue(any("DarkArisen.uproject" in finding for finding in findings))

    def test_private_aws_contract_rejects_public_rdp(self) -> None:
        root = Path(__file__).resolve().parents[2]
        template = (root / "Tools/streaming/aws/private-single-player.yaml").read_text(encoding="utf-8")
        policy = (root / "Tools/streaming/aws/tailnet-policy.example.hujson").read_text(encoding="utf-8")
        join_script = (root / "Tools/streaming/aws/join-private-tailnet.ps1").read_text(encoding="utf-8")
        install_services = (root / "Tools/streaming/install-services.ps1").read_text(encoding="utf-8")

        unsafe = template.replace("FromPort: 41641", "FromPort: 3389", 1)
        findings = validate_private_aws_text(unsafe, policy, join_script, install_services)
        self.assertTrue(any("3389" in finding for finding in findings))

    def test_private_aws_contract_rejects_tailnet_wildcard(self) -> None:
        root = Path(__file__).resolve().parents[2]
        template = (root / "Tools/streaming/aws/private-single-player.yaml").read_text(encoding="utf-8")
        policy = (root / "Tools/streaming/aws/tailnet-policy.example.hujson").read_text(encoding="utf-8")
        join_script = (root / "Tools/streaming/aws/join-private-tailnet.ps1").read_text(encoding="utf-8")
        install_services = (root / "Tools/streaming/install-services.ps1").read_text(encoding="utf-8")

        unsafe = policy.replace(
            '"src": ["FLO_TAILSCALE_LOGIN@example.invalid"]',
            '"src": ["*"]',
            1,
        )
        findings = validate_private_aws_text(template, unsafe, join_script, install_services)
        self.assertTrue(any("wildcard" in finding for finding in findings))

    def test_private_aws_contract_rejects_broad_tailnet_identity(self) -> None:
        root = Path(__file__).resolve().parents[2]
        template = (root / "Tools/streaming/aws/private-single-player.yaml").read_text(encoding="utf-8")
        policy = (root / "Tools/streaming/aws/tailnet-policy.example.hujson").read_text(encoding="utf-8")
        join_script = (root / "Tools/streaming/aws/join-private-tailnet.ps1").read_text(encoding="utf-8")
        install_services = (root / "Tools/streaming/install-services.ps1").read_text(encoding="utf-8")

        unsafe = policy.replace(
            '"src": ["FLO_TAILSCALE_LOGIN@example.invalid"]',
            '"src": ["autogroup:member"]',
            1,
        )
        findings = validate_private_aws_text(template, unsafe, join_script, install_services)
        self.assertTrue(any("identity" in finding for finding in findings))

    def test_private_aws_contract_rejects_funnel_enablement(self) -> None:
        root = Path(__file__).resolve().parents[2]
        template = (root / "Tools/streaming/aws/private-single-player.yaml").read_text(encoding="utf-8")
        policy = (root / "Tools/streaming/aws/tailnet-policy.example.hujson").read_text(encoding="utf-8")
        join_script = (root / "Tools/streaming/aws/join-private-tailnet.ps1").read_text(encoding="utf-8")
        install_services = (root / "Tools/streaming/install-services.ps1").read_text(encoding="utf-8")

        unsafe = join_script + "\n& $TailscaleExe funnel --bg 8080\n"
        findings = validate_private_aws_text(template, policy, unsafe, install_services)
        self.assertTrue(any("Funnel" in finding for finding in findings))


if __name__ == "__main__":
    unittest.main()

@{
    # Non-secret machine paths. Copy to config.local.psd1 on the streaming host.
    EngineRoot = "C:\Program Files\Epic Games\UE_5.5"
    InfrastructureRoot = "C:\DarkArisen\PixelStreamingInfrastructure"
    DeploymentRoot = "C:\DarkArisen\Deployments"
    RuntimeRoot = "C:\ProgramData\DarkArisen"
    # Put RuntimeRoot/UserData on persistent provider storage. Never use local
    # instance-store/NVMe for saves because it is erased when some VMs stop.
    NssmPath = "C:\Tools\nssm\win64\nssm.exe"
    # Caddy is needed only for the legacy public-password provider path.
    CaddyPath = "C:\Tools\caddy\caddy.exe"

    # Provider shutdown adapter. It must deallocate/stop billing, not merely shut down Windows.
    ShutdownCommand = "D:\DarkArisen\Project\Tools\streaming\providers\aws-stop-instance.ps1"
}

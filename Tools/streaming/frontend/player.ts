// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

export * from '@epicgames-ps/lib-pixelstreamingfrontend-ue5.5';
export * from '@epicgames-ps/lib-pixelstreamingfrontend-ui-ue5.5';

import {
    Config,
    Flags,
    LogLevel,
    Logger,
    NumericParameters,
    PixelStreaming
} from '@epicgames-ps/lib-pixelstreamingfrontend-ue5.5';
import {
    Application,
    PixelStreamingApplicationStyle,
    UIElementCreationMode
} from '@epicgames-ps/lib-pixelstreamingfrontend-ui-ue5.5';

declare global {
    interface Window { pixelStreaming: PixelStreaming; }
}

document.body.onload = () => {
    Logger.InitLogging(LogLevel.Warning, true);

    const config = new Config({ useUrlParams: false });
    config.setFlagEnabled(Flags.TouchInput, true);
    config.setFlagEnabled(Flags.GamepadInput, true);
    config.setFlagEnabled(Flags.MouseInput, true);
    config.setFlagEnabled(Flags.KeyboardInput, true);
    config.setFlagEnabled(Flags.FakeMouseWithTouches, false);
    config.setFlagEnabled(Flags.AFKDetection, true);
    config.setNumericSetting(NumericParameters.AFKTimeoutSecs, 1800);
    config.setNumericSetting(NumericParameters.AFKCountdownSecs, 30);

    const stream = new PixelStreaming(config);
    const styles = new PixelStreamingApplicationStyle();
    styles.applyStyleSheet();

    const disabledElement = { creationMode: UIElementCreationMode.Disable };
    const application = new Application({
        stream,
        settingsPanelConfig: { isEnabled: false, visibilityButtonConfig: disabledElement },
        statsPanelConfig: { isEnabled: false, visibilityButtonConfig: disabledElement },
        xrControlsConfig: disabledElement,
        videoQpIndicatorConfig: { disableIndicator: true },
        hideControlsInFullscreen: false
    });
    document.body.prepend(application.rootElement);
    window.pixelStreaming = stream;

    const latencyBadge = document.getElementById('latency-badge');
    let latencyTimer: number | undefined;

    stream.addEventListener('latencyTestResult', (event) => {
        const milliseconds = event.data.latencyTimings.endToEndLatency;
        if (latencyBadge && Number.isFinite(milliseconds)) {
            latencyBadge.textContent = `Latency ${Math.round(milliseconds)} ms`;
            latencyBadge.style.color = milliseconds < 120 ? '#b9e6c5' : '#f3bf87';
        }
    });

    stream.addEventListener('webRtcConnected', () => {
        window.clearInterval(latencyTimer);
        window.setTimeout(() => stream.requestLatencyTest(), 1500);
        latencyTimer = window.setInterval(() => stream.requestLatencyTest(), 5000);
    });

    stream.addEventListener('webRtcDisconnected', () => {
        window.clearInterval(latencyTimer);
        latencyTimer = undefined;
        if (latencyBadge) {
            latencyBadge.textContent = 'Latency —';
            latencyBadge.style.color = '#f2f3f4';
        }
    });

    installTouchIndicator();
};

function installTouchIndicator(): void {
    const base = document.getElementById('touch-base');
    const knob = document.getElementById('touch-knob');
    if (!base || !knob) return;

    let movementIdentifier: number | undefined;
    let originX = 0;
    let originY = 0;
    const radius = 60;

    window.addEventListener('touchstart', (event) => {
        if (movementIdentifier !== undefined) return;
        const touch = Array.from(event.changedTouches).find((candidate) =>
            candidate.clientX < window.innerWidth * 0.5);
        if (!touch) return;

        movementIdentifier = touch.identifier;
        originX = touch.clientX;
        originY = touch.clientY;
        for (const element of [base, knob]) {
            element.style.display = 'block';
            element.style.left = `${originX}px`;
            element.style.top = `${originY}px`;
        }
    }, { passive: true, capture: true });

    window.addEventListener('touchmove', (event) => {
        if (movementIdentifier === undefined) return;
        const touch = Array.from(event.changedTouches).find((candidate) =>
            candidate.identifier === movementIdentifier);
        if (!touch) return;

        const deltaX = touch.clientX - originX;
        const deltaY = touch.clientY - originY;
        const distance = Math.hypot(deltaX, deltaY);
        const scale = distance > radius ? radius / distance : 1;
        knob.style.left = `${originX + deltaX * scale}px`;
        knob.style.top = `${originY + deltaY * scale}px`;
    }, { passive: true, capture: true });

    const release = (event: TouchEvent) => {
        if (movementIdentifier === undefined) return;
        const ended = Array.from(event.changedTouches).some((candidate) =>
            candidate.identifier === movementIdentifier);
        if (!ended) return;
        movementIdentifier = undefined;
        base.style.display = 'none';
        knob.style.display = 'none';
    };
    window.addEventListener('touchend', release, { passive: true, capture: true });
    window.addEventListener('touchcancel', release, { passive: true, capture: true });
}


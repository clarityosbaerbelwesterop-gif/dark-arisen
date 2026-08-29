# Kimi K3 job — animation production coverage

This job reviews the authored animation requirements and produces an implementation/asset backlog. It does **not** claim that an animation asset exists and it does not invent new narrative beats.

## Required work

1. Reconcile the named animation requirements in `animation system.md` against the current native animation state bridge.
2. Separate requirements into locomotion, combat, wound/posture, interaction, mounted, boss/creature and authored performance/cinematic work.
3. Identify which requirements can reuse an existing state, notify, montage slot or gameplay component and which require a new authored asset.
4. For every missing asset, specify the gameplay state/input it must bind to, the minimum readable silhouette/timing requirements and the source that requires it.
5. Protect the six-frame deflection window, eight-frame minimum attack tell, no attack-cancel rule, Katana wound-set exception and all protected playable moments.
6. Produce an editor-production order suitable for UE 5.5: blockout/prototype, gameplay hookup, polish, QA.
7. Flag any narrative/performance detail that is not sourced as `DESIGN-GAP` rather than writing it.

## Writing restriction

Do not create new dialogue, monologues, lore or character beats. Do not rewrite existing lines to sound more polished. Any text suggestions must follow `Docs/WRITING_STYLE_GUARDRAILS.md`.

## Runtime honesty

Do not say an animation, montage, Control Rig, IK setup, retarget, Sequencer shot or mocap take exists unless the supplied repository context proves it. Missing binary assets remain missing production work.

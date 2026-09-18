#!/usr/bin/env bash
set -euo pipefail
command -v higgsfield >/dev/null || { echo "Install official Higgsfield CLI first."; exit 2; }
echo "Higgsfield: $(higgsfield version)"
echo "Account / workspace status:"
higgsfield account || { echo "Authenticate with: higgsfield auth login"; exit 3; }
echo "Animation actions available:"
higgsfield preset list animation-action --query walk --json
higgsfield preset list animation-action --group Fighting --category Punching --json
echo "Live production model schemas:"
higgsfield model get tripo_3d --json
higgsfield model get image_to_3d --json
higgsfield model get 3d_rigging --json
higgsfield model get cinematic_studio_3_0 --json
higgsfield model get mirelo_text_to_audio --json
cat <<'EOF'
Dark Arisen production policy:
- Generate/reference characters consistently before rigging.
- Use image_to_3d or multi_image_to_3d for hero characters; tripo_3d for props.
- Rig/animate only after topology/scale review.
- Cinematic video is reference/previs unless explicitly imported as a movie asset.
- Never replace native UE gameplay/story/combat authority with generated media.
- Check account/workspace cost before a batch. The CLI itself does not prove unlimited generation.
EOF

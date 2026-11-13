/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "item_frame.hpp"

namespace a11y {

int item_side(float dx) {
    if (dx < 0.0f) {
        return -1;
    }
    return dx > 0.0f ? 1 : 0;
}

std::optional<VoiceTargets> item_targets(
    const A11yMatchState& match, float x, float y, int side, const ItemMapping& mapping) {
    VoiceTargets targets;
    targets.gain = 1.0f;
    if (!mapping.relative) {
        targets.pan = stage_pan(match.stage, x, mapping.stage.pan_to_camera);
        targets.pitch = stage_pitch(match.stage, y, mapping.stage.pitch_range);
        return targets;
    }
    if (match.you.out) {
        return std::nullopt;
    }
    targets.pan = relative_pan(x - match.you.x, side, mapping.from_you);
    targets.pitch = relative_pitch(y - match.you.y, mapping.stage.pitch_range, mapping.from_you);
    return targets;
}

}  // namespace a11y

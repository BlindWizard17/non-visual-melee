/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "item_appearance.hpp"
#include "item_names.hpp"
#include "sentences.hpp"
#include <algorithm>

namespace a11y {
namespace {

/* Final Destination's frame as measured (docs/a11y/gameplay-state.md,
 * "Sizes measured"), the preview's stage, with you at its centre. */
A11yMatchState preview_match() {
    A11yMatchState match{};
    match.running = true;
    match.you.can_move = true;
    A11yStageFrame& stage = match.stage;
    stage.blast_left = -246.0f;
    stage.blast_right = 246.0f;
    stage.blast_top = 188.0f;
    stage.blast_bottom = -140.0f;
    stage.camera_left = -170.0f;
    stage.camera_right = 170.0f;
    stage.camera_top = 114.0f;
    stage.camera_bottom = -80.0f;
    return match;
}

/* Where the preview's items appear from you: to the side, about where a
 * Capsule lands a few steps away, and above, about a jump's height. */
constexpr float kPreviewSide = 60.0f;
constexpr float kPreviewAbove = 40.0f;

/* An item's one-shot, or none while it has no place. */
std::optional<VoiceTargets> placed(
    const A11yMatchState& match, const A11yMatchItem& item, const ItemMapping& mapping) {
    return item_targets(match, item.x, item.y, item_side(item.x - match.you.x), mapping);
}

}  // namespace

std::span<const A11yMatchItem> NewItems::tick(const A11yMatchState& match) {
    m_new.clear();
    if (!match.running) {
        return m_new;
    }
    /* Before GO, on its tick and from GAME! on, what is listed is taken as
     * seen. */
    const bool quiet = !m_started || match.decided;
    m_started = m_started || match.you.can_move;
    for (int i = 0; i < match.item_count; i++) {
        const A11yMatchItem& item = match.items[i];
        if (m_seen.insert(item.serial).second && !quiet) {
            m_new.push_back(item);
        }
    }
    return m_new;
}

void NewItems::forget() {
    m_started = false;
    m_seen.clear();
    m_new.clear();
}

std::vector<VoiceTargets> AppearanceCue::tick(const A11yMatchState& match,
    std::span<const A11yMatchItem> appeared, const ItemMapping& mapping) {
    std::vector<VoiceTargets> plays;
    if (!match.running) {
        return plays;
    }
    if (match.paused) {
        for (const A11yMatchItem& item : appeared) {
            m_held.push_back(item.serial);
        }
        return plays;
    }
    const std::span<const A11yMatchItem> listed(match.items, match.item_count);
    for (int serial : m_held) {
        const auto found = std::find_if(listed.begin(), listed.end(),
            [serial](const A11yMatchItem& item) { return item.serial == serial; });
        if (found == listed.end()) {
            continue;
        }
        if (std::optional<VoiceTargets> targets = placed(match, *found, mapping)) {
            plays.push_back(*targets);
        }
    }
    m_held.clear();
    for (const A11yMatchItem& item : appeared) {
        if (std::optional<VoiceTargets> targets = placed(match, item, mapping)) {
            plays.push_back(*targets);
        }
    }
    return plays;
}

std::array<OneShotPlay, 3> appearance_preview(const ItemMapping& mapping) {
    const A11yMatchState match = preview_match();
    const float places[3][2] = {{-kPreviewSide, 0.0f}, {kPreviewSide, 0.0f}, {0.0f, kPreviewAbove}};
    std::array<OneShotPlay, 3> plays{};
    for (std::size_t i = 0; i < plays.size(); i++) {
        const float x = places[i][0];
        const float y = places[i][1];
        plays[i].at = static_cast<double>(i) * kAppearancePreviewGap;
        /* You are never out in the preview, so there is always a place. */
        plays[i].targets =
            item_targets(match, x, y, item_side(x), mapping).value_or(VoiceTargets{});
    }
    return plays;
}

std::string appearance_text(std::span<const A11yMatchItem> appeared, LogOnce& log_once) {
    std::string text;
    for (const A11yMatchItem& item : appeared) {
        std::optional<std::string_view> name = item_name(item.kind);
        if (!name.has_value()) {
            log_once("[a11y] item appearance: no name for item kind " +
                     std::to_string(static_cast<int>(item.kind)));
        }
        add_part(text, name.value_or(kUnknownItem));
    }
    return text;
}

void AppearanceAnnouncement::tick(
    std::span<const A11yMatchItem> appeared, const MatchAnnouncementValues& values) {
    if (appeared.empty() || !values.on || !values.item_appearances) {
        return;
    }
    m_speech.announce(appearance_text(appeared, m_log_once), Mode::queue);
}

}  // namespace a11y

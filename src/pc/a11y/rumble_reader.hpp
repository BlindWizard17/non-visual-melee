/* SPDX-License-Identifier: GPL-3.0-or-later */
/* Rumble (mnvibration.c): a panel per controller, P1 to P4, each showing its
 * rumble setting, or a red cross while the controller is unplugged, and a
 * names list beside them. Every controller acts alone: A on its panel
 * switches its rumble. The opening says each plugged-in controller's panel,
 * the keys and the line at the screen's foot; a controller switching,
 * plugged in or unplugged says its panel, by the screen's label, "P2: Off"
 * or "P2 disconnected" (.scratch/options-screens/spec.md, "Speech:
 * Rumble"). */
#pragma once
#include "leaf_reader.hpp"
#include <array>
#include <string>

namespace a11y {

class RumbleReader final : public LeafReader {
public:
    /* text must outlive this. */
    explicit RumbleReader(MenuText& text) : m_text(text) {}

    std::string opening(const A11yMenuState& state) override;
    std::string frame(const A11yMenuState& state) override;

private:
    void remember(const A11yMenuState& state);

    MenuText& m_text;
    std::array<A11yRumblePort, A11Y_RUMBLE_PORTS> m_ports{};
};

}  // namespace a11y

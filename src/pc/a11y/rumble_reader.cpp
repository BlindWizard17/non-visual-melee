/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "rumble_reader.hpp"
#include "sentences.hpp"
#include <cstddef>
#include <string_view>

namespace a11y {
namespace {

constexpr std::string_view kKeys = "A to turn on or off.";

/* The panel's label as the screen draws it: "P1" to "P4". */
std::string label(std::size_t port) {
    return "P" + std::to_string(port + 1);
}

/* A plugged-in controller's panel: "P1: On". */
std::string panel(std::size_t port, const A11yRumblePort& state) {
    return label(port) + (state.rumble ? ": On" : ": Off");
}

}  // namespace

std::string RumbleReader::opening(const A11yMenuState& state) {
    std::string out;
    for (std::size_t port = 0; port < A11Y_RUMBLE_PORTS; port++) {
        const A11yRumblePort& now = state.rumble.ports[port];
        if (now.connected) {
            add_sentence(out, panel(port, now));
        }
    }
    add_sentence(out, kKeys);
    add_sentence(out, m_text.description(state.rumble.bottom_line, state,
                          "[a11y] menu " + std::to_string(state.menu) + " bottom line"));
    remember(state);
    return out;
}

std::string RumbleReader::frame(const A11yMenuState& state) {
    std::string out;
    for (std::size_t port = 0; port < A11Y_RUMBLE_PORTS; port++) {
        const A11yRumblePort& now = state.rumble.ports[port];
        const A11yRumblePort& before = m_ports[port];
        if (now.connected != before.connected) {
            add_sentence(out, now.connected ? panel(port, now) : label(port) + " disconnected");
        } else if (now.connected && now.rumble != before.rumble) {
            add_sentence(out, panel(port, now));
        }
    }
    remember(state);
    return out;
}

void RumbleReader::remember(const A11yMenuState& state) {
    for (std::size_t port = 0; port < A11Y_RUMBLE_PORTS; port++) {
        m_ports[port] = state.rumble.ports[port];
    }
}

}  // namespace a11y

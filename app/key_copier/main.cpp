/*
 * Key Copier for PortaPack Mayhem.
 * Based on KeyCopier for Flipper Zero, Copyright (c) 2024 zinongli, MIT License (see LICENSE-KeyCopier-MIT).
 * Distributed under GPL-2.0-or-later, like Mayhem.
 */
#include "ui.hpp"
#include "ui_key_copier.hpp"
#include "ui_navigation.hpp"
#include "external_app.hpp"

namespace ui::external_app::key_copier {
void initialize_app(ui::NavigationView& nav) {
    nav.push<KeyCopierView>();
}
}  // namespace ui::external_app::key_copier

extern "C" {

__attribute__((section(".external_app.app_key_copier.application_information"), used)) application_information_t _application_information_key_copier = {
    /*.memory_location = */ (uint8_t*)0x00000000,
    /*.externalAppEntry = */ ui::external_app::key_copier::initialize_app,
    /*.header_version = */ CURRENT_HEADER_VERSION,
    /*.app_version = */ VERSION_MD5,

    /*.app_name = */ "Key Copier",
    /*.bitmap_data = */ {
        0x00,
        0x00,
        0x00,
        0x00,
        0x3C,
        0x00,
        0x42,
        0x00,
        0x42,
        0x00,
        0x42,
        0x00,
        0x3C,
        0x00,
        0x10,
        0x00,
        0x10,
        0x00,
        0x30,
        0x00,
        0x10,
        0x00,
        0x30,
        0x00,
        0x10,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    },
    /*.icon_color = */ ui::Color::orange().v,
    /*.menu_location = */ app_location_t::UTILITIES,
    /*.desired_menu_position = */ -1,

    /*.m4_app_tag = portapack::spi_flash::image_tag_none */ {0, 0, 0, 0},
    /*.m4_app_offset = */ 0x00000000,  // will be filled at compile time
};
}

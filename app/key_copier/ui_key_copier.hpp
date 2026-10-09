/*
 * Key Copier for PortaPack Mayhem - measure the bitting of a key you own.
 * Based on KeyCopier for Flipper Zero, Copyright (c) 2024 zinongli, MIT License (see LICENSE-KeyCopier-MIT).
 * Distributed under GPL-2.0-or-later, like Mayhem.
 */
#pragma once

#include "ui.hpp"
#include "ui_widget.hpp"
#include "ui_painter.hpp"
#include "ui_navigation.hpp"
#include "key_formats.hpp"

using namespace ui;

namespace ui::external_app::key_copier {

class KeyCopierView : public View {
   public:
    KeyCopierView(NavigationView& nav);
    void paint(Painter& painter) override;
    void focus() override;
    bool on_key(const KeyEvent key) override;
    bool on_encoder(const EncoderEvent delta) override;
    std::string title() const override { return "Key Copier"; }

   private:
    static constexpr int kMaxPins = 10;

    void select_format(int index);
    void line(Painter& painter, int x0, int y0, int x1, int y1);
    Point rot_point(int x, int y) const;  // maps drawing coordinates through the current orientation
    // Draws text at local coordinates. In landscape (90/270) the text is rotated with the key so it reads upright when
    // the device is turned on its side.
    void text_at(Painter& painter, int x, int y, Color fg, std::string_view s);
    int rel_depth(int pin) const;  // 0 outside the pin range
    int px(float inches) const;

    NavigationView& nav_;
    int format_index_{0};
    int pin_{0};
    int rot_{0};  // 0..3 = 0/90/180/270 degrees
    int cx_{120}, cy_{170};  // rotation centre
    int depth_[kMaxPins]{};
    Color ink_{Color::black()};
};

}  // namespace ui::external_app::key_copier

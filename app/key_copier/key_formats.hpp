/*
 * Copyright (C) 2026 osahv
 *
 * This file is part of PortaPack.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 *
 * Key Copier is a port of KeyCopier for Flipper Zero (https://github.com/zinongli/KeyCopier),
 * Copyright (c) 2024 zinongli, MIT License (see LICENSE-KeyCopier-MIT in this folder).
 * The key format table and the contour algorithm come from there.
 */

#pragma once
namespace ui::external_app::key_copier {

#define FORMAT_NUM 23

struct KeyFormat {
    const char* manufacturer;
    const char* name;
    int sides;  // 2 = double-sided key
    int stop;   // 2 = draw the stop line at the end of the blade
    float first_pin_inch;
    float pin_increment_inch;
    int pin_num;
    float pin_width_inch;
    float tangent;  // tan((180 - drill_angle) / 2), precomputed
    float elbow_inch;
    float uncut_depth_inch;
    float depth_step_inch;
    int min_depth;
    int max_depth;
    int macs;
    int clearance;
    float last_pin_inch;
};

extern const KeyFormat all_formats[FORMAT_NUM];

}  // namespace ui::external_app::key_copier

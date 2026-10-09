// Key format table ported from zinongli/KeyCopier (MIT) - see NOTICE.
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

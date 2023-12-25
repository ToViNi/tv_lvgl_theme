#pragma once

#include "esphome.h"
#include "lvgl.h"

namespace esphome {
namespace tv_lvgl_theme {

class tvLVGLTheme;
using ThemeCallback  = void(tvLVGLTheme*, lv_theme_t*);
using StyleCallback  = void(tvLVGLTheme*, lv_style_t*);
using ApplyCallback  = void(tvLVGLTheme*,   lv_obj_t*);
using CustomParentCallback = lv_theme_t*(tvLVGLTheme*);

static const char *const TAG = "tv_lvgl_theme";

enum ParentThemeSelection {
    DefaultTheme,
    DarkDefaultTheme,
    MonoTheme,
    DarkMonoTheme,
    BasicTheme,
    CustomTheme,
    NoParentTheme
};

class tvLVGLTheme : public Component {
public:
    void setup() override {initialize_theme();   ESP_LOGCONFIG(TAG, "tvLVGLTheme setup done");}
    void loop () override {}
    float get_setup_priority() const override { return setup_priority::LATE; };// AFTER_CONNECTION

    // Function to apply the theme to the current display
    void set_apply_theme(bool do_it) { apply_ = do_it;};
    void set_force_apply_theme(bool force) { force_apply_ = force;};

    // Function to get the theme pointer
    lv_theme_t *theme() { return &theme_; };

    // Functions to set/get parameters and callbacks
    void set_name(const std::string &value) { name_ = value;};
    std::string get_name(){return name_;};

    void setPrimaryColor_lv(const lv_color_t &value)   { theme_.color_primary   = value; };
    void setSecondaryColor_lv(const lv_color_t &value) { theme_.color_secondary = value; };  
    void setPrimaryColor(const std::uint32_t &value)   { theme_.color_primary   = lv_color_hex(value); };
    void setSecondaryColor(const std::uint32_t &value) { theme_.color_secondary = lv_color_hex(value); };  
    void selectParentTheme(ParentThemeSelection theme) { ParentThemeSel_ = theme; };    
  
    void setupLambda      (ThemeCallback cb) { setup_lambda_callback_       = cb; }
    void afterSetupLambda (ThemeCallback cb) { after_setup_lambda_callback_ = cb; }
    void applyLambda      (ApplyCallback cb) { apply_callback_              = cb; }
    void setCustomCallback(CustomParentCallback cb){custom_parent_callback_ = cb; }
    
    void setParentTheme (lv_theme_t *parent_theme)  {
        if (parent_theme != &theme_) {
            parent_theme_ptr_ = parent_theme;
            if (done_) {initialize_theme();}; // if parent theme is changed after setup, call initialize_theme() again
        } else {
            ESP_LOGW(TAG, "Trying to set parent theme to internal theme, ignoring.");
        }
    };

    void dump() {
        ESP_LOGI(TAG, "Dump");
        if (done_) ESP_LOGI(TAG, "initialize_theme done");
    };

    void initialize_theme() {
        if (!done_) { // call this only once
            if (setup_lambda_callback_) {setup_lambda_callback_(this, &theme_);}
        }

        lv_theme_t *parent_th = new lv_theme_t;
        
        bool dark = (ParentThemeSel_ == DarkDefaultTheme || ParentThemeSel_ == DarkMonoTheme); // Check for dark mode
        switch (ParentThemeSel_) {
        case BasicTheme:
            parent_th = lv_theme_basic_init(NULL);
            break;
        case DefaultTheme:
        case DarkDefaultTheme:
            parent_th = lv_theme_default_init(NULL, theme_.color_primary, theme_.color_secondary, dark, NULL);
            break;
        case DarkMonoTheme:
        case MonoTheme:
            parent_th = lv_theme_mono_init(NULL, dark, NULL);
            break;
        case NoParentTheme:
            parent_th = nullptr;
            break;
        case CustomTheme:
            if (custom_parent_callback_ != nullptr) {
                parent_th = custom_parent_callback_(this);
                if (parent_th == nullptr) ESP_LOGE(TAG, "Parent theme from lambda selected, but lambda returns nullptr!"); else ESP_LOGD(TAG, "Parent theme from lambda");
            } else {
                ESP_LOGE(TAG, "Parent theme from lambda selected, but no lambda function given!");
            }
            break;
        }

        // ESP_LOGD(TAG, "Lambda parent theme ptr %i",parent_th);
        lv_theme_set_parent(&theme_, parent_th);

        theme_.font_small = LV_FONT_DEFAULT;
        theme_.font_normal = LV_FONT_DEFAULT;
        theme_.font_large = LV_FONT_DEFAULT;
        done_ = true;

        // Store a pointer to the class inst in the user_data field
        theme_.user_data = this;        

        // Here we call the user callbacks to set style variables for all widget types

        call_style_callback(cb_style_screen,            style_screen           );
        call_style_callback(cb_style_button,            style_button           );
        call_style_callback(cb_style_button_pressed,    style_button_pressed   );
        call_style_callback(cb_style_button_disabled,   style_button_disabled  );
        call_style_callback(cb_style_button_checked,    style_button_checked   );
        call_style_callback(cb_style_button_focused,    style_button_focused   );
        call_style_callback(cb_style_switch,            style_switch           );
        call_style_callback(cb_style_switch_disabled,   style_switch_disabled  );
        call_style_callback(cb_style_switch_focused,    style_switch_focused   );
        call_style_callback(cb_style_label,             style_label            );
        call_style_callback(cb_style_image,             style_image            );
        call_style_callback(cb_style_list,              style_list             );
        call_style_callback(cb_style_slider,            style_slider           );
        call_style_callback(cb_style_slider_pressed,    style_slider_pressed   );
        call_style_callback(cb_style_slider_disabled,   style_slider_disabled  );
        call_style_callback(cb_style_checkbox,          style_checkbox         );
        call_style_callback(cb_style_checkbox_pressed,  style_checkbox_pressed );
        call_style_callback(cb_style_checkbox_disabled, style_checkbox_disabled);
        call_style_callback(cb_style_checkbox_focused,  style_checkbox_focused );
        call_style_callback(cb_style_checkbox_checked,  style_checkbox_checked );
        call_style_callback(cb_style_dropdown,          style_dropdown         );
        call_style_callback(cb_style_spinner,           style_spinner          );
        call_style_callback(cb_style_chart,             style_chart            );
        call_style_callback(cb_style_bar,               style_bar              );
        call_style_callback(cb_style_table,             style_table            );
        call_style_callback(cb_style_textarea,          style_textarea         );
        
       // Set the style apply callback function for the new theme
        lv_theme_set_apply_cb(&theme_, [](lv_theme_t* th, lv_obj_t* obj) {
            if (th && (th->user_data!=nullptr)) {
                tvLVGLTheme* ci = static_cast<tvLVGLTheme*>(th->user_data);
                if (ci && obj) {
                    // Check if the user data is of the expected type
                    if (dynamic_cast<tvLVGLTheme*>(ci)) { // check LVGL object type and add style if style variable is set in class
                        if      ((ci->cb_style_screen           ) && (lv_obj_get_parent(obj) == NULL))                 lv_obj_add_style(obj, ci->style_screen           ,LV_PART_MAIN     );
                        else if                                      (lv_obj_check_type(obj, &lv_btn_class))           {
                            if  ( ci->cb_style_button_pressed   )                                                      lv_obj_add_style(obj, ci->style_button_pressed   ,LV_STATE_PRESSED );
                            if  ( ci->cb_style_button_disabled  )                                                      lv_obj_add_style(obj, ci->style_button_disabled  ,LV_STATE_DISABLED);
                            if  ( ci->cb_style_button_checked   )                                                      lv_obj_add_style(obj, ci->style_button_checked   ,LV_STATE_CHECKED );
                            if  ( ci->cb_style_button_focused   )                                                      lv_obj_add_style(obj, ci->style_button_focused   ,LV_STATE_FOCUSED );
                            if  ( ci->cb_style_button           )                                                      lv_obj_add_style(obj, ci->style_button           ,0                );
                        }
                        else if                                      (lv_obj_check_type(obj, &lv_switch_class))        {
                            if  ( ci->cb_style_switch_disabled  )                                                      lv_obj_add_style(obj, ci->style_switch_disabled  ,LV_STATE_DISABLED);
                            if  ( ci->cb_style_switch_focused   )                                                      lv_obj_add_style(obj, ci->style_switch_focused   ,LV_STATE_FOCUSED );
                            if  ( ci->cb_style_switch           )                                                      lv_obj_add_style(obj, ci->style_switch           ,0                );
                        }
                        else if                                      (lv_obj_check_type(obj, &lv_slider_class))        {
                            if  ( ci->cb_style_slider_disabled  )                                                      lv_obj_add_style(obj, ci->style_slider_disabled  ,LV_STATE_DISABLED);
                            if  ( ci->cb_style_slider_pressed   )                                                      lv_obj_add_style(obj, ci->style_slider_pressed   ,LV_STATE_PRESSED );
                            if  ( ci->cb_style_slider           )                                                      lv_obj_add_style(obj, ci->style_slider           ,0                );
                        }
                        else if                                      (lv_obj_check_type(obj, &lv_checkbox_class))      {
                            if  ( ci->cb_style_checkbox_pressed )                                                      lv_obj_add_style(obj, ci->style_checkbox_pressed ,LV_STATE_PRESSED );
                            if  ( ci->cb_style_checkbox_disabled)                                                      lv_obj_add_style(obj, ci->style_checkbox_disabled,LV_STATE_DISABLED);
                            if  ( ci->cb_style_checkbox_checked )                                                      lv_obj_add_style(obj, ci->style_checkbox_checked ,LV_STATE_CHECKED );
                            if  ( ci->cb_style_checkbox_focused )                                                      lv_obj_add_style(obj, ci->style_checkbox_focused ,LV_STATE_FOCUSED );
                            if  ( ci->cb_style_checkbox         )                                                      lv_obj_add_style(obj, ci->style_checkbox         ,0                );
                        }
                        else if ((ci->cb_style_bar              ) && (lv_obj_check_type(obj, &lv_bar_class)))          lv_obj_add_style(obj, ci->style_bar              ,0                );
                        else if ((ci->cb_style_image            ) && (lv_obj_check_type(obj, &lv_img_class)))          lv_obj_add_style(obj, ci->style_image            ,0                );
                        else if ((ci->cb_style_list             ) && (lv_obj_check_type(obj, &lv_list_class)))         lv_obj_add_style(obj, ci->style_list             ,0                );
                        else if ((ci->cb_style_chart            ) && (lv_obj_check_type(obj, &lv_chart_class)))        lv_obj_add_style(obj, ci->style_chart            ,0                );
                        else if ((ci->cb_style_table            ) && (lv_obj_check_type(obj, &lv_table_class)))        lv_obj_add_style(obj, ci->style_table            ,0                );
                        else if ((ci->cb_style_label            ) && (lv_obj_check_type(obj, &lv_label_class)))        lv_obj_add_style(obj, ci->style_label            ,0                );
                        else if ((ci->cb_style_spinner          ) && (lv_obj_check_type(obj, &lv_spinner_class)))      lv_obj_add_style(obj, ci->style_spinner          ,0                );
                        else if ((ci->cb_style_textarea         ) && (lv_obj_check_type(obj, &lv_textarea_class)))     lv_obj_add_style(obj, ci->style_textarea         ,0                );
                        else if ((ci->cb_style_dropdown         ) && (lv_obj_check_type(obj, &lv_dropdownlist_class))) lv_obj_add_style(obj, ci->style_dropdown         ,0                );
                        if (ci->apply_callback_) ci->apply_callback_(ci, obj);// Custom add on 
                        // if (lv_obj_get_parent(obj) == NULL) ESP_LOGD(TAG, "SCREEN!"); else ESP_LOGD(TAG, "other...");
                    };
                }
            }
        });

        if (after_setup_lambda_callback_) {after_setup_lambda_callback_(this, &theme_);}
        if (apply_) {
            lv_disp_set_theme(lv_disp_get_default(), &theme_);
            lv_obj_remove_style_all(lv_scr_act());
            ESP_LOGD(TAG, "Apply theme");
            if (force_apply_) {
                apply_theme(&theme_, lv_scr_act());
                for (lv_obj_t *obj = lv_scr_act(); obj != NULL; obj = obj->parent) recursive_theme_update(obj);
            } else {
                apply_theme(&theme_,lv_scr_act());
            }
            lv_obj_report_style_change(NULL);
            lv_obj_invalidate(lv_scr_act());
        };
    };

    // function to extact color from lambda, not for end user
    inline lv_color_t handleColorLambda(std::function<lv_color_t()> lambda) {lv_color_t color = lambda(); return color;}

    // Style callback pointers for various widget types
    void (*cb_style_screen               )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_button               )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_button_pressed       )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_button_disabled      )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_button_checked       )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_button_focused       )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_switch               )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_switch_disabled      )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_switch_focused       )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_label                )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_image                )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_list                 )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_slider               )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_slider_pressed       )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_slider_disabled      )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_checkbox             )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_checkbox_pressed     )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_checkbox_disabled    )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_checkbox_focused     )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_checkbox_checked     )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_dropdown             )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_spinner              )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_chart                )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_bar                  )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_table                )(tvLVGLTheme *, lv_style_t *) = nullptr;
    void (*cb_style_textarea             )(tvLVGLTheme *, lv_style_t *) = nullptr;

    // Style variables for various widget types, should be private, but must be public for c callback.
    lv_style_t* style_screen           = new lv_style_t;
    lv_style_t* style_button           = new lv_style_t;
    lv_style_t* style_button_pressed   = new lv_style_t;
    lv_style_t* style_button_disabled  = new lv_style_t;
    lv_style_t* style_button_checked   = new lv_style_t;
    lv_style_t* style_button_focused   = new lv_style_t;
    lv_style_t* style_switch           = new lv_style_t;
    lv_style_t* style_switch_disabled  = new lv_style_t;
    lv_style_t* style_switch_focused   = new lv_style_t;
    lv_style_t* style_label            = new lv_style_t;
    lv_style_t* style_image            = new lv_style_t;
    lv_style_t* style_list             = new lv_style_t;
    lv_style_t* style_slider           = new lv_style_t;
    lv_style_t* style_slider_pressed   = new lv_style_t;
    lv_style_t* style_slider_disabled  = new lv_style_t;
    lv_style_t* style_checkbox         = new lv_style_t;
    lv_style_t* style_checkbox_pressed = new lv_style_t;
    lv_style_t* style_checkbox_disabled= new lv_style_t;
    lv_style_t* style_checkbox_focused = new lv_style_t;
    lv_style_t* style_checkbox_checked = new lv_style_t;
    lv_style_t* style_dropdown         = new lv_style_t;
    lv_style_t* style_spinner          = new lv_style_t;
    lv_style_t* style_chart            = new lv_style_t;
    lv_style_t* style_bar              = new lv_style_t;
    lv_style_t* style_table            = new lv_style_t;
    lv_style_t* style_textarea         = new lv_style_t;

private:

    static void apply_theme(lv_theme_t * th, lv_obj_t * obj)
    {
        if(th->parent) apply_theme(th->parent, obj);
        if(th->apply_cb) th->apply_cb(th, obj);
    }

    void recursive_theme_update(lv_obj_t *obj) {
        for (uint32_t i = 0; i < lv_obj_get_child_cnt(obj); i++) {
            lv_obj_t *child = lv_obj_get_child(obj, i);
            apply_theme(&theme_, child);
            if (child->parent != NULL) { recursive_theme_update(child); }
        }
    }

    void call_style_callback(StyleCallback callback_var, lv_style_t* style_var) {
        // Call callback if it exists
        if (callback_var != nullptr) {
            lv_style_init(style_var); // reset the style
            callback_var(this, style_var);// Pass the address of the style pointer to the callback
        }
    }

    // Resulting theme
    lv_theme_t theme_ = *new lv_theme_t; 

    // Private variables
    std::string name_;
    bool done_  = false;
    bool apply_ = false;
    bool force_apply_ = false;
    ParentThemeSelection ParentThemeSel_ = DefaultTheme;

    // Pointer to parent theme from yaml code if set.  
    lv_theme_t *parent_theme_ptr_ = nullptr;

    // Pointers for lambda callbacks
    void (*setup_lambda_callback_        )(tvLVGLTheme*, lv_theme_t*) = nullptr;
    void (*after_setup_lambda_callback_  )(tvLVGLTheme*, lv_theme_t*) = nullptr;
    void (*apply_callback_               )(tvLVGLTheme*,   lv_obj_t*) = nullptr;
    lv_theme_t* (*custom_parent_callback_)(tvLVGLTheme*) = nullptr;

};

}  // namespace tvLVGLTheme
}  // namespace esphome

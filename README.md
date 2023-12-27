#tv_lvgl_theme Esphome Component

Release 0.0.1

The `tv_lvgl_theme` component is a flexible tool for customizing the visual appearance of LVGL objects in ESPHome. It allows you to create themes that define the colors, fonts, and sizes of various elements on the screen, buttons, and labels and other widgets.
This allows you to create a consistent and visually appealing user interface for your ESPHome projects.

### Usage/Purpose

The `tv_lvgl_theme` component is used to create themes that are applied to LVGL objects.

### Parameters

| Parameter | Description |
|---|---|
| **`id`** | Component id. |
| **`name`** | The name of the theme. |
| **`primary_color`** | The color of the primary theme color. For you own usage. Can be given as a lv_color_t value, or a uint32_t |
| **`secondary_color`** | The color of the secondary theme color. For you own usage. Can be given as a lv_color_t value, or a uint32_t |
| **`parent_theme`** | The id of a parent tv_lvgl_theme to inherit from, or lambda code that returns pointer to a lv_theme_t. |
| **`apply_theme`** | Whether to apply the theme immediately. Can be *true* or *false*.  If it's true, it will also force update all existing objects.  Instead you can use these sub items:
|| **`apply`**  Whether to apply at setup. Only screen/display changed if force is false|
|| **`force`**  Whether to force the theme to be applied to all object now. |
| **`select_parent_theme`**| This select the parent theme. Options: |
|| **`DefaultTheme`**  Sets the parent theme to the default theme. |
|| **`DarkDefaultTheme`**  Sets the parent theme to the dark default theme. |
|| **`MonoTheme`**  Sets the parent theme to the mono theme. |
|| **`DarkMonoTheme`**  Sets the parent theme to the dark mono theme. |
|| **`BasicTheme`**  Sets the parent theme to the basic theme. |
|| **`CustomTheme`**  Sets the parent theme to a custom theme spesified by parent_theme parameter. |
|| **`NoParentTheme`**  No parent theme will be used. |
## Style lambdas

This table lists all the available style lambdas for customizing the appearance of widgets in LVGL. Each lambda has a default state and can also be customized for different states such as pressed, disabled, and focused.

| Style lambda name | Description |
|---|---|
| `style_screen` | Customizes the overall appearance of the screen. |
| `style_button` | Customizes the appearance of buttons. |
| `style_button_disabled` | Customizes the appearance of disabled buttons. |
| `style_button_pressed` | Customizes the appearance of buttons when pressed. |
| `style_button_checked` | Customizes the appearance of buttons when checked. |
| `style_button_focused` | Customizes the appearance of buttons when focused. |
| `style_slider` | Customizes the appearance of sliders. |
| `style_slider_pressed` | Customizes the appearance of sliders when pressed. |
| `style_slider_disabled` | Customizes the appearance of disabled sliders. |
| `style_checkbox` | Customizes the appearance of checkboxes. |
| `style_checkbox_pressed` | Customizes the appearance of checkboxes when pressed. |
| `style_switch` | Customizes the appearance of switches. |
| `style_switch_pressed` | Customizes the appearance of switches when pressed. |
| `style_switch_disabled` | Customizes the appearance of disabled switches. |
| `style_switch_focused` | Customizes the appearance of switches when focused. |
| `style_spinner` | Customizes the appearance of spinners. |
| `style_spinner_pressed` | Customizes the appearance of spinners when pressed. |
| `style_spinner_disabled` | Customizes the appearance of disabled spinners. |
| `style_spinner_focused` | Customizes the appearance of spinners when focused. |
| `style_label` | Customizes the appearance of labels. |
| `style_img` | Customizes the appearance of images. |
| `style_list` | Customizes the appearance of lists. |
| `style_chart` | Customizes the appearance of charts. |
| `style_bar` | Customizes the appearance of bars. |
| `style_table` | Customizes the appearance of tables. |
| `style_dropdown` | Customizes the appearance of dropdown menus. |
| `style_textarea` | Customizes the appearance of textareas. |

**Example:**

This example shows how to use the `style_screen` lambda to customize the background color and text color:

```yaml
style_screen: !lambda |-
    lv_style_set_text_color(style, lv_color_hex(0x555555));
    lv_style_set_bg_color(style, lv_color_hex(0x222222));
    lv_style_set_bg_opa(style, LV_OPA_COVER);
```

**Lambda callbacks:**

| Parameter | Description | Arguments |
|---|---|---|
| **`setup_lambda`** | A lambda that is called when the theme is first created. | `sender` a pointer to the tv_lvgl_theme object. `theme` is a pointer to the lv_theme_t object. |
| **`after_setup_lambda`** | A lambda that is called after the theme has been changed/updated. | `sender` a pointer to the tv_lvgl_theme object. `theme` is a pointer to the lv_theme_t object. |
| **`apply_lambda`** | A lambda that is called to apply the theme to all objects. Here you can add code to change styles not directly supported by the component. | `sender` a pointer to the tv_lvgl_theme object. `obj` pointer the lvgl object to modify. |

**Callable c++ (lambda) functions:**

| Parameter | Description | Argument |
|---|---|---|
| **`setPrimaryColor_lv`** | Call this to modify primary color. | lv_color_t |
| **`setPrimaryColor`** | Call this to modify primary color. | uint32_t |
| **`setSecondaryColor_lv`** | Call this to modify primary color. | lv_color_t |
| **`setSecondaryColor`** | Call this to modify primary color. | uint32_t |
| **`selectParentTheme`** | Select parent theme type | Se select_parent_theme parameter. |
| **`setParentTheme`** | Set the user theme. | lv_theme_t |
| **`initialize_theme`** | Initialize/reinitalize theme. | void |
| **`set_apply_theme`** | Make initialize apply the theme. | bool |
| **`set_force_apply_theme`** | When apply, force update to all current objects | bool |


**Complete usage example:**

```yaml
tv_lvgl_theme:
  - id: theme1
    name: "Theme 1"
    select_parent_theme: DefaultTheme
    style_button: !lambda |-
      lv_style_set_border_width(style, 5);
      lv_style_set_text_color(style, lv_color_hex(0xCFCFFF));

  - id: theme2
    name: Theme 2
    primary_color: !lambda |-
      return lv_color_hex(0x123456);
    secondary_color: 0x0000ff
    setup_lambda: !lambda |-
      ESP_LOGD("test", "setup_lambda");
    after_setup_lambda: !lambda |-
      ESP_LOGD("test", "after_setup_lambda");
    style_button_checked: !lambda |-
      lv_style_set_bg_color(style, lv_color_hex(0x888822));
    style_label: !lambda |-
      lv_style_set_bg_opa(style, LV_OPA_0);
    apply_theme: 
      apply: true
      force: true
    select_parent_theme: CustomTheme
    parent_theme: !lambda |-
        return id(theme1).theme();

  - id: theme3
    parent_theme: theme2
```
*Written by ToViNi during many dedicated nights, navigating the strange intricacies of Python and C++, learning by the way.*
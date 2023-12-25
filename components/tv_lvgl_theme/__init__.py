import esphome.codegen as cg
from esphome.codegen import process_lambda
import re
import esphome.config_validation as cv
from enum import Enum
from esphome import core
from esphome.const import CONF_COMPONENTS, CONF_ID, CONF_LAMBDA
from esphome.core import  CORE, Lambda

MULTI_CONF = True

tv_lvgl_theme_ns = cg.esphome_ns.namespace('tv_lvgl_theme')
tv_lvgl_theme = tv_lvgl_theme_ns.class_('tvLVGLTheme', cg.Component)

# parameters
PRIMARY_COLOR = 'primary_color'
SECONDARY_COLOR = 'secondary_color'
PARENT_THEME = 'parent_theme'
APPLY_THEME = 'apply_theme'
SETUP_LAMBDA = 'setup_lambda'
APPLY_LAMBDA = 'apply_lambda'
AFTER_SETUP_LAMBDA = 'after_setup_lambda'
SELECT_PARENT_THEME = 'select_parent_theme'
NAME = 'name'
APPLY = 'apply'
FORCE = 'force'

# style variables
STYLE_BUTTON = 'style_button'
STYLE_BUTTON_DISABLED = 'style_button_disabled'
STYLE_BUTTON_PRESSED = 'style_button_pressed'
STYLE_BUTTON_CHECKED = 'style_button_checked'
STYLE_BUTTON_FOCUSED = 'style_button_focused'
STYLE_SLIDER = 'style_slider'
STYLE_SLIDER_PRESSED = 'style_slider_pressed'
STYLE_SLIDER_DISABLED = 'style_slider_disabled'
STYLE_CHECKBOX = 'style_checkbox'
STYLE_CHECKBOX_PRESSED = 'style_checkbox_pressed'
STYLE_CHECKBOX_DISABLED = 'style_checkbox_disabled'
STYLE_CHECKBOX_FOCUSED = 'style_checkbox_focused'
STYLE_CHECKBOX_CHECKED = 'style_checkbox_checked'
STYLE_SPINNER = 'style_spinner'
STYLE_SWITCH = 'style_switch'
STYLE_SWITCH_DISABLED = 'style_switch_disabled'
STYLE_SWITCH_FOCUSED = 'style_switch_focused'
# only default state
STYLE_SCREEN = 'style_screen'
STYLE_LABEL = 'style_label'
STYLE_IMG = 'style_img'
STYLE_LIST = 'style_list'
STYLE_CHART = 'style_chart'
STYLE_BAR = 'style_bar'
STYLE_TABLE = 'style_table'
STYLE_DROPDOWN = 'style_dropdown'
STYLE_TEXTAREA = 'style_textarea'


style_names = [
    STYLE_SCREEN,
    STYLE_BUTTON,
    STYLE_BUTTON_DISABLED,
    STYLE_BUTTON_PRESSED,
    STYLE_BUTTON_CHECKED,
    STYLE_BUTTON_FOCUSED,
    STYLE_LABEL,
    STYLE_IMG,
    STYLE_LIST,
    STYLE_SLIDER,
    STYLE_SLIDER_PRESSED,
    STYLE_SLIDER_DISABLED,
    STYLE_CHECKBOX,
    STYLE_CHECKBOX_PRESSED,
    STYLE_CHECKBOX_DISABLED,
    STYLE_CHECKBOX_FOCUSED,
    STYLE_CHECKBOX_CHECKED,
    STYLE_DROPDOWN,
    STYLE_SPINNER,
    STYLE_CHART,
    STYLE_BAR,
    STYLE_TABLE,
    STYLE_TEXTAREA,
    STYLE_SWITCH,
    STYLE_SWITCH_DISABLED,
    STYLE_SWITCH_FOCUSED,
]

class ParentTheme(Enum):
    DefaultTheme     = str('DefaultTheme'    )
    DarkDefaultTheme = str('DarkDefaultTheme')
    MonoTheme        = str('MonoTheme'       )
    DarkMonoTheme    = str('DarkMonoTheme'   )
    BasicTheme       = str('BasicTheme'      )
    CustomTheme      = str('CustomTheme'     )
    NoParentTheme    = str('NoParentTheme'   )

def id_or_str(value):
    if cv.use_id(tv_lvgl_theme)  or isinstance(value, isinstance(value, str)): return value
    raise cv.Invalid("Value must be either an theme id or a lv_theme reference.")

def select_parent_theme_enum_str():
    all_values = []
    for enum_val in ParentTheme:
        all_values.append(enum_val.value)
    return ','.join(all_values)

def select_parent_theme(value):
    for theme in ParentTheme:
        if value == theme.name: return value
    raise cv.Invalid("Invalid select_parent_theme value: {}, possible values: {}".format(value, select_parent_theme_enum_str()))

# Create the schema entries dynamically for style variables
style_entries = {cv.Optional(style): cv.lambda_ for style in style_names}

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(tv_lvgl_theme),
    cv.Optional(NAME               ): cv.string,
    cv.Optional(PRIMARY_COLOR      ): cv.Any(cv.uint32_t, cv.lambda_),
    cv.Optional(SECONDARY_COLOR    ): cv.Any(cv.uint32_t, cv.lambda_),
    cv.Optional(PARENT_THEME       ): cv.Any(cv.lambda_, cv.string),
    cv.Optional(SELECT_PARENT_THEME): select_parent_theme,
    cv.Optional(APPLY_THEME        ): cv.Any(
        cv.boolean,
        cv.Schema({
            cv.Optional(APPLY      ): cv.boolean,
            cv.Optional(FORCE      ): cv.boolean,
        })
    ),
    cv.Optional(SETUP_LAMBDA       ): cv.lambda_,
    cv.Optional(AFTER_SETUP_LAMBDA ): cv.lambda_,
    cv.Optional(APPLY_LAMBDA       ): cv.lambda_,
    **style_entries  # Include style variables dynamically
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if NAME in config: cg.add(var.set_name(config[NAME]))

    if PRIMARY_COLOR in config: 
        pc = config[PRIMARY_COLOR]
        if isinstance(pc, int):
            cg.add(var.setPrimaryColor(pc))
        else:
            la = await cg.process_lambda(pc, [], '', cg.esphome_ns.namespace('LVGL_H').lv_color_t) 
            cg.add(var.setPrimaryColor_lv(var.handleColorLambda(la)))

    if SECONDARY_COLOR in config: 
        sc = config[SECONDARY_COLOR]
        if isinstance(sc, int):
            cg.add(var.setSecondaryColor(sc))
        else:
            la = await cg.process_lambda(sc, [], '', cg.esphome_ns.namespace('LVGL_H').lv_color_t) 
            cg.add(var.setSecondaryColor_lv(var.handleColorLambda(la)))

    # Add any style lambda from style_names list 
    if any(style_name in config for style_name in style_names):
        for style_name in style_names:        
            if style_name in config:                
                style_template = await cg.process_lambda(config[style_name], [((tv_lvgl_theme), "*sender"), (cg.esphome_ns.namespace('LVGL_H').lv_style_t, '*style')], '')
                output = f'{var}->cb_{style_name}={style_template}'
                cg.add(cg.RawExpression(output))

    if SETUP_LAMBDA in config:
        tp = await cg.process_lambda(config[SETUP_LAMBDA], [((tv_lvgl_theme), "*sender"), (cg.esphome_ns.namespace('LVGL_H').lv_theme_t, "*theme")], '') 
        cg.add(var.setupLambda(tp))

    if AFTER_SETUP_LAMBDA in config:
        tp = await cg.process_lambda(config[AFTER_SETUP_LAMBDA], [((tv_lvgl_theme), "*sender"), (cg.esphome_ns.namespace('LVGL_H').lv_theme_t, "*theme")], '') 
        cg.add(var.afterSetupLambda(tp))

    if APPLY_LAMBDA in config:
        tp = await cg.process_lambda(config[APPLY_LAMBDA], [((tv_lvgl_theme), "*sender"), (cg.esphome_ns.namespace('LVGL_H').lv_obj_t, "*obj")], '') 
        cg.add(var.applyLambda(tp))

    if SELECT_PARENT_THEME in config:
        parent_theme = config[SELECT_PARENT_THEME]
        selstr = f"{tv_lvgl_theme_ns}::{parent_theme}"
        cg.add(var.selectParentTheme(cg.RawExpression(selstr)))

    if PARENT_THEME in config:
        inp = config[PARENT_THEME]
        try:
            parentid = cv.use_id(tv_lvgl_theme)(inp)
            if isinstance(parentid, core.ID):
                # the "string" matches an esphome id and we treat it as an id
                if parent_ == config[CONF_ID]: raise cv.Invalid("parent_id can't point to the same object.")
                # create a lambda that points to the theme of the parent object selected by id
                lambda_ = f"[](tv_lvgl_theme::tvLVGLTheme *sender) -> lv_theme_t* {{ return id({inp}).theme(); }}"
                cg.add(var.setCustomCallback(cg.RawExpression(lambda_)))
        except:        
            # in case there's an exception, assume the parameter was lambda code and process it
            try:
                la = await cg.process_lambda(inp, [((tv_lvgl_theme), "*sender")], '') # Was unable to spesify pointer to theme returned from the lambda.
                cg.add(var.setCustomCallback(la))
            except:
                raise cv.Invalid("As parent_id value, an ID to another tv_lvgl_theme object, or a lambda expression returning a lv_theme_t pointer is expected.")

    if APPLY_THEME in config:
        at_config = config[APPLY_THEME]
        if isinstance(at_config, bool): # simple config, set both at the same time
            cg.add(var.set_apply_theme(at_config))
            cg.add(var.set_force_apply_theme(at_config))
        elif isinstance(at_config, dict): # dict, specify each
            if APPLY in at_config:
                do_it = at_config.get(APPLY)
                cg.add(var.set_apply_theme(do_it))
            if FORCE in at_config:
                force = at_config.get(FORCE)
                cg.add(var.set_force_apply_theme(force))
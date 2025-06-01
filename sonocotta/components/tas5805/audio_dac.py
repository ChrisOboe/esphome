from esphome import automation, pins
import esphome.codegen as cg
from esphome.components import i2c, switch
from esphome.components.audio_dac import AudioDac
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONFG_MODE, CONF_PIN

CODEOWNERS = ["@ChrisOboe"]
DEPENDENCIES = ["i2c"]

tas5805_ns = cg.esphome_ns.namespace("tas5805")
TAS5805 = tas5805_ns.class_("TAS5805", AudioDac, switch.Switch, cg.Component, i2c.i2CDevice)


CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(AIC3204),
            cs.Required(CONF_PIN): pins.gpio_output_pin_schema,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(0x2D))

)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
    await switch.new_switch(config)


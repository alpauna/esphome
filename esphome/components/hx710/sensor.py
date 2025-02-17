import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import sensor
from esphome.const import (
    CONF_CLK_PIN,
    CONF_MODE,
    ICON_SCALE,
    STATE_CLASS_MEASUREMENT,
)

hx710_ns = cg.esphome_ns.namespace("hx710")
HX710Sensor = hx710_ns.class_("HX710Sensor", sensor.Sensor, cg.PollingComponent)

CONF_DOUT_PIN = "dout_pin"

HX710Mode = hx710_ns.enum("HX710Mode")
MODES = {
    1: HX710Mode.HX710_DIFFERENTIAL_INPUT_10HZ,
    2: HX710Mode.HX710_OTHER_INPUT_40HZ,
    3: HX710Mode.HX710_DIFFERENTIAL_INPUT_40HZ,
}

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        HX710Sensor,
        icon=ICON_SCALE,
        accuracy_decimals=0,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend(
        {
            cv.Required(CONF_DOUT_PIN): pins.gpio_input_pin_schema,
            cv.Required(CONF_CLK_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_MODE, default=1): cv.enum(MODES, int=True),
        }
    )
    .extend(cv.polling_component_schema("60s"))
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)

    dout_pin = await cg.gpio_pin_expression(config[CONF_DOUT_PIN])
    cg.add(var.set_dout_pin(dout_pin))
    sck_pin = await cg.gpio_pin_expression(config[CONF_CLK_PIN])
    cg.add(var.set_sck_pin(sck_pin))
    cg.add(var.set_gain(config[CONF_MODE]))

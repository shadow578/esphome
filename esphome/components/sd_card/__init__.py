import esphome.codegen as cg
from esphome.components import spi
import esphome.config_validation as cv

from esphome.const import CONF_ID


CODEOWNERS = ["@shadow578"]
DEPENDENCIES = ["spi"]

CONF_SD_CARD_ID = "sd_card_id"

sd_card_namespace = cg.esphome_ns.namespace("sd_card")

SDCardComponent = sd_card_namespace.class_("SDCardComponent", cg.Component, spi.SPIDevice)

CONFIG_SCHEMA = cv.All(
    cv.Schema({
      cv.GenerateID(): cv.declare_id(SDCardComponent),
  }).extend(spi.spi_device_schema(cs_pin_required=True)),
  cv.only_with_arduino
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)

    # Add library dependencies:
    # SD
    # |-- FS
    # |-- SPI
    cg.add_library("SPI", None)
    cg.add_library("FS", None)
    cg.add_library("SD", None)

sd_card_id_schema = cv.use_id(SDCardComponent)

async def register_sd_card_device(var, config):
    parent = await cg.get_variable(config[CONF_SD_CARD_ID])
    cg.add(var.set_sd_card_parent(parent))

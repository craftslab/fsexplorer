#!/bin/bash

dtctool=$1

$dtctool -p 1024 -I dts -O dtb -o sample.dtb sample.dts \
  -E duplicate_node_names \
  -E duplicate_property_names \
  -E node_name_chars \
  -E node_name_format \
  -E property_name_chars \
  -E name_is_string \
  -E name_properties \
  -E duplicate_label \
  -E explicit_phandles \
  -E phandle_references \
  -E path_references \
  -E address_cells_is_cell \
  -E size_cells_is_cell \
  -E interrupt_cells_is_cell \
  -E device_type_is_string \
  -E model_is_string \
  -E status_is_string \
  -E addr_size_cells \
  -E reg_format \
  -E ranges_format \
  -E avoid_default_addr_size \
  -E obsolete_chosen_interrupt_controller

echo $?

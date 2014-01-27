DTC := dtcc
dtc_cmd = $(shell $(DTC) -p 1024 -I dts -O dtb -o sample.dtb sample.dts)

define dtc-fun
  $(dtc_cmd)
  echo $$?
  if [ $$? != "0" ]; then \
    echo "error occur in dts"; \
    exit; \
  fi
endef

dtc_rc: sample.dts
	@$(dtc-fun)

# 3DE Command Plugin Makefile

BUILD_DIR = build
PLUGIN_NAME = 3decommand

.PHONY: clean build install all

all: clean build install

clean:
	@echo "Cleaning..."
	rm -rf $(BUILD_DIR)

build:
	@echo "Building..."
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. -DCMAKE_INSTALL_PREFIX=/usr
	cd $(BUILD_DIR) && make

install:
	@echo "Installing..."
	cd $(BUILD_DIR) && sudo make install
	@echo "Done! Restart Kate to see changes."


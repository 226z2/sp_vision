RED      := \033[1;31m
GREEN    := \033[1;32m
YELLOW   := \033[1;33m
BLUE     := \033[1;34m
MAGENTA  := \033[1;35m
CYAN     := \033[1;36m
WHITE    := \033[1;37m
RESET    := \033[0m

# 镜像/容器基础参数
IMAGE_NAME      ?= sp_vision_25
TAG             ?= dev_5
CONTAINER_NAME  ?= $(IMAGE_NAME)_dev

DATA_DIR        ?= $(CURDIR)/data
CACHE_DIR       ?= $(CURDIR)/.cache
ROS_CACHE_DIR   ?= $(CACHE_DIR)/ros

# 默认共享宿主网络（WSL2 mirrored / 原生 Linux）
# NAT 模式：将 USE_HOST_NET=no 并在 FORWARD_PORTS 中列出需要映射的端口
USE_HOST_NET    ?= yes
FORWARD_PORTS   ?= 4000 4002 9877

PARALLEL_JOBS   ?= $(shell nproc)

IS_WSL := $(shell uname -r | grep -iE 'WSL|Microsoft')

define map_ports
$(if $(strip $(1)),$(foreach port,$(1),-p $(port):$(port)),)
endef

NETWORK_FLAG := $(if $(filter yes,$(USE_HOST_NET)),--network=host,)
PORT_FLAGS   := $(if $(filter yes,$(USE_HOST_NET)),,$(call map_ports,$(FORWARD_PORTS)))

# 串口设备（宿主通过 udev 建立 /dev/gimbal 别名；如无，可改为 /dev/ttyACM0）
GIMBAL_DEV_HOST ?= /dev/gimbal
GIMBAL_DEV_CONT ?= /dev/gimbal

# 是否启用 GPU 直通
ENABLE_GPU      ?= 0
GPU_FLAGS       :=
ifeq ($(ENABLE_GPU),1)
GPU_FLAGS := -v /dev/dri:/dev/dri
endif

# 特权/能力配置：默认用 --privileged，后续可细化为 --cap-add
DOCKER_PRIVILEGED_FLAGS ?= --privileged

# ------------------------------
# 离线依赖仓库白名单（仅 CMakeLists 需要的库）
# 格式：NAME|URL|TAG|NEEDS_SUBMODULES
# ------------------------------
REPOSITORIES := \
	opencv|https://github.com/opencv/opencv.git|4.11.0|false \
	opencv_contrib|https://github.com/opencv/opencv_contrib.git|4.11.0|false \
	spdlog|https://github.com/gabime/spdlog.git|v1.15.3|false \
	fmt|https://github.com/fmtlib/fmt.git|11.0.2|false \
	json|https://github.com/nlohmann/json.git|v3.12.0|false \
	yaml-cpp|https://github.com/jbeder/yaml-cpp.git|yaml-cpp-0.7.0|false \
	ceres-solver|https://github.com/ceres-solver/ceres-solver.git|2.2.0|false

# 说明：
# - ROS 相关的 ament_package wheel 等离线资源，请放在 $(ROS_CACHE_DIR) 下，
#   Dockerfile 会在构建时从该目录中安装 ament_package（仅使用本地文件，不访问网络）。

# HikRobot SDK：下载后缓存到 .cache/vendor（完全离线）
SDK_URL       := https://www.hikrobotics.com/cn2/source/vision/video/2024/12/23/MvCamCtrlSDK_STD_V4.5.0_241128.zip
SDK_DEB_FILE  := MvCamCtrlSDK_Runtime-4.5.0_x86_64_20241128.deb

# ------------------------------
# 仓库自动规则
# ------------------------------

REPO_NAMES   := $(foreach repo,$(REPOSITORIES),$(word 1,$(subst |, ,$(repo))))
REPO_TARGETS := $(addprefix $(CACHE_DIR)/,$(REPO_NAMES))

define get_repo_info
$(word $(2),$(subst |, ,$(filter $(1)|%,$(REPOSITORIES))))
endef

define CLONE_TEMPLATE
$(CACHE_DIR)/$(1): | $(CACHE_DIR)
	@echo "$(MAGENTA)--> Cloning $(1) $$(call get_repo_info,$(1),3)...$(RESET)"
	@if [ ! -d $$@ ]; then \
		git clone --depth 1 --branch $$(call get_repo_info,$(1),3) $$(call get_repo_info,$(1),2) $$@ && \
		if [ "$$(call get_repo_info,$(1),4)" = "true" ]; then \
			cd $$@ && git submodule update --init --recursive; \
		fi; \
	else \
		echo "$(GREEN)$(1) already cloned: $$@$(RESET)"; \
	fi
endef

$(foreach repo,$(REPO_NAMES),$(eval $(call CLONE_TEMPLATE,$(repo))))

.PHONY: all build build-dev build-prod build-type download download-sdk clone-repos force-download \
	run run-gui bash daemon stop clean clean-cache clean-all info check-deps

all: build-dev

build: build-dev

build-dev: download
	@echo "$(BLUE)--> Building dev image $(IMAGE_NAME):$(TAG)$(RESET)"
	@env -u http_proxy -u https_proxy -u HTTP_PROXY -u HTTPS_PROXY -u no_proxy -u NO_PROXY \
	 docker build \
		$(NETWORK_FLAG) \
		--build-arg PARALLEL_JOBS=$(PARALLEL_JOBS) \
		--build-arg OPENVINO_APT_YEAR=2024 \
		--build-arg ALLOW_NET_FOR_VENDOR=0 \
		--target runtime-dev \
		-t $(IMAGE_NAME):$(TAG) .

build-prod: download
	@echo "$(BLUE)--> Building prod image $(IMAGE_NAME):prod$(RESET)"
	@env -u http_proxy -u https_proxy -u HTTP_PROXY -u HTTPS_PROXY -u no_proxy -u NO_PROXY \
	 docker build \
		$(NETWORK_FLAG) \
		--build-arg PARALLEL_JOBS=$(PARALLEL_JOBS) \
		--build-arg OPENVINO_APT_YEAR=2024 \
		--build-arg ALLOW_NET_FOR_VENDOR=0 \
		--target runtime-prod \
		-t $(IMAGE_NAME):prod .

$(CACHE_DIR):
	@mkdir -p $@

download: $(CACHE_DIR)/.download_complete

$(CACHE_DIR)/.download_complete: makefile | $(CACHE_DIR)
	@echo "$(MAGENTA)--> Downloading all offline deps into .cache ...$(RESET)"
	@$(MAKE) download-sdk clone-repos
	@echo "$(GREEN)✓ Offline deps ready$(RESET)"
	@touch $@

download-sdk: $(CACHE_DIR)/vendor/$(SDK_DEB_FILE)

$(CACHE_DIR)/vendor/$(SDK_DEB_FILE): | $(CACHE_DIR)
	@mkdir -p $(CACHE_DIR)/vendor
	@if [ ! -f $@ ]; then \
		echo "$(MAGENTA)--> Downloading HikRobot SDK package...$(RESET)"; \
		wget -q --timeout=30 --tries=3 -O $(CACHE_DIR)/vendor/hikrobot_sdk.zip "$(SDK_URL)" && \
		unzip -j -o $(CACHE_DIR)/vendor/hikrobot_sdk.zip "*$(SDK_DEB_FILE)" -d $(CACHE_DIR)/vendor/ && \
		rm -f $(CACHE_DIR)/vendor/hikrobot_sdk.zip; \
	else \
		echo "$(GREEN)Hik SDK deb found: $@$(RESET)"; \
	fi

clone-repos: $(REPO_TARGETS)

force-download:
	@echo "$(YELLOW)--> Force re-download offline repos$(RESET)"
	@rm -f $(CACHE_DIR)/.download_complete
	@$(MAKE) download

# ------------------------------
# 容器运行：挂载代码和 USB 相机
# ------------------------------

run: ensure-dirs
	@echo "$(BLUE)--> Running container $(CONTAINER_NAME)$(RESET)"
	@docker run --rm -it \
		$(NETWORK_FLAG) \
		$(PORT_FLAGS) \
		$(DOCKER_PRIVILEGED_FLAGS) \
		-v /dev/bus/usb:/dev/bus/usb \
		$(GPU_FLAGS) \
		--device=$(GIMBAL_DEV_HOST):$(GIMBAL_DEV_CONT) \
		-v $(CURDIR):/app/sp_vision \
		-v $(DATA_DIR):/app/data \
		-w /app/sp_vision \
		--entrypoint bash \
		--name $(CONTAINER_NAME) \
		$(IMAGE_NAME):$(TAG)

run-gui: ensure-dirs
ifeq ($(strip $(IS_WSL)),)
	@echo "$(BLUE)--> Native Linux GUI run$(RESET)"
	@xhost +
	@docker run --rm -it \
		$(NETWORK_FLAG) \
		$(PORT_FLAGS) \
		$(DOCKER_PRIVILEGED_FLAGS) \
		-v /dev/bus/usb:/dev/bus/usb \
		$(GPU_FLAGS) \
		--device=$(GIMBAL_DEV_HOST):$(GIMBAL_DEV_CONT) \
		--ipc=host \
		-v /tmp/.X11-unix:/tmp/.X11-unix \
		-e DISPLAY=$${DISPLAY} \
		-v $${XAUTHORITY:-$$HOME/.Xauthority}:/tmp/.Xauthority:ro \
		-e XAUTHORITY=/tmp/.Xauthority \
		-v $(CURDIR):/app/sp_vision \
		-v $(DATA_DIR):/app/data \
		-w /app/sp_vision \
		--name $(CONTAINER_NAME) \
		$(IMAGE_NAME):$(TAG)
	@xhost -
else
	@echo "$(BLUE)--> WSL2 GUI run (DISPLAY=$${DISPLAY:-127.0.0.1:0.0})$(RESET)"
	@DISPLAY=$${DISPLAY:-127.0.0.1:0.0} 
	@docker run --rm -it \
		$(NETWORK_FLAG) \
		$(PORT_FLAGS) \
		$(DOCKER_PRIVILEGED_FLAGS) \
		-v /dev/bus/usb:/dev/bus/usb \
		$(GPU_FLAGS) \
		--device=$(GIMBAL_DEV_HOST):$(GIMBAL_DEV_CONT) \
		-v /tmp/.X11-unix:/tmp/.X11-unix \
		-e DISPLAY \
		-v $(CURDIR):/app/sp_vision \
		-v $(DATA_DIR):/app/data \
		-w /app/sp_vision \
		--name $(CONTAINER_NAME) \
		$(IMAGE_NAME):$(TAG)
endif

bash: ensure-dirs
	@docker run --rm -it \
		$(NETWORK_FLAG) \
		$(PORT_FLAGS) \
		$(DOCKER_PRIVILEGED_FLAGS) \
		-v /dev/bus/usb:/dev/bus/usb \
		$(GPU_FLAGS) \
		--device=$(GIMBAL_DEV_HOST):$(GIMBAL_DEV_CONT) \
		-v $(CURDIR):/app/sp_vision \
		-v $(DATA_DIR):/app/data \
		-w /app/sp_vision \
		--entrypoint bash \
		--name $(CONTAINER_NAME) \
		$(IMAGE_NAME):$(TAG)

daemon: ensure-dirs
	@docker run -d \
		$(NETWORK_FLAG) \
		$(PORT_FLAGS) \
		--restart unless-stopped \
		$(DOCKER_PRIVILEGED_FLAGS) \
		-v /dev/bus/usb:/dev/bus/usb \
		$(GPU_FLAGS) \
		--device=$(GIMBAL_DEV_HOST):$(GIMBAL_DEV_CONT) \
		-v $(CURDIR):/app/sp_vision \
		-v $(DATA_DIR):/app/data \
		-w /app/sp_vision \
		--name $(CONTAINER_NAME) \
		$(IMAGE_NAME):$(TAG)

stop:
	@docker rm -f $(CONTAINER_NAME) 2>/dev/null || true

clean:
	@$(MAKE) stop
	@rm -rf $(DATA_DIR)
	@docker rmi $(IMAGE_NAME):$(TAG) 2>/dev/null || true

clean-cache:
	@rm -rf $(CACHE_DIR)

clean-all: clean clean-cache

ensure-dirs:
	@mkdir -p $(DATA_DIR)

ACCEPTANCE_CMD ?= mkdir -p build && cd build && cmake .. && make -j$(PARALLEL_JOBS)

acceptance: ensure-dirs
	@echo "$(BLUE)--> Running acceptance flow (cmake .. && make) inside fresh container$(RESET)"
	@docker run --rm \
		$(NETWORK_FLAG) \
		$(PORT_FLAGS) \
		$(DOCKER_PRIVILEGED_FLAGS) \
		-v $(CURDIR):/app/sp_vision \
		-v $(DATA_DIR):/app/data \
		-w /app/sp_vision \
		--name $(CONTAINER_NAME)_accept \
		$(IMAGE_NAME):$(TAG) \
		bash -lc "set -euo pipefail; $(ACCEPTANCE_CMD)"

info:
	@echo "$(CYAN)Image: $(IMAGE_NAME):$(TAG)$(RESET)"
	@echo "$(CYAN)Container: $(CONTAINER_NAME)$(RESET)"
	@echo "$(CYAN)USE_HOST_NET=$(USE_HOST_NET)$(RESET)"
	@echo "$(CYAN)FORWARD_PORTS=$(FORWARD_PORTS)$(RESET)"
	@echo "$(CYAN)DATA_DIR=$(DATA_DIR)$(RESET)"
	@echo "$(CYAN)CACHE_DIR=$(CACHE_DIR)$(RESET)"
	@echo "$(CYAN)Repos: $(REPO_NAMES)$(RESET)"

check-deps:
	@command -v docker >/dev/null 2>&1 && echo "$(GREEN)✓ docker$(RESET)" || echo "$(RED)✗ docker$(RESET)"
	@command -v git >/dev/null 2>&1 && echo "$(GREEN)✓ git$(RESET)" || echo "$(RED)✗ git$(RESET)"
	@command -v wget >/dev/null 2>&1 && echo "$(GREEN)✓ wget$(RESET)" || echo "$(RED)✗ wget$(RESET)"

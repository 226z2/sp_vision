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
FORWARD_PORTS   ?= 4000 4002 9877 8765

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
# 是否在镜像构建阶段安装 OpenVINO GPU runtime（默认跟随 ENABLE_GPU）
ENABLE_GPU_RUNTIME ?= $(ENABLE_GPU)
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

.PHONY: all build build-dev build-prod build-type download clone-repos force-download \
	run run-gui bash daemon stop clean clean-cache clean-all info check-deps

all: build-dev

build: build-dev

build-dev:
	@echo "$(BLUE)--> Building dev image $(IMAGE_NAME):$(TAG)$(RESET)"
	@env -u http_proxy -u https_proxy -u HTTP_PROXY -u HTTPS_PROXY -u no_proxy -u NO_PROXY \
	 docker build \
		$(NETWORK_FLAG) \
		--build-arg PARALLEL_JOBS=$(PARALLEL_JOBS) \
		--build-arg OPENVINO_APT_YEAR=2024 \
		--build-arg ENABLE_GPU_RUNTIME=$(ENABLE_GPU_RUNTIME) \
		--target runtime-dev \
		-t $(IMAGE_NAME):$(TAG) .

build-prod: download
	@echo "$(BLUE)--> Building prod image $(IMAGE_NAME):prod$(RESET)"
	@env -u http_proxy -u https_proxy -u HTTP_PROXY -u HTTPS_PROXY -u no_proxy -u NO_PROXY \
	 docker build \
		$(NETWORK_FLAG) \
		--build-arg PARALLEL_JOBS=$(PARALLEL_JOBS) \
		--build-arg OPENVINO_APT_YEAR=2024 \
		--build-arg ENABLE_GPU_RUNTIME=$(ENABLE_GPU_RUNTIME) \
		--target runtime-prod \
		-t $(IMAGE_NAME):prod .

$(CACHE_DIR):
	@mkdir -p $@

download: $(CACHE_DIR)/.download_complete

$(CACHE_DIR)/.download_complete: makefile | $(CACHE_DIR)
	@echo "$(MAGENTA)--> Downloading all offline deps into .cache ...$(RESET)"
	@$(MAKE) clone-repos
	@echo "$(GREEN)✓ Offline deps ready$(RESET)"
	@touch $@

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
	@echo "$(CYAN)SPV_WS_ENABLE=$(SPV_WS_ENABLE)$(RESET)"
	@echo "$(CYAN)SPV_WS_PORT=$(SPV_WS_PORT)$(RESET)"
	@echo "$(CYAN)DATA_DIR=$(DATA_DIR)$(RESET)"
	@echo "$(CYAN)CACHE_DIR=$(CACHE_DIR)$(RESET)"
	@echo "$(CYAN)Repos: $(REPO_NAMES)$(RESET)"

check-deps:
	@command -v docker >/dev/null 2>&1 && echo "$(GREEN)✓ docker$(RESET)" || echo "$(RED)✗ docker$(RESET)"
	@command -v git >/dev/null 2>&1 && echo "$(GREEN)✓ git$(RESET)" || echo "$(RED)✗ git$(RESET)"
	@command -v wget >/dev/null 2>&1 && echo "$(GREEN)✓ wget$(RESET)" || echo "$(RED)✗ wget$(RESET)"

# ------------------------------
# spv: 容器内构建/运行快捷命令
# ------------------------------
SPV_BUILD_DIR      ?= build
SPV_BUILD_TYPE     ?= Release
SPV_CMAKE_ARGS     ?=
SPV_ROS_WS         ?= /app/sp_vision/ros2_ws
SPV_ROS_PACKAGES   ?= sp_msgs
SPV_ROS_AUTO_INSTALL_COLCON ?= 1
SPV_RUN_TARGET     ?=
SPV_RUN_ARGS       ?=
SPV_CMD            ?=
CMD                ?=
SPV_FOXGLOVE_PORT  ?= 8765
SPV_WS_ENABLE      ?= 1
SPV_WS_PORT        ?= 8878
SPV_WS_BIND_HOST   ?= 0.0.0.0
SPV_DAEMON_NAME    ?= $(CONTAINER_NAME)_svc
SPV_SUPERVISOR_CONTAINER_NAME ?= $(CONTAINER_NAME)_supervised
SPV_AUTOSTART      ?= 1
SPV_SUPERVISE      ?= 1
SPV_REQUIRED_CAMERA ?= 1
SPV_REQUIRED_SERIAL ?= 1
SPV_SERIAL_CANDIDATES ?= /dev/gimbal,/dev/ttyACM0,/dev/ttyACM1,/dev/ttyACM2,/dev/ttyUSB0,/dev/ttyUSB1,/dev/ttyUSB2
SPV_SERIAL_BAUD    ?= 115200
SPV_RESTART_DELAY_MS ?= 1500
SPV_STARTUP_TIMEOUT_SEC ?= 0
SPV_FAIL_NOTIFY_ENABLE ?= 1
SPV_FAIL_NOTIFY_HOST ?= 127.0.0.1
SPV_FAIL_NOTIFY_PORT ?= 18878
SPV_FAIL_NOTIFY_SERVICE_NAME ?= spv_supervisor
SPV_SYSTEMD_SERVICE_NAME ?= spv-supervisor.service
SPV_CMD_REAL       := $(if $(strip $(SPV_CMD)),$(SPV_CMD),$(CMD))
SPV_COMPONENTS     ?= auto
SPV_AUTO_SYNC_COMPONENT_DEPS ?= 1
SPV_FLATBUFFERS_VERSION ?= v24.3.25
SPV_FLATC_VERSION ?= v24.3.25
SPV_FLATC_ASSET ?= Linux.flatc.binary.g++-13.zip
SPV_KILL_PORT ?= $(SPV_FOXGLOVE_PORT)
SPV_KILL_BEFORE_RUN ?= 0

define maybe_map_port
$(if $(filter yes,$(USE_HOST_NET)),,$(if $(filter $(1),$(FORWARD_PORTS)),,-p $(1):$(1)))
endef

define DOCKER_RUN_SPV
docker run --rm \
	$(NETWORK_FLAG) \
	$(PORT_FLAGS) \
	$(call maybe_map_port,$(SPV_FOXGLOVE_PORT)) \
	$(call maybe_map_port,$(SPV_WS_PORT)) \
	$(DOCKER_PRIVILEGED_FLAGS) \
	-v /dev/bus/usb:/dev/bus/usb \
	$(GPU_FLAGS) \
	--device=$(GIMBAL_DEV_HOST):$(GIMBAL_DEV_CONT) \
	-e SPV_FOXGLOVE_PORT=$(SPV_FOXGLOVE_PORT) \
	-e SPV_WS_ENABLE=$(SPV_WS_ENABLE) \
	-e SPV_WS_PORT=$(SPV_WS_PORT) \
	-e SPV_WS_BIND_HOST=$(SPV_WS_BIND_HOST) \
	-v $(CURDIR):/app/sp_vision \
	-v $(DATA_DIR):/app/data \
	-w /app/sp_vision \
	--entrypoint /bin/bash \
	$(IMAGE_NAME):$(TAG)
endef

.PHONY: spv-config spv-build spv-rebuild spv-build-with-ros spv-run spv-cmd spv-ros-init spv-ros-build spv-ros-rebuild spv-daemon-up spv-daemon-down spv-daemon-shell spv-ros-daemon-start spv-ros-daemon-stop spv-ros-daemon-status spv-sync-deps spv-kill-port spv-kill-foxglove-port spv-supervisor-run spv-supervisor-install spv-supervisor-status spv-supervisor-logs spv-help

spv-sync-deps:
	@set -e; \
	if [ "$(SPV_AUTO_SYNC_COMPONENT_DEPS)" != "1" ]; then \
		exit 0; \
	fi; \
	want_foxglove=0; \
	case ",$(SPV_COMPONENTS)," in \
		*,all,*|*,foxglove,*) want_foxglove=1 ;; \
	esac; \
	if [ "$(SPV_COMPONENTS)" = "auto" ]; then \
		case " $(SPV_CMAKE_ARGS) " in \
			*" -DSPV_ENABLE_FOXGLOVE=ON "*|*" -DSPV_PLOTTER_BACKEND=foxglove "*) want_foxglove=1 ;; \
		esac; \
	fi; \
	if [ "$$want_foxglove" = "1" ]; then \
		echo "$(BLUE)--> Sync component deps: foxglove$(RESET)"; \
		[ -f 3rdparty/flatbuffers/include/flatbuffers/flatbuffers.h ] || \
			bash tools/foxglove/download_flatbuffers_headers.sh $(SPV_FLATBUFFERS_VERSION); \
		[ -x tools/bin/flatc/linux-x86_64/flatc ] || \
			bash tools/foxglove/download_flatc.sh $(SPV_FLATC_VERSION) $(SPV_FLATC_ASSET); \
		[ -f 3rdparty/asio/asio/include/asio.hpp ] || \
			bash tools/foxglove/download_asio_headers.sh asio-1-31-0; \
		[ -f 3rdparty/websocketpp/websocketpp/config/asio_no_tls.hpp ] || \
			bash tools/foxglove/download_websocketpp_headers.sh 0.8.2; \
	fi

spv-config: ensure-dirs spv-sync-deps
	@$(DOCKER_RUN_SPV) -lc 'set -eo pipefail; \
		source /opt/ros/jazzy/setup.bash; \
		if [ -f /app/sp_vision/ros2_ws/install/setup.bash ]; then source /app/sp_vision/ros2_ws/install/setup.bash; fi; \
		cmake -S . -B $(SPV_BUILD_DIR) -DCMAKE_BUILD_TYPE=$(SPV_BUILD_TYPE) $(SPV_CMAKE_ARGS)'

spv-build: ensure-dirs spv-sync-deps
	@$(DOCKER_RUN_SPV) -lc 'set -eo pipefail; \
		source /opt/ros/jazzy/setup.bash; \
		if [ -f /app/sp_vision/ros2_ws/install/setup.bash ]; then source /app/sp_vision/ros2_ws/install/setup.bash; fi; \
		cmake -S . -B $(SPV_BUILD_DIR) -DCMAKE_BUILD_TYPE=$(SPV_BUILD_TYPE) $(SPV_CMAKE_ARGS); \
		cmake --build $(SPV_BUILD_DIR) -j $(PARALLEL_JOBS)'

spv-rebuild: ensure-dirs spv-sync-deps
	@$(DOCKER_RUN_SPV) -lc 'set -eo pipefail; \
		rm -rf $(SPV_BUILD_DIR); \
		source /opt/ros/jazzy/setup.bash; \
		if [ -f /app/sp_vision/ros2_ws/install/setup.bash ]; then source /app/sp_vision/ros2_ws/install/setup.bash; fi; \
		cmake -S . -B $(SPV_BUILD_DIR) -DCMAKE_BUILD_TYPE=$(SPV_BUILD_TYPE) $(SPV_CMAKE_ARGS); \
		cmake --build $(SPV_BUILD_DIR) -j $(PARALLEL_JOBS)'

spv-run: ensure-dirs
	@if [ "$(SPV_KILL_BEFORE_RUN)" = "1" ]; then \
		$(MAKE) --no-print-directory spv-kill-port SPV_KILL_PORT=$(SPV_FOXGLOVE_PORT); \
	fi
	@if [ -z "$(SPV_RUN_TARGET)" ]; then \
		echo "$(RED)Error: specify SPV_RUN_TARGET=<binary or path>$(RESET)"; \
		echo "$(YELLOW)Example: make $@ SPV_RUN_TARGET=camera_detect_test SPV_RUN_ARGS='--config-path configs/standard3.yaml'$(RESET)"; \
		exit 1; \
	fi
	@$(DOCKER_RUN_SPV) -lc 'set -eo pipefail; \
		source /opt/ros/jazzy/setup.bash; \
		if [ -f /app/sp_vision/ros2_ws/install/setup.bash ]; then source /app/sp_vision/ros2_ws/install/setup.bash; fi; \
		cd /app/sp_vision; \
		t="$(SPV_RUN_TARGET)"; \
		if [ -x "$$t" ]; then exec "$$t" $(SPV_RUN_ARGS); fi; \
		if [ -x "./$$t" ]; then exec "./$$t" $(SPV_RUN_ARGS); fi; \
		if [ -x "$(SPV_BUILD_DIR)/$$t" ]; then exec "$(SPV_BUILD_DIR)/$$t" $(SPV_RUN_ARGS); fi; \
		if [ -x "$(SPV_BUILD_DIR)/bin/$$t" ]; then exec "$(SPV_BUILD_DIR)/bin/$$t" $(SPV_RUN_ARGS); fi; \
		echo "target not found: $$t"; \
		exit 1'

spv-cmd: ensure-dirs
	@if [ -z "$(SPV_CMD_REAL)" ]; then \
		echo "$(RED)Error: specify SPV_CMD=\"...\" or CMD=\"...\"$(RESET)"; \
		echo "$(YELLOW)Example: make $@ SPV_CMD='ls -la build'$(RESET)"; \
		echo "$(YELLOW)Example: make $@ CMD='ls -la build'$(RESET)"; \
		exit 1; \
	fi
	@$(DOCKER_RUN_SPV) -lc 'set -eo pipefail; $(SPV_CMD_REAL)'

spv-kill-port:
	@echo "$(YELLOW)--> Killing TCP listeners on port $(SPV_KILL_PORT)$(RESET)"
	@set -e; \
	killed_any=0; \
	if command -v docker >/dev/null 2>&1; then \
		conflicts="$$(docker ps --filter publish=$(SPV_KILL_PORT) --format '{{.ID}} {{.Names}}' 2>/dev/null || true)"; \
		if [ -n "$$conflicts" ]; then \
			echo "$(YELLOW)--> Stopping docker containers publishing $(SPV_KILL_PORT)$(RESET)"; \
			echo "$$conflicts" | while read -r cid cname; do \
				[ -n "$$cid" ] || continue; \
				docker stop "$$cid" >/dev/null 2>&1 || true; \
				echo "$(GREEN)✓ Stopped container $$cname ($$cid)$(RESET)"; \
			done; \
			killed_any=1; \
		fi; \
	fi; \
	if command -v fuser >/dev/null 2>&1; then \
		if fuser -n tcp $(SPV_KILL_PORT) >/dev/null 2>&1; then \
			fuser -k $(SPV_KILL_PORT)/tcp >/dev/null 2>&1 || true; \
			echo "$(GREEN)✓ Killed host listeners on $(SPV_KILL_PORT)$(RESET)"; \
			killed_any=1; \
		fi; \
	else \
		echo "$(RED)fuser not found; install psmisc or kill manually$(RESET)"; \
		exit 2; \
	fi; \
	if [ "$$killed_any" = "0" ]; then \
		echo "$(BLUE)No listener on $(SPV_KILL_PORT)$(RESET)"; \
	fi

spv-kill-foxglove-port: spv-kill-port

spv-ros-build: ensure-dirs
	@$(DOCKER_RUN_SPV) -lc 'set -eo pipefail; \
		source /opt/ros/jazzy/setup.bash; \
		if [ "$(SPV_ROS_AUTO_INSTALL_COLCON)" = "1" ] && ! command -v colcon >/dev/null 2>&1; then \
			apt-get update; \
			apt-get install -y --no-install-recommends python3-colcon-common-extensions; \
		fi; \
		if ! command -v colcon >/dev/null 2>&1; then \
			echo "colcon not found; set SPV_ROS_AUTO_INSTALL_COLCON=1 or install colcon in image"; \
			exit 2; \
		fi; \
		if [ -d $(SPV_ROS_WS)/src ]; then \
			cd $(SPV_ROS_WS); \
			colcon build --merge-install --symlink-install --packages-up-to $(SPV_ROS_PACKAGES); \
			echo "ROS workspace built: $(SPV_ROS_WS)/install"; \
		else \
			echo "ROS workspace src not found: $(SPV_ROS_WS)/src"; \
			exit 2; \
		fi'

spv-ros-init: spv-ros-build

spv-ros-rebuild: ensure-dirs
	@$(DOCKER_RUN_SPV) -lc 'set -eo pipefail; \
		source /opt/ros/jazzy/setup.bash; \
		if [ "$(SPV_ROS_AUTO_INSTALL_COLCON)" = "1" ] && ! command -v colcon >/dev/null 2>&1; then \
			apt-get update; \
			apt-get install -y --no-install-recommends python3-colcon-common-extensions; \
		fi; \
		if ! command -v colcon >/dev/null 2>&1; then \
			echo "colcon not found; set SPV_ROS_AUTO_INSTALL_COLCON=1 or install colcon in image"; \
			exit 2; \
		fi; \
		if [ -d $(SPV_ROS_WS)/src ]; then \
			rm -rf $(SPV_ROS_WS)/build $(SPV_ROS_WS)/install $(SPV_ROS_WS)/log; \
			cd $(SPV_ROS_WS); \
			colcon build --merge-install --symlink-install --packages-up-to $(SPV_ROS_PACKAGES); \
			echo "ROS workspace rebuilt: $(SPV_ROS_WS)/install"; \
		else \
			echo "ROS workspace src not found: $(SPV_ROS_WS)/src"; \
			exit 2; \
		fi'

spv-build-with-ros: ensure-dirs spv-sync-deps
	@$(DOCKER_RUN_SPV) -lc 'set -eo pipefail; \
		source /opt/ros/jazzy/setup.bash; \
		if [ "$(SPV_ROS_AUTO_INSTALL_COLCON)" = "1" ] && ! command -v colcon >/dev/null 2>&1; then \
			apt-get update; \
			apt-get install -y --no-install-recommends python3-colcon-common-extensions; \
		fi; \
		if ! command -v colcon >/dev/null 2>&1; then \
			echo "colcon not found; set SPV_ROS_AUTO_INSTALL_COLCON=1 or install colcon in image"; \
			exit 2; \
		fi; \
		if [ -d $(SPV_ROS_WS)/src ]; then \
			cd $(SPV_ROS_WS); \
			colcon build --merge-install --symlink-install --packages-up-to $(SPV_ROS_PACKAGES); \
			source $(SPV_ROS_WS)/install/setup.bash; \
		else \
			echo "ROS workspace src not found: $(SPV_ROS_WS)/src"; \
			exit 2; \
		fi; \
		cd /app/sp_vision; \
		cmake -S . -B $(SPV_BUILD_DIR) -DCMAKE_BUILD_TYPE=$(SPV_BUILD_TYPE) $(SPV_CMAKE_ARGS); \
		cmake --build $(SPV_BUILD_DIR) -j $(PARALLEL_JOBS)'

spv-daemon-up: ensure-dirs
	@docker rm -f $(SPV_DAEMON_NAME) >/dev/null 2>&1 || true
	@docker run -d \
		$(NETWORK_FLAG) \
		$(PORT_FLAGS) \
		$(DOCKER_PRIVILEGED_FLAGS) \
		-v /dev/bus/usb:/dev/bus/usb \
		$(GPU_FLAGS) \
		--device=$(GIMBAL_DEV_HOST):$(GIMBAL_DEV_CONT) \
		-v $(CURDIR):/app/sp_vision \
		-v $(DATA_DIR):/app/data \
		-w /app/sp_vision \
		--entrypoint /bin/bash \
		--name $(SPV_DAEMON_NAME) \
		$(IMAGE_NAME):$(TAG) -lc 'trap : TERM INT; sleep infinity & wait'
	@echo "$(GREEN)daemon up: $(SPV_DAEMON_NAME)$(RESET)"

spv-daemon-down:
	@docker rm -f $(SPV_DAEMON_NAME) >/dev/null 2>&1 || true
	@echo "$(YELLOW)daemon down: $(SPV_DAEMON_NAME)$(RESET)"

spv-daemon-shell:
	@docker exec -it $(SPV_DAEMON_NAME) /bin/bash

spv-ros-daemon-start:
	@docker exec $(SPV_DAEMON_NAME) /bin/bash -lc 'source /opt/ros/jazzy/setup.bash; ros2 daemon start'

spv-ros-daemon-stop:
	@docker exec $(SPV_DAEMON_NAME) /bin/bash -lc 'source /opt/ros/jazzy/setup.bash; ros2 daemon stop'

spv-ros-daemon-status:
	@docker exec $(SPV_DAEMON_NAME) /bin/bash -lc 'source /opt/ros/jazzy/setup.bash; ros2 daemon status'

spv-supervisor-run: ensure-dirs
	@env \
		SPV_ROOT_DIR=$(CURDIR) \
		IMAGE_NAME=$(IMAGE_NAME) \
		TAG=$(TAG) \
		DATA_DIR=$(DATA_DIR) \
		USE_HOST_NET=$(USE_HOST_NET) \
		FORWARD_PORTS='$(FORWARD_PORTS)' \
		ENABLE_GPU=$(ENABLE_GPU) \
		DOCKER_PRIVILEGED_FLAGS='$(DOCKER_PRIVILEGED_FLAGS)' \
		GIMBAL_DEV_CONT=$(GIMBAL_DEV_CONT) \
		SPV_BUILD_DIR=$(SPV_BUILD_DIR) \
		SPV_RUN_TARGET='$(SPV_RUN_TARGET)' \
		SPV_RUN_ARGS='$(SPV_RUN_ARGS)' \
		SPV_AUTOSTART=$(SPV_AUTOSTART) \
		SPV_SUPERVISE=$(SPV_SUPERVISE) \
		SPV_REQUIRED_CAMERA=$(SPV_REQUIRED_CAMERA) \
		SPV_REQUIRED_SERIAL=$(SPV_REQUIRED_SERIAL) \
		SPV_SERIAL_CANDIDATES='$(SPV_SERIAL_CANDIDATES)' \
		SPV_SERIAL_BAUD=$(SPV_SERIAL_BAUD) \
		SPV_RESTART_DELAY_MS=$(SPV_RESTART_DELAY_MS) \
		SPV_STARTUP_TIMEOUT_SEC=$(SPV_STARTUP_TIMEOUT_SEC) \
		SPV_FOXGLOVE_PORT=$(SPV_FOXGLOVE_PORT) \
		SPV_WS_ENABLE=$(SPV_WS_ENABLE) \
		SPV_WS_PORT=$(SPV_WS_PORT) \
		SPV_WS_BIND_HOST=$(SPV_WS_BIND_HOST) \
		SPV_FAIL_NOTIFY_ENABLE=$(SPV_FAIL_NOTIFY_ENABLE) \
		SPV_FAIL_NOTIFY_HOST=$(SPV_FAIL_NOTIFY_HOST) \
		SPV_FAIL_NOTIFY_PORT=$(SPV_FAIL_NOTIFY_PORT) \
		SPV_FAIL_NOTIFY_SERVICE_NAME=$(SPV_FAIL_NOTIFY_SERVICE_NAME) \
		SPV_SUPERVISOR_CONTAINER_NAME=$(SPV_SUPERVISOR_CONTAINER_NAME) \
		bash tools/spv_supervisor.sh

spv-supervisor-install:
	@env \
		SPV_SYSTEMD_SERVICE_NAME=$(SPV_SYSTEMD_SERVICE_NAME) \
		bash tools/install_spv_supervisor_service.sh

spv-supervisor-status:
	@systemctl status $(SPV_SYSTEMD_SERVICE_NAME) --no-pager

spv-supervisor-logs:
	@journalctl -u $(SPV_SYSTEMD_SERVICE_NAME) -f

spv-help:
	@echo "spv-config      : container cmake configure"
	@echo "spv-build       : container cmake configure+build"
	@echo "spv-rebuild     : clean build dir and rebuild in container"
	@echo "spv-build-with-ros : build ROS workspace first, then build main project"
	@echo "spv-run         : run built target in container (SPV_RUN_TARGET=...)"
	@echo "                 : NAT mode auto-maps SPV_WS_PORT/SPV_FOXGLOVE_PORT when missing from FORWARD_PORTS"
	@echo "spv-cmd         : run custom command in container (SPV_CMD=...)"
	@echo "spv-ros-build   : build ROS workspace packages (SPV_ROS_PACKAGES=...)"
	@echo "spv-ros-rebuild : clean and rebuild ROS workspace"
	@echo "spv-ros-init    : alias of spv-ros-build"
	@echo "spv-daemon-up   : start long-running dev container"
	@echo "spv-daemon-down : stop long-running dev container"
	@echo "spv-daemon-shell: shell into long-running dev container"
	@echo "spv-ros-daemon-start|stop|status : control ros2 daemon inside spv daemon"
	@echo "spv-supervisor-run    : run host-side supervisor"
	@echo "spv-supervisor-install: install and enable systemd unit for host-side supervisor"
	@echo "spv-supervisor-status : show systemd status for host-side supervisor"
	@echo "spv-supervisor-logs   : follow journalctl logs for host-side supervisor"
	@echo "spv-sync-deps   : sync optional component deps (SPV_COMPONENTS=auto|foxglove|all)"
	@echo "spv-kill-port   : kill listener on TCP port (SPV_KILL_PORT=..., default SPV_FOXGLOVE_PORT)"
	@echo "spv-kill-foxglove-port : alias of spv-kill-port"

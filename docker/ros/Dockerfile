##### sp_vision_25 Dockerfile (Ubuntu 24.04, 离线构建, HikRobot USB 相机)

# 说明：
# - 仅围绕 CMakeLists.txt 中的依赖做离线构建：
#   OpenCV / fmt / Eigen3 / spdlog / yaml-cpp / nlohmann_json / OpenVINO / HikRobot SDK
# - 其余调试工具只用系统包或 Python 包，不做额外源码离线构建
# - .cache 目录结构（由根目录 makefile 维护）：
#   .cache/vendor/  (HikRobot SDK *.deb 已经下载好)
#   .cache/opencv
#   .cache/opencv_contrib
#   .cache/spdlog
#   .cache/json
#   .cache/yaml-cpp
# - 严格区分 builder / runtime：CMake 只在 builder 里跑；runtime 只运行可执行文件

ARG PARALLEL_JOBS=12
ARG OPENVINO_APT_YEAR=2024
ARG OPENVINO_COMPONENTS="openvino openvino-libraries-dev"  # 不拉 samples
ARG ALLOW_NET_FOR_VENDOR=0  # 是否允许为 Hik SDK 自动补依赖（默认 0=完全离线）
ARG ENABLE_GPU_RUNTIME=0   # 0: 关闭, 1: 启用 GPU runtime

############################
# base-builder: 编译环境
############################
FROM ubuntu:24.04 AS base-builder
SHELL ["/bin/bash", "-o", "pipefail", "-c"]

ARG DEBIAN_FRONTEND=noninteractive
ARG PARALLEL_JOBS
ARG OPENVINO_APT_YEAR
ARG OPENVINO_COMPONENTS

ENV TZ=Asia/Shanghai
ENV MAKEFLAGS=-j${PARALLEL_JOBS}
ENV CMAKE_FLAGS_OFFLINE="-DFETCHCONTENT_FULLY_DISCONNECTED=ON"

RUN set -eux; \
	apt-get update; \
	apt-get install -y --no-install-recommends ca-certificates software-properties-common curl wget gnupg; \
	printf 'Acquire::Retries "5";\nAcquire::http::Timeout "30";\nAcquire::https::Timeout "30";\nAcquire::ForceIPv4 "true";\n' \
	  >/etc/apt/apt.conf.d/99spv-net; \
	add-apt-repository -y universe; \
	sed -i 's|http://archive.ubuntu.com/ubuntu/|https://mirrors.aliyun.com/ubuntu/|g' /etc/apt/sources.list; \
	apt-get update; \
	apt-get install -y --no-install-recommends \
		build-essential ninja-build pkg-config git \
		cmake \
		libeigen3-dev \
        libwebpdemux2 \
        libwebpmux3 \
		libjpeg-dev libpng-dev libtiff-dev libwebp-dev libopenjp2-7-dev \
		libavcodec-dev libavformat-dev libavutil-dev libswscale-dev \
		libgtk-3-dev libx11-6 libv4l-dev libusb-1.0-0-dev \
		libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
		libtbb-dev liblapack-dev libopenblas-dev \
		python3 python3-venv python3-dev python3-pip \
		ffmpeg; \
	rm -rf /var/lib/apt/lists/*

# OpenVINO Runtime & Dev（APT，ubuntu24）
RUN set -eux; \
	install -d -m 0755 /etc/apt/keyrings; \
	curl -fsSL https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB \
	  | gpg --dearmor -o /etc/apt/keyrings/intel-openvino.gpg; \
	printf '%s\n' "deb [signed-by=/etc/apt/keyrings/intel-openvino.gpg] https://apt.repos.intel.com/openvino/${OPENVINO_APT_YEAR} ubuntu24 main" \
	  | tee /etc/apt/sources.list.d/intel-openvino-${OPENVINO_APT_YEAR}.list >/dev/null; \
	apt-get update; \
	apt-get install -y --no-install-recommends ${OPENVINO_COMPONENTS}; \
	rm -rf /var/lib/apt/lists/*

RUN set -eux; \
	cand=""; \
	if ls -d /opt/intel/openvino* >/dev/null 2>&1; then cand="$(ls -1d /opt/intel/openvino* | head -n1)"; \
	elif ls -d /usr/lib/openvino-* >/dev/null 2>&1; then cand="$(ls -1d /usr/lib/openvino-* | sort -r | head -n1)"; fi; \
	[ -n "${cand}" ] || { echo "OpenVINO not found"; exit 1; }; \
	mkdir -p /opt/intel; ln -sfn "${cand}" /opt/intel/openvino_latest; echo "OpenVINO resolved to: ${cand}"

ENV OPENVINO_ROOT=/opt/intel/openvino_latest
ENV OpenVINO_DIR=${OPENVINO_ROOT}/runtime/cmake
ENV LD_LIBRARY_PATH=${OPENVINO_ROOT}/runtime/lib/intel64
ENV CMAKE_PREFIX_PATH=${OPENVINO_ROOT}/runtime/cmake:/usr/lib/cmake/openvino

# 安装 ROS 2 Jazzy（开发环境，包含构建 sp_msgs 所需组件）
RUN set -eux; \
	apt-get update; \
	install -d -m 0755 /etc/apt/keyrings; \
	curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key \
	  -o /etc/apt/keyrings/ros-archive-keyring.gpg; \
	. /etc/os-release; \
	echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu ${UBUNTU_CODENAME} main" \
	  > /etc/apt/sources.list.d/ros2.list; \
	apt-get update; \
	apt-get install -y --no-install-recommends \
		ros-jazzy-ros-base \
		ros-jazzy-rclcpp \
		ros-jazzy-ament-cmake \
		ros-jazzy-rosidl-default-generators \
		python3-colcon-common-extensions; \
	rm -rf /var/lib/apt/lists/*

ENV ROS_DISTRO=jazzy

############################
# 第三方库离线构建
############################

# fmt
FROM base-builder AS fmt-builder
ARG PARALLEL_JOBS
COPY .cache/fmt /tmp/fmt
RUN cd /tmp/fmt && mkdir -p build && cd build && \
	cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/fmt \
		-DBUILD_SHARED_LIBS=ON -DFMT_DOC=OFF -DFMT_TEST=OFF ${CMAKE_FLAGS_OFFLINE} && \
	make -j${PARALLEL_JOBS} && make install && ldconfig

# spdlog
FROM base-builder AS spdlog-builder
ARG PARALLEL_JOBS
COPY --from=fmt-builder /opt/fmt /opt/fmt
ENV CMAKE_PREFIX_PATH=/opt/fmt:${CMAKE_PREFIX_PATH}
COPY .cache/spdlog /tmp/spdlog
RUN cd /tmp/spdlog && mkdir -p build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/spdlog \
             -DSPDLOG_BUILD_SHARED=ON \
             -DSPDLOG_BUILD_TESTS=OFF -DSPDLOG_BUILD_EXAMPLES=OFF \
             -DSPDLOG_FMT_EXTERNAL=ON \
             -DSPDLOG_USE_STD_FORMAT=OFF ${CMAKE_FLAGS_OFFLINE} && \
    make -j${PARALLEL_JOBS} && make install && ldconfig

# nlohmann_json
FROM base-builder AS json-builder
ARG PARALLEL_JOBS
COPY .cache/json /tmp/json
RUN cd /tmp/json && mkdir -p build && cd build && \
	cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/json -DJSON_BuildTests=OFF -DJSON_Install=ON ${CMAKE_FLAGS_OFFLINE} && \
	make -j${PARALLEL_JOBS} && make install

# yaml-cpp（0.7.x 源码，24.04 APT 只有 0.8，因此全程使用离线版本）
FROM base-builder AS yaml-cpp-builder
ARG PARALLEL_JOBS
COPY .cache/yaml-cpp /tmp/yaml-cpp
RUN cd /tmp/yaml-cpp && mkdir -p build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/yaml-cpp \
             -DYAML_BUILD_SHARED_LIBS=ON -DYAML_CPP_BUILD_TESTS=OFF -DYAML_CPP_BUILD_TOOLS=OFF ${CMAKE_FLAGS_OFFLINE} && \
    make -j${PARALLEL_JOBS} && make install && ldconfig


# Ceres Solver
FROM base-builder AS ceres-builder
ARG PARALLEL_JOBS
COPY .cache/ceres-solver /tmp/ceres-solver
RUN cd /tmp/ceres-solver && mkdir -p build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/ceres \
		     -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF \
		     -DMINIGLOG=ON -DGLOG_PREFER_EXPORTED_GLOG_CMAKE_CONFIGURATION=OFF \
		     -DEIGENSPARSE=ON -DSUITESPARSE=OFF -DGFLAGS=OFF ${CMAKE_FLAGS_OFFLINE} && \
    make -j${PARALLEL_JOBS} && make install && ldconfig
    
    
# OpenCV
FROM base-builder AS opencv-builder
ARG PARALLEL_JOBS
COPY .cache/opencv /tmp/opencv
COPY .cache/opencv_contrib /tmp/opencv_contrib
RUN cd /tmp/opencv && mkdir -p build && cd build && \
	cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/opencv \
		-DOPENCV_EXTRA_MODULES_PATH=/tmp/opencv_contrib/modules \
		-DBUILD_SHARED_LIBS=ON -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_EXAMPLES=OFF \
		-DWITH_CUDA=OFF -DWITH_GSTREAMER=ON -DWITH_V4L=ON \
		-DWITH_JPEG=ON -DWITH_PNG=ON -DWITH_TIFF=ON -DWITH_WEBP=ON -DWITH_OPENJPEG=ON \
		-DENABLE_PRECOMPILED_HEADERS=OFF \
		-DBUILD_opencv_wechat_qrcode=OFF \
		-DBUILD_opencv_xfeatures2d=OFF \
		-DBUILD_opencv_face=OFF \
		-DOPENCV_IPP_ENABLE=OFF \
		-DWITH_IPP=OFF \
		-DINSTALL_C_EXAMPLES=OFF -DINSTALL_PYTHON_EXAMPLES=OFF \
		${CMAKE_FLAGS_OFFLINE} && \
	make -j${PARALLEL_JOBS} && make install && ldconfig

############################
# builder: 真正编译 sp_vision
############################
FROM base-builder AS builder

ARG PARALLEL_JOBS
ARG ALLOW_NET_FOR_VENDOR

WORKDIR /app/sp_vision

# 拷贝第三方库
COPY --from=spdlog-builder /opt/spdlog /opt/spdlog
COPY --from=fmt-builder /opt/fmt /opt/fmt
COPY --from=json-builder   /opt/json   /opt/json
COPY --from=yaml-cpp-builder /opt/yaml-cpp /opt/yaml-cpp
COPY --from=opencv-builder /opt/opencv /opt/opencv
COPY --from=ceres-builder /opt/ceres /opt/ceres

ENV LD_LIBRARY_PATH=/opt/spdlog/lib:/opt/fmt/lib:/opt/yaml-cpp/lib:/opt/opencv/lib:/opt/ceres/lib:${LD_LIBRARY_PATH}
ENV PKG_CONFIG_PATH=/opt/spdlog/lib/pkgconfig:/opt/fmt/lib/pkgconfig:/opt/yaml-cpp/lib/pkgconfig:/opt/opencv/lib/pkgconfig:/opt/ceres/lib/pkgconfig
ENV CMAKE_PREFIX_PATH=/opt/spdlog:/opt/fmt:/opt/json:/opt/yaml-cpp:/opt/opencv:/opt/ceres:${CMAKE_PREFIX_PATH}

# 安装 HikRobot SDK（完全离线：用户提前把 deb 放到 .cache/vendor）
COPY .cache/vendor/ /tmp/sdk_install/
RUN set -eux; \
	ARCH=$(dpkg --print-architecture); \
	INSTALL_ARCH=$([ "${ARCH}" = "amd64" ] && echo "x86_64" || echo "${ARCH}"); \
	SDK_PKG=$(cd /tmp/sdk_install && ls MvCamCtrlSDK_Runtime*_${INSTALL_ARCH}_*.deb 2>/dev/null | head -n1 || true); \
	[ -n "${SDK_PKG}" ] || { echo "Error: SDK Runtime .deb not found in .cache/vendor"; ls -R /tmp/sdk_install; exit 1; }; \
	if [ "${ALLOW_NET_FOR_VENDOR}" = "1" ]; then \
		apt-get update && apt-get install -y /tmp/sdk_install/${SDK_PKG} && apt-get -f install -y; \
	else \
		dpkg -i /tmp/sdk_install/${SDK_PKG} || { echo "Strict offline: missing deps for Hik SDK"; exit 1; }; \
	fi; \
	rm -rf /tmp/sdk_install /var/lib/apt/lists/*

# 设置海康SDK （编译时需要）	
ENV LD_LIBRARY_PATH=/opt/MVS/lib/64:${LD_LIBRARY_PATH}
ENV MVCAM_COMMON_RUNENV=/opt/MVS

# 复制项目源码并构建（仅在 builder 执行 CMake/Make）
COPY . /app/sp_vision

# 先在 builder 中构建 ROS2 工作区的 sp_msgs，再构建 sp_vision 本体
RUN set -ex; \
	source /opt/ros/jazzy/setup.bash; \
	if [ -d /app/sp_vision/ros2_ws/src/sp_msgs ]; then \
		cd /app/sp_vision/ros2_ws; \
		colcon build --packages-select sp_msgs; \
		source /app/sp_vision/ros2_ws/install/setup.bash; \
	fi; \
	cd /app/sp_vision; \
	mkdir -p build; \
	cd build; \
	cmake .. -DCMAKE_BUILD_TYPE=Release && \
	make -j${PARALLEL_JOBS}

############################
# runtime-base: 纯运行环境
############################
FROM ubuntu:24.04 AS runtime-base
SHELL ["/bin/bash", "-o", "pipefail", "-c"]

ARG DEBIAN_FRONTEND=noninteractive
ARG OPENVINO_APT_YEAR
ARG OPENVINO_COMPONENTS
ARG ENABLE_GPU_RUNTIME=0    # 0: 关闭 GPU runtime, 1: 启用

ENV TZ=Asia/Shanghai

RUN set -eux; \
	apt-get update; \
	apt-get install -y --no-install-recommends ca-certificates software-properties-common curl wget gnupg; \
	printf 'Acquire::Retries "5";\nAcquire::http::Timeout "30";\nAcquire::https::Timeout "30";\nAcquire::ForceIPv4 "true";\n' \
	  >/etc/apt/apt.conf.d/99spv-net; \
	add-apt-repository -y universe; \
	sed -i 's|http://archive.ubuntu.com/ubuntu/|https://mirrors.aliyun.com/ubuntu/|g' /etc/apt/sources.list; \
	apt-get update; \
	apt-get install -y --no-install-recommends \
		libeigen3-dev \
        libwebpdemux2 \
        libwebpmux3 \
        libopenblas-dev \
		libzmq5 ffmpeg libgomp1 libglib2.0-0 \
		libgtk-3-0 libgstreamer1.0-0 libgstreamer-plugins-base1.0-0 libv4l-0 libusb-1.0-0 \
		libgl1 libx11-6 \
        libusb-1.0-0-dev \
		libjpeg-turbo8 libpng16-16 libtiff6 libwebp7 libopenjp2-7 \
		libtbb12 liblapack3 libopenblas0 \
		net-tools vim htop udev; \
	rm -rf /var/lib/apt/lists/*

# 安装 ROS 2 Jazzy 运行时（仅运行需要，不含编译工具）
RUN set -eux; \
	apt-get update; \
	install -d -m 0755 /etc/apt/keyrings; \
	curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key \
	  -o /etc/apt/keyrings/ros-archive-keyring.gpg; \
	. /etc/os-release; \
	echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu ${UBUNTU_CODENAME} main" \
	  > /etc/apt/sources.list.d/ros2.list; \
	apt-get update; \
	apt-get install -y --no-install-recommends \
		ros-jazzy-ros-base; \
	rm -rf /var/lib/apt/lists/*

ENV ROS_DISTRO=jazzy

# 可选：离线 Intel GPU Runtime（OpenCL / Level Zero）
# 约定：用户将 GPU 相关 .deb 预先放入 .cache/gpu-runtime，构建镜像时通过
#   --build-arg ENABLE_GPU_RUNTIME=1
# 打开安装；默认 0 不装，保持纯 CPU 环境。
COPY .cache/gpu-runtime/ /tmp/gpu-runtime/
RUN set -eux; \
	if [ "${ENABLE_GPU_RUNTIME}" = "1" ]; then \
		if ls /tmp/gpu-runtime/*.deb >/dev/null 2>&1; then \
			dpkg -i /tmp/gpu-runtime/*.deb || apt-get -f install -y; \
			rm -rf /var/lib/apt/lists/*; \
		else \
			echo "ENABLE_GPU_RUNTIME=1 但 .cache/gpu-runtime 为空"; \
			exit 1; \
		fi; \
	fi

# OpenVINO Runtime
RUN set -eux; \
	install -d -m 0755 /etc/apt/keyrings; \
	curl -fsSL https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB \
	  | gpg --dearmor -o /etc/apt/keyrings/intel-openvino.gpg; \
	printf '%s\n' "deb [signed-by=/etc/apt/keyrings/intel-openvino.gpg] https://apt.repos.intel.com/openvino/${OPENVINO_APT_YEAR} ubuntu24 main" \
	  | tee /etc/apt/sources.list.d/intel-openvino-${OPENVINO_APT_YEAR}.list >/dev/null; \
	apt-get update; \
	apt-get install -y --no-install-recommends ${OPENVINO_COMPONENTS}; \
	rm -rf /var/lib/apt/lists/*

RUN set -eux; \
	cand=""; \
	if ls -d /opt/intel/openvino* >/dev/null 2>&1; then cand="$(ls -1d /opt/intel/openvino* | head -n1)"; \
	elif ls -d /usr/lib/openvino-* >/dev/null 2>&1; then cand="$(ls -1d /usr/lib/openvino-* | sort -r | head -n1)"; fi; \
	[ -n "${cand}" ] || { echo "OpenVINO not found"; exit 1; }; \
	mkdir -p /opt/intel; ln -sfn "${cand}" /opt/intel/openvino_latest; echo "OpenVINO resolved to: ${cand}"

ENV OPENVINO_ROOT=/opt/intel/openvino_latest
ENV OpenVINO_DIR=${OPENVINO_ROOT}/runtime/cmake

# 拷贝 builder 中装好的第三方库 & Hik SDK 安装结果
COPY --from=spdlog-builder /opt/spdlog /opt/spdlog
COPY --from=fmt-builder /opt/fmt /opt/fmt
COPY --from=opencv-builder /opt/opencv /opt/opencv
COPY --from=json-builder   /opt/json   /opt/json
COPY --from=yaml-cpp-builder /opt/yaml-cpp /opt/yaml-cpp
COPY --from=ceres-builder /opt/ceres /opt/ceres
COPY --from=builder /opt/MVS /opt/MVS

# 拷贝在 builder 中生成的 sp_msgs 安装结果，供运行时 source 使用
COPY --from=builder /app/sp_vision/ros2_ws/install /ros2_ws/install

ENV LD_LIBRARY_PATH=/opt/spdlog/lib:/opt/fmt/lib:/opt/yaml-cpp/lib:/opt/opencv/lib:/opt/ceres/lib:/opt/MVS/lib/64:${OPENVINO_ROOT}/runtime/lib/intel64
ENV PKG_CONFIG_PATH=/opt/spdlog/lib/pkgconfig:/opt/fmt/lib/pkgconfig:/opt/yaml-cpp/lib/pkgconfig:/opt/opencv/lib/pkgconfig:/opt/ceres/lib/pkgconfig
ENV CMAKE_PREFIX_PATH=/opt/spdlog:/opt/fmt:/opt/json:/opt/yaml-cpp:/opt/opencv:/opt/ceres:${OPENVINO_ROOT}/runtime/cmake:/usr/lib/cmake/openvino
ENV MVCAM_COMMON_RUNENV=/opt/MVS

RUN ldconfig

# 进入交互式 bash 时自动激活 ROS2 与自定义消息包环境
RUN set -eux; \
	touch /root/.bashrc; \
	{	echo ''; \
		echo '# Auto-source ROS 2 Jazzy and custom workspace'; \
		echo 'if [ -f /opt/ros/jazzy/setup.bash ]; then'; \
		echo '  . /opt/ros/jazzy/setup.bash'; \
		echo 'fi'; \
		echo 'if [ -f /ros2_ws/install/setup.bash ]; then'; \
		echo '  . /ros2_ws/install/setup.bash'; \
		echo 'fi'; \
	} >> /root/.bashrc

############################
# runtime-dev: 开发环境
############################
FROM runtime-base AS runtime-dev
SHELL ["/bin/bash", "-o", "pipefail", "-c"]

RUN set -eux; \
	apt-get update; \
	apt-get install -y --no-install-recommends \
		build-essential ninja-build pkg-config git gdb gdbserver \
		cmake xvfb; \
	rm -rf /var/lib/apt/lists/*

# 进入交互式 bash 时，如无 DISPLAY 则自动启动 Xvfb，避免 OpenCV GUI 报错
RUN set -eux; \
	touch /root/.bashrc; \
	{ echo ''; \
	  echo '# Start Xvfb for headless OpenCV GUI when DISPLAY is empty'; \
	  echo 'if [ -z "$DISPLAY" ]; then'; \
	  echo '  if ! pgrep Xvfb >/dev/null 2>&1; then'; \
	  echo '    Xvfb :99 -screen 0 1920x1080x24 &'; \
	  echo '  fi'; \
	  echo '  export DISPLAY=:99'; \
	  echo 'fi'; \
	} >> /root/.bashrc

WORKDIR /app/sp_vision
COPY --from=builder /app/sp_vision/build /app/sp_vision/build
COPY --from=builder /app/sp_vision/configs /app/sp_vision/configs
COPY --from=builder /app/sp_vision/assets /app/sp_vision/assets
COPY autostart.sh /app/sp_vision/autostart.sh

RUN chmod +x /app/sp_vision/autostart.sh

ENTRYPOINT ["/app/sp_vision/autostart.sh"]

############################
# runtime-prod: 纯运行
############################
FROM runtime-base AS runtime-prod
WORKDIR /app/sp_vision
COPY --from=builder /app/sp_vision/build /app/sp_vision/build
COPY --from=builder /app/sp_vision/configs /app/sp_vision/configs
COPY --from=builder /app/sp_vision/assets /app/sp_vision/assets
COPY autostart.sh /app/sp_vision/autostart.sh

RUN chmod +x /app/sp_vision/autostart.sh

ENTRYPOINT ["/app/sp_vision/autostart.sh"]

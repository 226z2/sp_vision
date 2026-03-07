#pragma once

#include <cstddef>

#include <flatbuffers/flatbuffers.h>

namespace utils::foxglove::fb {

[[nodiscard]] inline flatbuffers::FlatBufferBuilder& tls_builder(std::size_t initial_size = 4096) {
  thread_local flatbuffers::FlatBufferBuilder builder(initial_size);
  builder.Clear();
  builder.ForceDefaults(true);
  return builder;
}

}  // namespace utils::foxglove::fb

#include <utility>

#include <utility>

/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <exception>

#include "extensions/impl/memory_extension.hpp"

namespace kagome::extensions {
  MemoryExtension::MemoryExtension(std::shared_ptr<runtime::WasmMemory> memory)
      : memory_(std::move(memory)),
        logger_{common::createLogger(kDefaultLoggerTag)} {
    BOOST_ASSERT_MSG(memory_ != nullptr, "memory is nullptr");
  }

  void MemoryExtension::reset() {
    memory_->reset();
  }

  runtime::WasmPointer MemoryExtension::ext_malloc(runtime::WasmSize size) {
    logger_->trace("WASM_DEBUG: try to allocate {} bytes", size);
    auto ptr = memory_->allocate(size);
    logger_->trace("WASM_DEBUG: allocated {} bytes, return ptr {}", size, ptr);
    return ptr;
  }

  void MemoryExtension::ext_free(runtime::WasmPointer ptr) {
    logger_->trace("WASM_DEBUG: try to deallocate memory by ptr {}", ptr);
    auto opt_size = memory_->deallocate(ptr);
    if (not opt_size) {
      logger_->warn(
          "Ptr {} does not point to any memory chunk in wasm memory. Nothing "
          "deallocated",
          ptr);
      return;
    }
    logger_->trace(
        "WASM_DEBUG: deallocated {} bytes by ptr {}", opt_size.value(), ptr);
  }

  runtime::WasmPointer MemoryExtension::ext_allocator_malloc_version_1(
      runtime::WasmSize size) {
    return ext_malloc(size);
  }

  void MemoryExtension::ext_allocator_free_version_1(runtime::WasmPointer ptr) {
    return ext_free(ptr);
  }
}  // namespace kagome::extensions

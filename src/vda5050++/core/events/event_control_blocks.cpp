// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/core/events/event_control_blocks.h"

#include <fmt/format.h>

#include "vda5050++/core/common/exception.h"

void vda5050pp::core::events::EventControlBlock::teardown() {
  if (this->teardown_ != nullptr) {
    this->teardown_();
  }
}

void vda5050pp::core::events::EventControlChain::next() noexcept(false) {
  if (this->blocks_.empty()) {
    throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot call next() on empty chain"));
  }

  // Remove recently teared down chain element
  this->blocks_.front()->setTeardown(nullptr);
  this->blocks_.pop_front();

  // Teardown chain or enable next
  if (this->blocks_.empty()) {
    this->teardown();
  } else {
    this->blocks_.front()->enable();
  }
}

void vda5050pp::core::events::EventControlChain::enable() noexcept(false) {
  if (this->blocks_.empty()) {
    throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot enable empty chain"));
  }

  this->blocks_.front()->enable();
}

void vda5050pp::core::events::EventControlChain::add(
    std::shared_ptr<EventControlBlock> block) noexcept(false) {
  if (block == nullptr) {
    throw vda5050pp::VDA5050PPNullPointer(MK_EX_CONTEXT("Cannot add nullptr block"));
  }

  block->setTeardown([this] { this->next(); });

  this->blocks_.push_back(std::move(block));
}

void vda5050pp::core::events::EventControlAlternative::enable() {
  if (this->blocks_.empty()) {
    throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot enable empty alternative"));
  }

  for (const auto &block_ptr : this->blocks_) {
    block_ptr->enable();
  }
}

void vda5050pp::core::events::EventControlAlternative::add(
    std::shared_ptr<EventControlBlock> block) noexcept(false) {
  if (block == nullptr) {
    throw vda5050pp::VDA5050PPNullPointer(MK_EX_CONTEXT("Cannot add nullptr block"));
  }

  block->setTeardown([this] {
    for (const auto &block_ptr : this->blocks_) {
      block_ptr->setTeardown(nullptr);
    }
    this->blocks_.clear();
    this->teardown();
  });

  this->blocks_.push_back(std::move(block));
}

void vda5050pp::core::events::FunctionBlock::enable() {
  if (this->fn_ != nullptr) {
    this->fn_();
  }
  this->teardown();
}

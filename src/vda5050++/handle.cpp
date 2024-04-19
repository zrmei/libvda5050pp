//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/handle.h"

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/instance.h"
#include "vda5050++/core/logger.h"

void vda5050pp::Handle::initialize(const vda5050pp::Config &config) const noexcept(true) {
  if (auto ptr = vda5050pp::core::Instance::init(config).lock(); ptr) {
    ptr->start();
  } else {
    core::getInstanceLogger()->error("Initialization failed. Instance::init() returned nullptr.");
  }
}

void vda5050pp::Handle::registerActionHandler(
    std::shared_ptr<vda5050pp::handler::BaseActionHandler> action_handler) const noexcept(false) {
  auto instance = vda5050pp::core::Instance::get().lock();

  if (instance == nullptr) {
    throw vda5050pp::VDA5050PPNotInitialized(MK_EX_CONTEXT(""));
  }

  instance->addActionHandler(action_handler);
}

void vda5050pp::Handle::registerNavigationHandler(
    std::shared_ptr<vda5050pp::handler::BaseNavigationHandler> navigation_handler) const
    noexcept(false) {
  auto instance = vda5050pp::core::Instance::get().lock();

  if (instance == nullptr) {
    throw vda5050pp::VDA5050PPNotInitialized(MK_EX_CONTEXT(""));
  }

  instance->setNavigationHandler(navigation_handler);
}

void vda5050pp::Handle::registerQueryHandler(
    std::shared_ptr<vda5050pp::handler::BaseQueryHandler> query_handler) const noexcept(false) {
  auto instance = vda5050pp::core::Instance::get().lock();

  if (instance == nullptr) {
    throw vda5050pp::VDA5050PPNotInitialized(MK_EX_CONTEXT(""));
  }

  instance->setQueryHandler(query_handler);
}

vda5050pp::sinks::StatusSink vda5050pp::Handle::getStatusSink() const {
  return vda5050pp::sinks::StatusSink();
}

vda5050pp::sinks::NavigationSink vda5050pp::Handle::getNavigationSink() const {
  return vda5050pp::sinks::NavigationSink();
}

#ifdef LIBVDA5050PP_EXPOSE_LOGGER
std::shared_ptr<spdlog::logger> vda5050pp::Handle::getLogger(std::string_view key) const {
  return spdlog::get(key.data());
}
#endif

void vda5050pp::Handle::shutdown() const noexcept(false) {
  if (auto ptr = vda5050pp::core::Instance::get().lock(); ptr) {
    ptr->stop();
  } else {
    core::getInstanceLogger()->warn("Handle::shutdown() called without initializing.");
  }
  vda5050pp::core::Instance::reset();
}
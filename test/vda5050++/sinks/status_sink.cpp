// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/sinks/status_sink.h"

#include <fmt/format.h>

#include <catch2/catch.hpp>

#include "vda5050++/core/instance.h"

template <typename Event> inline void testEvent(const std::function<void()> &call_sink) {
  SECTION(fmt::format("Sink dispatch {}", typeid(Event).name())) {
    auto sub =
        vda5050pp::core::Instance::ref().getStatusEventManager().getScopedStatusEventSubscriber();
    auto event = std::make_shared<Event>();
    bool called = false;
    sub.subscribe([&called](std::shared_ptr<Event>) { called = true; });
    call_sink();
    REQUIRE(called);
  }
}

template <typename Event> inline void testSyncEvent(const std::function<void()> &call_sink) {
  SECTION(fmt::format("Sink dispatch {}", typeid(Event).name())) {
    auto sub =
        vda5050pp::core::Instance::ref().getStatusEventManager().getScopedStatusEventSubscriber();
    auto event = std::make_shared<Event>();
    bool called = false;
    sub.subscribe([&called](std::shared_ptr<Event> evt) {
      auto tkn = evt->acquireResultToken();
      tkn.setValue((typename Event::result_type){});
      called = true;
    });
    call_sink();
    REQUIRE(called);
  }
}

TEST_CASE("sinks::StatusSink", "[events][sinks]") {
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().useWhiteList();
  cfg.refGlobalConfig().refEventManagerOptions().synchronous_event_dispatch = true;
  vda5050pp::core::Instance::reset();
  auto instance = vda5050pp::core::Instance::init(cfg).lock();

  vda5050pp::sinks::StatusSink sink;

  testEvent<vda5050pp::events::ErrorAdd>([sink]() { sink.addError(vda5050::Error{}); });
  testEvent<vda5050pp::events::ErrorsAlter>(
      [sink]() { sink.alterErrors([](auto &) { /* NOP */ }); });
  testEvent<vda5050pp::events::InfoAdd>([sink]() { sink.addInfo(vda5050::Info{}); });
  testEvent<vda5050pp::events::InfosAlter>([sink]() { sink.alterInfos([](auto &) { /* NOP */ }); });
  testEvent<vda5050pp::events::LoadAdd>([sink]() { sink.addLoad(vda5050::Load{}); });
  testEvent<vda5050pp::events::LoadRemove>([sink]() { sink.removeLoad(""); });
  testEvent<vda5050pp::events::LoadsAlter>([sink]() { sink.alterLoads([](auto &) { /* NOP */ }); });
  testSyncEvent<vda5050pp::events::LoadsGet>([sink]() { sink.getLoads(); });
  testEvent<vda5050pp::events::OperatingModeSet>(
      [sink]() { sink.setOperatingMode(vda5050::OperatingMode::AUTOMATIC); });
  testSyncEvent<vda5050pp::events::OperatingModeGet>([sink]() { sink.getOperatingMode(); });
  testEvent<vda5050pp::events::OperatingModeAlter>(
      [sink]() { sink.alterOperatingMode([](auto x) { return x; }); });
  testEvent<vda5050pp::events::BatteryStateSet>([sink]() { sink.setBatteryState({}); });
  testSyncEvent<vda5050pp::events::BatteryStateGet>([sink]() { sink.getBatteryState(); });
  testEvent<vda5050pp::events::BatteryStateAlter>(
      [sink]() { sink.alterBatteryState([](auto &) { /* NOP */ }); });
  testEvent<vda5050pp::events::RequestNewBase>([sink]() { sink.requestNewBase(); });
}
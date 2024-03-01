//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include <catch2/catch_all.hpp>

#include "vda5050++/core/instance.h"

using namespace std::chrono_literals;

TEST_CASE("Behaviour of AGV side Status events", "[event][status]") {
  vda5050pp::Config cfg;
  vda5050pp::config::EventManagerOptions evt_mgr_opt;
  evt_mgr_opt.synchronous_event_dispatch = true;
  cfg.refGlobalConfig().setEventManagerOptions(evt_mgr_opt);
  cfg.refGlobalConfig().useWhiteList();
  cfg.refGlobalConfig().bwListModule(vda5050pp::core::module_keys::k_state_event_handler_key);
  vda5050pp::core::Instance::reset();
  vda5050pp::core::Instance::init(cfg);

  vda5050::Load test_load;
  test_load.loadId = "Test ID";
  test_load.boundingBoxReference = vda5050::BoundingBoxReference();
  test_load.boundingBoxReference->x = 1;
  test_load.boundingBoxReference->y = 2;
  test_load.loadDimensions = vda5050::LoadDimensions();
  test_load.loadDimensions->height = 3;
  test_load.loadDimensions->width = 4;
  test_load.loadPosition = "Test position";
  test_load.loadType = "Test type";
  test_load.weight = 123;

  vda5050::Load test_load2;
  test_load.loadId = "Test ID2";
  test_load.boundingBoxReference = vda5050::BoundingBoxReference();
  test_load.boundingBoxReference->x = 1;
  test_load.boundingBoxReference->y = 2;
  test_load.loadDimensions = vda5050::LoadDimensions();
  test_load.loadDimensions->height = 3;
  test_load.loadDimensions->width = 4;
  test_load.loadPosition = "Test position";
  test_load.loadType = "Test type";
  test_load.weight = 123;

  vda5050::Load test_load3;
  test_load.loadId = "Test ID3";

  WHEN("A LoadAdd event is dispatched") {
    auto evt = std::make_shared<vda5050pp::events::LoadAdd>();
    auto evt2 = std::make_shared<vda5050pp::events::LoadAdd>();
    evt->load = test_load;
    evt2->load = test_load2;
    vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(evt);
    vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(evt2);

    THEN("The StatusManager contains the Load") {
      const auto &loads = vda5050pp::core::Instance::ref().getStatusManager().getLoads();
      REQUIRE(loads.size() == 2);
      REQUIRE(loads[0] == test_load);
      REQUIRE(loads[1] == test_load2);
    }

    WHEN("A LoadRemove event is dispatched") {
      auto evt_rem = std::make_shared<vda5050pp::events::LoadRemove>();
      evt_rem->load_id = *test_load.loadId;
      vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(evt_rem);

      THEN("The correct event was removed") {
        const auto &loads = vda5050pp::core::Instance::ref().getStatusManager().getLoads();
        REQUIRE(loads.size() == 1);
        REQUIRE(loads[0] == test_load2);
      }
    }

    WHEN("A LoadGet event is dispatched") {
      auto evt_get = std::make_shared<vda5050pp::events::LoadsGet>();
      auto future = evt_get->getFuture();
      vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(evt_get);

      THEN("The correct event result is returned") {
        REQUIRE(future.wait_for(100ms) == std::future_status::ready);
        auto loads = future.get();
        REQUIRE(loads.size() == 2);
        REQUIRE(loads[0] == test_load);
        REQUIRE(loads[1] == test_load2);
      }
    }

    WHEN("A LoadAlter event is dispatched") {
      bool alter_fn_called = false;
      auto evt_alter = std::make_shared<vda5050pp::events::LoadsAlter>();
      evt_alter->alter_function = [&alter_fn_called,
                                   &test_load3](std::vector<vda5050::Load> &loads) {
        loads[0] = test_load3;
        alter_fn_called = true;
      };
      vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(evt_alter);

      THEN("The alter function will be applied") {
        const auto &loads = vda5050pp::core::Instance::ref().getStatusManager().getLoads();
        REQUIRE(alter_fn_called);
        REQUIRE(loads.size() == 2);
        REQUIRE(loads[0] == test_load3);
        REQUIRE(loads[1] == test_load2);
      }
    }
  }

  WHEN("OperatingModeSet event is dispatched") {
    auto evt_set = std::make_shared<vda5050pp::events::OperatingModeSet>();
    evt_set->operating_mode = vda5050::OperatingMode::SEMIAUTOMATIC;
    vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(evt_set);

    THEN("The op mode is set") {
      REQUIRE(vda5050pp::core::Instance::ref().getStatusManager().getOperatingMode() ==
              evt_set->operating_mode);
    }

    WHEN("OperatingModeAlter is dispatched") {
      auto evt_alter = std::make_shared<vda5050pp::events::OperatingModeAlter>();
      bool alter_fn_called = false;
      vda5050::OperatingMode old_mode;
      evt_alter->alter_function = [&alter_fn_called, &old_mode](vda5050::OperatingMode old) {
        alter_fn_called = true;
        old_mode = old;
        return vda5050::OperatingMode::TEACHIN;
      };
      vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(evt_alter);

      THEN("The function was applied properly") {
        REQUIRE(alter_fn_called);
        REQUIRE(old_mode == evt_set->operating_mode);
        REQUIRE(vda5050pp::core::Instance::ref().getStatusManager().getOperatingMode() ==
                vda5050::OperatingMode::TEACHIN);
      }
    }

    WHEN("OperatingModeGet is dispatched") {
      auto evt_get = std::make_shared<vda5050pp::events::OperatingModeGet>();
      auto future = evt_get->getFuture();
      vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(evt_get);

      THEN("The op mode is returned") {
        REQUIRE(future.wait_for(100ms) == std::future_status::ready);
        REQUIRE(future.get() == evt_set->operating_mode);
      }
    }
  }

  WHEN("BatteryStateSet event is dispatched") {
    auto evt_set = std::make_shared<vda5050pp::events::BatteryStateSet>();
    evt_set->battery_state.batteryCharge = 1234.5;
    vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(evt_set);

    THEN("The battery state is set") {
      REQUIRE(vda5050pp::core::Instance::ref().getStatusManager().getBatteryState() ==
              evt_set->battery_state);
    }

    WHEN("BatteryStateAlter is dispatched") {
      auto evt_alter = std::make_shared<vda5050pp::events::BatteryStateAlter>();
      bool alter_fn_called = false;
      evt_alter->alter_function = [&alter_fn_called](vda5050::BatteryState &bs) {
        alter_fn_called = true;
        bs.batteryHealth = 100;
      };
      vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(evt_alter);

      THEN("The function was applied properly") {
        REQUIRE(alter_fn_called);
        auto bs = vda5050pp::core::Instance::ref().getStatusManager().getBatteryState();
        REQUIRE(bs.batteryCharge == evt_set->battery_state.batteryCharge);
        REQUIRE(bs.batteryHealth == 100);
      }
    }

    WHEN("BatteryStateGet is dispatched") {
      auto evt_get = std::make_shared<vda5050pp::events::BatteryStateGet>();
      auto future = evt_get->getFuture();
      vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(evt_get);

      THEN("The BatteryState is returned") {
        REQUIRE(future.wait_for(100ms) == std::future_status::ready);
        REQUIRE(future.get() == evt_set->battery_state);
      }
    }
  }

  WHEN("RequestNewBase event is dispatched") {
    auto evt = std::make_shared<vda5050pp::events::RequestNewBase>();
    vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(evt);

    THEN("The field is set") {
      vda5050::State buffer;
      buffer.newBaseRequest = false;
      vda5050pp::core::Instance::ref().getStatusManager().dumpTo(buffer);
      REQUIRE(buffer.newBaseRequest);
    }
  }

  vda5050::Error test_error1;
  test_error1.errorDescription = "Error 1";
  vda5050::Error test_error2;
  test_error2.errorDescription = "Error 2";

  WHEN("ErrorsAdd event is dispatched") {
    auto evt_add1 = std::make_shared<vda5050pp::events::ErrorAdd>();
    auto evt_add2 = std::make_shared<vda5050pp::events::ErrorAdd>();
    evt_add1->error = test_error1;
    evt_add2->error = test_error2;

    vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(evt_add1);
    vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(evt_add2);

    THEN("The errors are set") {
      vda5050::State buffer;
      vda5050pp::core::Instance::ref().getStatusManager().dumpTo(buffer);

      REQUIRE(buffer.errors.size() == 2);
      REQUIRE(buffer.errors[0] == test_error1);
      REQUIRE(buffer.errors[1] == test_error2);
    }

    WHEN("ErrorsAlter event is dispatched") {
      auto evt_alter = std::make_shared<vda5050pp::events::ErrorsAlter>();
      bool alter_fn_called = false;
      evt_alter->alter_function = [&alter_fn_called](std::vector<vda5050::Error> &errors) {
        alter_fn_called = true;
        std::swap(errors[0], errors[1]);
      };

      vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(evt_alter);

      THEN("The function was applied properly") {
        vda5050::State buffer;
        vda5050pp::core::Instance::ref().getStatusManager().dumpTo(buffer);

        REQUIRE(alter_fn_called);
        REQUIRE(buffer.errors.size() == 2);
        REQUIRE(buffer.errors[0] == test_error2);
        REQUIRE(buffer.errors[1] == test_error1);
      }
    }
  }

  vda5050::Info test_info1;
  test_info1.infoDescription = "Info 1";
  vda5050::Info test_info2;
  test_info2.infoDescription = "Info 2";

  WHEN("InfoAdd event is dispatched") {
    auto evt_add1 = std::make_shared<vda5050pp::events::InfoAdd>();
    auto evt_add2 = std::make_shared<vda5050pp::events::InfoAdd>();
    evt_add1->info = test_info1;
    evt_add2->info = test_info2;

    vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(evt_add1);
    vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(evt_add2);

    THEN("The infos are set") {
      vda5050::State buffer;
      vda5050pp::core::Instance::ref().getStatusManager().dumpTo(buffer);

      REQUIRE(buffer.information.size() == 2);
      REQUIRE(buffer.information[0] == test_info1);
      REQUIRE(buffer.information[1] == test_info2);
    }

    WHEN("InfosAlter event is dispatched") {
      auto evt_alter = std::make_shared<vda5050pp::events::InfosAlter>();
      bool alter_fn_called = false;
      evt_alter->alter_function = [&alter_fn_called](std::vector<vda5050::Info> &infos) {
        alter_fn_called = true;
        std::swap(infos[0], infos[1]);
      };

      vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(evt_alter);

      THEN("The function was applied properly") {
        vda5050::State buffer;
        vda5050pp::core::Instance::ref().getStatusManager().dumpTo(buffer);

        REQUIRE(alter_fn_called);
        REQUIRE(buffer.information.size() == 2);
        REQUIRE(buffer.information[0] == test_info2);
        REQUIRE(buffer.information[1] == test_info1);
      }
    }
  }
}
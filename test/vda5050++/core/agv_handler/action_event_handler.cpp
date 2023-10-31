// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/core/agv_handler/action_event_handler.h"

#include <eventpp/eventdispatcher.h>
#include <eventpp/eventqueue.h>

#include <catch2/catch.hpp>

#include "vda5050++/core/instance.h"

using namespace std::chrono_literals;

class TestError {};

static std::list<vda5050::AgvAction> getTestAgvActions() {
  vda5050::AgvAction act;  // almost default init
  act.actionType = "test_type";

  return {act};
}

class TestActionHandler : public vda5050pp::handler::BaseActionHandler {
private:
  std::shared_ptr<vda5050pp::handler::ActionState> action_state_;
  std::optional<std::tuple<vda5050::Action, vda5050pp::misc::ActionContext>> validate_params_;
  bool on_start_called_ = false;
  bool on_pause_called_ = false;
  bool on_resume_called_ = false;
  bool on_cancel_called_ = false;
  bool throw_on_validate_ = false;
  bool throw_on_prepare_ = false;
  bool disable_callbacks_ = false;

public:
  int actionStateRefCount() const { return action_state_.use_count(); }

  void reset() {
    validate_params_ = std::nullopt;
    on_start_called_ = false;
    on_pause_called_ = false;
    on_resume_called_ = false;
    on_cancel_called_ = false;
    throw_on_validate_ = false;
    throw_on_prepare_ = false;
    disable_callbacks_ = false;
  }

  bool isOnStartCalled() const { return on_start_called_; }

  bool isOnPauseCalled() const { return on_pause_called_; }

  bool isOnResumeCalled() const { return on_resume_called_; }

  bool isOnCancelCalled() const { return on_cancel_called_; }

  void setThrowOnValidate(bool throw_on_validate) { throw_on_validate_ = throw_on_validate; }

  void setThrowOnPrepare(bool throw_on_prepare) { throw_on_prepare_ = throw_on_prepare; }

  void setDisableCallbacks(bool disable_callbacks) { disable_callbacks_ = disable_callbacks; }

  std::optional<std::tuple<vda5050::Action, vda5050pp::misc::ActionContext>> getValidateParams()
      const {
    return validate_params_;
  }

  bool match(const vda5050::Action &) const noexcept(false) override { return true; }

  vda5050pp::handler::ActionCallbacks prepare(
      std::shared_ptr<vda5050pp::handler::ActionState> action_state,
      std::shared_ptr<vda5050pp::handler::ParametersMap> parameters) noexcept(false) override {
    this->action_state_ = action_state;

    vda5050pp::handler::ActionCallbacks callbacks;
    if (!disable_callbacks_) {
      callbacks.on_start = [this](auto &) { on_start_called_ = true; };
      callbacks.on_pause = [this](auto &) { on_pause_called_ = true; };
      callbacks.on_resume = [this](auto &) { on_resume_called_ = true; };
      callbacks.on_cancel = [this](auto &) { on_cancel_called_ = true; };
    }

    if (throw_on_prepare_) {
      throw TestError();
    }

    return callbacks;
  }

  vda5050pp::handler::ValidationResult validate(
      const vda5050::Action &action,
      vda5050pp::misc::ActionContext context) noexcept(false) override {
    this->validate_params_ = std::make_tuple(action, context);

    if (throw_on_validate_) {
      throw TestError();
    }

    return {};
  }

  std::list<vda5050::AgvAction> getActionDescription() const noexcept(false) override {
    return getTestAgvActions();
  }
};

TEST_CASE("core::agv_handler::ActionEventHandler", "[core][agv_handler]") {
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().refEventManagerOptions().synchronous_event_dispatch = true;
  cfg.refGlobalConfig().useWhiteList();
  cfg.refGlobalConfig().bwListModule(vda5050pp::core::module_keys::k_action_event_handler_key);
  vda5050pp::core::Instance::reset();
  auto instance = vda5050pp::core::Instance::init(cfg).lock();

  auto handler = std::make_shared<TestActionHandler>();

  instance->addActionHandler(handler);

  auto evt_handler = std::dynamic_pointer_cast<vda5050pp::core::agv_handler::ActionEventHandler>(
      vda5050pp::core::Instance::lookupModule(
          vda5050pp::core::module_keys::k_action_event_handler_key)
          .lock());

  auto action = std::make_shared<vda5050::Action>();
  action->actionType = "test_type";
  action->actionId = "test_id";

  WHEN("An action validate event is send") {
    auto evt_valid = std::make_shared<vda5050pp::events::ActionValidate>();
    evt_valid->action = action;
    evt_valid->keep = true;
    evt_valid->context = vda5050pp::misc::ActionContext::k_instant;

    instance->getActionEventManager().dispatch(evt_valid);

    THEN("The validation params are correct") {
      auto params = handler->getValidateParams();
      REQUIRE(params.has_value());
      const auto &[act, ctx] = params.value();
      REQUIRE(act == *action);
      REQUIRE(ctx == evt_valid->context);
    }

    WHEN("The action is prepared and the handler throws") {
      auto evt_prepare = std::make_shared<vda5050pp::events::ActionPrepare>();
      evt_prepare->action = action;
      handler->setThrowOnPrepare(true);
      THEN("The error is propagated") {
        REQUIRE_THROWS_AS(instance->getActionEventManager().dispatch(evt_prepare, true), TestError);
      }
    }

    WHEN("The action is prepared without any callbacks") {
      auto evt_prepare = std::make_shared<vda5050pp::events::ActionPrepare>();
      evt_prepare->action = action;
      handler->setDisableCallbacks(true);

      WHEN("An action cancel event is dispatched") {
        auto evt_cancel = std::make_shared<vda5050pp::events::ActionCancel>();
        evt_cancel->action_id = action->actionId;

        THEN("An exception is thrown on dispatch") {
          REQUIRE_THROWS_AS(instance->getActionEventManager().dispatch(evt_cancel, true),
                            vda5050pp::VDA5050PPCallbackNotSet);
        }
      }
      WHEN("An action pause event is dispatched") {
        auto evt_cancel = std::make_shared<vda5050pp::events::ActionPause>();
        evt_cancel->action_id = action->actionId;

        THEN("An exception is thrown on dispatch") {
          REQUIRE_THROWS_AS(instance->getActionEventManager().dispatch(evt_cancel, true),
                            vda5050pp::VDA5050PPCallbackNotSet);
        }
      }
      WHEN("An action resume event is dispatched") {
        auto evt_cancel = std::make_shared<vda5050pp::events::ActionResume>();
        evt_cancel->action_id = action->actionId;

        THEN("An exception is thrown on dispatch") {
          REQUIRE_THROWS_AS(instance->getActionEventManager().dispatch(evt_cancel, true),
                            vda5050pp::VDA5050PPCallbackNotSet);
        }
      }
      WHEN("An action start event is dispatched") {
        auto evt_cancel = std::make_shared<vda5050pp::events::ActionStart>();
        evt_cancel->action_id = action->actionId;

        THEN("An exception is thrown on dispatch") {
          REQUIRE_THROWS_AS(instance->getActionEventManager().dispatch(evt_cancel, true),
                            vda5050pp::VDA5050PPCallbackNotSet);
        }
      }
    }

    WHEN("The action is prepared") {
      auto evt_prepare = std::make_shared<vda5050pp::events::ActionPrepare>();
      evt_prepare->action = action;
      instance->getActionEventManager().dispatch(evt_prepare);

      THEN("The action state is set") { REQUIRE(handler->actionStateRefCount() == 2); }

      WHEN("The action is started") {
        auto evt_start = std::make_shared<vda5050pp::events::ActionStart>();
        evt_start->action_id = action->actionId;
        handler->reset();
        instance->getActionEventManager().dispatch(evt_start);

        THEN("on start was called") {
          REQUIRE(handler->isOnStartCalled());
          REQUIRE_FALSE(handler->isOnPauseCalled());
          REQUIRE_FALSE(handler->isOnResumeCalled());
          REQUIRE_FALSE(handler->isOnCancelCalled());
        }
      }

      WHEN("The action is paused") {
        auto evt_pause = std::make_shared<vda5050pp::events::ActionPause>();
        evt_pause->action_id = action->actionId;
        handler->reset();
        instance->getActionEventManager().dispatch(evt_pause);

        THEN("on pause was called") {
          REQUIRE_FALSE(handler->isOnStartCalled());
          REQUIRE(handler->isOnPauseCalled());
          REQUIRE_FALSE(handler->isOnResumeCalled());
          REQUIRE_FALSE(handler->isOnCancelCalled());
        }
      }

      WHEN("The action is resumed") {
        auto evt_resume = std::make_shared<vda5050pp::events::ActionResume>();
        evt_resume->action_id = action->actionId;
        handler->reset();
        instance->getActionEventManager().dispatch(evt_resume);

        THEN("on resume was called") {
          REQUIRE_FALSE(handler->isOnStartCalled());
          REQUIRE_FALSE(handler->isOnPauseCalled());
          REQUIRE(handler->isOnResumeCalled());
          REQUIRE_FALSE(handler->isOnCancelCalled());
        }
      }

      WHEN("The action is canceled") {
        auto evt_cancel = std::make_shared<vda5050pp::events::ActionCancel>();
        evt_cancel->action_id = action->actionId;
        handler->reset();
        instance->getActionEventManager().dispatch(evt_cancel);

        THEN("on cancel was called") {
          REQUIRE_FALSE(handler->isOnStartCalled());
          REQUIRE_FALSE(handler->isOnPauseCalled());
          REQUIRE_FALSE(handler->isOnResumeCalled());
          REQUIRE(handler->isOnCancelCalled());
        }
      }

      WHEN("The action is forgotten") {
        auto evt_forget = std::make_shared<vda5050pp::events::ActionForget>();
        evt_forget->action_id = action->actionId;
        handler->reset();
        instance->getActionEventManager().dispatch(evt_forget);

        THEN("The action state is removed") { REQUIRE(handler->actionStateRefCount() == 1); }
      }

      WHEN("The action is listed") {
        auto evt_list = std::make_shared<vda5050pp::events::ActionList>();
        auto result = evt_list->getFuture();
        handler->reset();
        instance->getActionEventManager().dispatch(evt_list);

        THEN("The result contains the correct listing") {
          REQUIRE(result.wait_for(0s) == std::future_status::ready);
          REQUIRE(*result.get() == getTestAgvActions());
        }
      }
    }
  }

  WHEN("An empty validate event is sent") {
    auto evt = std::make_shared<vda5050pp::events::ActionValidate>();
    evt->action = nullptr;

    REQUIRE_THROWS_AS(instance->getActionEventManager().dispatch(evt, true),
                      vda5050pp::VDA5050PPInvalidEventData);
    evt = nullptr;
    REQUIRE_THROWS_AS(instance->getActionEventManager().dispatch(evt, true),
                      vda5050pp::VDA5050PPInvalidEventData);
  }

  WHEN("An acquired validate event is sent") {
    auto evt = std::make_shared<vda5050pp::events::ActionValidate>();
    evt->action = std::make_shared<vda5050::Action>();
    auto tkn = evt->acquireResultToken();

    REQUIRE_NOTHROW(instance->getActionEventManager().dispatch(evt, true));
  }

  WHEN("An action validate event is sent and the handler throws") {
    auto evt = std::make_shared<vda5050pp::events::ActionValidate>();
    evt->action = std::make_shared<vda5050::Action>();
    handler->setThrowOnValidate(true);

    THEN("The exception is propagated") {
      REQUIRE_THROWS_AS(instance->getActionEventManager().dispatch(evt, true), TestError);
    }
  }

  WHEN("An empty action list event is sent") {
    std::shared_ptr<vda5050pp::events::ActionList> evt;

    REQUIRE_THROWS_AS(instance->getActionEventManager().dispatch(evt, true),
                      vda5050pp::VDA5050PPInvalidEventData);
  }

  WHEN("An acquired action list event is sent") {
    auto evt = std::make_shared<vda5050pp::events::ActionList>();
    auto tkn = evt->acquireResultToken();

    REQUIRE_NOTHROW(instance->getActionEventManager().dispatch(evt, true));
  }

  WHEN("An empty action prepare event is sent") {
    std::shared_ptr<vda5050pp::events::ActionPrepare> evt;

    REQUIRE_THROWS_AS(instance->getActionEventManager().dispatch(evt, true),
                      vda5050pp::VDA5050PPInvalidEventData);
  }

  WHEN("An unknown action prepare event is sent") {
    auto evt = std::make_shared<vda5050pp::events::ActionPrepare>();
    auto a = std::make_shared<vda5050::Action>();
    a->actionType = "unknown";
    evt->action = a;

    REQUIRE_NOTHROW(instance->getActionEventManager().dispatch(evt, true));
  }

  WHEN("An empty action cancel event is sent") {
    std::shared_ptr<vda5050pp::events::ActionCancel> evt;

    REQUIRE_THROWS_AS(instance->getActionEventManager().dispatch(evt, true),
                      vda5050pp::VDA5050PPInvalidEventData);
  }

  WHEN("An unknown action cancel event is sent") {
    auto evt = std::make_shared<vda5050pp::events::ActionCancel>();
    evt->action_id = "unknown";

    REQUIRE_NOTHROW(instance->getActionEventManager().dispatch(evt, true));
  }

  WHEN("An empty action pause event is sent") {
    std::shared_ptr<vda5050pp::events::ActionPause> evt;

    REQUIRE_THROWS_AS(instance->getActionEventManager().dispatch(evt, true),
                      vda5050pp::VDA5050PPInvalidEventData);
  }

  WHEN("An unknown action pause event is sent") {
    auto evt = std::make_shared<vda5050pp::events::ActionPause>();
    evt->action_id = "unknown";

    REQUIRE_NOTHROW(instance->getActionEventManager().dispatch(evt, true));
  }

  WHEN("An empty action start event is sent") {
    std::shared_ptr<vda5050pp::events::ActionStart> evt;

    REQUIRE_THROWS_AS(instance->getActionEventManager().dispatch(evt, true),
                      vda5050pp::VDA5050PPInvalidEventData);
  }

  WHEN("An unknown action start event is sent") {
    auto evt = std::make_shared<vda5050pp::events::ActionStart>();
    evt->action_id = "unknown";

    REQUIRE_NOTHROW(instance->getActionEventManager().dispatch(evt, true));
  }

  WHEN("An empty action resume event is sent") {
    std::shared_ptr<vda5050pp::events::ActionResume> evt;

    REQUIRE_THROWS_AS(instance->getActionEventManager().dispatch(evt, true),
                      vda5050pp::VDA5050PPInvalidEventData);
  }

  WHEN("An unknown action resume event is sent") {
    auto evt = std::make_shared<vda5050pp::events::ActionResume>();
    evt->action_id = "unknown";

    REQUIRE_NOTHROW(instance->getActionEventManager().dispatch(evt, true));
  }

  WHEN("An empty action forget event is sent") {
    std::shared_ptr<vda5050pp::events::ActionForget> evt;

    REQUIRE_THROWS_AS(instance->getActionEventManager().dispatch(evt, true),
                      vda5050pp::VDA5050PPInvalidEventData);
  }

  WHEN("An unkown action forget event is sent") {
    auto evt = std::make_shared<vda5050pp::events::ActionForget>();
    evt->action_id = "unknown";

    REQUIRE_NOTHROW(instance->getActionEventManager().dispatch(evt, true));
  }
}
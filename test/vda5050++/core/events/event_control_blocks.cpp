// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/core/events/event_control_blocks.h"

#include <catch2/catch.hpp>

#include "vda5050++/core/generic_event_manager.h"
#include "vda5050++/events/event_type.h"

using namespace vda5050pp::core;

struct TestEvent : public vda5050pp::events::Event<int> {};

struct Event1 : public vda5050pp::events::EventId<TestEvent, 1> {};
struct Event2 : public vda5050pp::events::EventId<TestEvent, 2> {};
struct Event3 : public vda5050pp::events::EventId<TestEvent, 3> {};

using EventManager = GenericEventManager<TestEvent>;

template <typename EventT> class TestLatch : public events::EventLatch<EventT> {
private:
  bool force_return_ = false;
  int done_called_ = 0;
  int predicate_called_ = 0;

public:
  TestLatch(EventManager::ScopedSubscriber &&sub) : events::EventLatch<EventT>(std::move(sub)) {}

  bool predicate(std::shared_ptr<EventT> evt) override {
    this->predicate_called_++;
    return this->force_return_;
  }

  void done() override { this->done_called_++; }

  int getDoneCalled() const { return this->done_called_; }

  int getPredicateCalled() const { return this->predicate_called_; }

  void setForceReturn(bool fr) { this->force_return_ = fr; }
};

TEST_CASE("core::events::EventLatch behaviour", "[core][events]") {
  vda5050pp::config::EventManagerOptions opts;
  opts.synchronous_event_dispatch = true;

  GenericEventManager<TestEvent> mgr(opts);
  TestLatch<Event1> latch(std::move(mgr.getScopedSubscriber()));

  WHEN("When the latch is not enabled") {
    WHEN("When an event is dispatched") {
      mgr.dispatch(std::make_shared<Event1>());

      THEN("The predicate was not called") { REQUIRE(latch.getPredicateCalled() == 0); }
    }
  }
  WHEN("When the latch is enabled") {
    latch.enable();

    WHEN("When an event is dispatched and the predicate returns false") {
      mgr.dispatch(std::make_shared<Event1>());
      latch.setForceReturn(false);

      THEN("The predicate was called") { REQUIRE(latch.getPredicateCalled() == 1); }
      THEN("The done was not called") { REQUIRE(latch.getDoneCalled() == 0); }
    }

    WHEN("When an event is dispatched and the predicate returns true") {
      latch.setForceReturn(true);
      mgr.dispatch(std::make_shared<Event1>());

      THEN("The predicate was called") { REQUIRE(latch.getPredicateCalled() == 1); }
      THEN("The done was called") { REQUIRE(latch.getDoneCalled() == 1); }
    }
  }
}

TEST_CASE("core::events::EventControlChain behaviour", "[core][events]") {
  vda5050pp::config::EventManagerOptions opts;
  opts.synchronous_event_dispatch = true;
  int teardown_called = 0;

  GenericEventManager<TestEvent> mgr(opts);
  auto latch_1 = std::make_shared<TestLatch<Event1>>(std::move(mgr.getScopedSubscriber()));
  auto latch_2 = std::make_shared<TestLatch<Event2>>(std::move(mgr.getScopedSubscriber()));
  auto latch_3 = std::make_shared<TestLatch<Event1>>(std::move(mgr.getScopedSubscriber()));
  latch_1->setForceReturn(true);
  latch_2->setForceReturn(true);
  latch_3->setForceReturn(true);

  events::EventControlChain chain;
  chain.add(latch_1);
  chain.add(latch_2);
  chain.add(latch_3);
  chain.setTeardown([&teardown_called] { teardown_called++; });

  WHEN("The chain is not enabled and any event is dispatched") {
    mgr.dispatch(std::make_shared<Event1>());
    mgr.dispatch(std::make_shared<Event2>());

    THEN("No latch was called") {
      REQUIRE(latch_1->getPredicateCalled() == 0);
      REQUIRE(latch_2->getPredicateCalled() == 0);
      REQUIRE(latch_3->getPredicateCalled() == 0);
    }
  }

  WHEN("The chain is enabled") {
    chain.enable();

    WHEN("Event1 is dispatched") {
      mgr.dispatch(std::make_shared<Event1>());
      THEN("The predicate was called the first latch only") {
        REQUIRE(latch_1->getPredicateCalled() == 1);
        REQUIRE(latch_2->getPredicateCalled() == 0);
        REQUIRE(latch_3->getPredicateCalled() == 0);
      }
      THEN("The first latch is done") {
        REQUIRE(latch_1->getDoneCalled() == 1);
        REQUIRE(latch_2->getDoneCalled() == 0);
        REQUIRE(latch_3->getDoneCalled() == 0);
      }
      THEN("Teardown of the first latch was called") { REQUIRE(latch_1.use_count() == 1); }

      WHEN("Event2 is dispatched") {
        mgr.dispatch(std::make_shared<Event2>());
        THEN("The predicate was called for the second latch only") {
          REQUIRE(latch_1->getPredicateCalled() == 1);
          REQUIRE(latch_2->getPredicateCalled() == 1);
          REQUIRE(latch_3->getPredicateCalled() == 0);
        }
        THEN("The second latch is done") {
          REQUIRE(latch_1->getDoneCalled() == 1);
          REQUIRE(latch_2->getDoneCalled() == 1);
          REQUIRE(latch_3->getDoneCalled() == 0);
        }
        THEN("Teardown of the second latch was called") { REQUIRE(latch_2.use_count() == 1); }
        WHEN("Event1 is dispatched") {
          mgr.dispatch(std::make_shared<Event1>());
          THEN("The predicate was called the third latch only") {
            REQUIRE(latch_1->getPredicateCalled() == 2);
            REQUIRE(latch_2->getPredicateCalled() == 1);
            REQUIRE(latch_3->getPredicateCalled() == 1);
          }
          THEN("The third latch is done") {
            REQUIRE(latch_1->getDoneCalled() == 2);
            REQUIRE(latch_2->getDoneCalled() == 1);
            REQUIRE(latch_3->getDoneCalled() == 1);
          }
          THEN("Teardown is called") { REQUIRE(teardown_called == 1); }
          THEN("Teardown of the third latch was called") { REQUIRE(latch_3.use_count() == 1); }
        }
      }
      WHEN("Event2 is dispatched") {
        mgr.dispatch(std::make_shared<Event2>());
        THEN("No new predicate was called") {
          REQUIRE(latch_1->getPredicateCalled() == 1);
          REQUIRE(latch_2->getPredicateCalled() == 1);
          REQUIRE(latch_3->getPredicateCalled() == 0);
        }
        THEN("The third latch is not done") {
          REQUIRE(latch_1->getDoneCalled() == 1);
          REQUIRE(latch_2->getDoneCalled() == 1);
          REQUIRE(latch_3->getDoneCalled() == 0);
        }
        THEN("Teardown was not called") { REQUIRE(teardown_called == 0); }
      }
    }
    WHEN("Event2 is dispatched") {
      mgr.dispatch(std::make_shared<Event2>());
      THEN("The predicate was called for no latch") {
        REQUIRE(latch_1->getPredicateCalled() == 0);
        REQUIRE(latch_2->getPredicateCalled() == 0);
        REQUIRE(latch_3->getPredicateCalled() == 0);
      }
      THEN("No latch is done") {
        REQUIRE(latch_1->getDoneCalled() == 0);
        REQUIRE(latch_2->getDoneCalled() == 0);
        REQUIRE(latch_3->getDoneCalled() == 0);
      }
    }
  }
}

TEST_CASE("core::events::EventControlAlternative behaviour", "[core][events]") {
  vda5050pp::config::EventManagerOptions opts;
  opts.synchronous_event_dispatch = true;
  int teardown_called = 0;

  GenericEventManager<TestEvent> mgr(opts);
  auto latch_1 = std::make_shared<TestLatch<Event1>>(std::move(mgr.getScopedSubscriber()));
  auto latch_2 = std::make_shared<TestLatch<Event2>>(std::move(mgr.getScopedSubscriber()));
  auto latch_3 = std::make_shared<TestLatch<Event3>>(std::move(mgr.getScopedSubscriber()));
  latch_1->setForceReturn(true);
  latch_2->setForceReturn(true);
  latch_3->setForceReturn(true);

  events::EventControlAlternative alternative;
  alternative.add(latch_1);
  alternative.add(latch_2);
  alternative.add(latch_3);
  alternative.setTeardown([&teardown_called] { teardown_called++; });

  WHEN("The alternative is not enabled and any event is dispatched") {
    mgr.dispatch(std::make_shared<Event1>());
    mgr.dispatch(std::make_shared<Event2>());

    THEN("No latch was called") {
      REQUIRE(latch_1->getPredicateCalled() == 0);
      REQUIRE(latch_2->getPredicateCalled() == 0);
      REQUIRE(latch_3->getPredicateCalled() == 0);
    }
  }

  WHEN("The alternative is enabled") {
    alternative.enable();

    WHEN("Event1 is dispatched") {
      mgr.dispatch(std::make_shared<Event1>());
      THEN("The predicate was called the first latch only") {
        REQUIRE(latch_1->getPredicateCalled() == 1);
        REQUIRE(latch_2->getPredicateCalled() == 0);
        REQUIRE(latch_3->getPredicateCalled() == 0);
      }
      THEN("The first latch is done") {
        REQUIRE(latch_1->getDoneCalled() == 1);
        REQUIRE(latch_2->getDoneCalled() == 0);
        REQUIRE(latch_3->getDoneCalled() == 0);
      }
      THEN("Teardown of the alternative was called once") { REQUIRE(teardown_called == 1); }
      WHEN("Any Event is dispatched") {
        mgr.dispatch(std::make_shared<Event1>());
        mgr.dispatch(std::make_shared<Event2>());
        mgr.dispatch(std::make_shared<Event3>());

        THEN("Teardown was not called again") { REQUIRE(teardown_called == 1); }
      }
    }

    WHEN("Event2 is dispatched") {
      mgr.dispatch(std::make_shared<Event2>());
      THEN("The predicate was called the second latch only") {
        REQUIRE(latch_1->getPredicateCalled() == 0);
        REQUIRE(latch_2->getPredicateCalled() == 1);
        REQUIRE(latch_3->getPredicateCalled() == 0);
      }
      THEN("The first latch is done") {
        REQUIRE(latch_1->getDoneCalled() == 0);
        REQUIRE(latch_2->getDoneCalled() == 1);
        REQUIRE(latch_3->getDoneCalled() == 0);
      }
      THEN("Teardown of the alternative was called once") { REQUIRE(teardown_called == 1); }
    }

    WHEN("Event3 is dispatched") {
      mgr.dispatch(std::make_shared<Event3>());
      THEN("The predicate was called the third latch only") {
        REQUIRE(latch_1->getPredicateCalled() == 0);
        REQUIRE(latch_2->getPredicateCalled() == 0);
        REQUIRE(latch_3->getPredicateCalled() == 1);
      }
      THEN("The first latch is done") {
        REQUIRE(latch_1->getDoneCalled() == 0);
        REQUIRE(latch_2->getDoneCalled() == 0);
        REQUIRE(latch_3->getDoneCalled() == 1);
      }
      THEN("Teardown of the alternative was called once") { REQUIRE(teardown_called == 1); }
    }
  }
}
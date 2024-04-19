//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef INCLUDE_PUBLIC_VDA5050_2B_2B_HANDLE_H_
#define INCLUDE_PUBLIC_VDA5050_2B_2B_HANDLE_H_

#include "vda5050++/config.h"
#include "vda5050++/handler/base_action_handler.h"
#include "vda5050++/handler/base_navigation_handler.h"
#include "vda5050++/handler/base_query_handler.h"
#include "vda5050++/sinks/navigation_sink.h"
#include "vda5050++/sinks/status_sink.h"

#ifdef LIBVDA5050PP_EXPOSE_LOGGER
#include <spdlog/spdlog.h>
#endif

namespace vda5050pp {

///
///\brief The Handle class is used to control the library.
/// It can be constructed and deconstructed at will. The library can be controlled
/// by the member functions.
///
class Handle {
public:
  /// \brief Initialize zhe library.
  /// \param config The configuration to use
  /// The library will try to establish an MQTT Connection and will be
  /// ready to process messages.
  void initialize(const vda5050pp::Config &config) const noexcept(true);

  /// \brief Register a new ActionHandler. Upon receiving actions an appropiate
  /// registered handler will be used.
  /// \param action_handler a pointer to the ActionHandler
  /// NOTE: An ActionHandler simply wraps ActionEvents. As a user you could use a
  /// vda5050pp::events::EventHandle.
  void registerActionHandler(
      std::shared_ptr<vda5050pp::handler::BaseActionHandler> action_handler) const noexcept(false);

  ///
  ///\brief Register a new NavigationHandler and overwrite the old one. As soon, as the AGV is
  /// instructed to move, this handler is called.
  /// There can only be one NavigationHandler at a time, such that the old one is overwritten.
  ///\param navigation_handler a pointer to the new NavigationHandler
  /// NOTE: An ActionHandler simply wraps NavigationEvents. As a user you could use a
  /// vda5050pp::events::EventHandle.
  ///
  void registerNavigationHandler(
      std::shared_ptr<vda5050pp::handler::BaseNavigationHandler> navigation_handler) const
      noexcept(false);

  ///
  ///\brief Register a new QueryHandler and overwrite the old one. As soon, as the library needs
  /// some info , this handler is called.
  /// There can only be one QueryHandler at a time, such that the old one is overwritten.
  ///\param query_handler a pointer to the new QueryHandler
  /// NOTE: An ActionHandler simply wraps QueryEvents. As a user you could use a
  /// vda5050pp::events::EventHandle.
  ///
  void registerQueryHandler(
      std::shared_ptr<vda5050pp::handler::BaseQueryHandler> query_handler) const noexcept(false);

  ///
  ///\brief Get a new StatusSink. It can be used to pass the AGV status into the library.
  ///
  ///\return vda5050pp::sinks::StatusSink
  ///
  vda5050pp::sinks::StatusSink getStatusSink() const;

  ///
  ///\brief Get a new NavigationSink. It can be used to pass the AGV odometry into the library.
  ///
  ///\return vda5050pp::sinks::NavigationSink
  ///
  vda5050pp::sinks::NavigationSink getNavigationSink() const;

#ifdef LIBVDA5050PP_EXPOSE_LOGGER
  ///
  ///\brief Get a spdlog logger created in the libraries's registry. If the logger does not exist,
  /// it will return a nullptr. This is intended to access the logger's sinks in user-code
  /// to prevent synchronization issues and access the log-files of the library.
  ///
  /// Available names:
  //   "agv_handler"
  //   "interpreter"
  //   "messages"
  //   "order"
  //   "state"
  //   "mqtt"
  //   "validation"
  //   "events"
  //   "instance" (global library logger)
  //   "factsheet"
  ///
  ///\param key the logger's key
  ///\return std::shared_ptr<spdlog::logger> the spdlog logger (can be nullptr if not found).
  ///
  std::shared_ptr<spdlog::logger> getLogger(std::string_view key) const;
#endif

  /// @brief Shutdown the library. Send an offline message and disconnect from the MQTT Broker.
  void shutdown() const noexcept(false);
};

}  // namespace vda5050pp

#endif  // INCLUDE_PUBLIC_VDA5050_2B_2B_HANDLE_H_

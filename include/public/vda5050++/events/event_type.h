//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_EVENTS_EVENT_TYPE_H_
#define PUBLIC_VDA5050_2B_2B_EVENTS_EVENT_TYPE_H_

namespace vda5050pp::events {

/// @brief The non-templated EventBase used for inheritance checks
struct EventBase {};

/// @brief The Event Base. Provides Interface for ID retrieving.
///
/// It is currently used for internal events. Public events might inherit from it in future
/// versions.
///
/// @tparam IdT the Event Identifier Type (usually an enum)
template <typename IdT> struct Event : EventBase {
  /// @brief Event Identifier Type (usually an enum)
  using EventIdType = IdT;

  virtual ~Event() = default;

  /// @brief Get the id of the specific event instance
  /// @return event id
  virtual EventIdType getId() const = 0;
};

///
///\brief The EventId type. Used to assign an ID to an specialized Event.
///
///\tparam Base The Event Base
///\tparam t The Event Identifier Type value
///
template <typename Base, typename Base::EventIdType t> struct EventId : public Base {
  using EventBase = Base;

  ~EventId() override = default;

  /// @brief Retrieve the id at compile time
  /// @return id
  static constexpr typename Base::EventIdType id() { return t; }

  /// @brief Get the Event Id if this instance.
  /// @return id
  typename Base::EventIdType getId() const override { return EventId<Base, t>::id(); }
};

}  // namespace vda5050pp::events

#endif  // PUBLIC_VDA5050_2B_2B_EVENTS_EVENT_TYPE_H_

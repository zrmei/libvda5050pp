// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
//

#ifndef CORE_COMMON_MATH_LINEAR_PATH_LENGTH_CALCULATOR_H_
#define CORE_COMMON_MATH_LINEAR_PATH_LENGTH_CALCULATOR_H_

#include <optional>
#include <utility>

#include "vda5050++/core/common/math/geometry.h"

namespace vda5050pp::core::common::math {

///
/// \brief Consumes a path of vertexes and yields the linear interpolated length of the path
///
/// \tparam Num the number type
///
template <typename Num> class LinearPathLengthCalculator {
private:
  Num accum_ = 0;
  std::optional<math::Vector2<Num>> last_vertex_;

public:
  LinearPathLengthCalculator() noexcept(true) = default;
  explicit LinearPathLengthCalculator(Num initial_length) noexcept(true) : accum_(initial_length) {}
  explicit LinearPathLengthCalculator(const Vector2<Num> &initial_vertex_) noexcept(true)
      : last_vertex_(initial_vertex_) {}
  explicit LinearPathLengthCalculator(Vector2<Num> &&initial_vertex_) noexcept(true)
      : last_vertex_(std::move(initial_vertex_)) {}

  ///
  /// \brief Get the length of the path
  ///
  /// \return Num path length
  ///
  Num getLength() const noexcept(true) { return this->accum_; }

  ///
  /// \brief Add any number of vertexes to the path
  ///
  /// \param vertices
  ///
  void addVertices(std::initializer_list<Vector2<Num>> vertices) noexcept(true) {
    for (const auto &vx : vertices) {
      this->addVertex(vx);
    }
  }

  ///
  /// \brief Add a single vertex to the path
  ///
  /// \param vertex
  ///
  void addVertex(const Vector2<Num> &vertex) noexcept(true) {
    if (this->last_vertex_.has_value()) {
      this->accum_ += math::euclidDistance(vertex, *this->last_vertex_);
    }

    this->last_vertex_ = vertex;
  }

  ///
  /// \brief Reset the path
  /// \param zero the value to zero the accumulator to
  ///
  void reset(Num zero = 0) noexcept(true) {
    this->accum_ = zero;
    this->last_vertex_.reset();
  }
};

}  // namespace vda5050pp::core::common::math

#endif  // CORE_COMMON_MATH_LINEAR_PATH_LENGTH_CALCULATOR_H_

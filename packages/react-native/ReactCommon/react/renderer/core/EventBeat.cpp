/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "EventBeat.h"

#include <utility>

namespace facebook::react {

EventBeat::EventBeat(
    std::shared_ptr<OwnerBox> ownerBox,
    RuntimeExecutor runtimeExecutor)
    : ownerBox_(std::move(ownerBox)),
      runtimeExecutor_(std::move(runtimeExecutor)) {}

void EventBeat::request() const {
  isRequested_ = true;
}

void EventBeat::setBeatCallback(BeatCallback beatCallback) {
  beatCallback_ = std::move(beatCallback);
}

void EventBeat::induce() const {
  if (!isRequested_ || isBeatCallbackScheduled_) {
    return;
  }

  isRequested_ = false;
  isBeatCallbackScheduled_ = true;

  runtimeExecutor_([this, ownerBox = ownerBox_](jsi::Runtime& runtime) {
    auto owner = ownerBox->owner.lock();
    if (!owner) {
      return;
    }

    isBeatCallbackScheduled_ = false;
    if (beatCallback_) {
      beatCallback_(runtime);
    }
  });
}

} // namespace facebook::react

/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <jsi/jsi.h>
#include <react/renderer/core/EventBeat.h>
#include <react/renderer/uimanager/primitives.h>

#include "AndroidEventBeat.h"

namespace facebook::react {

AndroidEventBeat::AndroidEventBeat(
    std::shared_ptr<OwnerBox> ownerBox,
    EventBeatManager* eventBeatManager,
    RuntimeExecutor runtimeExecutor,
    jni::global_ref<jobject> javaUIManager)
    : EventBeat(std::move(ownerBox), std::move(runtimeExecutor)),
      eventBeatManager_(eventBeatManager),
      javaUIManager_(std::move(javaUIManager)) {
  eventBeatManager->addObserver(*this);
}

AndroidEventBeat::~AndroidEventBeat() {
  eventBeatManager_->removeObserver(*this);
}

void AndroidEventBeat::tick() const {
  induce();
}

void AndroidEventBeat::request() const {
  bool alreadyRequested = isRequested_;
  EventBeat::request();
  if (!alreadyRequested) {
    // Notifies java side that an event will be dispatched (e.g. LayoutEvent)
    static auto onRequestEventBeat =
        jni::findClassStatic("com/facebook/react/fabric/FabricUIManager")
            ->getMethod<void()>("onRequestEventBeat");
    onRequestEventBeat(javaUIManager_);
  }
}

} // namespace facebook::react

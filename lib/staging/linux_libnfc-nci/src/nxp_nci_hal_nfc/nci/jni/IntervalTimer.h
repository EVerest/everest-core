/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 *  Asynchronous interval timer.
 */

#include <time.h>

class IntervalTimer {
 public:
  typedef void (*TIMER_FUNC)(union sigval);

  IntervalTimer();
  ~IntervalTimer();
  bool set(int ms, TIMER_FUNC cb);
  void kill();
  bool create(TIMER_FUNC);

 private:
  timer_t mTimerId;
  TIMER_FUNC mCb;
};

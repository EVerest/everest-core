/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
const { evlog, boot_module } = require('everestjs');

boot_module(() => {
  // nothing to initialize for this module
}).then((mod) => {
  evlog.info(`Providing phase_count: ${mod.config.impl.main.phase_count}`);
  evlog.info(`Providing max_current: ${mod.config.impl.main.max_current}`);
  mod.provides.main.publish.phase_count(mod.config.impl.main.phase_count);
  mod.provides.main.publish.max_current(mod.config.impl.main.max_current);
});

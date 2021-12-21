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

boot_module().then((mod) => {
  setInterval(() => {
    const data = {
      token: mod.config.impl.main.token,
      type: mod.config.impl.main.type,
      timeout: mod.config.impl.main.timeout,
    };
    evlog.info('Publishing new dummy token: ', data);
    mod.provides.main.publish.token(data);
  }, mod.config.impl.main.sleep * 1000);
});

//
// Copyright [2018] [Comcast NBCUniversal]
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#ifndef __SHELL_SERVICE_H__
#define __SHELL_SERVICE_H__

#include "../defs.h"
#include "../rpcserver.h"

class ShellService : public BasicRpcService
{
public:
  ShellService();
  virtual ~ShellService();
  virtual void init(cJSON const* conf, RpcNotificationFunction const& callback) override;
  virtual cJSON* invokeMethod(std::string const& name, cJSON const* req) override;
private:
  cJSON*  m_commands;
};

#endif

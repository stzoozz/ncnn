// Tencent is pleased to support the open source community by making ncnn available.
//
// Copyright (C) 2021 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#include "fuse_module_pass.h"

namespace pnnx {

class BatchNorm1d : public FuseModulePass
{
public:
    const char* match_type_str() const
    {
        return "__torch__.torch.nn.modules.batchnorm.BatchNorm1d";
    }

    const char* type_str() const
    {
        return "nn.BatchNorm1d";
    }

    void write(Operator* op, const TorchGraphProxy& graph, const TorchModuleProxy& mod) const
    {
        const TorchNodeProxy* bn = graph.find_node_by_kind("aten::batch_norm");

        const TorchTensorProxy& running_mean = mod.attr("running_mean");
        const TorchTensorProxy& running_var = mod.attr("running_var");

        op->params["num_features"] = running_mean.size(0);
        op->params["eps"] = bn->namedInput("eps");
        op->params["affine"] = mod.hasattr("weight") && mod.hasattr("bias");

        op->attrs["running_mean"] = running_mean;
        op->attrs["running_var"] = running_var;
        if (mod.hasattr("weight") && mod.hasattr("bias"))
        {
            op->attrs["weight"] = mod.attr("weight");
            op->attrs["bias"] = mod.attr("bias");
        }
    }
};

REGISTER_GLOBAL_PNNX_FUSE_MODULE_PASS(BatchNorm1d)

} // namespace pnnx

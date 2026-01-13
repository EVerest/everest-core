# RpcApi types

To change the JSON-RPC types, edit `json_rpc_api.yaml`.

To generate and use an updated header in `modules/API/RpcApi/types/json_rpc_api/`:

1. Copy `json_rpc_api.yaml` into the top-level `types/` directory.
2. Start a build.
3. The generated header appears at `build/generated/include/generated/types/json_rpc_api.hpp`.
4. Move that header to `modules/API/RpcApi/types/json_rpc_api/`.

Commit both the updated YAML and the moved header to git.

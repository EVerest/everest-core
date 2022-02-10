// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

//
// author: aw@pionix.de
//

#ifndef JS_EXEC_CTX_HPP
#define JS_EXEC_CTX_HPP

#include <future>

#include <napi.h>

class JsExecCtx {
private:
    static void tramp(Napi::Env env, Napi::Function callback, std::nullptr_t*, JsExecCtx* this_);
    static Napi::Value on_fulfill(const Napi::CallbackInfo& info);
    static Napi::Value on_reject(const Napi::CallbackInfo& info);

public:
    using TsfnType = Napi::TypedThreadSafeFunction<std::nullptr_t, JsExecCtx, JsExecCtx::tramp>;
    using ArgFuncType = std::function<std::vector<napi_value>(Napi::Env&)>;
    using ResFuncType = std::function<void(const Napi::CallbackInfo&, bool)>;

    // FIXME (aw): proper module_instance handling if nullptr
    JsExecCtx(const Napi::Env& env, const Napi::Function& func, const std::string& res_name = "RequestDispatcher") :
        tsfn(TsfnType::New(env, func, res_name, 0, 1)),
        result_handler_ref(Napi::Persistent(Napi::Object::New(env))),
        func_ref(Napi::Persistent(func)) {

        result_handler_ref.Value().DefineProperty(Napi::PropertyDescriptor::Value(
            "fulfill", Napi::Function::New(env, on_fulfill, nullptr, this), napi_enumerable));
        result_handler_ref.Value().DefineProperty(Napi::PropertyDescriptor::Value(
            "reject", Napi::Function::New(env, on_reject, nullptr, this), napi_enumerable));
    }

    ~JsExecCtx() {
        tsfn.Release();
    }

    void exec(const ArgFuncType& arg_func, const ResFuncType& res_func);

private:
    ArgFuncType arg_func;
    ResFuncType res_func;
    // FIXME (aw): will the referenced object be GC'd when the references get destroyed?
    //             and is okay to be destroyed in our async thread?
    Napi::ObjectReference result_handler_ref;
    Napi::FunctionReference func_ref{};
    TsfnType tsfn;
    std::mutex exec_mutex;
    std::promise<void> promise;
};

#endif // JS_EXEC_CTX_HPP

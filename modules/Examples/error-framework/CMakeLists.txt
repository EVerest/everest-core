ev_add_module(ExampleErrorRaiser)
ev_add_module(ExampleErrorSubscriber)
ev_add_module(ExampleErrorGlobalSubscriber)

if(EVEREST_ENABLE_PY_SUPPORT)
    ev_add_module(PyExampleErrorRaiser)
    ev_add_module(PyExampleErrorSubscriber)
endif()

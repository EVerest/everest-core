install(DIRECTORY framework/ DESTINATION include/everest
        FILES_MATCHING PATTERN "*.hpp"
)

install(FILES utils/date.hpp utils/serial.hpp utils/thread.hpp
        DESTINATION include/everest/utils
)

#pragma once

#include "Dialog.hpp"

#include <glm/glm.hpp>

#include <string>

class EventLog {
private:
    Dialog event_dialog_;
    int entry_num_ { 0 };
    int replace_pointer_ { 0 };

    EventLog();

public:
    static EventLog& Instance() 
    {
        static EventLog event_log;
        return event_log;
    }

    void LogEvent(const std::string& log);
    void Draw(const glm::uvec2& drawable_size);
};

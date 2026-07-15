#ifndef TASKSMODULE_HPP_
#define TASKSMODULE_HPP_

#include "IModule.hpp"
#include <sstream>
#include <iomanip>
#include <vector>

class TasksModule : public Krell::IModule {
private:
    mutable std::string cached_data;
    mutable std::vector<std::string> tasks_data;
    
public:
    TasksModule() = default;
    virtual ~TasksModule() = default;
    
    std::string getName() const override {
        return "Tasks";
    }
    
    void update() override;
    std::string getData() const override;
    std::vector<std::string> getTasksData() const;
};

#endif

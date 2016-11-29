#include "ros_rosch/task_attribute_processer.h"
#include <ctime>
#include <float.h>
#include <iostream>
#include <resch/api.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define _GNU_SOURCE 1

using namespace rosch;

TaskAttributeProcesser::TaskAttributeProcesser() : is_rt_(false) {}

TaskAttributeProcesser::~TaskAttributeProcesser() {}

void TaskAttributeProcesser::setRealtimePriority(int priority) {
  // gurad
  if (is_rt_) {
    return;
  }
  is_rt_ = true;
  
  ros_rt_set_scheduler(SCHED_FP); /* you can also set SCHED_EDF. */
  ros_rt_set_priority(priority);
}

bool TaskAttributeProcesser::setCoreAffinity(std::vector<int> core) {
  cpu_set_t mask;
  CPU_ZERO(&mask);
  for (int i = 0; i < core.size(); ++i) {
    CPU_SET(core.at(i), &mask);
  }
  if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
    std::cerr << "Failed to set CPU affinity" << std::endl;
    return false;
  }
  return true;
}

void TaskAttributeProcesser::setCFS() {
  // gurad
  if (is_rt_) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    int core_max = sysconf(_SC_NPROCESSORS_CONF);
    std::vector<int> core;
    for (int i(0); i < core_max; ++i)
      core.push_back(i);
    setCoreAffinity(core);

    ros_rt_set_scheduler(SCHED_FAIR); /* you can also set SCHED_EDF. */
    is_rt_ = false;
  }
}
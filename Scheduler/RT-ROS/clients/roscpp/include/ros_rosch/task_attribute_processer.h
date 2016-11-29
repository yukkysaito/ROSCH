#ifndef TASK_ATTRIBUTE_PROCESSER_H
#define TASK_ATTRIBUTE_PROCESSER_H

#include <vector>

namespace rosch {
class TaskAttributeProcesser {
public:
  TaskAttributeProcesser();
  ~TaskAttributeProcesser();
  void setRealtimePriority(int priority);
  bool setCoreAffinity(std::vector<int> core);
  void setCFS();

private:
  bool is_rt_;
};
}

#endif
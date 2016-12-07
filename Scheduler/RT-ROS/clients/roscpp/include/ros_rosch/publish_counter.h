#ifndef PUBLISH_COUNTER_H
#define PUBLISH_COUNTER_H

#include "type.h"
#include <string>
#include <vector>

namespace rosch {
class SingletonSchedNodeManager {
private:
  SingletonSchedNodeManager();
  SingletonSchedNodeManager(const SingletonSchedNodeManager &);
  SingletonSchedNodeManager &operator=(const SingletonSchedNodeManager &);
  ~SingletonSchedNodeManager();
  NodeInfo node_info_;
  int poll_time_ms_;
  bool missed_deadline_;
  bool running_fail_safe_function_;

  class PublishCounter {
  private:
    std::vector<std::string> v_remain_pubtopic_;
    SingletonSchedNodeManager *sched_node_manager_;

  public:
    PublishCounter(SingletonSchedNodeManager *sched_node_manager);
    size_t getRemainPubTopicSize();
    size_t getPubTopicSize();
    bool removeRemainPubTopic(const std::string &topic_name);
    bool isRemainPubTopic(const std::string &topic_name);
    void resetRemainPubTopic();
  };

  class SubscribeCounter {
  private:
    std::vector<std::string> v_remain_subtopic_;
    SingletonSchedNodeManager *sched_node_manager_;

  public:
    SubscribeCounter(SingletonSchedNodeManager *sched_node_manager);
    size_t getRemainSubTopicSize();
    size_t getSubTopicSize();
    bool removeRemainSubTopic(const std::string &topic_name);
    void resetRemainSubTopic();
  };
  // class SchedulerSetter {
  //   void set_rt() {
  //     cpu_set_t mask;
  //     CPU_ZERO(&mask);
  //     int index = graph_analyzer_->get_node_index(node_name_);
  //     set_affinity(core_count_manager_->get_core());
  //     int prio = 99;
  //     ros_rt_set_scheduler(SCHED_FP); /* you can also set SCHED_EDF. */
  //     ros_rt_set_priority(prio);
  //   }
  //
  //   void set_fair() {
  //     cpu_set_t mask;
  //     CPU_ZERO(&mask);
  //     int core_max = sysconf(_SC_NPROCESSORS_CONF);
  //     set_affinity(core_max);
  //     ros_rt_set_scheduler(SCHED_FAIR); /* you can also set SCHED_EDF. */
  //   }
  // }

public:
  static SingletonSchedNodeManager &getInstance();
  NodeInfo getNodeInfo();
  void setNodeInfo(const NodeInfo &node_info);
  void subPollTime(int time_ms);
  int getPollTime();
  void resetPollTime();
  void init(const NodeInfo &node_info);
  void missedDeadline();
  bool isDeadlineMiss();
  void resetDeadlineMiss();
  PublishCounter publish_counter;
  SubscribeCounter subscribe_counter;
  void runFailSafeFunction();
  bool isFailSafeFunction();
  void (*func)(void);
};
}

#endif // ANALYZER_HPP

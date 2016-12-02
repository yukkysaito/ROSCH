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

  class PublishCounter {
  private:
    std::vector<std::string> v_remain_pubtopic_;
    SingletonSchedNodeManager *sched_node_manager_;

  public:
    PublishCounter(SingletonSchedNodeManager *sched_node_manager);
    size_t getRemainPubTopicSize();
    size_t getPubTopicSize();
    bool removeRemainPubTopic(const std::string &topic_name);
    void resetRemainPubTopic();
  };

public:
  static SingletonSchedNodeManager &getInstance();
  NodeInfo getNodeInfo();
  void setNodeInfo(const NodeInfo &node_info);
  void init(const NodeInfo &node_info);
  PublishCounter publish_counter;
};
}

#endif // ANALYZER_HPP

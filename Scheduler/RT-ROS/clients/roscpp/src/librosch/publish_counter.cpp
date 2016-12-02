#include "ros_rosch/publish_counter.h"
#include <iostream>
using namespace rosch;

SingletonSchedNodeManager::SingletonSchedNodeManager()
    : publish_counter(this) {}
SingletonSchedNodeManager::~SingletonSchedNodeManager() {}

SingletonSchedNodeManager &SingletonSchedNodeManager::getInstance() {
  static SingletonSchedNodeManager inst;
  return inst;
}
void SingletonSchedNodeManager::setNodeInfo(const NodeInfo &node_info) {
  node_info_ = node_info;
}
NodeInfo SingletonSchedNodeManager::getNodeInfo() { return node_info_; }
void SingletonSchedNodeManager::init(const NodeInfo &node_info) {
  setNodeInfo(node_info);
  publish_counter.resetRemainPubTopic();
}

SingletonSchedNodeManager::PublishCounter::PublishCounter(
    SingletonSchedNodeManager *sched_node_manager)
    : sched_node_manager_(sched_node_manager) {}
void SingletonSchedNodeManager::PublishCounter::resetRemainPubTopic() {
  v_remain_pubtopic_ = sched_node_manager_->node_info_.v_pubtopic;
}
size_t SingletonSchedNodeManager::PublishCounter::getRemainPubTopicSize() {
  return v_remain_pubtopic_.size();
}
size_t SingletonSchedNodeManager::PublishCounter::getPubTopicSize() {
  return sched_node_manager_->node_info_.v_pubtopic.size();
}
bool SingletonSchedNodeManager::PublishCounter::removeRemainPubTopic(
    const std::string &topic_name) {
  std::vector<std::string>::iterator itr = v_remain_pubtopic_.begin();
  while (itr != v_remain_pubtopic_.end()) {
    if (*itr == topic_name) {
      itr = v_remain_pubtopic_.erase(itr);
      return true;
    } else {
      itr++;
    }
  }
  return false;
}

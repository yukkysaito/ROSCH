#ifndef ANALYZER_HPP
#define ANALYZER_HPP
#include "ros_rosch/exec_time.hpp"
#include <string>
#include <fstream>
#include "ros_rosch/node_graph.hpp"


namespace rosch {
class Analyzer : ExecTime {
public:
    explicit Analyzer(const std::string& node_name,
                      const std::string& topic,
                      const unsigned int& max_times=100,
                      const unsigned int& ignore_times=20);
    ~Analyzer();
    void start_time(); //
    void end_time(); //
    void finish_myself();  //
    unsigned int get_counter();
    double get_max_time_ms();
    double get_min_time_ms();
    double get_exec_time_ms();
    int get_target_index();
    bool is_in_range();
    bool is_target();
    bool is_target_topic();
    bool is_target_node();
    bool is_in_node_graph();
    void set_rt();
    void set_fair();
    std::string get_node_name();
    std::string get_topic_name();
    void update_graph();
private:
    SingletonNodeGraphAnalyzer* graph_analyzer_;
    unsigned int max_analyze_times_;
    unsigned int ignore_times_;
    unsigned int counter_;
    double max_ms_;
    double min_ms_;
    double average_ms_;
    std::string node_name_;
    std::string topic_;
    bool is_aleady_rt_;
    std::ofstream ofs_;
    std::ofstream ofs_inform_;
    std::ifstream ifs_inform_;
};
}

#endif // ANALYZER_HPP

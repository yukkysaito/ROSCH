#ifndef ANALYZER_HPP
#define ANALYZER_HPP
#include "ros_rosch/exec_time.hpp"
#include <string>
#include <fstream>

namespace rosch {
class Analyzer : ExecTime {
public:
    explicit Analyzer(const std::string& node_name,
                      const std::string& topic,
                      const unsigned int& max_times=1000,
                      const unsigned int& ignore_times=20);
    ~Analyzer();
    void start_time();
    void end_time();
    unsigned int get_counter();
    double get_max_time_ms();
    double get_min_time_ms();
    double get_exec_time_ms();
    bool is_target();
    void set_target();
private:

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
};
}

#endif // ANALYZER_HPP

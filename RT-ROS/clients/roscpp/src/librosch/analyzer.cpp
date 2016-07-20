#include "ros_rosch/analyzer.hpp"
#include "ros_rosch/node_graph.hpp"
#include <ctime>
#include <float.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <resch/api.h>

using namespace rosch;

std::string remove_begin_slash(std::string source,
                        const std::string& replace_source)
{
    std::string::size_type pos(source.find(replace_source));
    if(pos == 0) {
        std::cout << "pos:" << pos << std::endl;
        source.erase(source.begin());
    }
    std::cout << "pos:" << pos << "source:" << source <<  std::endl;

    return source;
}

std::string replace_all(std::string source,
                        const std::string& replace_source,
                        const std::string& replace_target)
{
    std::string::size_type pos(source.find(replace_source));

    while(pos != std::string::npos) {
        source.replace(pos, replace_source.length(), replace_target);
        pos = source.find(replace_source, pos + replace_target.length());
    }

    return source;
}

Analyzer::Analyzer(const std::string& node_name,
                   const std::string& topic,
                   const unsigned int& max_times,
                   const unsigned int& ignore_times)
    :max_analyze_times_(max_times+ignore_times)
    , ignore_times_(ignore_times)
    , counter_(0)
    , max_ms_(0)
    , min_ms_(DBL_MAX)
    , average_ms_(0)
    , topic_(topic)
    , node_name_(node_name)
    , is_aleady_rt_(false)
    , ifs_inform_("inform_rosch_.txt")
    , ofs_inform_("inform_rosch_.txt")
{
    graph_analyzer_ = &SingletonNodeGraphAnalyzer::getInstance();
    if (topic_ != "/clock") {
        // Create dir.
        std::string dir_name(node_name);
        dir_name = remove_begin_slash(dir_name, "/");
        dir_name = replace_all(dir_name, "/", "__" );
        mkdir(dir_name.c_str(), 0755);
        // Create output file.
        std::string file_name(topic+".csv");
        file_name = remove_begin_slash(file_name, "/");
        file_name = replace_all(file_name, "/", "__" );
        file_name = dir_name+"/"+file_name;
        ofs_.open(file_name.c_str());
    }
}

Analyzer::~Analyzer()
{
    if (topic_ != "/clock") {
        ofs_.close();
    }
}

void Analyzer::start_time() {
    ExecTime::start_time();
}

void Analyzer::end_time() {
    ExecTime::end_time();

    if(ignore_times_ <= counter_){
        if (max_ms_ < get_exec_time_ms())
            max_ms_ = get_exec_time_ms();
        if (get_exec_time_ms() < min_ms_)
            min_ms_ = get_exec_time_ms();
        ofs_ << get_exec_time_ms() << std::endl;
    }
    ++counter_;
}

double Analyzer::get_max_time_ms() {
    return max_ms_;
}

double Analyzer::get_min_time_ms() {
    return min_ms_;
}

double Analyzer::get_exec_time_ms() {
    return ExecTime::get_exec_time_ms();
}

bool Analyzer::is_in_range() {
    return counter_ < max_analyze_times_ ? true : false;
}

int Analyzer::get_target_index() {
    return graph_analyzer_->get_target_node()->index;
}

void Analyzer::update_graph() {
    std::string str;
    while(getline(ifs_inform_, str)) {
        graph_analyzer_->finish_node(std::atoi(str.c_str()));
        std::cout << "finish node"<<str << std::endl;
    }
    ifs_inform_.clear();
    ifs_inform_.seekg(0, std::ios_base::beg);
}

bool Analyzer::is_target() {
    return graph_analyzer_->get_target_node()->index
            == graph_analyzer_->get_node_index(get_node_name())
            ? true : false;
}

void Analyzer::finish_myself() {
    ofs_inform_ << graph_analyzer_->get_node_index(get_node_name()) << std::endl;
    graph_analyzer_->finish_node(graph_analyzer_->get_node_index(get_node_name()));
}

unsigned int Analyzer::get_counter() {
    return counter_;
}

void Analyzer::set_target() {
    // gurad
    if(is_aleady_rt_) {
        return;
    }
    is_aleady_rt_ = true;

    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);

    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        perror("Failed to set CPU affinity");
        return;
    }
    int prio = 99;
    ros_rt_set_scheduler(SCHED_FP); /* you can also set SCHED_EDF. */
    ros_rt_set_priority(prio);
}

std::string Analyzer::get_node_name() {
    return node_name_;
}

std::string Analyzer::get_topic_name() {
    return topic_;
}

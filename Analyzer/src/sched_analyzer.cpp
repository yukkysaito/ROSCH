#include "sched_analyzer.h"
#include "config.h"
#include "node.h"
#include "node_graph.h"
#include "spec.h"
#include "yaml-cpp/yaml.h"
#include <algorithm>
#include <limits.h>
#include <string>

using namespace sched_analyzer;

SchedAnalyzer::SchedAnalyzer()
    : node_graph_analyzer_(SingletonNodeGraphAnalyzer::getInstance()),
      config_(), makespan_(0) {
  std::string filename(config_.get_specpath());
  load_spec_(filename);
  v_sched_cpu_task_.resize(get_spec_core());
}

SchedAnalyzer::~SchedAnalyzer() {}

void SchedAnalyzer::load_spec_(const std::string &filename) {
  std::cout << filename << std::endl;
  try {
    YAML::Node spec;
    spec = YAML::LoadFile(filename);
    const YAML::Node core = spec["core"];
    const YAML::Node memory = spec["memory"];
    spec_.core = core.as<int>();
    spec_.memory = core.as<int>();
  } catch (YAML::Exception &e) {
    std::cerr << e.what() << std::endl;
  }
}

// bool sort_greater_(const P_core_i &left, const P_core_i &right) {
//   return left.second > right.second;
// }

int SchedAnalyzer::run() {

  /*compute laxity and sort by laxity*/
  node_graph_analyzer_.compute_laxity();
  node_graph_analyzer_.sched_all_node_queue(node_queue_);
  /*
   * node_queue_ : sorted nodes by inv_laxity
   * v_cpu_esc : each cpu's escapsed time
   * v_sched_cpu_task_ (Main) : scheduled node on each cpu
   */
  for (int i(0); (size_t)i < node_queue_.size(); ++i) {
    /* search vacancy core */
    for (int node_core(0); node_core < node_queue_.at(i).core; ++node_core) {
      std::vector<sort_esc_time_t> v_cpu_esc(get_spec_core());
      get_cpu_status(v_cpu_esc);
      std::sort(v_cpu_esc.begin(), v_cpu_esc.end());

      /* create sched_node */
      /* search min start time */
      int min_start_time(0);
      get_min_start_time(node_queue_.at(i).index, min_start_time);
      /* determine core that be set sched_(v_)node_t*/
      sched_node_info_t sched_node_info;
      sched_v_node_info_t sched_vacancy_node_info;
      int is_vacancy_node;
      get_sched_node_info(min_start_time, sched_node_info);
      is_vacancy_node = get_sched_vacancy_node_info(
          node_queue_.at(i).run_time, min_start_time, sched_vacancy_node_info);
      /* push a node instead of vacancy node */
      if (is_vacancy_node == 1 &&
          sched_vacancy_node_info.start_time <= sched_node_info.start_time) {
        int run_time = node_queue_.at(i).run_time;
        int index = node_queue_.at(i).index;
        set_sched_vacancy_node(index, run_time, sched_vacancy_node_info);
      }
      /* push back in v_sched_cpu_task_ */
      else {
        if (sched_node_info.need_empty) {
          sched_node_t sched_empty_node;
          int empty_start_time = 0;
          if (v_sched_cpu_task_.at(sched_node_info.core_index).empty()) {
            empty_start_time = 0;
          } else {
            empty_start_time = v_sched_cpu_task_.at(sched_node_info.core_index)
                                   .back()
                                   .esc_time;
          }
          create_empty_node(empty_start_time, sched_node_info.start_time,
                            sched_empty_node);
          /* set sched_empty_node to v_sched_cpu_task_ */
          v_sched_cpu_task_.at(sched_node_info.core_index)
              .push_back(sched_empty_node);
        }
        sched_node_t sched_node;
        create_sched_node(node_queue_.at(i).index, sched_node_info.start_time,
                          node_queue_.at(i).run_time, sched_node);
        /* set sched_node to v_sched_cpu_task_ */
        v_sched_cpu_task_.at(sched_node_info.core_index).push_back(sched_node);
      }
    }
  }

  /* show tasks on each core*/
  compute_makespan(makespan_);
  return 0;
}

int SchedAnalyzer::get_cpu_status(std::vector<sort_esc_time_t> &v_cpu_status) {
  for (int i(0); i < get_spec_core(); ++i) {
    if (v_sched_cpu_task_.at(i).size() == 0) {
      sort_esc_time_t status = {i, 0};
      v_cpu_status.at(i) = status;
    } else {
      sort_esc_time_t status = {i, v_sched_cpu_task_.at(i).back().esc_time};
      v_cpu_status.at(i) = status;
    }
  }
  return 0;
}

int SchedAnalyzer::compute_makespan(int &makespan) {
  for (int i(0); i < get_spec_core(); ++i) {
    if (!v_sched_cpu_task_.at(i).empty()) {
      if (makespan < v_sched_cpu_task_.at(i).back().esc_time)
        makespan = v_sched_cpu_task_.at(i).back().esc_time;
    }
  }
  return 0;
}

int SchedAnalyzer::show_sched_cpu_tasks() {
  for (int i(0); i < get_spec_core(); ++i) {
    for (int j(0); (size_t)j < v_sched_cpu_task_.at(i).size(); ++j) {
      if (v_sched_cpu_task_.at(i).at(j).empty) {
        std::cout << "\x1b[36m(core" << i
                  << ")\x1b[m\x1b[35mindex:\x1b[m\x1b[31mempty\x1b[m "
                     "\x1b[34m\truntime:"
                  << v_sched_cpu_task_.at(i).at(j).run_time
                  << "\x1b[m\t\x1b[31m["
                  << v_sched_cpu_task_.at(i).at(j).start_time << "~"
                  << v_sched_cpu_task_.at(i).at(j).end_time << "]\x1b[m"
                  << std::endl;
      } else {
        std::vector<node_t> v_child;
        node_graph_analyzer_.sched_child_list(
            v_sched_cpu_task_.at(i).at(j).node_index, v_child);

        std::cout << "\x1b[36m(core" << i << ")\x1b[m\x1b[35mindex:"
                  << v_sched_cpu_task_.at(i).at(j).node_index
                  << "\x1b[m\x1b[34m\t\truntime:"
                  << v_sched_cpu_task_.at(i).at(j).run_time << "\x1b[m\t["
                  << v_sched_cpu_task_.at(i).at(j).start_time << "~"
                  << v_sched_cpu_task_.at(i).at(j).end_time << "]";
        std::cout << "\t\x1b[32mchild list:";

        for (int k(0); (size_t)k < v_child.size(); ++k) {
          std::cout << v_child.at(k).index << " ";
        }
        std::cout << "\x1b[m" << std::endl;
      }
    }
    if (!v_sched_cpu_task_.at(i).empty())
      std::cout << std::endl;
  }
  std::cout << "makespan:" << makespan_ << std::endl;

  return 0;
}

int SchedAnalyzer::get_cpu_taskset(std::vector<V_sched_node>& v_sched_cpu_task) {
  v_sched_cpu_task = v_sched_cpu_task_;
  return 0;
}

int SchedAnalyzer::get_min_start_time(int index, int &min_start_time) {
  std::vector<node_t> v_child;
  min_start_time = 0;
  node_graph_analyzer_.sched_child_list(index, v_child);
  for (int core(0); core < get_spec_core(); ++core) {
    for (int l(0); (size_t)l < v_sched_cpu_task_.at(core).size(); ++l) {
      for (int j(0); (size_t)j < v_child.size(); ++j) {
        if (v_child.at(j).index ==
            v_sched_cpu_task_.at(core).at(l).node_index) {
          min_start_time =
              min_start_time < v_sched_cpu_task_.at(core).at(l).esc_time
                  ? v_sched_cpu_task_.at(core).at(l).esc_time
                  : min_start_time;
        }
      }
    }
  }
  return 0;
}

int SchedAnalyzer::get_sched_node_info(const int min_start_time,
                                       sched_node_info_t &sched_node_info) {
  /* determine core that be set sched_node_t*/
  struct score_t {
    int score;
    int core_index;
  };
  score_t core_score = {INT_MAX, -1};
  sched_node_info.start_time = -1;
  sched_node_info.core_index = -1;
  sched_node_info.need_empty = false;

  /* when esc_time is less than min_start_time */
  for (int core(0); (size_t)core < v_sched_cpu_task_.size(); ++core) {
    int core_esc_time(0);
    if (!v_sched_cpu_task_.at(core).empty())
      core_esc_time = v_sched_cpu_task_.at(core).back().esc_time;
    if (core_score.score > min_start_time - core_esc_time &&
        min_start_time - core_esc_time >= 0) {
      core_score.score = min_start_time - core_esc_time;
      core_score.core_index = core;
    }
  }
  if (INT_MAX != core_score.score) {
    sched_node_info.start_time = min_start_time;
    if (0 != core_score.score)
      sched_node_info.need_empty = true;
  }
  /* when min_start_time is less than esc_time */
  else {
    for (int core(0); (size_t)core < v_sched_cpu_task_.size(); ++core) {
      int core_esc_time(0);
      if (!v_sched_cpu_task_.at(core).empty())
        core_esc_time = v_sched_cpu_task_.at(core).back().esc_time;
      if (core_score.score > core_esc_time - min_start_time &&
          core_esc_time - min_start_time >= 0) {
        core_score.score = core_esc_time - min_start_time;
        core_score.core_index = core;
      }
    }
    sched_node_info.start_time =
        v_sched_cpu_task_.at(core_score.core_index).back().esc_time;
  }
  sched_node_info.core_index = core_score.core_index;
  return 0;
}

int SchedAnalyzer::create_empty_node(const int empty_start_time,
                                     const int empty_end_time,
                                     sched_node_t &sched_empty_node) {
  int empty_run_time = empty_end_time - empty_start_time;
  sched_empty_node.empty = true;
  sched_empty_node.run_time = empty_run_time;
  sched_empty_node.node_index = -1;
  sched_empty_node.esc_time = empty_start_time + empty_run_time;
  sched_empty_node.start_time = empty_start_time;
  sched_empty_node.end_time = empty_start_time + empty_run_time;
  return 0;
}

int SchedAnalyzer::create_sched_node(const int index, const int start_time,
                                     const int run_time,
                                     sched_node_t &sched_node) {

  sched_node_t sn = {false,      run_time,
                     index,      start_time + run_time,
                     start_time, start_time + run_time};
  sched_node = sn;
  return 0;
}

int SchedAnalyzer::get_sched_vacancy_node_info(
    const int run_time, const int min_start_time,
    sched_v_node_info_t &sched_vacancy_node_info) {
  /* init sched_vacancy_node_info */
  sched_vacancy_node_info.start_time = -1;
  sched_vacancy_node_info.core_index = -1;
  sched_vacancy_node_info.empty_index = -1;
  sched_vacancy_node_info.need_before_empty = false;
  sched_vacancy_node_info.need_after_empty = false;

  struct score_t {
    int fin_time;
    int rem_time;
    int core;
    int empty_index;
  };
  score_t score = {INT_MAX, INT_MAX, -1, -1};

  for (int core(0); core < get_spec_core(); ++core) {
    for (int l(0); (size_t)l < v_sched_cpu_task_.at(core).size(); ++l) {
      if (v_sched_cpu_task_.at(core).at(l).empty) {
        if (v_sched_cpu_task_.at(core).at(l).start_time <= min_start_time) {
          if (run_time <=
              v_sched_cpu_task_.at(core).at(l).end_time - min_start_time) {
            score_t this_score = {min_start_time + run_time,
                                  v_sched_cpu_task_.at(core).at(l).end_time -
                                      (min_start_time + run_time),
                                  core, l};
            if (this_score.fin_time < score.fin_time) {
              score = this_score;
            } else if (this_score.fin_time == score.fin_time &&
                       this_score.rem_time < score.rem_time) {
              score = this_score;
            }
          }
        } else {
          if (run_time <= v_sched_cpu_task_.at(core).at(l).run_time) {
            score_t this_score = {
                v_sched_cpu_task_.at(core).at(l).start_time + run_time,
                v_sched_cpu_task_.at(core).at(l).end_time -
                    (v_sched_cpu_task_.at(core).at(l).start_time + run_time),
                core, l};
            if (this_score.fin_time < score.fin_time) {
              score = this_score;
            } else if (this_score.fin_time == score.fin_time &&
                       this_score.rem_time < score.rem_time) {
              score = this_score;
            }
          }
        }
      }
    }
  }
  if (score.fin_time != INT_MAX) {
    sched_vacancy_node_info.start_time = score.fin_time - run_time;
    sched_vacancy_node_info.core_index = score.core;
    sched_vacancy_node_info.empty_index = score.empty_index;
    if (v_sched_cpu_task_.at(score.core).at(score.empty_index).start_time <
        sched_vacancy_node_info.start_time) {
      sched_vacancy_node_info.need_before_empty = true;
    }
    if (score.fin_time <
        v_sched_cpu_task_.at(score.core).at(score.empty_index).end_time) {
      sched_vacancy_node_info.need_after_empty = true;
    }

    return 1;
  }
  return -1;
}

int SchedAnalyzer::set_sched_vacancy_node(
    const int index, const int run_time,
    const sched_v_node_info_t &sched_vacancy_node_info) {
  if (index < 0 || run_time < 0 || sched_vacancy_node_info.core_index < 0)
    return -1;
  int core = sched_vacancy_node_info.core_index;
  int e_index = sched_vacancy_node_info.empty_index;

  V_sched_node v_swap;

  for (int swap_i = 0; swap_i != e_index; ++swap_i) {
    v_swap.push_back(v_sched_cpu_task_.at(core).at(swap_i));
  }
  if (sched_vacancy_node_info.need_before_empty) {
    sched_node_t sched_empty_node;
    create_empty_node(v_sched_cpu_task_.at(core).at(e_index).start_time,
                      sched_vacancy_node_info.start_time, sched_empty_node);
    v_swap.push_back(sched_empty_node);
  }
  sched_node_t sched_node;
  create_sched_node(index, sched_vacancy_node_info.start_time, run_time,
                    sched_node);
  v_swap.push_back(sched_node);
  if (sched_vacancy_node_info.need_after_empty) {
    sched_node_t sched_empty_node;
    create_empty_node(sched_vacancy_node_info.start_time + run_time,
                      v_sched_cpu_task_.at(core).at(e_index).end_time,
                      sched_empty_node);
    v_swap.push_back(sched_empty_node);
  }
  for (int swap_i = e_index + 1;
       (size_t)swap_i != v_sched_cpu_task_.at(core).size(); ++swap_i) {
    v_swap.push_back(v_sched_cpu_task_.at(core).at(swap_i));
  }
  v_sched_cpu_task_.at(core).swap(v_swap);
  return 1;
}

spec_t SchedAnalyzer::get_spec() { return spec_; }
int SchedAnalyzer::get_spec_core() { return spec_.core; }
int SchedAnalyzer::get_makespan() { return makespan_; }
int SchedAnalyzer::get_node_list_size() {
  return node_graph_analyzer_.get_node_list_size();
}
int SchedAnalyzer::get_node_name(int index, std::string &node_name) {
  node_name = node_graph_analyzer_.get_node_name(index);
  return 0;
}

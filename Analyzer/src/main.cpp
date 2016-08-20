#include "node.h"
#include "node_graph.h"
#include "node_graph_core.h"
#include "sched_analyzer.h"
#include <fstream>
#include <iostream>
#include <vector>

int main(void) {
  std::cout << "start Sched Analyzer" << std::endl;

// std::ofstream ofs_inform_;
// ofs_inform_.open("test.csv", std::ios::app);

//  analyzer.show_tree_dfs();
#if 1
  sched_analyzer::SchedAnalyzer analyzer;
  analyzer.run();
  analyzer.show_sched_cpu_tasks();
#else
  while (true) {
    std::vector<node_t> node_list;
    if (!analyzer.sched_leaf_list(node_list)) {
      std::cerr << "ERROR:sched_node_list." << std::endl
                << "It is caused by to small MAX_LEAF_LIST." << std::endl;
      return 0;
    }

    node_t sched_node;
    native_c::node_init(&sched_node);
    for (int i(0); (size_t)i < node_list.size(); ++i) {
      if (node_list.at(i).name == "root") {
        goto OUT;
      }
      if (sched_node.inv_laxity < node_list.at(i).inv_laxity)
        sched_node = node_list.at(i);
    }
    analyzer.finish_node(sched_node.index);
    std::cout << "-----------------" << std::endl;
    std::cout << sched_node.name << "@" << sched_node.index << "["
              << sched_node.run_time << "@" << sched_node.inv_laxity << "]"
              << std::endl;
    // for (int i(0); i < spec.core; ++i) {
    //   ofs_inform_ << i << "," << std::endl;
    // }
  }
OUT:
#endif

  // ofs_inform_.close();

  // analyzer.show_tree_dfs();
  // analyzer.show_leaf_list();

  return 0;
}

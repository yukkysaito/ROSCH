## ROSCH
Real-time extension module for ROS.
This following functions are supported :

- Automatic analyzer for execution times of ROS nodes
- Automatic analyzer for fixted priority scheduling
- Real-time scheduling bease on fixed priority

## Requirements
- ROS indigo(Ubuntu 14.04)

## Install

```
$./install.sh
```
## Uninstall

```
$./uninstall.sh
```

## Re-install
If you change config or source files, please put following commond.

```
$./reinstall.sh
```
## How to Start
node_graph.yaml is written node list that were wanted to analyze.
Please, change node_graph.yaml.

After that, you run nodes that were written node_graph.yaml.
Results is in ~/.ros/rosch/***
# Problem Specification

## Problem

The goal of SLAM algorithms (Simultaneous Localization and Mapping) is to map an
environment navigated by and autonomous vehicle, while simultaneously locating
it in the map, without access to pre-existing maps or external devices. This
project will focus on applying SLAM algorithms in the context of sub-aquatic
navigation using a sonar device.

The group will have access to sonar data measured by CRAS.

## Goals

The goal is to develop a densely mapped system using empty spaces in the
environment (which allows to infer obstacle's positions). The information on the
map is updated each time there's a new measurement using a probabilistic mapping
function.  
Furthermore, feature identification and cleanup algorithms need to be applied to
the sonar data in order to remove noise (multiple reflections, echos, self
reflections, and multipath euros) and make it usable for mapping.

In this first part of the project, we intend to develop the 2D view of the map
and assume that the sonar is static (i.e. not moving between measurements).

## Functionalities

These are the functionalities planned for this first part of the project.

- **Filter noise**/undesirable effects in data;
- Represent map using an **Octomap/Octree**;
- Implement static/dynamic **probabilistic mapping algorithms** based on sonar
  data;
- Support **incremental map growth**;

### Data Structures

- Octrees/Octomaps
  - Update a cell with a given probability
  - Split a region into smaller cells

### Planned algorithms

- Sonar data Filtering:
  - Kalman Filter, Extended Kalman Filter, Particle Filter;
- Heuristics for Data Cleaning:
  - Edge Detection, Differential Threshold, Range to first feature;
- Probabilistic Mapping:
  - Inverse Model Estimator;
- Ray casting:
  - #d Bresenham algorithm.

## Planned robustness and scalability requirements

- **Spacial efficiency** - The sub-aquatic environment can be enormous, as such,
  the system must store the map in an efficient and scalable manner;
- **Time efficiency** - The system must be efficient in a way that can keep with
  sonar data in real time (about 40ms between measures). There's also
  constraints related to the vehicle velocity: data in the direction the vehicle
  is moving needs to be incorporated into the map quickly enough to avoid
  collisions;
- **Lightweight** - The system must be robust to limited hardware resources.

## Planning

- Problem definition and data structures (Mar 21) - João Costa
- Key algorithms (Mar 28) - João Martins
- Empirical analysis (Apr 4) - Henrique Ribeiro e Tiago Silva

# Problem Specification

## Problem

The goal of SLAM algorithms (Simultaneous Localization and Mapping) is to map an
environment navigated by an autonomous vehicle, while simultaneously locating
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
reflections, and multipath errors) and make it usable for mapping.

In the second part of the project, we intend to expand the existing implementation
to 3D space, while focusing on improving performance by employing new algorithms
and data structures.

## Functionalities

These are the functionalities planned for the second part of the project.

- Map the environment in 3D space
- Using raycasts to simulate sonar beam
- Support **incremental map growth**;

### Data Structures

- Set. Operations:
  - TODO

### Planned algorithms

Olha n sei

## Planned robustness and scalability requirements

Requirements remain unchanged from the first part of the project. These are:

- **Spacial efficiency** - The sub-aquatic environment can be enormous, as such,
  the system must store the map in an efficient and scalable manner;
- **Time efficiency** - The system must be efficient in a way that can keep with
  sonar data in real time (about 40ms between measures). There's also
  constraints related to the vehicle velocity: data in the direction the vehicle
  is moving needs to be incorporated into the map quickly enough to avoid
  collisions;
- **Lightweight** - The system must be robust to limited hardware resources.

## Planning

TODO

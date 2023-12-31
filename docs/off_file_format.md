# Object File Format (.off)

This is just a backup of
[**source**](https://segeval.cs.princeton.edu/public/off_format.html).

Object File Format (.off) files are used to represent the geometry of a model by
specifying the polygons of the model's surface. The polygons can have any number
of vertices.

The .off files in the Princeton Shape Benchmark conform to the following
standard. OFF files are all ASCII files beginning with the keyword OFF. The next
line states the number of vertices, the number of faces, and the number of
edges. The number of edges can be safely ignored.

The vertices are listed with x, y, z coordinates, written one per line. After
the list of vertices, the faces are listed, with one face per line. For each
face, the number of vertices is specified, followed by indices into the list of
vertices. See the examples below.

Note that earlier versions of the model files had faces with -1 indices into the
vertex list. That was due to an error in the conversion program and should be
corrected now.

```
OFF numVertices numFaces numEdges
x y z
x y z
... numVertices like above
NVertices v1 v2 v3 ... vN
MVertices v1 v2 v3 ... vM
... numFaces like above
```

**Note:** that vertices are numbered starting at 0 (not starting at 1), and that
numEdges will always be zero.

A simple example for a cube:

```txt
OFF
8 6 0
-0.500000 -0.500000 0.500000
0.500000 -0.500000 0.500000
-0.500000 0.500000 0.500000
0.500000 0.500000 0.500000
-0.500000 0.500000 -0.500000
0.500000 0.500000 -0.500000
-0.500000 -0.500000 -0.500000
0.500000 -0.500000 -0.500000
4 0 1 3 2
4 2 3 5 4
4 4 5 7 6
4 6 7 1 0
4 1 7 5 3
4 6 0 2 4
```

# slam-project

We use our notebook to import, export and visualize data. Our main
implementation has octrees, probabilistic mapping, data preprocessing.

## Build

```sh
mkdir build
cd build
cmake ..
make
```

## Run

```sh
./SLAM
```

## Dependencies

### Jupyter Notebook

Used to process the matlab dataset containing the sonar measurements.

- Matplotlib
- Scipy
- Pypng

### C++

#### Need install

These libs are dynamic linked and need to be installed in the system in order to
compile and run the program.

- [OpenCV](https://opencv.org), and its dependencies:
  - glew
  - vtk
    - pugixml
  - hdf5
- [Boost](https://www.boost.org)

#### Bundled/Optional

These libs are either optional or bundled with the application code, thus not
needing any special action/attention from the user.

- OpenMP (**Optional** - Improves performance)
- [Parallel Hashmap](https://github.com/greg7mdp/parallel-hashmap) - See
  [License](./slam/include/parallel_hashmap/LICENSE).

## Point-cloud dataset

[ModelNet40](https://www.kaggle.com/balraj98/modelnet40-princeton-3d-object-dataset)
in off format.

## TODO

- ~~Usar log-odds como main (em vez de prob)~~;
- ~~Log-odds clamping~~;
- ~~Update prob (em contraste com set prob): probably default arg na func~~;
- ~~Ray casting na tree~~;
- ~~Investigar dynamic bitset para keys~~;
- ~~Antes de dar update a nó, ver se o novo valor pode afetar o atual (nó já
  estável) => tem overhead em search mas deve ajudar em muitos casos~~;
- ~~Pointcloud insert pode ser paralelizado => calcular rays em paralelo e
  inserir num set de frees e ocupados~~;
- ~~Descretizar endpoints? Basicamente so guardar o primeiro raio para cada
  endpoint na point cloud => menos raycasts mas resultado diferente~~;
- ~~Lazy eval => n fazer o update das occupancy e pruning no insert. No final,
  chamar método que da sync aos nos intermedios. Bom para muitas inserções
  seguidas~~;
- ~~Documentar coisas~~;
- Tratar range de valores à volta do 0.5 (e.g. [0.45, 0.55]) como unknown tmb;
- Bounds check on keys;

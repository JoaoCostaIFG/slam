# slam-project

## Dependencies

- Matplotlib
- Scipy
- Pypng

## Point-cloud dataset

[ModelNet40](https://www.kaggle.com/balraj98/modelnet40-princeton-3d-object-dataset)
in off format.

## TODO

- ~~Usar log-odds como main (em vez de prob)~~;
- ~~Log-odds clamping~~;
- ~~Update prob (em contraste com set prob): probably default arg na func~~;
- ~~Ray casting na tree~~;
- ~~Investigar dynamic bitset para keys~~;
- ~~Antes de dar update a nó, ver se o novo valor pode afetar o atual (nó já estável) => tem overhead em search mas deve
  ajudar em muitos casos~~;
- ~~Pointcloud insert pode ser paralelizado => calcular rays em paralelo e inserir num set de frees e ocupados~~;
- ~~Descretizar endpoints? Basicamente so guardar o primeiro raio para cada endpoint na point cloud => menos raycasts
  mas resultado diferente~~;
- Lazy eval => n fazer o update das occupancy e pruning no insert. No final, chamar método que da sync aos nos
  intermedios. Bom para muitas inserções seguidas;
- Tratar range de valores à volta do 0.5 (e.g. [0.45, 0.55]) como unknown tmb;
- Documentar coisas;
- Explicar keys;
- Bounds check on points;

# Notinhas do Irão

## Occupancy maps

- **Global occupancy map (GOM)** - estacionário;
- **Vehicle-attached occupancy map (BOM)** - dinámico, centrado no centro de
  buoyancy do veículo. Move-se e roda com o veículo;
- Probabilidade de célula estar ocupada varia entre 0 (não ocupado) e 1
  (ocupado). **0.5** é considerado unknown (**valor inicializado**).

![GOM (preto) e VOM (vermelho) ao longo do tempo](img/gom_vom.png)

### Grid mapping algorithm

![Grid mapping algorithm](img/grid_mapping_algo.png)

## Dynamic inverse-sonar model

- **Source level (SL)** - cálculado com base no transducer;
- **Directivity index (DI)** - varia com o ângulo dos raios;
- **Transmission loss (TL)** - causado pelo aumento da _ensonified area ovar the
  range_;
- **Attenuation loss (AL)** - energia dissipada para o meio durante a
  propagação;
- **Target strength (TS)** - energia refletida do target para nós (a que não
  absorvida/defletida para outras direções);

![Ângulo de incidência de um raio a colidir com o terreno](img/incident_ray.png)

- O _forward looking angle_ cria **bias** (_arc-shapes_);

![Bias](img/bias.png)

## Inverting sonar model



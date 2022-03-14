# Problem Specification

## Empirical Analysis
Mostrar o result
Mas tambem fazer grafos e mostrar como da scale

## Problem
Algoritmos de SLAM (Simultaneous Localization and Mapping) têm por objetivo o mapeamento do ambiente navegado por um veículo autónomo, localizando simultaneamente o mesmo no mapa construído, sem o recurso a mapas pré-existentes ou dispositivos externos. Este projeto incidirá na aplicação de algoritmos de SLAM em contexto de navegação subaquática recorrendo a instrumentos sonar.

## Goals
O objetivo passa por desenvolver um sistema de mapeamento denso focado nos espaços vazios do ambiente (o que permite inferir sobre os obstáculos presentes no mesmo).

## Functionalities
+ Filtrar noise/efeitos indesejáveis nos dados
+ Octrees e Octomaps
+ Algoritmos probabilisticos de mapeamento
	+ Estatico
	+ Dinamico
+ Adaptar para sistema em tempo real

### Data Structures
+ Octrees/Octomaps
+ Listas hahahahha

### Planned algorithms
+ Filtrar dados
	+ Kalman Filter, Extended Kalman Filter, Partifcle Filter
+ Heuristicas para tratamento de dados
	+ Edge Detection, Differencial Threshold, Range to first feature
+ Mapeamento probabilistico
	+ Inverse Model Estimator - Hornung Approach
## Planned robustness and scalability requirements
+ Eficiente para ser usado em tempo real (40 ms per measure)
	+ Tem de funcionar para sonars mais rápidos
+ Robusto a limitação de recursos (pc's do padoru)
+ Eficiência em espaço

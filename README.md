# Direção Elétrica para Embarcação Solar

Instituto Federal de Educação, Ciência e Tecnologia de Santa Catarina - Campus Florianópolis

Departamento Acadêmico de Eletrônica Curso de Engenharia Eletrônica

Projeto Integrador III

Aluno:

* Gabriel Ayres Rodrigues

# Sumário

1. [Introdução](#introdução)
2. [Concepção](#concepção)
3. [Design](#design)
4. [Implementação](#implementação)
5. [Operação](#operação)
6. [Considerações Finais](#considerações)
7. [Bibliografia](#bibliografia)

# Introdução

Esse módulo tem como objetivo controlar a direção da rabeta da embarcação Guarapuvu II, da equipe Zênite Solar do IFSC.

É inspirado pelo sistema atualmente implementado, desenvolvido por Cesar Dias Parente. Os arquivos do projeto podem ser encontrados [aqui](https://github.com/ZeniteSolar/MDE18/tree/main).

A embarcação da equipe Zênite é operada em competições de barcos solares, onde são exigidas alta performance e robustez. A equipe obteve êxito em diversos anos de competição, a direção elétrica contribuindo para isso especialmente em provas de manobras como o Slalom.

O sistema eletrônico atual foi danificado após a competição em março de 2022, o que é um motivador para sua atualização. 

# Concepção

Pensando em um sistema de direção, a escolha de utilizar uma direção elétrica pode trazer maior responsividade mesmo tendo uma fração do peso de um sistema mecânico.[referência]. Além disso, sistemas mecânicos podem pedir esforço físco do motorista e uma necessidade maior de manutenção. Por essas razões

Abaixo está o diagrama representando um sistema de direção elétrica e suas partes. 
![Diagrama concepção](https://github.com/ayresgit/Modulo-Direcao-Eletrica/blob/cb3b7e7deb8563f8dec3147824b006344d2aa883/Imagens/Diagrama%20de%20blocos%20da%20dire%C3%A7%C3%A3o%20el%C3%A9trica.PNG)

Levando em contas os elementos acima, os seguintes problemas devem ser confrontados:
- Sistemas eletrônicos em veículos devem possuir alta robustez devido às vibrações ao qual são expostos. No caso da embarcação em alta velocidade, especialmente para operações marítimas, os impactos são um grande problema (além da oxidação dos circuitos).
- Como o veículo alvo é utilizado em competições cujo foco, e portanto o desafio, está na geração e consumo de energia, a solução deve ser tão eficiente quando possível para garantir o sucesso da equipe.
- Um ambiente competitivo entre veículos requer alta performance dessa direção, não apenas para alcançar a vitória mas garantir a segurança de todos. Portanto, a funcionalidade deve ser atendida tão bem quanto os outros parâmetros.

Para implementar o todo é preciso definir os requisitos das partes, e as possíveis propostas para satisfazê-los.

* ## Sensores

Existem algumas opções para os sensors de rotação como: indutivo, óptico, sensor de efeito Hall, potênciometro de rotação, encoder. A alternativa mais econômica e de simples implementação é um potênciometro linear comum, atualmente utilizados no sistema da equipe Zênite.

Os tipos mais utilizados para sensores de direção (steering) no mercado ([Bosch](https://www.bosch-motorsport.com/content/downloads/Raceparts/en-GB/120530059.html), [CUI Inc](https://www.cuidevices.com/catalog/motion/rotary-encoders), [TT Electronics](https://www.ttelectronics.com/products/categories/steering-sensors/search-results/)), são os resistivos, encoders, e os de tecnologia GMR ([Giant magnetoresistance](https://en.wikipedia.org/wiki/Giant_magnetoresistance)).

<!-- Explicar encoders e GMR?  -->
Sensores desse nível são tecnologia de ponta portanto estão fora de questão, no entanto, vale observar que o sensor resistivo permanece sendo utilizado (até mesmo em veículos comuns [link]()). 

Para um contraste entre os líderes: sensores resistivos têm desvantagem pelo desgaste e possível mal contato, os GMR podem sofrer interferência em ambientes com forte campo magnético, e os encoders (por serem digitais) apresentam um desafio na troca Precisão X Complexidade (preço). 

Buscando o equilíbrio entre robustez, facilidade de implementação e preço, o sensor resistivo se destaca. 
 
* ## Controlador

A rede CAN implementada elimina a necessidade de cabeamento entre popa e proa, reduzindo ruído na leitura dos sensores e eliminando problemas mecânicos com cabeamento. Além disso, a comunicação pela rede possibilita o envio de dados pelo módulo e o controle da direção utilizando mensagens de outros módulos.

Um microcontrolador de linha comercial será o suficiente para garantir a execução das tarefas e tempo de resposta desejados. Além de portas PWM e entradas do ADC, seria interessante um microcontrolador com interface CAN como o STM32F103xx ou equivalentes. No entanto, essa alternativa supera o preço de CI's transceiver e interface CAN, além disso é trabalhoso de gravar e regravar (sendo que a última opção é um grande diferencial para corrigir erros, durante uma competição por exemplo).

Pela facilidade de programação e preço, é proposto o [ATmega328P](https://br.mouser.com/ProductDetail/Microchip-Technology-Atmel/ATMEGA328P-PU?qs=K8BHR703ZXguOQv3sKbWcg%3D%3D). Para a conexão com a rede can, os CI's mais comum e de preço acessível são o transceiver 
[MCP2551](https://br.mouser.com/datasheet/2/268/20001667G-1115479.pdf) e o circuito de interface CAN/SPI [MCP2515](https://br.mouser.com/datasheet/2/268/MCP2515_Family_Data_Sheet_DS20001801K-2303489.pdf).

O controlador que não está em foco é necessário, no entanto, não há necessidade de criar um módulo CAN cujo único propósito é a leitura de um valor analógico. Essa função pode ser desempenhada por outro módulo já presente na proa.

* ## Potência

O motor DC brushed é acionado por uma ponte H, atualmente alimentada por uma bateria de 12V, essa bateria é exlusivamente utilizada para o sistema da direção. Comparado com a alternativa, de usar a tensão do banco de baterias, esse sistema tem desvantagem pelo peso da bateria adicional e pela necessidade de um módulo para carregar a mesma.

Para corrigir isso o circuito será dimensionado para a tensão do banco de baterias. Considernado uma troca iminente nesse sistema, será projetada para o sistema futuro do barco:

|| Banco Atual | Banco futuro |
|-----------|-----------|-----------|
| Vmax Célula | 15,6V | 2,8V |
| Baterias | 3 série | 21 série |
| Vmax Banco | 46,8V | 58,8V |

Testes de rotor travado indicaram o motor operando a um máximo de 50A com 12V, assim, os limites da ponte H são 60V e 50A.

* ## Medidas e Feedback

É interessante fornecer à rede CAN dados sobre o consumo do sistema, portanto é proposto um sensor de corrente para cada braço da ponte H. O [INA240](https://www.ti.com/lit/ds/symlink/ina240.pdf?ts=1649772128538) é um exemplo de sensor de corrente bidirecional com rejeição de PWM, mas existem alternativas com o resistor shunt integrado.

Essa medida deve constribuir para o feedback, que também terá o dado de posição da rabeta. Levar esses dados à um display garante que o piloto tenha conhecimento sobre o estado do sistema e sobre o consumo que sua pilotagem está provocando.

# Design


<!-- Aqui ou na etapa de Design? -->
<!-- | ATmega328P | MCP2515 | MCP2551 |
|--------|--------|--------|
|![ATmega328P](https://github.com/ayresgit/Modulo-Direcao-Eletrica/blob/ce93888b1ef8f3adbeb4987ba464b1fd88e084a3/Imagens/ATmega328P-PU.PNG)|![MCP2515](https://github.com/ayresgit/Modulo-Direcao-Eletrica/blob/ce93888b1ef8f3adbeb4987ba464b1fd88e084a3/Imagens/MCP2515.PNG)|![MCP2551](https://github.com/ayresgit/Modulo-Direcao-Eletrica/blob/ce93888b1ef8f3adbeb4987ba464b1fd88e084a3/Imagens/MCP2551.PNG)| -->



# Implementação


# Operação

# Considerações

# Bibliografia







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

Existem algumas opções para os sensors de rotação como: magnético, óptico, resistivos (como potenciômetro). Considerando a precisão e robustez necessária para o trabalho, e o preço das alternativas, é recomendado um potenciômetro linear comum.
<!-- Dúvida sobre instrumentação. Como definir (de maneira teórica) que o potenciômetro comum é o suficiente em termos de precisão? Sendo um componente passivo, trabalhando com uma grandeza contínua, o trabalho é mais do circuito de amostragem do que do Sensor? -->

* ## Controlador

A rede CAN implementada elimina a necessidade de cabeamento entre popa e proa, reduzindo ruído na leitura dos sensores e eliminando problemas mecânicos com cabeamento. Além disso, a comunicação pela rede possibilita o envio de dados pelo módulo e o controle da direção utilizando mensagens de outros módulos.

Um microcontrolador de linha comercial será o suficiente para garantir a execução das tarefas e tempo de resposta desejados. Além de portas PWM e entradas do ADC, seria interessante um microcontrolador com interface CAN como o STM32F103xx ou equivalentes. No entanto, essa alternativa supera o preço de CI's transceiver e interface CAN, além disso é trabalhoso de gravar e regravar (sendo que a última opção é um grande diferencial para corrigir erros, durante uma competição por exemplo.

Pela facilidade de programação e preço, é proposto o ATmega328P. Para a conexão com a rede can, os CI's mais comum e de preço acessível são o transceiver 
[MCP2551](https://br.mouser.com/datasheet/2/268/20001667G-1115479.pdf) e o circuito de interface CAN/SPI [MCP2515](https://br.mouser.com/datasheet/2/268/MCP2515_Family_Data_Sheet_DS20001801K-2303489.pdf).

| ATmega328P | MCP2515 | MCP2551 |
|--------|--------|--------|
|![ATmega328P](https://github.com/ayresgit/Modulo-Direcao-Eletrica/blob/ce93888b1ef8f3adbeb4987ba464b1fd88e084a3/Imagens/ATmega328P-PU.PNG | width=100)|![MCP2515](https://github.com/ayresgit/Modulo-Direcao-Eletrica/blob/ce93888b1ef8f3adbeb4987ba464b1fd88e084a3/Imagens/MCP2515.PNG)|![MCP2551](https://github.com/ayresgit/Modulo-Direcao-Eletrica/blob/ce93888b1ef8f3adbeb4987ba464b1fd88e084a3/Imagens/MCP2551.PNG)|




O controlador que não está em foco é necessário, no entanto, não há necessidade de criar um módulo CAN cujo único propósito é a leitura de um valor analógico. Essa função pode ser desempenhada por outro módulo já presente na proa.

* ## Potência
PONTE H
Eficiência, parâmetros elétricos de operação (tecnologia do mosfet; driver?) [parâmetros do motor, alimentação do circuito]. Robustez.

<!-- 
|| Sensor | Controlador | Potência |
|-----------|-----------|-----------|-----------|
| x | x | x | x | -->

# Design


# Implementação


# Operação

# Considerações

# Bibliografia







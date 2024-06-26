# Direção Elétrica para Embarcação Solar

Instituto Federal de Educação, Ciência e Tecnologia de Santa Catarina - Campus Florianópolis

Departamento Acadêmico de Eletrônica

Curso de Engenharia Eletrônica

Unidade Curricular: Projeto Integrador III

Professores:

* Robinson Pizzio e Daniel Lohmann

Aluno:

* Gabriel Ayres Rodrigues

# Sumário

1. [Introdução](#introdução)
2. [Concepção](#concepção)
3. [Design](#design)
4. [Implementação](#implementação)
5. [Operação](#operação)
6. [Considerações Finais](#considerações)
7. [Referências](#referências)

# Introdução

Esse módulo tem como objetivo controlar a direção da rabeta da embarcação Guarapuvu II, da equipe Zênite Solar do IFSC.

É inspirado no sistema atualmente implantado, desenvolvido por Cesar Dias Parente. Os arquivos do projeto podem ser encontrados [aqui](https://github.com/ZeniteSolar/MDE18/tree/main).

A embarcação da equipe Zênite é operada em competições de barcos solares, onde são exigidas alta performance e robustez. A equipe obteve êxito em diversos anos de competição, a direção elétrica contribuindo para isso especialmente em provas de manobras como o Slalom (Ziguezague).

O sistema eletrônico atual foi danificado após a competição em março de 2022, o que é um motivador para sua atualização.

# Concepção

Pensando em um sistema de direção, a escolha de utilizar uma direção elétrica pode trazer maior responsividade mesmo tendo uma fração do peso de um sistema mecânico. Além disso, sistemas mecânicos podem pedir esforço físico do motorista e uma necessidade maior de manutenção. Por essas razões a direção elétrica é vantajosa no sistema da embarcação.

Abaixo está o diagrama representando um sistema de direção elétrica e suas partes.
![Diagrama concepção](https://github.com/ayresgit/Modulo-Direcao-Eletrica/blob/cb3b7e7deb8563f8dec3147824b006344d2aa883/Imagens/Diagrama%20de%20blocos%20da%20dire%C3%A7%C3%A3o%20el%C3%A9trica.PNG)

Levando em contas os elementos acima, os seguintes problemas devem ser confrontados:
- Sistemas eletrônicos em veículos devem possuir alta robustez devido às vibrações ao qual são expostos. No caso da embarcação em alta velocidade, especialmente para operações marítimas, os impactos são um grande problema (além da oxidação dos circuitos).
- Como o veículo alvo é utilizado em competições cujo foco está na geração e consumo de energia, a solução deve ser tão eficiente quanto possível para garantir o sucesso da equipe.
- Um ambiente competitivo entre veículos requer alta performance dessa direção, não apenas para alcançar a vitória mas garantir a segurança de todos. Portanto, a funcionalidade deve ser atendida tão bem quanto os outros parâmetros.

Para implementar o todo é preciso definir os requisitos das partes, e as possíveis propostas para satisfazê-los.

* ## Sensores

Existem algumas opções para os sensores de rotação como: indutivo, óptico, sensor de efeito Hall, potênciometro de rotação, encoder. A alternativa mais econômica e de simples implementação é um potênciometro linear comum, atualmente utilizado no sistema da equipe Zênite.

Os tipos mais utilizados para sensores de direção (no sentido steering) no mercado são os seguintes: resistivos, encoders, e os de tecnologia GMR ([Giant magnetoresistance](https://en.wikipedia.org/wiki/Giant_magnetoresistance)). (veja as empresas consideradas em [Referências](#referências))

Sensores desse nível são tecnologia de ponta e portanto estão fora de questão, de qualquer forma vale observar que o sensor resistivo permanece sendo utilizado (até mesmo em veículos comuns, como o [sensor de posição borboleta](https://www.dpk.com.br/como-funciona-o-sensor-de-posicao-da-borboleta/)).

Para um contraste entre os líderes: sensores resistivos têm desvantagem pelo desgaste e possível mal contato, os GMR podem sofrer interferência em ambientes com forte campo magnético, e os encoders (por serem digitais) apresentam um desafio na troca Precisão X Complexidade (preço).

Buscando o equilíbrio entre robustez, facilidade de implementação e preço, o sensor resistivo se destaca.
 
* ## Controlador

A rede CAN implementada elimina a necessidade de cabeamento, com o próprio sinal, entre popa e proa. Isso garante redução de ruído na leitura dos sensores e aproveitar o cabeamento CAN já existente, compactando o sistema.

Além disso, a comunicação pela rede possibilita o envio de dados pelo módulo e o controle da direção utilizando mensagens de outros módulos. Dessa forma, fica aberto o leque para futuras implementações que podem incluir:

    Mensagens e/ou sensores de redundância
    Modo de baixo consumo - Ativado pelo módulo de processamento de dados
    Direção remota
    


Um microcontrolador de linha comercial será o suficiente para garantir a execução das tarefas e tempo de resposta desejados. Além de portas PWM e entradas do ADC, seria interessante um microcontrolador com interface CAN como o STM32F103xx ou equivalentes. No entanto, essa alternativa supera o preço de CI's transceiver e interface CAN, além disso sua programação é mais trabalhosa.

Pela facilidade de programação e preço, é proposto o [ATmega328P](https://br.mouser.com/ProductDetail/Microchip-Technology-Atmel/ATMEGA328P-PU?qs=K8BHR703ZXguOQv3sKbWcg%3D%3D). Para a conexão com a rede can, os CI's mais comum e de preço acessível são o transceiver
[MCP2551](https://br.mouser.com/datasheet/2/268/20001667G-1115479.pdf) e o circuito de interface CAN/SPI [MCP2515](https://br.mouser.com/datasheet/2/268/MCP2515_Family_Data_Sheet_DS20001801K-2303489.pdf).

O controlador que não está em foco é necessário. A função de leitura do potenciômetro do volante será desempenhada por outro módulo já presente na proa: [MIC19](https://github.com/ZeniteSolar/MIC19)


* ## Circuito de Potência

O motor DC brushed é acionado por uma ponte H, atualmente alimentada por uma bateria de 12V. O circuito atual é implementado por dois integrados meia-ponte [BTS7960](https://pdf1.alldatasheet.com/datasheet-pdf/view/152657/INFINEON/BTS7960.html).

Esse componente foi descontinuado, e a melhor alternativa encontrada foi o [FAN7093](https://www.mouser.com/datasheet/2/149/FAN7093-76982.pdf). Como o atual já está em mãos e esse projeto tem como foco integração e telemetria, foi utilizado o mesmo circuito integrado.


* ## Medidas e Feedback

É essencial fornecer à rede CAN dados sobre o consumo do sistema, por ser um sistema de potência alimentado por bateria. Portanto é proposto um sensor de corrente para cada braço da ponte H.

Convenientemente, o circuito integrado meia-ponte BTS7960 possui um pino de saída para sensoriamento de corrente — uma fonte de corrente é conectada à saída, e a corrente IS é proporcional à corrente na carga IL de acordo com o resistor de medida RIS.

Alternativamente, poderia ser usado um circuito como o [INA240](https://www.ti.com/lit/ds/symlink/ina240.pdf?ts=1649772128538), sensor de corrente bidirecional ultrapreciso com rejeição de PWM. Da mesma família de CI's, o INA826 será utilizado para medida de tensão da bateria (tensão na ponte H).

Essas medidas devem contribuir para o feedback, que também terá o dado de posição da rabeta. Levar esses dados à um display garante que o piloto tenha conhecimento sobre o estado do sistema e sobre o consumo que sua pilotagem está provocando.

# Design

O módulo foi desenvolvido com o software Kicad, o projeto está na pasta hardware desse git e seu esquemático completo pode ser visualizado [aqui](https://github.com/ayresgit/Modulo-Direcao-Eletrica/blob/main/hardware/PDF/steeringmodule.pdf). Abaixo está o esquemático geral:

![Esquemático geral](https://github.com/ayresgit/Modulo-Direcao-Eletrica/blob/a6de4a60345ccac29f99505179099171639378f8/Imagens/steeringmodule_esquema.PNG)

Os componentes escolhidos a partir da análise na concepção são os seguintes: Potenciômetros Lineares, Atmega328P, BTS7960, MCP2515, MCP2551 (ou MCP2561), e INA826.

Para estar de acordo com os módulos do sistema da embarcação, também serão necessários os seguintes componentes:

    2 conectores fêmea RJ45,
    2 borne fêmea 2EDGRC 2 vias (robusto e prático, alternativa é usar terminal block),
    soquete estampado 28 pinos para o atmega328p,
    diodos zener para proteção dos ADC do microcontrolador,
    2 cristais osciladores

O software Kicad possui uma função para exportação de lista completa de materiais (BOM - Bill Of Materials). Pode ser encontrada na pasta hardware, neste [link](https://github.com/ZeniteSolar/MDE22/blob/f59e0732b5ff4f93045a860adb85f9820dcc4e83/hardware/steeringmodule.csv).

Abaixo está a previsão em 3D do resultado final. Como será explicado na etapa de implementação, a PCB foi confeccionada com a ajuda de uma fresadora da instituição.

| Frente | Trás |
| -----  | ----- |
|<img src="https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/pcb%20front.PNG" width ="325" height="460">|<img src="https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/pcb%20back.PNG" width="325" height="460">|


Vale notar que este repositório possui uma branch exclusiva para uma placa encomendada, cujo layout foi aprimorado e a silkscreen foi propriamente desenhada para auxiliar na etapa de Implementação e possíveis reparos. Essa placa não foi utilizada durante o projeto pela data de entrega da mesma.

| Encomendada Frente | Encomendada Trás |
| -----  | ----- |
|<img src="https://github.com/ZeniteSolar/MDE22/blob/a99deb80abc7bee7c553f9edaa48115d4b41ed4c/Imagens/Encomendada%20Frente.jpg" width ="325" height="530">|<img src="https://github.com/ZeniteSolar/MDE22/blob/a99deb80abc7bee7c553f9edaa48115d4b41ed4c/Imagens/Encomendada%20Tr%C3%A1s.jpg" width="325" height="530">|

# Implementação

### Confecção da PCB

A PCB foi confeccionada pela fresadora CNC do DAELN, operada pelos técnicos do departamento, o resultado é mostrado abaixo. Como a placa confeccionada não apresentou o espaçamento mínimo desejado para a malha de potência, uma retificadeira foi usada para excluir as trilhas indesejadas — é importante lembrar que esse tipo de ferramenta pede o uso de EPI.

| Ajustes com a dremel | EPI |
| --- | --- |
|<img src="https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/Ajustes%20com%20a%20dremel.jpg" width="250" height="315">|<img src="https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/Use%20%C3%B3culos%20de%20prote%C3%A7%C3%A3o.jpg" width="250" height="315">

A placa foi estanhada para proteger o circuito contra oxidação, e as vias foram feitas com fios de cabo de rede, cortados no tamanho adequado e então soldados.

Da ordem dos componentes, a solda foi feita da seguinte forma: SMD, CI, Through-hole sendo os conectores os últimos (para evitar esforço físico na placa).

Após o trabalho na placa, foi alcançada a conclusão de que os CI da ponte H devem perceber os SMD. Essa troca é interessante pois o modelo precisa ser soldado com calor, no caso com auxílio de soprador térmico. A placa soldada pode ser vista na figura.

| Frente | Trás |
|:-----:|:-----:|
|<img src="https://github.com/ZeniteSolar/MDE22/blob/70da552cb5a2f2a8a49e6934afd4cba5a5405d8f/Imagens/PCB%20soldada%20frente.jpg" width="200" height="325">|<img src="https://github.com/ZeniteSolar/MDE22/blob/3640950bb22653374b23fce8999609d81420ede7/Imagens/PCB%20soldada%20tr%C3%A1s.jpg" width="200" height="325">|

Finalizados os processos de preparação, é dado início à programação do microcontrolador, que é feita na linguagem C. As bibliotecas utilizadas e código fonte podem ser encontradas na pasta firmware desse repositório.

# Operação

A etapa de operação fica dividida entre duas partes: Programação e testes de bancada. Dado início na lógica básica, como quais periféricos do microcontrolador são utilizados, estas etapas se desenvolvem em conjunto.

#### Programação

Durante todo o processo foi utilizado como referência o datasheet da microchip para o atmega328p, que se encontra nas [Referências](#referências) deste trabalho.

Na configuração em si dos periféricos, começando pelo ADC, foi encontrado um dilema sobre qual orientação dos potenciômetros do sistema. Como o sistema está em upgrade foi descartada a configuração atual. Fica então definido que o código desenvolvido pressupõe as seguintes relações:

| Potenciômetro | Posição da Rabeta |
| ------------- | ------------------- |
| Valor máximo  | limite de estibordo |
| Valor médio   | centrada  |
| Valor mínimo  | limite de bombordo |

- ADC

O sistema mecânico que acopla o motor DC ao potênciometro de leitura da rotação tem um limite próximo aos 270°. A capacidade de manobrar com mais de 90° para cada lado se provou útil no passado. Com isso em mente, e considerando que não é necessária uma precisão extrema no controle desse ângulo, o ADC de 8 bits é suficiente para os três canais:

- Sensor de tensão
- Sensor de corrente
- Sensor de rotação

A escala das medidas é configurada pelos coeficientes em adc.c, basta operar algumas medidas com coeficiente unitário e então fazer o cálculo.

$$ coeficiente = {ValorDesejado \over ValorObtido} $$
    
É interessante fazer esse teste com o valor de fundo de escala, pois o "Valor Desejado" é naturalmente conhecido.

- Rede CAN

Do trabalho com a biblioteca CAN: a rede CAN é implementada a partir de ID's, tendo identificação dos módulos no barramento e de suas mensagens. Para gerar esses ID's a equipe Zênite desenvolveu um script que gera esses valores para o módulo em desenvolvimento.

Esse caminho também trás segurança, garantindo que não haverão ID's iguais e reduzindo possíveis confusões causadas por erro humano e/ou desenvolvimento paralelo. Tudo isso pode ser encontrado no repositório [CAN_IDS](https://github.com/ZeniteSolar/CAN_IDS).

Tendo os ID's gerados para o módulo e suas mensagens, é preciso alterar o can_filter.h para incluir os ID's de quem se deseja escutar (no caso esse projeto precisa escutar o [MIC19](https://github.com/ZeniteSolar/MIC19)), e criar as funções de extração e envio de mensagens em can_app.c/h.

- PWM

Alguns pontos chave para a operação do PWM: Timer escolhido, prescaler (frequência desejada), definição do duty cycle (nesse caso implementado pela função set_pwm em hbridge.c).

Por um erro na etapa de Design, as saídas PWM estão conectadas nos pinos com o timer básico do ATmega328P, o Timer/Counter 0. O Modo de operação do timer deve ser o de phase correct PWM (que garante maior sincronismo em relação ao Fast PWM). Como descrito no datasheet a frequência pode ser calculada da seguinte forma:

$$ fOCnxPWM = {fclkI/O \over {N * 510}} $$

    fOCnxPWM é a frequência do PWM nas portas OCnx (no caso OC0A e OC0B, em PD5 e PD6)
    fclkI/O é a frequência de clock do cristal oscilador (16MHz)
    N é o prescaler configurado

Esse timer possui menos bits e menor funcionalidade em comparação aos outros dois. Para a aplicação o único impacto foi a utilização de uma frequência inferior ao desejado (~3921.5Hz). Isso acontece pela falta de opções de prescaler no Timer/Counter0:
    
    prescaler = 1, fPWM =  31.37kHz (supera os limites do CI half-bridge)
    prescaler = 8 --> 3.92kHz  	 (utilizado)


#### Testes no Circuito

Os testes em bancada foram feitos com o auxílio de fontes de tensão variada, nos laboratórios do IFSC câmpus Florianópolis.

<p align="center">
  <img src="https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/Fontes%20de%20bancada%203A.jpg" width="520" height="340">

Foram efetuados os testes do ADC e da rede CAN, esses resultados foram visualizados no computador pela comunicação serial conectando um arduino ao barramento Usart da placa.

No caso do PWM é necessário o uso de osciloscópio, portanto são deixadas as figuras para alguns dos testes executados com o auxílio de um reostato.

| Duty Cycle 40% Sinal | Duty Cycle 40% Valores |
| ------ | ------ |
|![Duty Cycle 40% signal](https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/Duty%20Cycle%2040%25%20Signal.jpg)|![Duty Cycle 40% values](https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/Duty%20Cycle%2040%25%20Values.jpg)|

| Duty Cycle 80% | Duty Cycle 80% Valores |
| ------ | ------ |
|![Duty Cycle 80% signal](https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/Duty%20Cycle%2080%25%20Signal.jpg)|![Duty Cycle 80% values](https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/Duty%20Cycle%2080%25%20Signal.jpg)|

Para testar o circuito com alta corrente foi reduzida a resistência no reostato e mantido o duty cycle:
| Duty Cycle 80% 3A  | Duty Cycle 80% Valores |
| ------ | ------ |
|![Duty 80% Signal (3A)](https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/Duty%20Cycle%2080%25%20Signal%20-%20Low%20resistance,%203A.jpg)|![Duty 80% Values (3A)](https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/Duty%20Cycle%2080%25%20Values%20-%20Low%20resistance,%203A.jpg)|

Esses resultados foram satisfatórios, tendo em vista que o circuito não estava acumulando calor, o PWM estava de acordo com o desejado a partir da configuração, e os valores de acordo com o datasheet.

#### Testes com Motor

O próximo passo é o teste com o motor. A montagem do teste em bancada é feita na embarcação, com fontes de bancada, placas e conexões necessárias.

<p align="center">
  <img src="https://github.com/ZeniteSolar/MDE22/blob/8283bc81c4a9d4ee74052ef4564bf314acc439f0/Imagens/Testes%20com%20Motor.jpg" width="420" height="250">

Durante o teste com motor foram implementadas lógicas de operação ao PWM da ponte H:

    Ponte H ativada pela diferença entre os potenciômetros
    Começo centrado                              		 
    Centro forçado caso não haja comunicação      		 
    Proteção contra lado invertido

A última lógica foi julgada interessante considerando que o próprio sistema é montado/desmontado para viagens e testes na água. É possível que haja um erro de conexão que, sem essa proteção, poderia fazer com que um giro contínuo de 360° danificasse o sistema.

A parte mecânica pode ser vista na figura abaixo. O eixo da transmissão é acoplado na correia pelo disco, e o motor DC da direção controla a correia.
No topo desse pode ser visto o potenciômetro utilizado para medida de rotação.

<p align="center">
  <img src="https://github.com/ZeniteSolar/MDE22/blob/8283bc81c4a9d4ee74052ef4564bf314acc439f0/Imagens/Motor%20e%20correia.jpg" width="340" height="250">

Os testes com motor foram um sucesso utilizando as fontes de bancada, o vídeo do funcionamento está na pasta Imagens. Devido ao tamanho do arquivo o github não pode executá-lo, portanto precisa ser baixado. Uma alternativa é utilizar este [link do drive](https://drive.google.com/file/d/12XCSZ1QREOxFkqlZAvMiaMpx_ZMUlxNi/view?usp=sharing).

Durante o teste alimentado com a bateria de 12V o sistema deixou de funcionar. A falta de capacitância para a tensão de entrada causou falha em um dos CI de meia ponte, após reposição do CI e correção dos valores no esquemático o sistema está seguro e operacional.

# Considerações

Todos os periféricos do microcontrolador estão funcionais. Todas as medidas elétricas, que eram um dos principais objetivos, apresentaram precisão satisfatória.

A lógica essencial do funcionamento está completa, existem travas de segurança e por fim o motor pode ser movimentado.

Performance dos componentes: satisfatória.

Confecção da placa: resultado ruim, a fabricação ocasionou mais trabalho e comprometeu a robustez do sistema. Para atender corretamente o objetivo de um sistema mais robusto deve ser utilizada a placa encomendada.

Abaixo podemos ver o sistema montado com um suporte experimental — devido às dimensões da placa, é preciso ser fabricado um suporte dedicado à mesma.

Ao lado é mostrada o módulo fonte principal ([MFP19](https://github.com/ZeniteSolar/MFP19)) que converte a tensão no banco de baterias e fornece os 18V da rede CAN. Os futuros suportes do barco devem seguir este modelo feito com impressão 3D.

| Sistema Montado | Exemplo de suporte |
| --- | --- |
|<img src="https://github.com/ZeniteSolar/MDE22/blob/70da552cb5a2f2a8a49e6934afd4cba5a5405d8f/Imagens/Sistema%20Montado.jpg" width="250" height="360">|<img src="https://github.com/ZeniteSolar/MDE22/blob/70da552cb5a2f2a8a49e6934afd4cba5a5405d8f/Imagens/Exemplo%20de%20suporte.jpg" width="250" height="360">|

Para efeito de comparação, aqui estão as imagens do sistema anterior.

| Sistema Antigo | Ponte H |
| --- | --- |
|<img src="https://github.com/ZeniteSolar/MDE22/blob/8283bc81c4a9d4ee74052ef4564bf314acc439f0/Imagens/Montagem%20do%20sistema%20antigo.jpg" width="250" height="280">|<img src="https://github.com/ZeniteSolar/MDE22/blob/8283bc81c4a9d4ee74052ef4564bf314acc439f0/Imagens/Montagem%20antiga%20PonteH.jpg" width="280" height="210">|

#### Próximos Passos

Alguns dos próximo passos para esse módulo e futuras atualizações:

- PWM responsivo aos movimentos do piloto
- Projeto da Ponte H ou escolha de um CI não descontinuado
- Mensagens e/ou sensores de redundância
- Modo de baixo consumo - Ativado pelo módulo de processamento de dados
- *Direção remota


*O sucesso dessa etapa criaria espaço para ainda outra atualização: O barco autônomo.

# Referências

Datasheet ATmega328P:
[Microchip, Atmel ATmega328P](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)

Pesquisa por sensores de steering no mercado:
[Bosch](https://www.bosch-motorsport.com/content/downloads/Raceparts/en-GB/120530059.html),
[CUI Inc](https://www.cuidevices.com/catalog/motion/rotary-encoders),
[TT Electronics](https://www.ttelectronics.com/products/categories/steering-sensors/search-results/)










# Projeto_Esteira
O algoritmo para esse esteira está separado em tasks que estão localizadas na pasta [lib](./lib) que possuem cada uma a sua função e executam paralelamente no microcontrolador, sendo as tasks utilizadas pela esteira as listadas abaixo:

- ## [TaskMotors](./lib/TaskMotors/src)

Essa task tem como função controlar o motor da esteira, ler o sensor IR que detecta as peças que devem ser rejeitadas e também controla o servo motor que direciona as peças para o compartimento apropriado.

  Dessa forma, a velocidade dos motores e o sentido da rotação deles serão definidos pela controladora sem fio da esteira e o sensor IR irá definir quais peças devem entrar no compartimento de peças com defeito e quais não, logo quando uma peça acionar o sensor IR o servomotor mudará de posição e direcionará a peça para o compartimento de peças defeituosas. Além disso, essa task envia pacotes contendo dados sobre o estado do sensor IR e da quantidade de peças rejeitadas para a task que trata da comunicação entre a esteira e a controladora dela.
- ## [TaskEspNow](./lib/TaskEspNow/src/)

Essa task tem como função tratar da comunicação sem fio da esteira com a controladora dela, via protocolo Esp-Now.

Além disso, os dados compartilhados entre as tasks e também utilizados como pacotes para o protocolo Esp-Now, estão na estrutura definida no arquivo [SharedStruct](./lib/SharedStruct/src)

Para mais informações sobre o controlador sem fio da esteira, acesse o repositório a seguir: [Controlador_esteira](https://github.com/Matheus-de-Sousa/Controlador_esteira) 

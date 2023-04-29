# Projeto_Esteira
O algoritmo para esse esteira está separado em tasks que estão localizadas na pasta [lib](./lib) que possuem cada uma a sua função e executam paralelamente no microcontrolador, sendo as tasks utilizadas pela esteira as listadas abaixo:

- ## [TaskMotors](./lib/TaskMotors/src)

Essa task tem como função controlar o motor da esteira, ler o sensor IR que detecta as peças que devem ser rejeitadas e também controla o servo motor que direciona as peças para o compartimento apropriado.
- ## [TaskEspNow](./lib/TaskEspNow/src/)

Essa task tem como função de tratar da comunicação sem fio da esteira com a controladora dela, via protocolo Esp-Now.

Além disso, os dados compartilhados entre as tasks e também utilizados como pacotes para o protocolo Esp-Now, estão na estrutura definida no arquivo [SharedStruct](./lib/SharedStruct/src)

Para mais informações sobre o controlador sem fio da esteira, acesse o repositório a seguir: [Controlador_esteira](https://github.com/Matheus-de-Sousa/Controlador_esteira) 

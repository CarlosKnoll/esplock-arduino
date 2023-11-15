# Projeto ESPLock

Iniciado como um projeto de Iniciação Científica na UTFPR-PG, ele busca desenvolver um sistema de controle de acesso de usuários a ambientes de sala de aula e laboratórios.

O sistema utiliza um microcontrolador ESP32 da Heltec, com interfaces sem fio WiFi, Bluetooth e LoRa.

O ESP possui um leitor de Tags RFID para controlar o acesso.

O ESP opera o WiFi em modo AP. Ele possui um Web Server embarcado que permite conexões de qualquer navegador web, seja de computador ou celular. A interface com o usuário é feita através de páginas web.

O sistema deve prover:

- Leitura de Tags RFID.
- Cadastro de códigos de Tags RFID e os usuários associados (adição e remoção).
- Registro dos acessos.
- Consulta aos registros dos acessos.
- Envio dos registros de acesso por WiFi a um servidor de banco de dados para armazenamento.
- Permite operação e configuração através de páginas web.
- Acionar a liberação da porta fornecendo um pulso de liberação para uma fechadura elétrica.

Detalhes do projeto:

(em construção)
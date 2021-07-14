READEME -- TEMA2 -- Protocoale de comunicatie -- 323CC Radulescu Florin

Tema reprezinta o aplicatie care respecta modelul client-server pentru gestiunea
mesajelor. Structura arhivei ce contine tema este urmatoarea:
1) helpers.h -> fisier cu importurile necesare fisierelor cpp, antetele functii-
  lor folosite si descrierea structurilor de mesaje concepute pentru protocolul
  implementat;
2) server.cpp -> implementarea serverului
3) subscriber.cpp -> implementarea clientului TCP
4) Makefile -> se va rula make pentru compilarea fisierelor cpp incluse
5) readme.txt

Descrierea pe larg a functionalitatii aplicatiei pornind din fisierele incluse:

1) helpers.h: - contine importurile necesare functionarii temei
  - contine un macro de verificare al erorilor (DIE), care va fi foarte folosit
    in cadrul fisierelor cpp pentru a obstructiona propagarea unor erori ce ar
    putea cauza oprirea sau functionarea deficitara a aplicatiei
  - 3 structuri asignate mesajelor trimise in aplicatie, dupa cum urmeaza:
    a) tcp2srv - mesaj trimis de catre un client TCP la server
    b) udp2srv - mesaj trimis de catre un client UDP la server
    c) srv2tcp - mesaj trimis de catre server unui client TCP
  - 2 antete de functii, dupa cum urmeaza:
    a) command - functie care e folosita de clientul TCP pentru a trimite catre
      server o comanda de tipul subscribe/unsubscribe
    b) parse_message - functie folosita de catre server pentru a transforma un
      mesaj de tip udp2srv intr-un mesaj de tip srv2tcp

2) server.cpp, care detine urmatoarele functionalitati:
  - pastrarea in memorie a unei sttucturi de date sub forma unui map ce contine
    topicurile la care clientii sunt abonati si o lista a acestor clienti speci-
    fica fiecarui topic in parte
  - creare si gestionare socketi si multime de file descriptori
  - ascultarea socketilor
  - interpretarea comenzii <exit> de la stdin cu efect inchidere a serverului si
    a tuturor clientilor conectati si ignorarea altor comenzi de la stdin
  - primirea si interpretarea mesajelor de la clientii UDP
  - acceptarea cererilor de conexiune si gestionarea conexiunii cu clientii TCP
  - receptionarea datelor de la clientii TCP
  - transmiterea eficienta a mesajelor sosite pe un topic la toti abonatii TCP
  - tratarea eficienta a erorilor (comenzi fara efect ale TCP, mesaje eronate)
  ! server.cpp nu detine functionalitatea de Store & Forward

3) subscriber.cpp, care detine urmatoarele functionalitati:
  - transformarea unui input intr-o comanda catre server
  - gestionarea eficienta e erorilor de input - orice eroare de input este igno-
    rata si lasa posibilitatea introducerii unui alt input
  - conectarea la server si trimiterea catre acesta a datelor necesare
  - transmiterea comenzilor de tip subscribe/unsubscribe/deconectare catre ser-
    ver
  - afisarea mesajelor primite de la server (notificari pe topicurile la care
    clientul este abonat)

Mentiuni: aplicatia functioneaza corect pentru inputuri sau mesaje eronate, a-
cestea neafectand transmiterea mesajelor sau pastrarea conexiunilor. Nu este im-
plementata functionalitatea de Store & Forward, drept urmare un client va primi
mesajele de pe un topic doar pana ce se deconecteaza, indiferenet daca parame-
trul SF a fost trimis catre server. La deconcetarea clientului este stearsa de
pe server orice informatie legata de acesta. La reconectarea lui este interpre-
tat ca fiind un client nou si nu va primi mesaje de la topicurile la care fusese
mai demult abonat.

### Lebada Daria-Cristiana - 323CA
# PCom tema4 - Client web. Comunicatie cu REST API

## Functionare aplicatie
Am folosit scheletul de la laboratorul 9 pentru implementarea temei, alaturi de explicatiile din
laborator. Pentru parsarea JSON am ales biblioteca parson. 

## Adaptare request.c
Am completat request.c conform explicatiilor din comentarii, respectand formatul cererii.
Pentru cererile de tip GET adaug numele metodei, calea si versiunea protocolului HTTP
(HTTP/1.1 in cazul nostru), host-ul (34.254.242.81) si daca exista tokenul jwt si/ sau
cookie-urile. O cerere de tip POST are suplimentar fata de GET Entity Headers prin care se
descriu informatii despre date (Content-Type si Content-Length), iar la final datele propriu-zise.
Cererea de tip DELETE, deoarece nu are date atasate, am creat-o dupa modelul cererii de tip GET
(fara cookies, deoarece sterg o carte, deci este necesar doar tokenul).

## Rezolvare client.c
Am doua variabile, logged_in si has_token, pe care le utilizez pe parcursul programului pentru a
tine evidenta daca am un user logat, respectiv daca userul curent are acces la librarie.
Citesc cate o comanda primita de la stdin si trimit catre server cererile/ informatiile necesare:
#### REGISTER:
Am considerat ca inregistrarea unui nou client se poate realiza doar cand userul curent
este deconectat. Dupa ce verific validitatea datelor (sa nu am spatii), trimit catre server o cerere
de tip POST cu username-ul si parola, apoi tratez raspunsul de la server (il anunt pe client daca
username-ul ales este disponibil). Dupa ce afisez un mesaj sugestiv pentru user, eliberez memoria
si inchid conexiunea cu serverul (pana la efectuarea urmatoarei actiuni valide).
Avand in vedere ca toate actiunile urmeaza sablonul descris la register, le voi explica mai succint.
#### LOGIN:
Asemanator cu register, doar ca, in cazul unor date valide, primesc de la server un mesaj
din care parsez cookie-urile si le memorez pentru a le utiliza mai tarziu. De asemenea marchez ca am
un user logat.
#### ENTER_LIBRARY:
Daca am deja un user conectat, primesc de la server un token jwt (pe care il memorez),
care demonstreaza ca userul curent are acces la biblioteca.
#### GET_BOOKS:
Daca userul conectat are acces la biblioteca, se afiseaza toate cartile disponibile cu id
si titlu.
#### GET_BOOK:
Asemanator comenzii anterioare, doar ca aici se afiseaza mai multe informatii despre o singura
carte in functie de id (doar daca id-ul este valid) - titlu, autor, gen, editura, numar de pagini.
#### ADD_BOOK:
Userul are optiunea de a adauga o noua carte in biblioteca. Realizez vaidarea datelor despre
carte in modul urmator: numarul de pagini nu poate contine litere sau caractere speciale, iar celelalte
date nu pot contine doar cifre (sunt string-uri), dar pot contine cifre impreuna cu litere (de exemplu,
titlul "Volum 1" este valid, dar titlul "1" nu - asa am interpretat eu cerinta ca titlul este un string,
sper ca este okay).
#### DELETE_BOOK:
Trimit un id la server si daca este valid, cartea cu id-ul respectiv este eliminata din
biblioteca.
#### LOGOUT:
Se deconecteaza userul curent si se elibereaza memoria alocata pentru cookies si token jwt (daca
utilizatorul detine unul).
#### EXIT:
Se elibereaza memoria ramasa alocata (daca exista) si se incheie programul.
#### Comanda invalida:
In caz de comanda invalida, userul va fi notificat ce comenzi poate utiliza.

## Parsare JSON
Am ales sa folosesc biblioteca parson, deoarece mi s-a parut cea mai usoara varianta. M-a ajutat foarte
mult README-ul pentru biblioteca pentru realizarea parsarilor. Pentru a transforma mesajul pe care il
trimit catre server in format JSON utilizez json_value_init_object() pentru a initializa un obiect, apoi
setez valorile pe care vreau sa le adaug (conform explicatiilor din README). Pentru conversia inversa
(preluarea datelor din JSON-ul primit de la server) folosesc functia json_parse_string(), apoi preiau valorile
cu get (conform explicatiilor din README).

# Observatii
A fost o tema foarte interesanta. Dupa ce am facut prima comanda si m-am documentat despre biblioteca
parson, a mers foarte rapid si mi-a facut placere sa scriu codul. 

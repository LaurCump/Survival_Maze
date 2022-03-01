# Survival_Maze
În cadrul acestui proiect, trebuie implementat un joc labirint, pe care jucătorul va trebui să îl rezolve. Pentru a îngreuna traseul jucătorului, în labirint se vor găsi inamici.

## Jucatorul
Jucatorul va putea controla in varianta Third Person un personaj realizat din cuburi colorate, sub forma unui avatar uman simplificat.

## Deplasare
Jucatorul se poate deplasa in scena folosind tastele directionale sau combinatia traditionala WASD. Tot ansamblul avatarului trebuie sa se miste unitar. Animarea, sub orice fel, a personajului este optionala.

## Atac
In mod implicit, personajul va avea atasata o camera Third Person, dar acesta va avea posibilitatea de atac prin lansarea unor proiectile. Intrarea in modul de atac se poate face pe tasta CTRL sau mouse-dreapta, mod in care camera va fi trecuta in First Person si va putea tinti si lansa proiectile pe directia camerei prin apasarea butonului mouse-stanga sau tasta SPACE.

Proiectilele pot fi obiecte simple (de ex sfere) care au o directie, o viteza si o durata de viata. In momentul in care acestea intalnesc un obstacol (inamic, perete) sau depasesc durata de viata vor disparea din scena.

## Labirint
Pentru a implementa labirintul, acesta poate fi gandit sub forma unui grid de dimensiune oarecare. Un perete poate fi reprezentat de un cub. Pentru a genera labirintul, ne putem folosi de o matrice de dimensiunea gridului. Această matrice poate avea 3 valori diferite:

* 0 - va reprezenta un drum liber (nu vom plasa un cub la această poziție)
* 1 - va reprezenta un perete din labirint (vom plasa un cub)
* 2 - un inamic

Jucătorul va fi poziționat inițial la o poziție aleatoare validă (pe o celulă ce reprezintă un drum liber) si se poate deplasa doar pe drumuri libere, in rest fiind blocat de coliziuni.

IMPORTANT! Labirintul trebuie sa fie diferit intre doua rulari si trebuie să existe cel puțin o ieșire din labirint conectată la calea pe care se află jucătorul.

## Inamici
In labirint jucatorul se poate intalni cu inamici. Acestia patruleaza coridorul (se misca in mod regulat in celula lor galbena din grid). Inamicii nu trebuie sa urmareasca jucatorul, dar pozitionarea lor ii poate bloca trecerea. Forma si culoarea inamicilor trebuie sa difere de forma si culoarea jucatorului. Este suficient ca inamicii sa fie reprezentati de forme simple (un singur mesh).

**Mișcare inamici**

Miscarea inamicilor are loc doar in cadrul patratului galben asociat lor, conform gridului de mai sus. Se poate alege orice tip de miscare, cu conditia ca aceasta sa acopere o suprafata cat mai mare din celula. De exemplu, inamicul poate patrula de-a lungul marginilor propriului patrat (ramanand in interiorul celulei). Ca un alt exemplu, inamicul se poate deplasa pe o traiectorie in forma de “8”.

**Coliziune cu jucătorul**

Pentru a ajunge la iesirea din labirint, jucatorul poate fi obligat sa treaca prin celule ce contin inamici. Totusi, daca jucatorul este atins de un inamic, el pierde viata. Atunci cand pierde toata viata disponibila, se poate afisa “Game Over!” la consola si jocul se va opri imediat.

**Coliziune cu proiectilul**

Jucatorul are posibilitatea sa lanseze proiectile spre inamici. Atunci cand acestea lovesc, inamicul respectiv va suferi o animatie de deformare realizata in Vertex Shader (vertex displacement), iar apoi va disparea. Jucatorul va putea astfel sa-si continue drumul. Deformarea pe care inamicii o sufera trebuie sa produca o modificare in geometria obiectului si nu doar o simpla transformare de rotatie/scalare/translatie.

Pentru deformarea primitivelor ce compun inamicii, se poate aplica de exemplu o funcție de zgomot pe direcția normalelor vârfurilor. Animația de deformare va înceta dupa cateva secunde.

## HUD
Pe langa lumea inconjuratoare, trebuie desenat si un HUD (heads-up display). Elementele de HUD trebuie sa fie proiectate ortografic. Este necesar sa aveti cel putin 2 elemente care apar intotdeauna pe ecranul jucatorului in aceeasi pozitie, ca elemente de interfata grafica:

* **Viață**: Viata este indicata sub forma unui healthbar prin 2 dreptunghiuri, unul wireframe altul solid.
* **Timpul ramas**: Timpul ramas este indicat printr-o a doua bara de stare, in mod similar cu bara de viata.

## Gameplay
Scopul jocului este ca jucatorul sa iasa cu succes din labirint intr-un timp limita.

De fiecare data cand jucatorul este atins de un inamic, acesta pierde din viata. Inamicul trebuie sa fie suficient de mare si/sau sa mearga suficient de rapid pentru a nu permite jucatorului sa il ocoleasca, astfel acesta fiind nevoit sa traga in inamic.

Cand jucatorul iese din labirint, ramane fara viata sau fara timp, jocul se termina.


Daca apasam tasta F1 vom putea vedea cursorul iar daca apasam tasta F2 vom putea intra in modul de developer unde avem viteza mare cand miscam cu left shift si putem sa mergem cu camera sus jos apasand Q sau E.

Harta a fost generate folosind un algoritm inspirat din backtracing.
void Level::GenerateMaze(glm::vec2 currentCell)
""level.cpp""

Pozitionarea initiala se face pur si simplu prin respectand conventia:
    mai jos suntem la coordonatele (0, 0, 0) si totul dupa va fi construit pe baza acestei celule
     |
     v
     0  0  0  0
     0  0  0  0
     0  0  0  0
     0  0  0  0
si folosind latimea unui cub putem sa generam pozitia exacta a unui cub (ocupat sau liber) in lume.

Personajul a fost construit avand la baza o dimensiune (campul size din ""player.h"") ce este un paralelipiped dreptunghic. Pentru coliziuni voi modifica atent aceasta dimensiune inainte de apelarea functiei de coliziune pentru a il dimensiona ca si un paralelipiped ce are doar lungimea inaltimii ca motiv pentru care nu este un cub. 
Astfel coliziunea la player va fi reprezentata de paralelipipedul urmator:
     _________
    / |      /|
   /__|_____/ |
   |  |    |  |
   |  |    |  |
   |  |    |  |
   |  |____|__|
   | /     | /
   |/______|/

Am ales sa fac asta pentru ca atunci cand jucatorul se roteste, se va roti doar VIZUAL, astfel acest "hitbox" nu se va roti niciodata, motiv pentru care e nevoie sa aiba OX cu OZ de lungime egala pentru a nu ii intra uneori corpul prin pereti.

Pentru deplasare vom adauga la pozitia jucatorului efectiv o distanta de miscare in functie de ce este apasat la tastatura. In acest proiect, jucatorul este cel ce se misca si harta va sta pe loc.

De asemenea ca logica de deplasare, jucatorul va avea cei 3 vectori de "forward, right, up" ce vor fi actualizati cadru de cadru pentru a face jucatorul sa se roteasca si sa se miste corespunzator.
""game.cpp""
 void Game::OnInputUpdate(float deltaTime, int mods)

Miscarea camerei se face total separat de miscarea jucatorului. Singura sincronizare intre aceste 2 mecanisme este in game::Init() unde pozitionez camera corespunzator fata de jucator.
""game_camera.h""

Voi tine un set de "position, forward, right, up" atat pentru first person cat si pentru third person ce vor fi actualizate mereu indiferent de modul curent de vizualizare (third person sau first person). Asadar atunci cand voi schimba intre first person si third person, doar voi returna folosind "getter functions" setul de informatii in functie de modul actual de vizualizare.
""game_camera.h""

Pentru proiectile folosesc o piscina in care adaug initial un numar de instante pentru a fi reciclate folosind booleana in fisierul ""bullet.h"" "available". Astfel un glont "available" este un glont ce nu apare in scena pentru ca nu este tras inca. Acesta este tras in directia forward al camerei.
Gloantele devin valabile dupa un numar de secunde.
""Game::ShootBullet()""
""Player::UpdateBullets()""

Inamicii sunt simple sfere ce vor fi adaugati in labirint folosind o sansa de apartiei pe fiecare celula libera mai putin cea de start.
""void Level::CreateEnemies()""
Acestia se vor misca in linie, folosind o variabila interna direction ce oscileaza intre -1 si 1 si invers de fiecare data cand se realizeaza o coliziune cu un perete al labirintului sau cand sa iasa din labirint.
""Game::DetectTriggerCollisions""

Pentru animarea in shader a inamicilor atunci cand sunt impuscati avem functia RenderSimpleMesh(..) pentru pasarea mai multor variabile catre vertex shader pentru realizarea animatiilor corespunzatoare. 
Pentru realizarea deformarilor am folosit o functie de atenuare (Fractal Brownian Motion) explicata in:
https://thebookofshaders.com/13/

Pentru coliziuni am creat un nou fisier cu namespace Collision in care am implementat functiile de pe:
https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection

Pentru a castiga jocul, folosim coliziune cu primul cub invizibil din afara labirintului de langa celula ce are deschidere in afara labirintului.

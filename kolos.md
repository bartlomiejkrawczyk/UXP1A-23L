UXP - Wymagania na kolokwium #1

* Wstęp i Procesy *
- Standardy: POSIX 1003 i inne
- Procesy:
	- tryb wykonania: user, kernel; 
	- syscall, przerwanie, wyjątek;
	- stany procesu;
	- deskryptor procesu (struct proc, struct u);
	- hierarchia procesów - PID i PPID;
	- podstawowa mapa pamięci - segmenty;
	- UID i EUID - real, effective, saved UID/GID;
	- czas i liczniki czasu;
	- timery
- Procesy
	- API: fork(), wait...(), exec...();
	- środowisko;
	- użytkownicy i grupy; 
- Sygnały, sygnały niezawodne, API 
	- obsługa, funkcja obsługi; 
	- maska;
	- kill(), signal(), sigaction(), sigsuspend, pause(), itp.
- Grupy procesów - wstrzymanie i ponowne uruchomienie, prace pierwszoplanowa i w tle, obsługa terminala, sygnały związane z grupami procesów
- Scheduler
	- klasy;
	- priorytety; 
	- podstawowy algorytm szeregowania;
	- kwant czasu;
	- nice
- Procesy typu demon

* System plików cz. #1 *
- Podstawy
	- drzewo plików; 
	- typy plików; 
	- nazwa i atrybuty; 
	- prawa dostępu i mode;  
	- podstawowe API: chmod(), chown(), close(),  creat(),  dup(),  dup2(), link(), lseek(), open(), read, stat(), umask(),  write(),  unlink(), symlink()
	
- prawa dostępu; 
	- słowo "mode"; 
	- bity dodatkowe;
- metryczka (inode) i struktura stat; 
	- funkcja stat(); 
	- znaczniki czasowe w metryczce
- organizacja logiczna nazwa-metryczka-zawartość; 
	- dowiązania sztywne i symboliczne
- rozmiar pliku: alokacja, a "size"; pozycjonowanie w pliku (lseek())
- funkcje fcntl() i ioctl() - różne zastosowania
- Operacja na katalogach; 
	- enumeracja zawartości katalogów
- Zajmowanie plików i rekordów; 
	- tryby zalecany i obowiązkowy

* Komunikacja i synchronizacja międzyprocesowa 
- Sygnały i pliki jako mechanizmy komunikacji 
- Potoki nienazwane pipe(); 
	- rozmiar bufora potoku; 
	- specyfika komunikacji przez potok
- Potoki nazwane fifo, podobieństwa i różnice do nienazwanych
- SYSV IPC: kolejki komunikatów, pamięć współdzielona i semafory
- SYSV IPC API: identyfikator; 
	- ipc_perm;  funkcje ...get(), ...ctl(), msgsend(), msgrcv(), semop(), działanie i specyfika shmem
- inne mechanizmy (o ile były omówione)
- komunikacja i synchronizacja - które mechanizmy komunikacji zapewniają synchronizację, a które nie?

# Treść Zadania

Napisać bibliotekę implementującą prosty system plików z operacjami zdefiniowanymi niżej.

## Założenia podstawowe:
- Biblioteka statyczna (.o lub .a), nagłówek .h oraz serwis (demon) [można też zrealizować jako bibliotekę .so]
- Pliki przechowywane są w jednym katalogu (struktura płaska, brak podkatalogów)
- Tylko pliki zwykłe (regular)
- Funkcje (działanie funkcji wynika z jej nazwy przez analogię do podobnej funkcji systemowej):
```c
fd_type libfs_create(char *name, long mode); // libfs_open nie tworzy pliku

int libfs_chmode(char *name, long mode);

int libfs_rename(char *oldname, char *newname);

int libfs_unlink(char *name);

fd_type libfs_open(char *name, int flags); // flagi: O_RD, O_WR, O_RDWR

int libfs_read(fd_type fd, char *buf, unsigned int size);

int libfs_write(fd_type fd, char *buf, unsigned int size);

int libfs_seek(fd_type fd, long int offset); // offset + lub - od akt. pozycji

int libfs_close(fd_type fd);
```
- Dodatkowo:
    - Kod powrotu - analogicznie do funkcji systemowych, dodatkowo zmienna `libfs_errno` powinna być ustawiana analogicznie do tego, jak dzieje się to dla zmiennej `errno`.
    - W razie wątpliwości dotyczących zachowania danej funkcji, należy przyjąć w miarę możności zachowanie maksymalnie zbliżone do odpowiadającej funkcji systemowej.
    - O ile w wariancie nie podano inaczej, kontrola praw dostępu odbywa się poprzez standardowe mechanizmy systemu, demon działa z tymi samymi uprawnieniami co użytkownik. Obsługę uprawnień należy przeanalizować i opisać.
    - Pliki mogą (ale nie muszą) być przechowywane w dedykowanym katalogu demona (alternaywnie: w pamięci).

## Wariant implementacyjny
Implementację komunikacji pomiedzy biblioteką a serwisem należy zrealizować poprzez **potoki nazwane (FIFO)**


## Wariant funkcjonalny
Dodatkowo należy zaimplementować:
- Dodać obsługę funkcji typu `stat()` oraz wynikających z niej atrybutów: wielkości, znaczników czasu.
- Dodać obsługę funkcji analogicznej do `link()`/`symlink()`

## Testowanie
Testy wymagają napisania programu lub programów (i ewentualnie skryptów) pokrywajacych funkcjonalność, przypadki brzegowe, obsługę błędów, itp.

Testy mogą zawierać prosty tekstowy interpreter pozwalający na uruchamianie poszczególnych funkcji w trybie konwersacyjnym. Dozwolone jest też napisanie szeregu prostych programów testowych, w takim przypadku zalecane jest stworzenie skryptów parametryzujących różne wywołania takich programów.

### Miejsce realizacji:
Dowolny system Unix/Linux zgodny z POSIX


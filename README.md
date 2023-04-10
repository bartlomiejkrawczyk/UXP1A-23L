# Unix - architektura, programowanie i administrowanie

Studenci:
```
Stanisław Zagórowski (lider),
Bartłomiej Krawczyk,
Mateusz Brzozowski,
Adam Sudoł
```

Data przekazania projektu wstępnego:
**19.04.2023**

# 1. Temat zadania
Napisać bibliotekę implementującą prosty system plików z operacjami zdefiniowanymi niżej.

Wariant: **W11 + W22**

### Założenia podstawowe:
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

### Wariant implementacyjny
Implementację komunikacji pomiedzy biblioteką a serwisem należy zrealizować poprzez **potoki nazwane (FIFO)**


### Wariant funkcjonalny
Dodatkowo należy zaimplementować:
- Dodać obsługę funkcji typu `stat()` oraz wynikających z niej atrybutów: wielkości, znaczników czasu.
- Dodać obsługę funkcji analogicznej do `link()`/`symlink()`

### Testowanie
Testy wymagają napisania programu lub programów (i ewentualnie skryptów) pokrywajacych funkcjonalność, przypadki brzegowe, obsługę błędów, itp.

Testy mogą zawierać prosty tekstowy interpreter pozwalający na uruchamianie poszczególnych funkcji w trybie konwersacyjnym. Dozwolone jest też napisanie szeregu prostych programów testowych, w takim przypadku zalecane jest stworzenie skryptów parametryzujących różne wywołania takich programów.

#### Miejsce realizacji:
Dowolny system Unix/Linux zgodny z POSIX

<!-- TODO: -->

# 2. Interpretecja treści zadania
Zdecydowaliśmy się przechowywać pliki w określonej z góry lokalizacji na dysku. Jest ona unikalna dla użytkownika, tworzona w jego katalogu domowym w ukrytym katalogu `$HOME/.local/state/libfs`.

## Opisy funkcji
<!-- wymaganych oraz dodatkowych -->

**Stwórz Plik**
```c
fd_type libfs_create(char *name, long mode);
```

**Zmień Uprawnienia do Pliku**
```c
int libfs_chmode(char *name, long mode);
```

**Zmień Nazwę Pliku**
```c
int libfs_rename(char *oldname, char *newname);
```
**Usuń nazwę i ewentualnie plik, do którego się odnosi**
```c
int libfs_unlink(char *name);
```

**Otwórz Isniejący Plik**

Wspieramy flagi biblioteczne:
- O_RD
- O_WR
- O_RDWR
<!-- - najpierw sprawdzamy czy plik istnieje, dopiero później wykonujemy open -->
```c
fd_type libfs_open(char *name, int flags);
```

**Odczyt z Deskryptora Pliku**
```c
int libfs_read(fd_type fd, char *buf, unsigned int size);
```
**Zapis do Deskryptora Pliku**
```c
int libfs_write(fd_type fd, char *buf, unsigned int size);
```

**Zmień Położenie Przesunięcia w czasie Odczytu/Zapisu do Pliku**

Położenie zmieniane relatywnie do aktualnej pozycji w źródle.
```c
int libfs_seek(fd_type fd, long int offset);
```

**Zamknięcie Deskryptora Pliku**
```c
int libfs_close(fd_type fd);
```

**Pobranie Statusu Pliku**


`struct libfs_stat_t` jest okrojoną wersją `struct stat` ze standardów. Usunięte są pola nam niepotrzebne, np. device ID lub user ID bądź group ID. Zakładamy że nasz system plików jest płaski, tzn. bez podkatalogów i istnieje w predefiniowanym miejscu na dysku. Jako że daemon pracuje z uprawnieniami użytkownika wykonującego do niego zapytania, pliki użytkownika biblioteki muszą znajdować się w miejscu w pełni dostępnym dla daemona, co ogranicza nas do `$HOME` użytkownika. Dane te byłyby więc stałe między plikami.

Nie planujemy powielać informacji o plikach w pamięci demona - `libfs_stat` będzie mapowała istniejącą informację o pliku z systemu plików na naszą strukturę zdefiniowaną w bibliotece.

```c
struct libfs_stat_t {
   ino_t     st_ino;         /* System Inode number */
   mode_t    st_mode;        /* File type and mode */
   nlink_t   st_nlink;       /* Number of hard links */
   off_t     st_size;        /* Total size, in bytes */
   blksize_t st_blksize;     /* Block size for filesystem I/O */
   blkcnt_t  st_blocks;      /* Number of 512B blocks allocated */

   struct timespec st_atim;  /* Time of last access */
   struct timespec st_mtim;  /* Time of last modification */
   struct timespec st_ctim;  /* Time of last status change */
};

int libfs_stat(const char *restrict pathname,
               struct libfs_stat_t *restrict statbuf);
```

**Dodanie Nowej Nazwy do Isniejącego Pliku**
```c
int libfs_link(const char* source, const char* destination);
```

**Dodanie Aliasu do Ścieżki Pliku**
```c
int libfs_symlink(const char* source, const char* destination);
```

# 3. Opis Funkcjonalny "Black-Box"
<!-- Krótki opis funkcjonalny – “black-box”, najlepiej w punktach. -->
- Udostępniamy prosty system plików w jednym katalogu bez podkatalogów.
- Obsługuje tylko pliki zwykłe, dowiązania twarde i symboliczne.
- Dostępne operacje to: tworzenie, zmiana trybu, zmiana nazwy, usuwanie, otwieranie, odczyt i zapis danych, przesunięcie wskaznika pozycji, zamknięcie pliku, pobranie statusu, dodanie twardych oraz symbolicznych dowiązań.
- Funkcje zwracają kod powrotu i ustawiają zmienną libfs_errno, która informuje o błędach.
- Obsługa uprawnień jest zgodna z mechanizmami systemowymi, systemy plików nie są dzielone między użytkownikami.

# 4. Opis rozwiązania

<!-- Opis i analizę poprawności stosowanych: struktur danych, metod komunikacja, metod synchronizacji, moduły wraz z przepływem sterowania i danych między nimi. -->

<!-- Podział na moduły i strukturę komunikacji między nimi (silnie wskazany rysunek). -->

```mermaid
flowchart LR
    subgraph client-daemon message types
        libfs-core[libfs-core.a]
    end

    subgraph User Executable Programs
        libfs-create
        libfs-chmode
        libfs-rename
        libfs-unlink
        libfs-open
        libfs-read
        libfs-write
        libfs-seek
        libfs-close
    end
    
    libfs-core ==> ld[libfs-daemon]
    
    libfs-core ==> libfs[libfs.a]
    
    libfs ==> libfs-create
    libfs ==> libfs-chmode
    libfs ==> libfs-rename
    libfs ==> libfs-unlink
    libfs ==> libfs-open
    libfs ==> libfs-read
    libfs ==> libfs-write
    libfs ==> libfs-seek
    libfs ==> libfs-close
    
    libfs-create <-.-> ld
    libfs-chmode <-.-> ld
    libfs-rename <-.-> ld
    libfs-unlink <-.-> ld
    libfs-open <-.-> ld
    libfs-read <-.-> ld
    libfs-write <-.-> ld
    libfs-seek <-.-> ld
    libfs-close <-.-> ld
```

Przykładowe użycie kilku z tych programów (sprowadzających się z grubsza do wywołania odpowiedniej funkcji biblioteki `libfs.a`) z poziomu bash'a:

```bash
libfs-create hello.txt
fdw=$(libfs-open -w hello.txt)
libfs-write $fdw "Hello"
libfs-close $fdw

fdr=$(libfs-open -r hello.txt)
content=$(libfs-read $fdr)
libfs-close $fdr

echo $content
```

<!-- Opis najważniejszych rozwiązań funkcjonalnych wraz z uzasadnieniem (opis protokołów, struktur danych, kluczowych funkcji, itp.) -->

<!-- Szczegółowy opis interfejsu użytkownika. -->

# 5. Współbieżność

<!-- Koncepcja realizacji współbieżności. -->

```mermaid
flowchart TB
    D[Libfs Daemon]
    FM[daemon request FIFO]
    S[daemon fifo access semaphore]
    
    FM --> D
    C1 --> S
    C2 --> S


    C1[Client 1]

    
    C2[Client 2]
    
    C1 --> FM
    C2 --> FM
    
    D -- client 1 fifo --> C1
    D -- client 2 fifo --> C2
    
```


# 6. Implementacja
<!-- Zarys koncepcji implementacji (język, biblioteki, narzędzia, etc.). -->

**Język**: C++17

**Biblioteki**:
- `unistd.h`

**Narzędzia**:
- **Kompilator**: GCC, kompilowanie z `-Wall -Werror -Wextra -Wconversion -Wpedantic` 
- **Formatowanie**: `clang-format`
- **Build System**: TODO
- **Edytor**: VS Code, neovim

Biblioteka będzie budowana jako archiwum `.a`.

<!-- Opis wykorzystanych narzędzi, itp. -->

# 7. Testowanie

<!-- Zarys koncepcji testów -->
Do testowania służyć będą małe programy wołające funkcje biblioteczne. Aby kompleksowo przetestować poszczególne przypadki, każdy przypadek użycia będzie reprezentowany skryptem bash, który będzie uruchamiać małe programy w zadanej kolejności.

Skrypty te będą wywoływane przez główny "runner", `test.sh`, który będzie porównywać ich output i exit code z oczekiwanymi.


<!--  Szczegółowy opis testów i wyników testowania -->



<!-- Należy pamiętać, że nie mamy opisywać kwestii znanych i omawianych na wykładzie, np. zasady funkcjonowania API i funkcji systemowych, standardowych narzędzi programistycznych, itp. -->
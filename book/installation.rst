.. highlight:: php
   :linenothreshold: 2

.. index::
   pair: Symfony2 Standard Edition; instalacja

Instalacja oraz konfiguracja Symfony
====================================

Rozdział ten traktuje o sposobach pobierania i uruchomiania aplikacji
zbudowanych na Symfony. W celu uproszczenia procesu tworzenia nowej aplikacji,
Symfony dostarcza instalatora aplikacji. 

Instalowanie instalatora Symfony
--------------------------------

Stosowanie **instalatora Symfony** jest jedynym, zalecanym sposobem tworzenia nowych
aplikacji Symfony. Instalator ten jest aplikacją PHP, która musi być zainstalowana
w systemie tylko raz a następnie można utworzyć z jej uzyciem dowolna ilość aplikacji
Symfony.

.. note::

    Instalator wymaga PHP 5.4 lub wersji wyższej. Jeśli nadal używa się wersji
    PHP 5.3, to nie można korzystać z instalatora Symfony. Proszę przecztać rozdział
    :ref:`book-creating-applications-without-the-installer`, aby dowiedzieć się,
    jak rozwiązać ten problem.

Instaler instalowany jest różnie, w zależności od systemu operacyjnego.

Systemy Linux i Mac OS X
~~~~~~~~~~~~~~~~~~~~~~~~

Otwórz konsole poleceń i wykonaj następujące polecenia:

.. code-block:: bash
<<<<<<< HEAD

    $ sudo curl -LsS http://symfony.com/installer -o /usr/local/bin/symfony
    $ sudo chmod a+x /usr/local/bin/symfony

Utworzy to w systemie globalne polecenie ``symfony``.

Systemy Windows
~~~~~~~~~~~~~~~

Otwórz konsole poleceń i wykonaj następujące polecenie:

.. code-block:: bash

    c:\> php -r "readfile('http://symfony.com/installer');" > symfony

Następnie przenieś pobrany plik ``symfony`` do katalogu swojego projektu i wykonaj
te polecenia:

.. code-block:: bash

    c:\> move symfony c:\projects
    c:\projects\> php symfony

Tworzenie aplikacji Symfony
---------------------------

Gdy dostępny jest instalator Symfony, to utworzenie npwego projektu sprowadza się
do wykonania polecenia ``synfony new``:

.. code-block:: bash

    # Linux, Mac OS X
    $ symfony new my_project_name

    # Windows
    c:\> cd projects/
    c:\projects\> php symfony new my_project_name

Polecenie to tworzy nowy katalog w miejscu skonfigurowania konsoli o nazwie
``my_project_name``, który zawiera nowy świeży projekt oparty na najbardziej
aktualnej dostępnej wersji Symfony. Dodatkowo intalator sprawdza czy system
spełnia techniczne wymagania konieczne do wykonywania aplikacji Symfony.
Jeśli nie, to zobaczysz listę zmian koniecznych do wykonania, aby spełnić te
wymagania.

.. tip::

    Ze wzgledów bezpieczeństwa, wszystkie wersje Symfony sa podpisane cyfrowo
    zanim trafia do dystrybucji. Jeśli chcesz zweryfikować integralność jakiejś
    wersji Symfony, wykonaj kroki `opisane w tym wpisie`_.

Oparcie projektu o określoną wersję Symfony
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

W przypadku konieczności zainstalowania projektu opartego na określonej wersji
Symfony, trzeba użyć opcjonalny, drugi argument polecenia ``symfony new``:

.. code-block:: bash

    # use the most recent version in any Symfony branch
    $ symfony new my_project_name 2.3
    $ symfony new my_project_name 2.5
    $ symfony new my_project_name 2.6

    # use a specific Symfony version
    $ symfony new my_project_name 2.3.26
    $ symfony new my_project_name 2.6.5

    # use the most recent LTS (Long Term Support) version
    $ symfony new my_project_name lts

Jeśli chce się, aby projekt oparty był o najnowszą :ref:`wersję LTS Symfony <releases-lts>`,
trzeba przekazać ``lts`` jako drugi argument polecenia ``symfony new``:

.. code-block:: bash

    # Linux, Mac OS X
    $ symfony new my_project_name lts

    # Windows
    c:\projects\> php symfony new my_project_name lts

Prosze przeczytać :doc:`Symfony Release process </contributing/community/releases>`
w celu zr9ozumienia, dlaczego istnieje kilka wersji Symfony i która z nich jest
dla Ciebie najodpowiedniejsza.

.. _book-creating-applications-without-the-installer:

Tworzenie aplikacji Symfony bez instalatora
-------------------------------------------

Jeśli nadal uzywasz PHP 5.3 lub nie możesz usruchamiać instalatora z innych powodów,
mozesz tworzyć aplikacje Symfony wykorzystując alternatywna meto de instalacji,
opartą o `Composer`_.

Composer jest menadżerem zależności używanym przez nowoczesne aplikacje PHP
i może też być użyty do tworzenia nowych aplikacji opartych na frameworku Symfony.
Jeśli jeszcze nie masz zainstalowanego globalnie tego narzędzia, rozpocznij od czytania
następnego rozdziału.

Globalne instalowanie Composer
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Rozpocznij od :doc:`globalne instalowanie Composer </cookbook/composer>`.

Tworzenie aplikacji Symfony przy użyciu Composer
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Po zainstalowaniu Composer, wykonaj polececenie ``create-project``, aby utworzyć
aplikację Symfony opartą na najnowszej stabilnej wersji:

.. code-block:: bash

    $ composer create-project symfony/framework-standard-edition my_project_name

Jeśli potrzeba oprzeć aplikacje na określonej wersji Symfony, należy dostarczyć
drugi argument w poleceniu ``create-project``:

.. code-block:: bash

    $ composer create-project symfony/framework-standard-edition my_project_name "2.3.*"

.. tip::

    Jeśli połączenie z Internetem jest powolne, można mieć wrażenie, że Composer
    nie robi nic. Jeśli tak jest, dodaj flagę ``-vvv`` do poprzedniego polecenia,
    co spowoduje wyświetlenie na wyjściu wszystkiego, co robi Composer.

Uruchamianie aplikacji Symfony
------------------------------

Symfony wykorzystuje wewnetrzny serwer internetowy PHP do uruchamiania aplikacji
podczas prac programistycznych. Dlatego uruchamianie aplikacji Symfony sprowadza
się do skonfigurowania się w katalogu projektu i wykonaniu polecenia uruchamiającego
serwer internetowy:

.. code-block:: bash

    $ cd my_project_name/
    $ php app/console server:run

Następnie trzeba otworzyć przeglądarkę i odwiedzić adres
``http://localhost:8000/app/example``,
co powinno skutkować wyświetleniem strony powitalnej Symfony:

.. image:: /images/quick_tour/welcome.png
   :align: center
   :alt:   Symfony Welcome Page

Zamiast strony powitalnej, można zobaczyć pustą stronę albo stronę błędu.
Jest to spowodowane brakiem uprawnień do niektórych katalogów aplikacji. Jest
kilka możliwych rozwiązań, w zależności od systemu operacyjnego. Wszystkie są
omówione w rozdziale :ref:`Ustawienie uprawnień <book-installation-permissions>`.

=======
   
   $ php composer.phar create-project symfony/framework-standard-edition /path/to/webroot/Symfony `2.3.*`
   
>>>>>>> refs/heads/nowe_rozdz
.. note::

    Wewnetrzny serwer internetowy PHP jest dostępny od PHP 5.4. Jeśli używasz
    nadal starszą wersję PHP 5.3, musisz skonfigurować w serwerze internetowym
    *host wirtualny*.

Polecenie ``server:run`` przeznaczone jest tylko do uruchamiania aplikacji
w środowisku programistycznym. W celu uruchamiania aplikacji Symfony w środowisku
produkcyjnym trzeba skonfigurować serwer internetowy `Apache`_ lub `Nginx`_, tak
jak wyjasniono to w artykule :doc:`/cookbook/configuration/web_server_configuration`.

Można zatrzymać serwer internetowy, po zakończeniu pracy z aplikacją Symfony, stosując
polecenie ``server:stop``:

.. code-block:: bash

    $ php app/console server:stop

Sprawdzanie konfiguracji i ustawień aplikacji Symfony
-----------------------------------------------------

Aplikacje Symfony są dostarczane z wizualnym testerem konfiguracji, który pokazuje,
czy obecne środowisko jest gotowe do używania Symfony. W celu sprawdzenia swojej
konfiguracji odwiedź następujacy adres URL:

.. code-block:: text

<<<<<<< HEAD
    http://localhost:8000/config.php
=======
    path/to/webroot/ <- your web root directory
        Symfony/ <- the new directory
            app/
                cache/
                config/
                logs/
            src/
                ...
            vendor/
                ...
            web/
                app.php
                ...

Opcja 2 - Pobranie archiwum
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Można również pobrać archiwum dystrybucji Synfony2 Standard Edition. W tym celu
trzeba pobrać archiwum .tgz albo .zip. Oba są równoważne, więc decyzja zależy tylko
od Twoich preferencj

Trzeba zdecydować się na pobranie archiwum z lub bez dostawców (*ang. vendors*).
Jeżeli planujesz używanie bibliotek lub pakietów (*ang. bundles*) niezależnych
dostawców i zarządzać nimi za pośrednictwem Composera, to przypuszczalnie lepszym
wyborem będzie pobranie dystrybucji *without vendors*.

Pobierz jedno z archiwów i rozpakuj go gdzieś w katalogu głównym serwera internetowego.
W systemie uniksowym można użyć w terminalu jedno z poniższych poleceń (zamieniając
``###`` na rzeczywistą nazwę pliku):

.. code-block:: bash

   # dla pliku .tgz
   $ tar -zxvf Symfony_Standard_Vendors_2.3.###.tgz
   
   # dla pliku .zip
   $ unzip Symfony_Standard_Vendors_2.3.###.zip

Jeśli pobrałeś archiwum *without vendors*, to koniecznie przeczytaj następny rozdział.

.. note::
   
   Można łatwo zastąpić domyślną strukturę katalogów. Przeczytaj artykuł
   :doc:`/cookbook/configuration/override_dir_structure` w celu uzyskania więcej
   informacji.

Aktualizacja bibliotek dostawców
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

W tym momencie powinieneś mieć pobrany i zainstalowany w pełni funkcjonalny projekt
Symfony, z którym możesz rozpocząć tworzenie własnej aplikacji. Projekt Symfony
zależy od wielu zewnętrznych bibliotek. Są one pobierane do katalogu *vendor/*
projektu. poprzez bibliotekę `Composer`_, o której była mowa w poprzednim rozdziale.

W zależności o sposobu pobrania Symfony, może być konieczne pobranie bibliotek
dostawców lub nie (bo znajdowały się w pliku archiwum instalacyjnego). Aktualizacja
bibliotek dostawców jest zawsze bezpieczna i gwarantuje, że ma się wszystkie potrzebne
biblioteki.

Instalacja Composer została dokładnie omówiona w rodziale :ref:`poprzednim<composer-installation>`.

Zainstalowanie lub zaktualizowanie bibliotek dostawców można osiągnąć poleceniem (pełna składnia):

.. code-block:: bash
   
   $cd /ścieżka/do/katalogu/symfony
   $ [sudo -u www-data] php composer.phar install

Powyższe polecenie instalujące (lub polecenie skrócone ``$ composer install``)
musi być uruchomione w katalogu, w którym znajduje się plik *composer.json* - domyślnie
jest to katalog główny projektu Symfony. Spowoduje ono pobranie lub zaktualizowanie
wszystkich bibliotek dostawców w katalogu *vendor/*. Instalacja lub aktualizacja
może się nie powieść, ze względu na brak uprawnień użytkownika dokonujacego instalacji
(aktualizacji) do zapisu katalogów *app/cache* i *app/logs*. Dlatego wcześniej należy
odpowiednio skonfigurować aplikację. Jest to omówione nieco dalej, w przypisie
"Konfiguracja uprawnień". Gdy użytkownkiem serwera jest ``www-data``  a użytkownik
linii poleceń należy do grupy mającej uprawnienia zapisu do w/w katalogów, to w podanym
poleceniu trzeba użyć opcji ``sudo -u www-data`` (w Ubuntu i podobnych systemach),
lub analogicznego.

Jeśli ma się zainstalowane biblioteki dostawców, to można wykonać tylko polecenie
aktualizujące:

.. code-block:: bash
   
   $ [sudo -u www-data] php composer.phar update

.. tip::
   
   Po zrealizowaniu polecenia ``php composer.phar install`` lub ``php composer.phar update``,
   Composer automatycznie wykonuje czyszczenie pamięci podręcznej i instalację zasobów.
   Zasoby są domyślnie kopiowane do katalogu „web”. Zamiast później przekopiowywać
   te zasoby, lepiej jest spowodować automatyczne utworzenie dowiązania symbolicznego
   poprzez wykonanie odpowiedniego wpisu w pliku composer.json z kluczem ``symfony-assets-install``
   a wartością ``symlink``:
   
   .. code-block:: json
      :linenos:
      
      "extra": {
         "symfony-app-dir": "app",
         "symfony-web-dir": "web",
         "symfony-assets-install": "symlink"
      }
   
   Jeżeli zamiast wpisu symlink zastosuje się wpis ``relative`` w wartości klucza
   ``symfony-assets-install``, to polecenie będzie generowało względne dowiązanie
   symboliczne.


Konfiguracja i ustawienia
~~~~~~~~~~~~~~~~~~~~~~~~~

W tym momencie wszystkie zewnętrzne biblioteki umiejscowione są w katalogu ``vendor/``.
Masz także wstępnie skonfigurowany projekt w katalogu ``app/`` wg ustawień domyślnych
oraz przykładowy kod w katalogu ``src/``.

Symfony2 dostarczane jest z wizualnym testerem konfiguracji serwera, aby pomóc w
sprawdzeniu prawidłowości konfiguracji serwera internetowego i PHP pod kątem działania
Symfony. Zakładając, że Symfony zostało zainstalowane w katalogu
/ścieżka/do/katalogu/wwwroot/symfony, użyj w przeglądarce następującego adresu URL,
aby sprawdzić swoją konfigurację:

.. code-block:: text

    http://localhost/Symfony/web/config.php
>>>>>>> refs/heads/nowe_rozdz

Jeśli są jakieś problemy, rozwiąż je teraz, zanim przejdziesz dalej.

.. _book-installation-permissions:

.. sidebar:: Ustawienie uprawnień

    Jednym powszechnym problemem prz instalowaniu Symfony jest to, że katalogi
    ``app/cache`` i ``app/logs`` muszą być zapisywalne zarówno przez serwer
    internetowy jak i przez użytkownika linii poleceń. W systemach uniksowych,
    gdy użytkownik serwera internetowego jest inny niż użytkownik linii poleceń,
    spróbuj jedno z poniższych rozwiązań.

    **1. Użycie tego samego użytkownika dla CLI i serwera internetowego**

    Jest powszechną praktyka, aby w uniksowych środowiskach programistycznych
    używać tego samego użytkownika CLI i serwera internetowego, ponieważ unika się
    jakichkolwiek problemów z prawami dostępu podczas ustawiania nowych projektów.
    Można to wykonać edytując konfiguracje serwera internetowego (zwykle httpd.conf
    lub apache2.conf dla Apache) i ustawiając jego użytkownika tak, aby był taki
    sam, jak użytkownik CLI (np. dla Apache, aktualizując wartości ``User`` i 
    ``Group``).

    **2. Użycie ACL w systemach obsługujacych chmod +a**

    Wiele systemów pozwala używać polecenia ``chmod +a``. Najpierw spróbuj zastosować
    to polecenie i gdy zwrócony zostanie błąd, spróbuj metody następnej.
    Tutaj najpierw spróbujemy ustalić użytkownika serwera internetowego i ustawić
    go jako ``HTTPDUSER``:

    .. code-block:: bash

        $ rm -rf app/cache/*
        $ rm -rf app/logs/*

        $ HTTPDUSER=`ps aux | grep -E '[a]pache|[h]ttpd|[_]www|[w]ww-data|[n]ginx' | grep -v root | head -1 | cut -d\  -f1`
        $ sudo chmod +a "$HTTPDUSER allow delete,write,append,file_inherit,directory_inherit" app/cache app/logs
        $ sudo chmod +a "`whoami` allow delete,write,append,file_inherit,directory_inherit" app/cache app/logs


    **3. Użycie ACL w systemach, które nie obsługują chmod +a**

    Niektóre systemy nie obsługują polecenia ``chmod +a``, ale obsługują inne
    narzędzie o nazwie ``setfacl``. Możesz spróbować `włączyć obsługę ACL`_ na partycji
    i zainstalować ``setfacl`` (w Ubuntu jest on zainstalowany domyślnie).
    Tutaj najpierw próbujemy ustalić użytkownika serwera internetowego i ustawić
    go jako ``HTTPDUSER``:
    
    .. code-block:: bash

        $ HTTPDUSER=`ps aux | grep -E '[a]pache|[h]ttpd|[_]www|[w]ww-data|[n]ginx' | grep -v root | head -1 | cut -d\  -f1`
        $ sudo setfacl -R -m u:"$HTTPDUSER":rwX -m u:`whoami`:rwX app/cache app/logs
        $ sudo setfacl -dR -m u:"$HTTPDUSER":rwX -m u:`whoami`:rwX app/cache app/logs

    Jeśli to nie zadziała, spróbuj dodać opcję ``-n``.

    **4. Bez stosowania ACL**

    Jeśli nie ma się dostępu do zmian ACL katalogów, to pozostaje zmiana ``umask``,
    tak aby katalogi *cache* i *log* były zapisywalne dla grupy lub każdego
    (w zależności od tego czy użytkownik serwera internetowego i użytkownik linii
    poleceń należą do tej samej grupy). Aby to osiągnąć należy wstawić następującą
    linię na samym początku plików ``app/console``, ``web/app.php``
    i ``web/app_dev.php``::

        umask(0002); // This will let the permissions be 0775

        // or

        umask(0000); // This will let the permissions be 0777

    Proszę mieć na uwadze, że zalecaną metodą jest zastosowanie ACL, gdy ma się
    dostęp do ACL na serwerze, ponieważ zmiana ``umask`` nie jest całkiem bezpieczna.

.. _installation-updating-vendors:

Aktualizowanie aplikacji Symfony
--------------------------------

W tym momencie mamy już utworzona w pełni funkcjonalną aplikację Symfony, z którą
możesz rozpocząć tworzenie swojego projektu. Aplikacja Symfony uzależniona jest
od zewnętrznych bibliotek. Ładowane są one do katalogu ``vendor/`` i zarządzane
za pomocą Composer.

Częste aktualizowanie tych zewnetrznych bibliotek jest dobrą praktyką, gdyż
zabezpiecza aplikacje przed błedami i lukami bezpieczeństwa. W tym celu trzeba
wykonać poniższe polecenie:

.. code-block:: bash

    $ cd my_project_name/
    $ composer update

W zależności od złożoności projektu, ten proces aktualozacji moze potrwać kilka
minut.

.. tip::

    Symfony dostarcza polecenie, pozwalające sprawdzić, czy zależności projektu
    zawierają jakieś znane luki bezpieczeństwa:

    .. code-block:: bash

        $ php app/console security:check

    Dobrą praktyką jest wykonywanie tego polecenia regularnie, tak aby można było
    jak najszybciej zaktualizować zależności lub usunąć wykryte luki.

Instalowanie demonstracyjnej aplikacji Symfony
----------------------------------------------

Aplikacja Symfony Demo jest w pełni funkcjonalną aplikacją, która pokazuje
zalecany sposób tworzenia aplikacji Symfony. Apolikacja ta została pomyślana jako
narzędzie nauki dla początkujących w Symfony a jej kod źródłowy zawiera tonę
komentarzy i pomocne uwagi.

W celu pobrania aplikacji Symfony trzeba wykonać polecenie ``symfony demo``
gdziekolwiek w swoim systemie:

.. code-block:: bash

    # Linux, Mac OS X
    $ symfony demo

    # Windows
    c:\projects\> php symfony demo

Po pobraniu, przejdź do katalogu ``symfony_demo/`` i uruchom wbudowany serwer
PHP, wykonując polecenie ``php app/console server:run``. Następnie w przeglądarce
odwiedź adres ``http://localhost:8000``, co uruchomi aplikację Symfony Demo.

.. _installing-a-symfony2-distribution:

Instalowanie dystrybucji Symfony
--------------------------------

Pakiety "dystrybucyjne" projektu Symfony, będące w pełni funkcjonalmymi aplikacjami,
które zawierają biblioteki rdzenia Symfony, wybór przydatnych pakietów i sensowną
strukturę katalogową oraz pewną domyślną konfigurację. Gdy tworzyliśmy aplikację
Symfony w poprzednim rozdziale, w rzeczywistości pobraliśmy domyślną dystrybucję
dostarczaną przez Symfony, która nosi nazwę *Symfony Standard Edition*.

*Symfony Standard Edition* jest bez wątpienia najpopularniejszą dystrybucją
i jest zdecydowanie zalecana dla programistów rozpoczynających pracę z Symfony.
Jednak społeczność Symfony opublikowała też inne dystrybucje, które można wykorzystać
dla swoich aplikacji:

* `Symfony CMF Standard Edition`_ jest najlepszą dystrybucją do rozpoczęcia projektu
  z `Symfony CMF`_, który ułatwia programistom dodawanie funkcjonalności CMS do
  aplikacji budowanej na bazie Symfony Framework.
* `Symfony REST Edition`_ pokazuje jak zbudować aplikację dostarczającą API
  RESTful przy użyciu FOSRestBundle i kilku innych powiązanych pakietów.

Korzystanie z kontroli wersji
-----------------------------

Jeśli używa się systemu kontroli wersji, takiego jak `Git`_, można bezpiecznie
wykonywać rewizje kodu swojego projektu, a to dlatego, że aplikacje Symfony już
zawierają specjalnie przygotowany plik ``.gitignore``.

Specjalna instrukcja korzystania z repozytorium Git dla aplikacji
Symfony znajduje sie w artykule :doc:`/cookbook/workflow/new_project_git`.

Pobieranie wersjonowanej aplikacji Symfony
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Jeśli wykorzystuje się Composer do zarządzania zależnościami aplikacji, zaleca się
ignorowanie całego katalogu ``vendor/`` przed zatwierdzeniem zmian kodu w repozytorium.
Oznacza to, że podczas pobierania aplikacji Symfony z repozytorium Git, katalog ``vendor/``
będzie pomijany i aplikacja nie bedzie działać od razu po pobraniu z repozytorium.

W celu doprowadzenia kodu do właściwego stanu, trzeba po pobrabiu aplikacji
Symfony wykonać polecenie ``composer install``, co spowoduje pobranie i zainstalowanie
wszystkich wymaganych aplikacji:

.. code-block:: bash

    $ cd my_project_name/
    $ composer install

Skąd Composer wie jakie ma zainstalować zależności? Ponieważ w czasie zatwierdzania
rewizji (migawki) kodu aplikacji Symfony, są też zatwierdzane pliki ``composer.json``
i ``composer.lock``. Pliki te informują Composer o wymaganych zależnościach
(i ich określonych wersjach), jakie muszą być zainstalowane w aplikacji.

Rozpoczęcie prac programistycznych
----------------------------------

Teraz, gdy masz już zainstalowana w pełni funkcjonalną aplikację, możesz rozpocząć
prace programistyczne. Dustrybucja, jaką używasz, może zawierać troche przykładowego
kodu - sprawdź plik ``README.md`` załączony do dystrybucji (otwórz go jako plik
tekstowy), aby dowiedziec sie, co zawiera przykładowy kod w tej dustrybucji.

Jeśli dopiero poznajesz Symfony, przeczytaj artykuł ":doc:`page_creation`", gdzie
dowiesz się, jak tworzyć strony, zmienic konfigurację i wszystko co potrzeba dla
nowe aplikacji.

Należy też zapoznac się z :doc:`Cookbook </cookbook/index>`, która to część zawiera
szeroki wybór artykułów na temat rozwiązywania konkretnych problemów z Symfony.

.. note::

    Jeśli chcesz usunąć przykładowy kod z dystrybucji, zapoznaj się z artykułem
    ":doc:`/cookbook/bundles/remove`"

.. _`explained in this post`: http://fabien.potencier.org/article/73/signing-project-releases
.. _`Composer`: https://getcomposer.org/
.. _`Composer download page`: https://getcomposer.org/download/
.. _`Apache`: http://httpd.apache.org/docs/current/mod/core.html#documentroot
.. _`Nginx`: http://wiki.nginx.org/Symfony
.. _`włączyć obsługę ACL`: https://help.ubuntu.com/community/FilePermissionsACLs
.. _`Symfony CMF Standard Edition`: https://github.com/symfony-cmf/symfony-cmf-standard
.. _`Symfony CMF`: http://cmf.symfony.com/
.. _`Symfony REST Edition`: https://github.com/gimler/symfony-rest-edition
.. _`FOSRestBundle`: https://github.com/FriendsOfSymfony/FOSRestBundle
.. _`Git`: http://git-scm.com/


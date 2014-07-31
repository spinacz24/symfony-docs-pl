.. index::
    single: instalacja; pierwsze kroki
    single: Symfony CMF Standard Edition

Instalowanie Symfony CMF Standard Edition
=========================================

Symfony CMF Standard Edition (SE) jest dystrybucją opartą na wszystkich głównych
komponentach potrzebnych we wszystkich najczęstszych przypadkach zastosowania
Symfony CMF.

Tematem tego kursu jest instalacja pakietów CMF z minimalnie konieczną konfiguracją
i zilustrowanie działania aplikacji Symfony 2 kilkoma przykładami.

Następnie uzyska się krótkie wprowadzenie na temat zainstalowanych pakietów.
Może to zostać wykorzystane w celu zapoznania się z CMF lub jako punkt wyjścia
do tworzenia własnych aplikacji.

.. note::

    Można również zainstalować piaskownicę CMF, gdzie jest prezentowana bardziej
    kompletna demonstracja instancji Symfony CMF. Można to zobaczyć na stronie
    `cmf.liip.ch`_. Piaskownicę można również zainstalować lokalnie, dzieki czemu
    można się będzie pobawić kodem. Instrukcje instalacji piaskownicy można znaleźć
    na w artykule ":doc:`../cookbook/editions/cmf_sandbox`".

Warunki wstępne
---------------

Jako że Symfony CMF oparty jest na Symfony2, trzeba upewnić się, że spełnione są 
:doc:`wymagania dla uruchamiania Symfony2 </reference/requirements>`. Dodatkowo
potrzeba mieć zainstalowany rozszerzenie PDO `SQLite`_ (``pdo_sqlite``), gdyż jest
ono wykorzystywane jako domyślne medium magazynowania.

.. note::

   Symfony CMF wykorzystuje domyślnie Jackalope + Doctrine DBAL i SQLite jako
   zaplecze DB. Jednak Symfony CMF jest obojętny pod względem magazynowania danych,
   co oznacza, że można użyć jednego z kilku dopuszczalnych mechanizmów magazynowania
   danych, bez konieczności przepisywania kodu. Więcej informacji o różnych dopuszczalnych
   mechanizmach magazynowania danych, ich instalacji i konfiguracji znajduje się
   w dokumencie :doc:`../bundles/phpcr_odm/introduction`.

Instalacja
----------

Standard Edition można zainstalować na dwa sposoby:

1) Composer
~~~~~~~~~~~

Najprostszym sposobem zainstalowania Symfony CMF jest użycie programu `Composer`_.
Pobierz go używając poleceń:

.. code-block:: bash

    $ curl -s https://getcomposer.org/installer | php
    $ sudo mv composer.phar /usr/local/bin/composer

i następnie pobierz kod Symfony CMF wykorzystując Composer (może to chwilę potrwać):

.. code-block:: bash

    $ php composer.phar create-project symfony-cmf/standard-edition <path-to-install> ~1.1
    $ cd <path-to-install>

.. note::

    Ścieżka ``<path-to-install>`` powinna albo znajdować się wewnątrz katalogu
    głównego serwera internetowego albo trzeba skonfigurować wirtualny host dla
    ścieżki ``<path-to-install>``.

Sklonuje to Standard Edition i zainstaluje wszystkie zależności oraz uruchomi kilka
początkowych poleceń. Polecenia te wymagają prawa do zapisu katalogów ``app/cache``
i ``app/logs``. W przypadku, gdy polecenia te zakończą się błędami dostępu, proszę
postępować zgodnie z `zaleceniami w podręczniku Symfony`_
w celu skonfigurowania uprawnień i uruchomić polecenie``install``:

.. code-block:: bash

    $ php composer.phar install

2) GIT
~~~~~~

Można też zainstalować Standard Edition używając GIT. Wystarczy sklonować
repozytorium z github:

.. code-block:: bash

    $ git clone git://github.com/symfony-cmf/standard-edition.git <path-to-install>
    $ cd <path-to-install>

Trzeba jeszcze pobrać poprzez Composer zależności. W celu prawidłowego pobrania
zależności, użyj polecenia ``install``:

.. code-block:: bash

    $ php composer.phar install


Konfiguracja bazy danych
------------------------

Następnym krokiem jest skonfigurowanie bazy danych. Jeśli chcesz użyć SQLite jako
zaplecze bazy danych, wystarczy uruchomić te polecenia:

.. code-block:: bash

    $ php app/console doctrine:database:create
    $ php app/console doctrine:phpcr:init:dbal
    $ php app/console doctrine:phpcr:repository:init
    $ php app/console doctrine:phpcr:fixtures:load

Pierwsze polecenie tworzy wewnątrz folderu app plik o nazwie ``app.sqlite``
zawierający treść bazy danych. Dwa następne polecenia ustawiają PHPCR a ostatnie
polecenie ładuje trochę konfiguratorów tresci (*ang. fixtures*), dzięki czemu
Standard Edition może uzyskiwać dostęp do serwera internetowego.

Projekt powinien być dostępny na serwerze internetowym. Jeśli masz zainstalowany
PHP 5.4, to możesz alternatywnie użyć wewnętrznego serwera internetowego PHP:

.. code-block:: bash

    $ php app/console server:run

i uzyskać do niego dostęp poprzez polecenie:

.. code-block:: text

    http://localhost:8000


.. sidebar:: Używanie zaplecza innych baz danych

    Jeśli wolisz używać innego zaplecza bazy danych, na przykład MySQL, uruchom
    konfigurator (wskazując przeglądarce adres ``http://localhost:8000/config.php``)
    lub ustaw połączenie z bazą danych w pliku ``app/config/parameters.yml``.
    Upewnij się, że masz ustawiony parametr ``database_path`` na ``null`` przy
    wyborze innego sterownika niż SQLite. Pozostawienie pola pliku konfiguracyjnego
    pustym jest interpretowane jako ``null``.

.. note::

    Prawidłowym terminem na domyślnej bazę danych CMF jest *repozytorium
    treści*. Ideą tej nazwy jest zasadniczo opisanie specjalistycznej bazy danych
    utworzonej specjalnie dla systemów zarządzania treścią. Akronim *PHPCR*
    rzeczywiście oznacza *PHP content repository (repozytorium treści w PHP)*.
    Lecz jak wspomniano wcześniej, CMF jest obojętny pod względem magazynowania
    danych, więc możliwe jest zastosowanie w CMF innego mechanizmu magazynowania,
    takiego jak Doctrine ORM, Propel itd.

Przegląd
--------

Ten rozdział pomoże zrozumieć podstawowe części Symfony CMF Standard
Edition (SE) i to, jak one współdziałają w celu dostarczenia stron, jakie można
zobaczyć podczas przeglądania pierwotnej instalacji Symfony CMF SE.

Zakładamy, że masz już zainstalowany Symfony CMF SE i jesteś po lekturze
:doc:`Podręcznika Symfony2 </book/index>`.

AcmeMainBundle i SimpleCmsBundle
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Symfony CMF SE dostarczany jest z pakietem AcmeDemoBundle w celu pomocy w rozpoczęcia
pracy, na wzór pakietu AcmeDemoBundle dostarczanego przez Symfony2 SE. Pakiet ten
udostępnia kilka stron demonstracyjnych, widocznych w przeglądarce.

.. sidebar:: Gdzie są kontrolery?

    AcmeDemoBundle nie zawiera kontrolerów ani plików konfiguracyjnych, jak można
    by się tego spodziewać. Zwiera on nieznacznie więcej niż plik Twig i dane
    `Fixtures`_, które są ładowane do bazy danych podczas instalacji. Największy
    fragment kodu, to ``UnitBlock``, który dostarcza dokument dla przykładowego bloku.

    Logika kontrolera jest w rzeczywistości dostarczana przez odpowiednie pakiety
    CMF, tak jak to opisani poniżej.

Istniej kilka pakietów współdziałających w celu włączenia danych osprzętu do
przeglądania witryny. Ogólnie, uproszczony proces jest następujący:

* Po odebraniu żądania, :doc:`dynamiczny router CMF Symfony <routing>` zostaje
  użyty do obsługi nadchodzących żądań;
* Dynamiczny router jest w stanie dopasować żądany URL do dokumentu ``Page``
  dostarczanego przez SimpleCmsBundle i przechowuje go w bazie danych;
* Informacja przypisana do pobieranego dokumentu zostaje użyta do ustalenia
  kontrolera i szablonu;
* Tak skonfigurowany pobrany dokument jest przekazywany do obiektu ``ContentController``
  dostarczanego przez ContentBundle, który renderuje dokument w ``layout.html.twig``
  pakietu AcmeMainBundle.

To jest bardzo uproszczony obraz bardzo prostego CMS zbudowanego na bazie CMF Symfony.
Aby w pełni zrozumieć wszystkie możliwości CMF, kontynuuj lekturę dalszych rozdziałów
niniejszego podręcznika.

Jeśli chcesz przejrzeć zawartość bazy danych PHPCR możesz użyć następujących poleceń:

.. code-block:: bash

    $ php app/console doctrine:phpcr:node:dump
    $ php app/console doctrine:phpcr:node:dump --props
    $ php app/console doctrine:phpcr:node:dump /path/to/node

Powyższy przykład pokazuje odpowiednio podsumowanie, szczegółowy widok oraz podsumowanie
węzła i wszystkie jego węzły potomne (zamiast rozpoczynać wyświetlenie drzewa od korzenia).

Aby poznać wszystkie możliwości obejrzyj wyjścia parametru ``--help``:

.. code-block:: bash

    $ php app/console doctrine:phpcr:node:dump --help

Dodawanie nowych stron
~~~~~~~~~~~~~~~~~~~~~~

Symfony CMF SE nie dostarcza żadnych narzędzi administracyjnych do tworzenia nowych
stron. Jeżeli chcesz dodać interfejs administracyjny, to jedno z rozwiązań jest
opisane w dokumencie :doc:`../cookbook/creating_a_cms/sonata-admin`. Jednak, jeśli
chcesz tylko w prosty sposób dodać nowe strony, to możesz edytować je poprzez edycję
inline, a następnie można użyć migratora ``page`` pakietu SimpleCmsBundle.
Symfony CMF SE dostarczany jest z przykładowym plikiem YAML umieszczonym
w ``app/Resources/data/pages/test.yml``. Zawartość tego pliku można ładować do
PHPCR wywołując:

.. code-block:: bash

    $ php app/console doctrine:phpcr:migrator:migrate page --identifier=/cms/simple/test

Należy zauważyć, że powyższy identyfikator jest odwzorowywany do
``app/Resources/data/pages/test.yml`` przez wycięcie konfiguracji  ``basepath``
pakietu SimpleCmsBundle (domyślnie ``/cms/simple``).

Dlatego, jeśli chce się określić stronę podrzędną ``foo`` dla ``/cms/simple/test``,
musi się utworzyć plik ``app/Resources/data/pages/test/foo.yml`` i następnie uruchomić
następujące polecenie:

.. code-block:: bash

    $ php app/console doctrine:phpcr:migrator:migrate page --identifier=/cms/simple/test/foo

.. _`cmf.liip.ch`: http://cmf.liip.ch
.. _`SQLite`: http://www.sqlite.org/
.. _`Composer`: http://getcomposer.org/
.. _`guidelines in the symfony book`: http://symfony.com/doc/master/book/installation.html#configuration-and-setup
.. _`Fixtures`: http://symfony.com/doc/current/bundles/DoctrineFixturesBundle/index.html

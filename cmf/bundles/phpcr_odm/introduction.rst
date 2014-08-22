.. highlight:: php
   :linenothreshold: 2

.. index::
    single: PHPCR; pakiety
    single: DoctrinePHPCRBundle

DoctrinePHPCRBundle
===================

Pakiet `DoctrinePHPCRBundle`_ zapewnia integrację z repozytorium treści PHP
i opcjonalnie z Doctrine PHPCR-ODM w celu dostarczenia menadżera dokumentów ODM
w Symfony.

Zaraz po instalacji pakiet obsługuje następujące implementacje PHPCR:

* `Jackalope`_ (Jackrabbit, Doctrine DBAL i transporty prismic)

.. tip::

    Informator ten wyjaśnia tylko integracje Symfony2 z PHPCR i PHPCR-ODM.
    Zapoznaj się ze `stroną internetową PHPCR`_, aby poznać stosowanie PHPCR
    oraz z `dokumentacja PHPCR-ODM`_ w celu poznania Doctrine PHPCR-ODM.

Montaż
------

Wymagania
~~~~~~~~~

* przy wykorzystaniu **jackalope-jackrabbit**: Java, Apache Jackalope i libxml
  wersja >= 2.7.0 (z powodu `błędu w libxml`_)
* przy wwykorzystaniu **jackalope-doctrine-dbal z MySQL**: MySQL >= 5.1.5
  (jeśli potrzeba funkcji xml, to ``ExtractValue``)

Instalacja
----------

Pakiet ten można zainstalować `poprzez Composer`_ używając pakietu `doctrine/phpcr-bundle`_.
Trzeba zażądać konkretną implementację API PHPCR. W tym przykładzie zakładamy,
że stosowana jest biblioteka Jackalope Doctrine DBAL. W celu poznania rozwiązań
alternatywnych zobacz :doc:`../../cookbook/database/choosing_phpcr_implementation`.

Jeśli chce się używać PHPCR-ODM, dodatkowo trzeba zażądać 
``doctrine/phpcr-odm``

.. code-block:: javascript
   :linenos:

    require: {
        ...
        "jackalope/jackalope-doctrine-dbal": "1.1.*",
        "doctrine/phpcr-odm": "1.1.*",
        "doctrine/phpcr-bundle": "1.1.*",
        ...
    }

Oprócz ``Doctrine\Bundle\PHPCRBundle\DoctrinePHPCRBundle`` trzeba również utworzyć
w jądrze aplikacji instancję ``Doctrine\Bundle\DoctrineBundle\DoctrineBundle``.

Konfiguracja
------------

Konfiguracja sesji PHPCR
~~~~~~~~~~~~~~~~~~~~~~~~

Sesja potrzebuje implementacji PHPCR określonej w sekcji ``backend`` przez pole
``type``, wraz z opcjami konfiguracyjnymi do implementowania bootstrap. Niniejsze
przykłady zakładają, że wykorzystuje się Jackalope Doctrine
DBAL. Pełna dokumentacja jest publikowana wa :doc:`dziale informatora
<../../reference/configuration/phpcr_odm>`.

Do wykorzystywania Jackalope Doctrine DBAL, trzeba skonfigurować połączenie z bazą
danych w pakiecie DoctrineBundle. Zobacz `dokumentację Symfony2 Doctrine`_ w celu
poznania szczegółów. Prosty przykład:

.. code-block:: yaml
   :linenos:

    # app/config/parameters.yml
    parameters:
        database_driver:   pdo_mysql
        database_host:     localhost
        database_name:     test_project
        database_user:     root
        database_password: password

    # ...

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        doctrine:
            dbal:
                driver:   "%database_driver%"
                host:     "%database_host%"
                dbname:   "%database_name%"
                user:     "%database_user%"
                password: "%database_password%"

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:doctrine="http://symfony.com/schema/dic/doctrine"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                                http://symfony.com/schema/dic/doctrine http://symfony.com/schema/dic/doctrine/doctrine-1.0.xsd">

            <doctrine:config>
                <doctrine:dbal
                    driver="%database_driver%"
                    host="%database_host%"
                    dbname="%database_name%"
                    user="%database_user%"
                    password="%database_password%"
                />
            </doctrine:config>

        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $configuration->loadFromExtension('doctrine', array(
            'dbal' => array(
                'driver'   => '%database_driver%',
                'host'     => '%database_host%',
                'dbname'   => '%database_name%',
                'user'     => '%database_user%',
                'password' => '%database_password%',
            ),
        ));

Jackalope Doctrine DBAL zapewnia implementację PHPCR bez jakichkolwiek wymagań
instalacyjnych, poza jakimkolwiek systemem RDBMS obsługiwanym przez Doctrine.
Po skonfigurowaniu Doctrine DBAL, można skonfigurować Jackalope:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        doctrine_phpcr:
            session:
                backend:
                    type: doctrinedbal
                    # requires DoctrineCacheBundle
                    # caches:
                    #     meta: doctrine_cache.providers.phpcr_meta
                    #     nodes: doctrine_cache.providers.phpcr_nodes
                    # enable logging
                    logging: true
                    # enable profiling in the debug toolbar.
                    profiling: true
                workspace: default
                username: admin
                password: admin

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">

            <config xmlns="http://doctrine-project.org/schema/symfony-dic/odm/phpcr">

                <session
                    workspace="default"
                    username="admin"
                    password="admin"
                >

                    <backend
                        type="doctrinedbal"
                        logging="true"
                        profiling="true"
                    >
                        <!--
                        <caches
                            meta="doctrine_cache.providers.phpcr_meta"
                            nodes="doctrine_cache.providers.phpcr_nodes"
                        />
                        -->
                    </backend>
                </session>
            </config>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('doctrine_phpcr', array(
            'session' => array(
                'backend' => array(
                    'type'       => 'doctrinedbal',
                    'logging'    => true,
                    'profiling'  => true,
                    //'caches' => array(
                    //    'meta' => 'doctrine_cache.providers.phpcr_meta'
                    //    'nodes' => 'doctrine_cache.providers.phpcr_nodes'
                    //),
                ),
                'workspace' => 'default',
                'username'  => 'admin',
                'password'  => 'admin',
            ),
        ));

Teraz trzeba się upewnić, że istnieje baza danych i ja zainicjować:

.. code-block:: bash

    # without Doctrine ORM
    php app/console doctrine:database:create
    php app/console doctrine:phpcr:init:dbal

.. tip::

    Oczywiście można użyć innego połączenia niż domyślne. Zaleca się używać oddzielnego
    połączenia dla każdej odrębnej bazy danych, jeśli również wykorzystuje się Doctrine
    ORM lub bezpośredni dostęp DBAL do danych, a nie mieszanie tych danych w tabelach
    wygenerowanych przez Jackalope Doctrine Dbal.  Jeśli ma się oddzielne połączenia,
    trzeba przekazać alternatywna nazwę połączenia do polecenia ``doctrine:database:create``
    z opcją ``--connection``. Ten parametr nie jest potrzebny dla poleceń Doctrine PHPCR,
    gdyż skonfigurowało się już połączenie do stosowania.

Jeśli używa się Doctrine ORM na tym samym połączeniu, to schemat jest zintegrowany
z ``doctrine:schema:create|update|drop``, jak tez z `DoctrineMigrationsBundle`_,
tak więc można tworzyć migracje.

.. code-block:: bash

    # Using Doctrine ORM
    php app/console doctrine:database:create
    php app/console doctrine:schema:create

.. note::

    W celu wykorzystywania pamięci podręcznej, trzeba zainstalować i skonfigurować
    :doc:`DoctrineCacheBundle <../../cookbook/database/doctrine_cache>`.
    Odkomentuj meta danych pamięci podręcznej i ustawienia węzłów.

Konfiguracja Doctrine PHPCR-ODM
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Ta sekcja konfiguracji zarządza systemem odwzorowywania dokumentów, który konwertuje
węzły PHPCR na obiekty modelu domeny. Jeśli nie skonfiguruje się tutaj niczego,
załadowane zostaną usługi ODM.

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        doctrine_phpcr:
            odm:
                auto_mapping: true
                auto_generate_proxy_classes: "%kernel.debug%"

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">

            <config xmlns="http://doctrine-project.org/schema/symfony-dic/odm/phpcr">

                <odm
                    auto-mapping="true"
                    auto-generate-proxy-classes="%kernel.debug%"
                />
            </config>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('doctrine_phpcr', array(
            'odm' => array(
                'auto_mapping' => true,
                'auto_generate_proxy_classes' => '%kernel.debug%',
            ),
        ));

W przypadku, gdy wyłączy się ``auto_mapping``, będzie można umieszczać dokumenty
w folderze ``Document`` wewnątrz swojego pakietu i stosować swoją adnotację lub
nazwy plików mapujących zgodnych z następujacym schematem:
``<Bundle>/Resources/config/doctrine/<DocumentClass>.phpcr.xml`` lub ``*.phpcr.yml``.

Jeśli ``auto_generate_proxy_classes`` ma wartość false, trzeba uruchomić polecenie
``cache:warmup`` w celu uzyskania klas proxy, generowanych po zmodyfikowaniu dokumentu.
Zwykle jest to wykonywane w środowisku produkcyjnym w celu uzyskania większej wydajności.


Rejestracja typu węzła
""""""""""""""""""""""

PHPCR-ODM używa `niestandardowy typ węzła`_ do śledzenia meta informacji bez
ingerowania w treść. Istnieje polecenie, które trywializuje rejestrowanie tego
typu jak też przestrzeni nazewniczej i ścieżek bazowych pakietów:

.. code-block:: bash

    $ php app/console doctrine:phpcr:repository:init

Wystarczy tylko uruchomić to polecenie podczas tworzenia nowego repozytorium, ale
nic się nie uda, jeśli uruchomi się go, na przykład, na oddzielny wdrożeniu.

Profilowanie i wykonywanie Jackalope
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

W przypadku używania jakiejkolwiek implementacji Jackalope PHPCR, można aktywować
rejestrowanie zdarzeń z zapisem do dziennika Symfony log lub profilować w celu
pokazania informacji na pasku narzędziowym Symfony2:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        doctrine_phpcr:
            session:
                backend:
                    # ...
                    logging: true
                    profiling: true

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">

            <config xmlns="http://doctrine-project.org/schema/symfony-dic/odm/phpcr">

                <session>

                    <backend
                        logging="true"
                        profiling="true"
                    />
                </session>
            </config>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.yml
        $container->loadFromExtension('doctrine_phpcr', array(
            'session' => array(
                'backend' => array(
                    // ...
                    'logging'   => true,
                    'profiling' => true,
                ),
            ),
        ));

Teraz można zobaczyć efekty zmian. Można spróbować dostosować globalną głębokość
osiągania, tak aby zmniejszyć liczbę i czas trwania zapytań. Ustaw opcję
``jackalope.fetch_depth`` na coś większego od 0, aby osiągać węzły potomne lub
poddrzewa. Może to zmniejszyć liczbę potrzebnych zapytań, ale trzeba uważać na
dłuższe zapytania, ponieważ pobierana jest większa liczba danych.

Podczas korzystania z Jackalope Doctrine DBAL zaleca się
:doc:`aktywowanie opcji pamięci podręcznej <../../cookbook/database/doctrine_cache>`.

Warto pamiętać, że można również ustawić *fetchDepth* dla sesji w locie, dla
określonych wywołań lub użyć opcję fetchDepth dla odwzorowań węzłów potomnych
dokumentów.

Parametr ``jackalope.check_login_on_server`` można ustawić na false, aby zapisać
początkowe wywołania z bazą danych dla sprawdzenia, czy działa połączenie.

Usługi
------

Pakiet ten dostarcza trzy główne usługi:

* ``doctrine_phpcr``- instancję ``ManagerRegistry`` z odniesieniem do wszystkich
  instancji sesji i menadżera dokumentów;
* ``doctrine_phpcr.default_session`` - instancje sesji PHPCR;
* ``doctrine_phpcr.odm.default_document_manager`` - instancję menadzera PHPCR-ODM.

.. _bundle-phpcr-odm-commands:

Polecenia Doctrine PHPCR
------------------------

Wszystkie polecenie dotyczące PHPCR są poprzedzone sekwencją ``doctrine:phpcr``
i możliwe jest użycie argumentu ``--session`` w celu zastosowania innej niż domyślnej
sesji, gdy konfiguruje się kilka sesji PHPCR.

Niektóre z tych poleceń są specyficzne dla zaplecza bazy danych lub ODM. Polecenia
te będą dostępne tylko, gdy skonfigurowane jest takie zaplecze.

Zastosuje ``app/console help <command>``, aby zobaczyć wszystkie opcje poleceń.

* **doctrine:phpcr:document:migrate-class**: polecenie dla migracji klas dokumentu;
* **doctrine:phpcr:fixtures:load**: ładuje dane testowe do baz danych PHPCR;
* **doctrine:phpcr:init:dbal**: przygotowuje bazę danych dla Jackalope Doctrine-Dbal;
* **doctrine:phpcr:jackrabbit**: uruchamia lub zatrzymuje serwer Jackrabbit (zobacz też
  :doc:`../../cookbook/database/running_jackrabbit`);
* **doctrine:phpcr:mapping:info**: pokazuje podstawowe informacje o wszystkich
  odwzorowanych dokumentach;
* **doctrine:phpcr:migrator:migrate**: migruje dane PHPCR;
* **doctrine:phpcr:node-type:list**: wykazuje wszystkie dostępne typy węzłów w repozytorium;
* **doctrine:phpcr:node-type:register**: rejestruje typy węzłów w repozytorium PHPCR;
* **doctrine:phpcr:node:dump**: zrzuca poddrzewa repozytorium treści;
* **doctrine:phpcr:node:move**: przenosi węzeł z jedej ścieżki do drugiej;
* **doctrine:phpcr:node:remove**: usuwa treść z repozytorium;
* **doctrine:phpcr:node:touch**: tworzy lub modyfikuje węzeł;
* **doctrine:phpcr:nodes:update**: polecenie do manipulowania węzłami w przestrzeni roboczej;
* **doctrine:phpcr:repository:init**: inicjuje repozytorium PHPCR;
* **doctrine:phpcr:workspace:create**: tworzy przestrzeń roboczą w skonfigurowanym repozytorium;
* **doctrine:phpcr:workspace:export**: eksportuje węzły z repozytorium,
  albo do formatu widoków systemu JCR albo do formatu widoku dokumentu;
* **doctrine:phpcr:workspace:import**: importuje dane xml do repozytorium,
  albo w formacie widoków systemu JCR albo jakiegośc xml;
* **doctrine:phpcr:workspace:list**: wykazuje wszystkie dostępne przestrzenie robocze w skonfigurowanym repozytorium;
* **doctrine:phpcr:workspace:purge**: usuwa wszystkie węzły z przestrzeni roboczej;
* **doctrine:phpcr:workspace:query**: wykonuje wyrażenie JCR SQL2.

.. note::

    Dla stosowania polecenia ``doctrine:phpcr:fixtures:load`` dodatkowo trzeba
    zainstalować pakiet `DoctrineFixturesBundle`_ i jego zależności. Zobacz
    :ref:`phpcr-odm-repository-fixtures`, aby dowiedzieć się jak używać konfiguratorów
    testowania (*ang. fixtures*).

Kilka przykładów uruchamiania poleceń
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Uruchomienie `SQL2 queries`_ z repozytorium:

.. code-block:: bash

    $ php app/console doctrine:phpcr:workspace:query "SELECT title FROM [nt:unstructured] WHERE NAME() = 'home'"

Zrzut węzłów w katalogu ``/cms/simple`` łącznie z ich właściwościami:

.. code-block:: bash

    $ php app/console doctrine:phpcr:node:dump /cms/simple --props

.. _phpcr-odm-backup-restore:

Prosta kopia bezpieczeństwa i przywracanie danych
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Dla wyeksportowania do pliku danych repozytorium  można użyć:

.. code-block:: bash

    $ php app/console doctrine:phpcr:workspace:export --path /cms /path/to/backup.xml

.. note::

    Zawsze zachodzi potrzeba określenia ścieżki eksportu. Bez podania jakiejkolwiek
    ścieżki zostanie dokonany eksport węzła głównego repozytorium, który będzie
    później importowany jako ``jcr:root``.

Do przywrócenie tej kopii zapasowej można uruchomić:

.. code-block:: bash

    $ php app/console doctrine:phpcr:workspace:import /path/to/backup.xml

Warto pamiętać, że można również eksportować i importować części repozytorium
wybierając różne ścieżki dla eksportu i określając opcję ``--parentpath`` dla
importu.

Jeśli masie już dane w swoim repozytorium, które chce się wymienić, można usunąć
najpierw docelowy węzeł:

.. code-block:: bash

    $ php app/console doctrine:phpcr:node:remove /cms

Przeczytaj też
--------------

* :doc:`events`
* :doc:`forms`
* :doc:`fixtures_initializers`
* :doc:`multilang`
* :doc:`multiple_sessions`

.. _`DoctrinePHPCRBundle`: https://github.com/doctrine/DoctrinePHPCRBundle
.. _`Symfony2 Doctrine documentation`: http://symfony.com/doc/current/book/doctrine.html
.. _`Jackalope`: http://jackalope.github.io/
.. _`stroną internetową PHPCR`: http://phpcr.github.io/
.. _`dokumentacja PHPCR-ODM`: http://docs.doctrine-project.org/projects/doctrine-phpcr-odm/en/latest/
.. _`błędu w libxml`: http://bugs.php.net/bug.php?id=36501)
.. _`z Composer`: http://getcomposer.org
.. _`doctrine/phpcr-bundle`: https://packagist.org/packages/doctrine/phpcr-bundle
.. _`metadata caching`: http://symfony.com/doc/master/reference/configuration/doctrine.html
.. _`PHPCR-ODM documentation on Multilanguage`: http://docs.doctrine-project.org/projects/doctrine-phpcr-odm/en/latest/reference/multilang.html
.. _`custom node type`: https://github.com/doctrine/phpcr-odm/wiki/Custom-node-type-phpcr%3Amanaged
.. _`the PHPCR-ODM documentation`: http://docs.doctrine-project.org/projects/doctrine-phpcr-odm/en/latest/reference/events.html
.. _`Symfony event subscriber`: http://symfony.com/doc/master/components/event_dispatcher/introduction.html#using-event-subscribers
.. _`Symfony cookbook entry`: http://symfony.com/doc/current/cookbook/doctrine/event_listeners_subscribers.html
.. _`Symfony documentation on the entity form type`: http://symfony.com/doc/current/reference/forms/types/entity.html
.. _SonataDoctrinePHPCRAdminBundle: http://sonata-project.org/bundles/doctrine-phpcr-admin/master/doc/index.html
.. _`currently broken`: https://github.com/sonata-project/SonataDoctrineORMAdminBundle/issues/145
.. _`DoctrineMigrationsBundle`: http://symfony.com/doc/current/bundles/DoctrineMigrationsBundle/index.html
.. _`DoctrineFixturesBundle`: http://symfony.com/doc/current/bundles/DoctrineFixturesBundle/index.html
.. _`Doctrine data-fixtures`: https://github.com/doctrine/data-fixtures
.. _`documentation of the DoctrineFixturesBundle`: http://symfony.com/doc/current/bundles/DoctrineFixturesBundle/index.html
.. _`SQL2 queries`: http://www.h2database.com/jcr/grammar.html
.. _`BurgovKeyValueFormBundle`: https://github.com/Burgov/KeyValueFormBundle

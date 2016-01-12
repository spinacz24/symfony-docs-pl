.. highlight:: php
   :linenothreshold: 2

.. index::
   single: Doctrine

Bazy danych a Doctrine
======================

Jednym z najczęstszych i trudnych zadań każdej aplikacji jest utrzymywanie
informacji zapisanej w bazie danych i jej odczytywanie . Chociaż framework Symfony
nie jest domyślnie zintegrowany z żadną biblioteką `ORM`_, to dystrybucja Symfony Standard
Edition, będąca najczęściej wykorzystywaną dystrybucją Symfony, dostarczana jest
ze zintegrowaną biblioteką `Doctrine`_, której jedynym celem jest dostarczenie skutecznych
narzędzi do uczynienia tego zadania łatwym. W tym rozdziale zapoznasz się z podstawami
filozofii stojącej za Doctrine i zobaczysz jak można łatwo pracować z bazą danych.

.. note::

    Doctrine jest całkowicie niezależną i oddzielną biblioteką i jej stosowanie
    w Symfony jest opcjonalne. Ten rozdział traktuje o Doctrine ORM, bibliotece
    pozwalającej odwzorować obiekty w relacyjnej bazie danych (takiej jak *MySQL*,
    *PostgreSQL* czy *Microsoft SQL*). Jeżeli wolisz używać surowych zapytań,
    to proste – zapoznaj się z artykułem ":doc:`/cookbook/doctrine/dbal`".

    Można również utrwalić dane w `MongoDB`_ stosując bibliotekę Doctrine ODM.
    Więcej informacji na ten temat znajdziesz w dokumentacji
    "`DoctrineMongoDBBundle`_".

Prosty przykład: "Produkty"
---------------------------

Najprostszym sposobem zrozumienia działania Doctrine jest zobaczenia jej w działaniu.
W tym rozdziale skonfigurujemy bazę danych, utworzymy obiekt ``Product``, zachowamy
go w bazie danych i pobierzemy go z powrotem.


Skonfigurowanie bazy danych
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Zanim naprawdę zaczniemy, musimy skonfigurować informację o połączeniu ze swoją
bazą danych. Zgodnie z konwencją informacja ta zapisywana jest w pliku
``app/config/parameters.yml``:

.. code-block:: yaml
   :linenos:

    # app/config/parameters.yml
    parameters:
        database_driver:    pdo_mysql
        database_host:      localhost
        database_name:      test_project
        database_user:      root
        database_password:  password

    # ...

.. note::

    Definiowanie konfiguracji przez ``parameters.yml`` to tylko konwencja.
    Parametry określone w tym pliku są odnoszone do głównego pliku konfiguracyjnego,
    w którym konfigurowana jest biblioteka Doctrine:

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
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    http://symfony.com/schema/dic/services/services-1.0.xsd
                    http://symfony.com/schema/dic/doctrine
                    http://symfony.com/schema/dic/doctrine/doctrine-1.0.xsd">

                <doctrine:config>
                    <doctrine:dbal
                        driver="%database_driver%"
                        host="%database_host%"
                        dbname="%database_name%"
                        user="%database_user%"
                        password="%database_password%" />
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

    Przez oddzielenie informacji z bazy danych do odrębnego pliku można łatwo
    przechowywać różne wersje pliku na każdym serwerze. Można również łatwo
    przechowywać poza projektem konfigurację bazy danych (lub jakieś poufne
    informacje), na przykład wewnątrz konfiguracji Apache. Więcej informacji na
    ten temat można uzyskać w artykule :doc:`/cookbook/configuration/external_parameters`.

Teraz, gdy Doctrine posiada informacje o bazie danych, Symfony może użyć tej biblioteki
do utworzenia bazy danych:

.. code-block:: bash

    $ php app/console doctrine:database:create

.. sidebar:: Konfiguracja bazy danych do UTF8

    Częstym błędem, który popełniają nawet doświadczeni programiści jest rozpoczęcie
    projektu Symfony bez ustawienia domyślnych wartości ``charset`` i ``collation``
    dla swojej bazy danych, co skutkuje łacińskim porządkiem sortowania, który jest
    domyślny dla większości systemów baz danych. Mogą oni nawet pamiętać, aby to zrobić
    za pierwszym razem, ale zapominają że czynią to już po uruchomieniu dość popularnych
    poleceń w czasie programowania:

    .. code-block:: bash
       
        $ php app/console doctrine:database:drop --force
        $ php app/console doctrine:database:create

    Nie ma sposobu aby skonfigurować te wartości domyślne wewnątrz Doctrine.
    Jedyną możliwością rozwiązania tego problemu jest skonfigurowanie tych wartości
    na poziomie serwera.

    Ustawienie domyślne UTF8 dla MySQL sprowdza się do dodanie kilku linii
    do pliku konfiguracyjnego serwera (przeważnie ``my.cnf``):

    .. code-block:: ini
       
        [mysqld]
        # W versji 5.5.3 wprowadzono zestawa "utf8mb4", który jest zalecany
        collation-server     = utf8mb4_general_ci # Replaces utf8_general_ci
        character-set-server = utf8mb4            # Replaces utf8
    
    Trzeba mieć na uwadze, że MySQL nie obsługuje 4-bajtowych znaków unicode
    w zestawach ``utf8`` i łańcuchy tekstowe zawierajace takie znaki są
    obcinane. Zostało to naprawione przez wprowadzenie `nowego zestawu znakowego utf8mb4`_.    

.. note::

    Jeśli chcesz stosować bazę danych SQLite, musisz ustawić ścieżkę do pliku bazy
    danych SQLite:

    .. configuration-block::

        .. code-block:: yaml
           :linenos:

            # app/config/config.yml
            doctrine:
                dbal:
                    driver: pdo_sqlite
                    path: "%kernel.root_dir%/sqlite.db"
                    charset: UTF8

        .. code-block:: xml
           :linenos:

            <!-- app/config/config.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:doctrine="http://symfony.com/schema/dic/doctrine"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    http://symfony.com/schema/dic/services/services-1.0.xsd
                    http://symfony.com/schema/dic/doctrine
                    http://symfony.com/schema/dic/doctrine/doctrine-1.0.xsd">

                <doctrine:config>
                    <doctrine:dbal
                        driver="pdo_sqlite"
                        path="%kernel.root_dir%/sqlite.db"
                        charset="UTF-8" />
                </doctrine:config>
            </container>

        .. code-block:: php
           :linenos:

            // app/config/config.php
            $container->loadFromExtension('doctrine', array(
                'dbal' => array(
                    'driver'  => 'pdo_sqlite',
                    'path'    => '%kernel.root_dir%/sqlite.db',
                    'charset' => 'UTF-8',
                ),
            ));

Utworzenie klasy encji
~~~~~~~~~~~~~~~~~~~~~~

Załóżmy, że budujemy aplikację w której powinny być wyświetlane produkty. Nawet
bez myślenia o Doctrine lub bazach danych wiesz już, że do reprezentowania produktów
potrzebny jest obiekt ``Product``. Utworzymy taką klasę wewnątrz katalogu ``Entity``
w ``AppBundle``::

    // src/AppBundle/Entity/Product.php
    namespace AppBundle\Entity;

    class Product
    {
        protected $name;
        protected $price;
        protected $description;
    }

Klasa ta, często nazywana "encją" (co oznacza podstawową klasę przechowującą dane),
jest prosta i pomaga spełnić w aplikacji wymóg procesów biznesowych potrzebujących
produktów. Na razie nie może ona być utrwalona w bazie danych - jest to tylko prosta
klasa PHP.

.. tip::

    Gdy się już pozna koncepcje stojące za Doctrine nie powinno być problemu
    z samodzielnym tworzeniem encji:
    
    .. code-block:: bash

       $ php app/console doctrine:generate:entity

.. index::
    single: Doctrine; dodawanie metadanych odwzorowania

.. _book-doctrine-adding-mapping:

Dodanie informacji odwzorowania
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Doctrine umożliwia pracę z bazami danych w sposób o wiele bardziej interesujacy
niż tylko pobieranie wierszy do tablic z tabel bazy danych. Doctrine umożliwia
ponadto utrwalanie w bazie danych całych obiektów. Działa to poprzez odwzorowanie
(mapowanie) klasy na tabelę bazy danych a właściwości klasy na kolumny tabeli:

.. image:: /images/book/doctrine_image_1.png
   :align: center

W celu wykonania tego w Doctrine, trzeba utworzyć "metadane" lub w konfiguracji
ustawić odwzorowanie klasy Product i jej właściwości na bazę danych. Metadane
można określić w kilku różnych formatach, włączając w to YAML, XML lub bezpośrednio
w klasie ``Product`` poprzez adnotacje:

.. configuration-block::

    .. code-block:: php-annotations
       :linenos:

        // src/AppBundle/Entity/Product.php
        namespace AppBundle\Entity;

        use Doctrine\ORM\Mapping as ORM;

        /**
         * @ORM\Entity
         * @ORM\Table(name="product")
         */
        class Product
        {
            /**
             * @ORM\Column(type="integer")
             * @ORM\Id
             * @ORM\GeneratedValue(strategy="AUTO")
             */
            protected $id;

            /**
             * @ORM\Column(type="string", length=100)
             */
            protected $name;

            /**
             * @ORM\Column(type="decimal", scale=2)
             */
            protected $price;

            /**
             * @ORM\Column(type="text")
             */
            protected $description;
        }

    .. code-block:: yaml
       :linenos:

        # src/AppBundle/Resources/config/doctrine/Product.orm.yml
        AppBundle\Entity\Product:
            type: entity
            table: product
            id:
                id:
                    type: integer
                    generator: { strategy: AUTO }
            fields:
                name:
                    type: string
                    length: 100
                price:
                    type: decimal
                    scale: 2
                description:
                    type: text

    .. code-block:: xml
       :linenos:

        <!-- src/AppBundle/Resources/config/doctrine/Product.orm.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <doctrine-mapping xmlns="http://doctrine-project.org/schemas/orm/doctrine-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://doctrine-project.org/schemas/orm/doctrine-mapping
                http://doctrine-project.org/schemas/orm/doctrine-mapping.xsd">

            <entity name="AppBundle\Entity\Product" table="product">
                <id name="id" type="integer">
                    <generator strategy="AUTO" />
                </id>
                <field name="name" type="string" length="100" />
                <field name="price" type="decimal" scale="2" />
                <field name="description" type="text" />
            </entity>
        </doctrine-mapping>

.. note::

    W pakiecie można zdefiniować metadane tylko w jednorodnym formacie. Na przykład,
    nie jest możliwe zmieszanie definicji w formacie YAML z adnotacjami umieszczonymi
    nad definicją klasy encji.

.. tip::

    W konfiguracji nazwa tabeli jest opcjonalna i jeżeli zostanie pominięta, to
    automatycznie zostanie przyjęta nazwa z klasy encji.


Doctrine umożliwia wybór typu pola spośród szerokiej gamy różnych rodzajów pól,
każdy z własnymi opcjami. Więcej informacji na ten temat można znaleźć w rozdziale
:ref:`book-doctrine-field-types`.

.. seealso::

    Można również zapoznać się z `Basic Mapping Documentation`_ w celu poznania
    szczegółowej informacji o odzwzorowaniu. Jeżeli stosuje się adnotacje, to trzeba
    poprzedzić wszystkie adnotacje przedrostkiem ``ORM\`` (np. ``ORM\Column(...)``),
    co nie jest opisane w dokumentacji Doctrine. Musi się również dołączyć wyrażenie
    ``use Doctrine\ORM\Mapping as ORM;``, które importuje przedrostek adnotacji ORM.

.. caution::

    Należy uważać aby nazwa klasy i właściwości nie zostały odwzorowane na chronione
    słowa kluczowe SQL (takie jak ``group`` lub ``user``). Na przykład, jeżeli
    nazwa klasy encji, to ``Group``, to domyślnie nazwa tabeli przybierze nazwę
    ``group``, co powodować będzie błąd SQL w niektórych silnikach.
    Zobacz rozdział `Reserved SQL keywords`_ w dokumentacji Doctrine, w celu
    poznania sposobu prawidłowego sposobu rozwiązania konfliktu tych nazw.
    Ewentualnie, jeżeli ma się wolną rękę w wyborze schematu bazy danych,
    to wystarczy odwzorować inną nazwę tabeli lub kolumny. Zobacz do rozdziałów
    `Creating Classes for the Database`_ i `Property Mapping`_ w dokumentacji Doctrine.

.. note::

    W przypadku korzystania z innej biblioteki lub programu (np. Doxygen), które
    wykorzystują adnotacje, trzeba umieścić w klasie z adnotacją wyrażenie
    ``@IgnoreAnnotation``, aby wskazać, które adnotacje mają być ignorowane przez
    Symfony. Na przykład, aby uniknąć zrzucania wyjątku przez adnotację ``@fn``
    trzeba dodać następujące wyrażenie::

        /**
         * @IgnoreAnnotation("fn")
         */
        class Product
        // ...

.. index::
      single: metoda akcesor

.. _book-doctrine-generating-getters-and-setters:

Wygenerowanie metod akcesorów
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Chociaż Doctrine już wie jak utrwalić obiekt ``Product`` w bazie danych, to sama klasa
nie jest jeszcze przydatna. Ponieważ ``Product`` jest zwykłą klasą PHP, to potrzeba
utworzyć metody akcesorów pobierających i ustawiających (*ang. getter i setter*)
(tj. ``getName()`` i ``setName()``) w celu uzyskania dostępu do właściwości tego
obiektu (gdyż właściwości te są chronione). Doctrine może utworzyć te akcesory
w wyniku polecenia:

.. code-block:: bash

    $ php app/console doctrine:generate:entities AppBundle/Entity/Product

Zastosowanie tego polecenia daje pewność, że w klasie ``Product`` zostaną wygenerowane
wszystkie niezbędne akcesory. Polecenie to jest bezpieczne – można uruchamiać je
w kółko - wygeneruje ono tylko nie istniejące akcesory (czyli nie nadpisuje istniejących
metod).

.. caution::

    Należy pamiętać, że generator encji Doctrine wytwarza proste akcesory.
    Trzeba sprawdzić wygenerowana encje i dostosować logikę tych akcesorów do
    własnych potrzeb.
    
.. sidebar:: Więcej o ``doctrine:generate:entities``

    Przy pomocy polecenia ``doctrine:generate:entities`` można:

    * generować akcesory;
   
   * generować klasy repozytorium konfigurowane adnotacją
     ``@ORM\Entity(repositoryClass="...")``;
   
   * generować właściwy konstruktor dla relacji 1:n i n:m.

    Polecenie ``doctrine:generate:entities`` zabezpiecza kopię zapasową oryginalego
    pliku ``Product.php`` mianując ją nazwą ``Product.php~``. W niektórych przypadkach
    obecność tego pliku może powodować błąd "Cannot redeclare class". Można wówczas
    ten plik bezpiecznie usunąć. Można też wykorzystać opcję ``--no-backup`` aby
    zapobiec generowaniu tych plików zapasowych.

    Proszę zauważyć, że nie musi się korzystać z powyższego polecenia.
    Doctrine tego nie wymaga. Wystarczy upewnić się, jak w zwykłej klasie PHP,
    czy wszystkie chronione właściwości klasy mają swoje akcesory. Polecenie to
    zostało utworzone ponieważ używanie Doctrine z linii poleceń jest popularne.

Można wygenerować wszystkie znane encje pakietu (tj. wszystkie klasy PHP określone
w informacji odwzorowania Doctrine) lub w całej przestrzeni nazw:

.. code-block:: bash

    # wygenerowanie wszystkich encji w AppBundle
    $ php app/console doctrine:generate:entities AppBundle

    # wygenerowanie wszystkich encji w rzestrzeni nazewniczej Acme
    $ php app/console doctrine:generate:entities Acme

.. note::

    Dla Doctrine jest wszystko jedno, czy właściwości są chronione czy prywatne,
    lub czy istnieją akcesory dla właściwości. Akcesory są generowane tylko dlatego,
    że potrzebna jest interakcja z obiektem PHP.

.. index::
      single: Doctrine; tworzenie schematu
      single: Doctrine; tworzenie tabel bazy danych

.. _book-doctrine-creating-the-database-tables-schema:

Utworzenie schematu i tabel bazy danych
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Mamy już teraz użyteczną klasę ``Product`` z informacją odwzorowania instruującą
Doctrine jak tą klasę obsługiwać. Nie mamy jeszcze odpowiadającej tej klasie
tabeli ``product`` w bazie danych. Doctrine może automatycznie tworzyć tabele
bazy danych potrzebne dla każdej znanej encji w aplikacji. Do zrobienia tego
wystarczy uruchomić polecenie:


.. code-block:: bash

    $ php app/console doctrine:schema:update --force

.. tip::

    Tak naprawdę polecenie to jest bardzo potężne. Porównuje ono informacje o tym
    jak powinna wyglądać baza danych (na podstawie informacji odwzorowania encji)
    z informacją o tym jak wygląda ona obecnie i generuje wyrażenia SQL potrzebne
    do zaktualizowania bazy danych. Innymi słowami, jeżeli doda się nowe właściwości
    w metadanych odwzorowania dla klasy Product i uruchomi się to zadanie ponownie,
    to zostanie wygenerowane wyrażenie "alter table" potrzebne do dodania nowej
    kolumny do istniejącej tabeli ``product``.

    Lepszym sposobem skorzystania z zaawansowanych możliwości tego polecenia jest
    użycie `migracji`_, które umożliwiają
    wygenerowanie tych wyrażeń SQL i zabezpieczenie ich w klasach migracyjnych,
    które można uruchamiać systematycznie na swoim serwerze produkcyjnym w celu
    śledzenia i migracji schematu bazy danych, bezpiecznie i niezawodnie.

Nasza baza danych ma teraz w pełni funkcjonalną tabelę ``product``, która zgodna
jest z określonymi metadanymi.


Utrwalanie obiektów w bazie danych
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Teraz mamy już encję ``Product`` odwzorowaną w odpowiadającej jej tabeli ``product``,
można więc przekazać dane do bazy danych. Dokonanie tego z poziomu kontrolera jest
całkiem proste. Dodamy następujaca metodę do ``DefaultController`` pakietu::

   // src/AppBundle/Controller/DefaultController.php

    // ...
    use AppBundle\Entity\Product;
    use Symfony\Component\HttpFoundation\Response;

    // ...
    public function createAction()
    {
        $product = new Product();
        $product->setName('A Foo Bar');
        $product->setPrice('19.99');
        $product->setDescription('Lorem ipsum dolor');

        $em = $this->getDoctrine()->getManager();

        $em->persist($product);
        $em->flush();

        return new Response('Created product id '.$product->getId());
    }

.. note::

    Jeśli wykonujesz nasz przykład, to aby zobaczyć jak to działa musisz utworzyć
    trasę wskazującą na tą akcję.

.. tip::

    W tym artykule pokazuje się pracę z Doctrine z poziomu kontrolera z użyciem
    metody :method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller::getDoctrine`
    tego kontrolera. Metoda ta jest skrótem do pobierania usługi ``doctrine``.
    Można pracować z Doctrine gdziekolwiek wstrzykując tą usługę do swojej usługi.
    Proszę przeczytać :doc:`/book/service_container` w celu uzyskania informacji
    o tworzeniu własnych usług.

Spójrzmy na powyższy kod bardziej szczegółowo:


* **linie 10-13** W tej sekcji tworzymy instancję klasy i działamy z obiektem ``$product``
  jak z innym zwykłym obiektem PHP;

* **linia 15** W tej linii pobieramy obiekt *menadżera encji* Doctrine, który jest
  odpowiedzialny za obsługę procesu utrwalania i pobierania obiektów z formularza
  do bazy danych;

* **linia 16** Metoda ``persist()`` powiadamia Doctrine aby "zarządzała" obiektem
  ``$product``. W rzeczywistości to nie powoduje wprowadzenia zapytania do bazy danych
  (na razie);

* **linia 17** Gdy wywoływana jest metoda ``flush()``, Doctrine przeszukuje wszystkie
  zarządzane obiekty, by sprawdzić, czy muszą one zostać utrwalone w bazie danych.
  W naszym przykładzie obiekt ``$product`` nie został jeszcze utrwalony, tak więc
  menadżer encji wykona zapytanie ``INSERT`` i utworzony zostanie wiersz w tabeli
  ``product``.

.. note::

  W rzeczywistości, ponieważ Doctrine ma informacje o wszystkich zarządzanych encjach,
  to gdy wywoła się metodę ``flush()``, przeliczy ona całkowity wskaźnik zmian
  i wykona zapytania w możliwie najlepszej kolejności. Wykorzystywane sa przy tym
  przygotowane i zbuforowane wyrażenia poprawiające trochę wydajność.  
  Przykładowo, jeżeli do utrwalenia jest w sumie 100 obiektów ``Product`` i wywoła
  się metodę ``flush()``, to Doctrine wykona 100 zapytań ``INSERT`` wykorzystując
  pojedynczy spreparowany obiekt.

Podczas tworzenia lub aktualizowania obiektów działanie jest zawsze takie samo.
W następnym rozdziale poznasz, jak Doctrine jest wystarczająco inteligentny aby
automatycznie wystawiać zapytanie ``UPDATE``, jeżeli rekord już istnieje w bazie danych.

.. tip::

    Doctrine dostarcza bibliotekę pozwalającą programowo załadować dane testowe
    do projektu (czyli tzw. "dane fikstur"). Więcej informacji uzyskasz w
    dokumentacji pakietu `DoctrineFixturesBundle`_.

Pobieranie obiektów z bazy danych
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Pobieranie z powrotem obiektów z bazy danych jest jeszcze bardziej łatwiejsze.
Na przykład, załóżmy, że skonfigurowana została trasa do wyświetlania konkretnego
produktu na podstawie jego wartości ``id``::

    public function showAction($id)
    {
        $product = $this->getDoctrine()
            ->getRepository('AppBundle:Product')
            ->find($id);

        if (!$product) {
            throw $this->createNotFoundException(
                'No product found for id '.$id
            );
        }

        // ... zrobić coś, na przykład przekazać obiekt $product do szablonu
    }

.. tip::

    Możesz osiągnąć odpowiednik tego bez pisania jakiegokolwiek kodu używając skrótu
    ``@ParamConverter``. Zobacz dokumentację `FrameworkExtraBundle documentation`_.
    
Gdy przesyła się zapytanie dotyczące określonego typu obiektu, zawsze używa się czegoś,
co nazywa się "repozytorium". Można myśleć o repozytorium jak o klasie PHP, której
jedynym zadaniem jest pomoc w pobieraniu encji okreśłonych klas. Można uzyskać dostęp do
obiektu repozytorium dla klasy encji poprzez::

    $repository = $this->getDoctrine()
        ->getRepository('AppBundle:Product');

.. note::

    Łańcuch ``AppBundle:Product`` jest skrótem, jaki można używać zawsze
    w Doctrine zamiast pełnej nazwy encji (tj. ``AppBundle\Entity\Product``).
    Będzie to działać dopóty ważna jest encja w przestrzeni nazw ``Entity`` pakietu.

Po utworzeniu repozytorium ma się dostęp do wszelkiego rodzaju przydatnych metod::

    // zapytanie przez klucz główny (zwykle "id")
    $product = $repository->find($id);

    // dynamiczne nazwy kolumn odnajdywane na podstawie wartości kolumnowej
    $product = $repository->findOneById($id);
    $product = $repository->findOneByName('foo');

    // odnajdywanie *all* produktów
    $products = $repository->findAll();

    // odnajdywanie grupy produktów na podstawie dowolnej wartości kolumnowej
    $products = $repository->findByPrice(19.99);

.. note::

    Oczywiście można również zadawać bardziej złożone zapytania o których można
    dowiedzieć się więcej w rozdziale :ref:`book-doctrine-queries`.

Można również wykorzystać przydatne metody ``findBy`` i ``findOneBy`` do łatwego
pobierania obiektu na podstawie różnych warunków::

    // zapytanie o jeden produkt o określonej nazwie i cenie
    $product = $repository->findOneBy(
        array('name' => 'foo', 'price' => 19.99)
    );

    // zapytanie o wszystkie produkty pasujace do określonej nazwy, posortowane wg. ceny
    $products = $repository->findBy(
        array('name' => 'foo'),
        array('price' => 'ASC')
    );

.. tip::

    Można zobaczyć, jak wiele zapytań jest wykonywanych podczas generowania strony
    na dolnym pasku debugowania, w prawym dolnym rogu.

    .. image:: /images/book/doctrine_web_debug_toolbar.png
       :align: center
       :scale: 100

    Po kliknięciu na ikonę otworzy się profiler, pokazując dokładnie wykonane zapytania.
    
    Ikona zmieni kolor na żółty, gdy będzie więcej niż 50 zapytań na stronę.
    Moze to wskazywać, że coś nie jest poprawne.

Aktualizacja obiektu
~~~~~~~~~~~~~~~~~~~~

Po pobraniu obiektu z Doctrine, jego aktualizacja jest prosta. Załóżmy, że mamy
trasę, która odwzorowuje ``id`` produktu do kontrolera w celu przeprowadzenia
aktualizacji danych::

    public function updateAction($id)
    {
        $em = $this->getDoctrine()->getManager();
        $product = $em->getRepository('AppBundle:Product')->find($id);

        if (!$product) {
            throw $this->createNotFoundException(
                'No product found for id '.$id
            );
        }

        $product->setName('New product name!');
        $em->flush();

        return $this->redirectToRoute('homepage');
    }

Aktualizacja obiektu obejmuje tylko trzy kroki:

#. pobranie obiektu przez Doctrine;
#. zmodyfikowanie obiektu;
#. wywołanie metody ``flush()`` w menadżerze encji.

Proszę zauważyć, że wywołanie ``$em->persist($product)`` nie jest konieczne.
Przypominamy, że metoda ta jedynie informuje Doctrine, aby zarządzało lub
"przyglądało się" obiektowi ``$product``. W naszym przypadku, ponieważ obiekt
``$product`` został już pobrany przez Doctrine, jest już on zarządzany.

Usunięcie obiektu
~~~~~~~~~~~~~~~~~

Usuwanie obiektu jet bardzo podobne, ale wymaga wywołania metody ``remove()``
menadżera encji::

    $em->remove($product);
    $em->flush();

Jak można się spodziewać, metoda ``remove()`` powiadamia Doctrine, że chce się
usunąć dany obiekt z bazy danych. Zapytanie ``DELETE`` nie jest wykonywane, do
czasu wywołania metody ``flush()``.

.. _`book-doctrine-queries`:

Odpytywanie obiektów
--------------------

Pokazywaliśmy już, jak obiekt repozytorium umożliwia uruchomienie podstawowych zapytań
bez specjalnego wysiłku::

    $repository->find($id);

    $repository->findOneByName('Foo');

Oczywiście Doctrine umożliwia również pisanie bardziej złożonych zapytań przy
użyciu Doctrine Query Language (DQL). DQL jest podobny do SQL, z tą różnicą, że
trzeba sobie wyobrazić, że tu odpytywane są obiekty klasy encji (np. ``Product``)
a nie wiersze tabeli (np. ``product``).

Podczas odpytywania w Doctrine, ma się dwie możliwości: pisanie czystych zapytań
Doctrine lub stosowanie budowniczego zapytań Doctrine.

Odpytywanie obiektów z użyciem DQL
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Proszę sobie wyobrazić, że chcemy zapytać o produkty, ale tylko takie, które kosztują więcej
niż ``19.99`` i są uporządkowane od najtańszych do najdroższych. Do wykonania
zapytania można użyć natywnego języka podobnego do SQL o nazwie DQL::

    $em = $this->getDoctrine()->getManager();
    $query = $em->createQuery(
        'SELECT p FROM AppBundle:Product p WHERE p.price > :price ORDER BY p.price ASC'
    )->setParameter('price', '19.99');

    $products = $query->getResult();
    // dla uzyskania jednego wyniku:
    // $product = $query->setMaxResults(1)->getOneOrNullResult();
    

Jeżeli zna się SQL, to z DQL powinno się czuć bardzo naturalnie.
Największą różnicą jest to, że w DQL powinno sie myśleć w kategoriach "obiektów"
zamiast wierszy bazy danych. Z tego powodu trzeba wybrać z (``FROM``) *obiektu*
 ``AppBundle:Product`` (opcjonalny skrót dla ``AppBundle\Entity\Product``)
 a następnie aliasować to jako ``p``.
 
.. tip::

    Trzeba barać pod uwagę metodę ``setParameter()``. Podczas pracy z Doctrine,
    dobrym pomysłem jest ustawianie wszystkich zewnętrznych wartości jako
    "wieloznaczniki" (``:price`` w poprzednim przykładzie) bo zabezpiecza to
    przed atakami wstrzykiwania SQL.

Metoda ``getResult()`` zwraca tablicę wyników. W celu pobrania tylko jedengo
wyniku trzeba uzyć ``getOneOrNullResult()``::

    $product = $query->setMaxResults(1)->getOneOrNullResult();
 
.. index::
      budowniczy zapytań
      single: Doctrine; QueryBuilder
      single: Doctrine; budowniczy zapytań
       

Stosowanie budowniczego zapytań Doctrine
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Zamiast pisać łańcuch DQL, można użyć pomocny obiekt o nazwie ``QueryBuilder``
do zbudowania łańcucha zapytania. Jest to przydatne, gdy rzeczywiste zapytanie
uzależnione jest od dynamicznych warunków, jako że kod staje sie trudny do odczytania
z DQL po rozpoczęciu łączenia łańcuchów::

    $repository = $this->getDoctrine()
        ->getRepository('AppBundle:Product');

    // createQueryBuilder automatically selects FROM AppBundle:Product
    // and aliases it to "p"
    $query = $repository->createQueryBuilder('p')
        ->where('p.price > :price')
        ->setParameter('price', '19.99')
        ->orderBy('p.price', 'ASC')
        ->getQuery();

    $products = $query->getResult();
    // to get just one result:
    // $product = $query->setMaxResults(1)->getOneOrNullResult();
    
Obiekt ``QueryBuilder`` zawiera wszystkie niezbędne metody do budowy zapytania.
Przez wywołanie metody ``thegetQuery()`` budowniczego zapytań zwraca zwykły obiekt
``Query``, który może być użyty do pobranie wyniku zapytania.

Więcej informacji o konstruktorze zapytań Doctrine można znaleźć w dokumentacji
`Query Builder`_.

.. _book-doctrine-custom-repository-classes:

Własne klasy repozytorium
~~~~~~~~~~~~~~~~~~~~~~~~~

W poprzednich rozdziałach rozpoczęliśmy konstruowanie i używanie bardziej złożonych
zapytań wewnątrz kontrolera. W celu izolacji, testowania i ponownego wykorzystania
zapytań, dobrą praktyką jest utworzenie własnej klasy repozytorium dla encji
i dodanie tam metod tworzących logikę zapytania.

Dla wykonania tego, należy dodać nazwę klasy repozytorium do definicji odwzorowania.

.. configuration-block::

    .. code-block:: php-annotations
       :linenos:

        // src/AppBundle/Entity/Product.php
        namespace AppBundle\Entity;

        use Doctrine\ORM\Mapping as ORM;

        /**
         * @ORM\Entity(repositoryClass="AppBundle\Entity\ProductRepository")
         */
        class Product
        {
            //...
        }

    .. code-block:: yaml
       :linenos:

        # src/AppBundle/Resources/config/doctrine/Product.orm.yml
        AppBundle\Entity\Product:
            type: entity
            repositoryClass: AppBundle\Entity\ProductRepository
            # ...

    .. code-block:: xml
       :linenos:

        <!-- src/AppBundle/Resources/config/doctrine/Product.orm.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <doctrine-mapping xmlns="http://doctrine-project.org/schemas/orm/doctrine-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://doctrine-project.org/schemas/orm/doctrine-mapping
                http://doctrine-project.org/schemas/orm/doctrine-mapping.xsd">

            <entity
                name="AppBundle\Entity\Product"
                repository-class="AppBundle\Entity\ProductRepository">

                <!-- ... -->
            </entity>
        </doctrine-mapping>

Doctrine może samo wygenerować klasę repozytorium po uruchomieniu tego samego
polecenia, które użyliśmy wcześniej do wygenerowania metod akcesorów:

.. code-block:: bash

    $ php app/console doctrine:generate:entities AppBundle

Następnie dodajemy nową metodę ``findAllOrderedByName()`` do nowo utworzonej klasy
repozytorium. Metoda ta będzie przepytywać wszystkie encje ``Product`` w kolejności
alfabetycznej.

.. code-block:: php
   :linenos:

    // src/AppBundle/Entity/ProductRepository.php
    namespace AppBundle\Entity;

    use Doctrine\ORM\EntityRepository;

    class ProductRepository extends EntityRepository
    {
        public function findAllOrderedByName()
        {
            return $this->getEntityManager()
                ->createQuery(
                    'SELECT p FROM AppBundle:Product p ORDER BY p.name ASC'
                )
                ->getResult();
        }
    }
    

.. tip::

    Menadżer encji może być dostępny poprzez ``$this->getEntityManager()``
    z poziomu repozytorium.

Można używać tej nowej metody, podobnie jak domyślnych metod wyszukujących repozytorium::

    $em = $this->getDoctrine()->getManager();
    $products = $em->getRepository('AppBundle:Product')
                ->findAllOrderedByName();

.. note::

    Podczas stosowania własnej klasy repozytorium nadal ma się dostęp do domyślnych
    metod, takich jak ``find()`` i ``findAll()``.

.. _`book-doctrine-relations`:

Relacje (powiązania) encji
--------------------------

Załóżmy, że produkty w naszej aplikacji należą do jednej "kategorii".
W tym przypadku będziemy potrzebować obiektu ``Category`` i jakiegoś sposobu
odzwierciedlenia relacji obiektu ``Product`` do obiektu ``Category``.
Rozpocznijmy od utworzenia encji ``Category``. Ponieważ wiesz już, że ostatecznie
trzeba będzi utrzymać klasę poprzez Doctrine, to możemy pozwolić, aby Doctrine
utworzyła tą klasę.

.. code-block:: bash
   
    $ php app/console doctrine:generate:entity --no-interaction \
        --entity="AppBundle:Category" \
        --fields="name:string(255)"
    
Zadanie to wygeneruje encję ``Category``, z polami ``id`` i ``name``,
oraz związanymi funkcjami akcesorów.

Metadane odwzorowania relacji
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Aby powiązać encje ``Category`` i ``Product`` trzeba rozpocząć od utworzenia
właściwości ``products`` w klasie ``Category``:

.. configuration-block::

    .. code-block:: php-annotations
       :linenos:

        // src/AppBundle/Entity/Category.php

        // ...
        use Doctrine\Common\Collections\ArrayCollection;

        class Category
        {
            // ...

            /**
             * @ORM\OneToMany(targetEntity="Product", mappedBy="category")
             */
            protected $products;

            public function __construct()
            {
                $this->products = new ArrayCollection();
            }
        }

    .. code-block:: yaml
       :linenos:

        # src/AppBundle/Resources/config/doctrine/Category.orm.yml
        AppBundle\Entity\Category:
            type: entity
            # ...
            oneToMany:
                products:
                    targetEntity: Product
                    mappedBy: category
            # don't forget to init the collection in the __construct() method
            # of the entity

    .. code-block:: xml
       :linenos:

        <!-- src/AppBundle/Resources/config/doctrine/Category.orm.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <doctrine-mapping xmlns="http://doctrine-project.org/schemas/orm/doctrine-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://doctrine-project.org/schemas/orm/doctrine-mapping
                http://doctrine-project.org/schemas/orm/doctrine-mapping.xsd">

            <entity name="AppBundle\Entity\Category">
                <!-- ... -->
                <one-to-many
                    field="products"
                    target-entity="Product"
                    mapped-by="category" />

                <!--
                    don't forget to init the collection in
                    the __construct() method of the entity
                -->
            </entity>
        </doctrine-mapping>


Po pierwsze, ponieważ obiekt ``Category`` będzie odnosić się do wielu obiektów
klasy ``Product``, to dodawana jest właściwość będąca tablicą produktów w celu
przechowywania tych obiektów ``Product``. Dla przypomnienia, nie jest tak dlatego,
że Doctrine wymaga tego rozwiązania, ale dlatego, że sensowne jest przechowywanie
tablicy obiektów ``Product`` dla każdej kategorii.

.. note::

    Kod w metodzie ``__construct()`` jest ważny, ponieważ Doctrine wymaga właściwości
    ``$products`` będącej obiektem ``ArrayCollection``. Obiekt ten wygląda i działa
    prawie tak samo jak tablica, ale ma dodatkową elastyczność. Jeżeli jest to dla
    Ciebie niewygodne, nie przejmuj się. Wystarczy sobie wyobrazić, że jest to tablica.

.. tip::

   Wartość ``targetEntity`` w adnotacji powyżej prezentowanej może odwoływać się
   do jakiejkolwiek encji z ważną przestrzenią nazw, nie tylko encji określonych
   w tej samej klasie. Aby odnieść ``targetEntity`` do encji zdefiniowanych w innej
   klasie lub pakiecie, trzeba wprowadzić pełną nazwę przestrzeni nazw jako wartość
   ``targetEntity``.

Następnie, ponieważ każda klasa ``Product`` odnosi się dokładnie do jednego obiektu
``Category``, dodamy właściwość ``$category`` do klasy ``Product``:

.. configuration-block::

    .. code-block:: php-annotations
       :linenos:

        // src/AppBundle/Entity/Product.php

        // ...
        class Product
        {
            // ...

            /**
             * @ORM\ManyToOne(targetEntity="Category", inversedBy="products")
             * @ORM\JoinColumn(name="category_id", referencedColumnName="id")
             */
            protected $category;
        }

    .. code-block:: yaml
       :linenos:

        # src/AppBundle/Resources/config/doctrine/Product.orm.yml
        AppBundle\Entity\Product:
            type: entity
            # ...
            manyToOne:
                category:
                    targetEntity: Category
                    inversedBy: products
                    joinColumn:
                        name: category_id
                        referencedColumnName: id

    .. code-block:: xml
       :linenos:

        <!-- src/AppBundle/Resources/config/doctrine/Product.orm.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <doctrine-mapping xmlns="http://doctrine-project.org/schemas/orm/doctrine-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://doctrine-project.org/schemas/orm/doctrine-mapping
                http://doctrine-project.org/schemas/orm/doctrine-mapping.xsd">

            <entity name="AppBundle\Entity\Product">
                <!-- ... -->
                <many-to-one
                    field="category"
                    target-entity="Category"
                    inversed-by="products"
                    join-column="category">

                    <join-column name="category_id" referenced-column-name="id" />
                </many-to-one>
            </entity>
        </doctrine-mapping>

Na koniec, teraz dodamy nową właściwość do obu klas ``Category`` i ``Product``,
powiadamiająca Doctrine, aby wygenerowało brakujące metody akcesorów:

.. code-block:: bash

    $ php app/console doctrine:generate:entities AppBundle

Zignorujmy na moment metadane Doctrine. Teraz mamy dwie klasy, ``Category``
i ``Product`` z naturalną relacją jeden-do-wielu. Klasa ``Category`` przechowuje
tablicę obiektów klasy ``Product`` zawierajaca produkty jednej kategorii. Innymi
słowami, mamy skonstruowane potrzebne klasy. Fakt, że muszą one zostać utrwalone
w bazie danych, jest kwestią wtórną

Proszę teraz spójrzeć na metadane sformułowane powyżej właściwości ``$category``
w klasie ``Product``. Informacja ta powiadamia Doctrine, że powiązana klasa jest
kategorią i że powinna przechowywać identyfikator ``id`` rekordu w polu ``category_id``,
które istnieje w tabeli ``product``. Innymi słowami, powiązany obiekt ``Category``
będzie przechowywane właściwości ``$category``, ale w tle, Doctrine będzie utrzymywać
tą relację przez przechowywanie wartości ``id`` kategorii w kolumnie ``category_id``
tabeli ``product``.

.. image:: /images/book/doctrine_image_2.png
   :align: center

Metadana powyżej właściwości ``$products`` obiektu ``Category`` jest mniej ważna
i tylko powiadamia Doctrine aby wyszukał właściwość ``Product.category`` w celu
ustalenia jaka relacja została odwzorowana.

Przed kontynuowaniem, należy się upewnić, że Doctrine jest poinformowane o nowej
tablicy ``category`` i kolumnie ``product.category_id`` oraz nowym kluczu zewnętrznym:

.. code-block:: bash

    $ php app/console doctrine:schema:update --force

.. note::

    Zadanie to powinno być wykonywane tylko w czasie programowania. W celu
    poznania bardziej solidnej metody systematycznego aktualizowania produkcyjnej
    bazy danych, przeczytaj artykuł o `migracjach`_.

Zapisywanie powiązanych encji
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Teraz możemy zobaczyć jak działa nowy kod. Przyjmijmy, że mamy następujący kod
kontrolera::

    // ...

    use AppBundle\Entity\Category;
    use AppBundle\Entity\Product;
    use Symfony\Component\HttpFoundation\Response;

    class DefaultController extends Controller
    {
        public function createProductAction()
        {
            $category = new Category();
            $category->setName('Main Products');

            $product = new Product();
            $product->setName('Foo');
            $product->setPrice(19.99);
            $product->setDescription('Lorem ipsum dolor');
            // relate this product to the category
            $product->setCategory($category);

            $em = $this->getDoctrine()->getManager();
            $em->persist($category);
            $em->persist($product);
            $em->flush();

            return new Response(
                'Created product id: '.$product->getId()
                .' and category id: '.$category->getId()
            );
        }
    }

Teraz pojedynczy wiersz jest dodawany do obu tabel ``category`` i ``product``.
Kolumna ``product.category_id`` dla nowego produktu jest ustawiana na identyfikator
nowej kategorii. Doctrine sam zarządza utrzymaniem tej relacji.

Pobieranie powiązanych obiektów
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Gdy zachodzi potrzeba pobrania powiązanych obiektów, działanie wygląda tak jak
miało to miejsce poprzednio. Najpierw trzeba pobrać obiekt ``$product``
a następnie uzyskać dostęp do powiązanego obiektu ``Category``::

    public function showAction($id)
    {
        $product = $this->getDoctrine()
            ->getRepository('AppBundle:Product')
            ->find($id);

        $categoryName = $product->getCategory()->getName();

        // ...
    }

W tym przykładzie, najpierw zapytamy o obiekt ``Product`` w oparciu o ``id`` produktu.
W tym celu sformujemy zapytanie tylko dla danych produktu i hydratów obiektu ``$product``
z tymi danymi. Później, gdy wywołamy ``$product->getCategory()->getName()``,
Doctrine niejawnie wykona drugie zapytanie aby odnaleźć kategorię powiązaną z produktem.
To przygotuje i zwróci obiekt ``$category``.

.. image:: /images/book/doctrine_image_3.png
   :align: center

Ważne jest to, że ma się łatwy dostęp do powiązanej z produktem kategorii, ale
dane kategorii nie są faktycznie pobierane, dopóki się nie zapyta o tą kategorię
(jest to tzw. „wzorzec leniwego ładowania", *ang. Lazily Loaded Pattern*).

Można również zapytać w drugą stronę::

    public function showProductsAction($id)
    {
        $category = $this->getDoctrine()
            ->getRepository('AppBundle:Category')
            ->find($id);

        $products = $category->getProducts();

        // ...
    }

W tym przypadku, postępowanie jest takie samo: najpierw pytamy o pojedynczy obiekt
``Category`` a następnie Doctrine wykonuje drugie zapytanie, aby pobrać powiązany
obiekt ``Product``, ale tylko raz, jeśli jest on potrzebny (tj. gdy wywołamy
``->getProducts()``). Zmienna ``$products`` jest tablicą obiektów ``Product``,
które są powiązane z określonym obiektem ``Category`` poprzez ich wartość ``category_id``.

.. sidebar:: Relacje a klasy Proxy

    To "leniwe ładowanie" jest możliwe, ponieważ w razie potrzeby Doctrine zwraca
    obiekt "proxy" w miejsce prawdziwego obiektu. Przeanalizujmy ponownie powyższy
    przykład::

        $product = $this->getDoctrine()
            ->getRepository('AppBundle:Product')
            ->find($id);

        $category = $product->getCategory();

        // prints "Proxies\AppBundleEntityCategoryProxy"
        dump(get_class($category));
        die();

    Ten obiekt proxy rozszerza prawdziwy obiekt ``Category``, wyglądając i funkcjonując
    jak on. Różnica jest taka, że przez użycie obiektu proxy, Doctrine może opóźnić
    utworzenie zapytania dla rzeczywistych danych ``Category`` do momentu, w którym
    te dane staną się potrzebne (tj. aż nie wywoła się ``$category->getName()``).

    Klasy proxy są generowane przez Doctrine i przechowywane w katalogu pamięci
    podręcznej. Choć przypuszczalnie nigdy nie będziesz ich zauważał, to ważne jest,
    aby pamiętać, że obiekt ``$category`` jest w rzeczywistości obiektem proxy.

    W następnym rozdziale, podczas pobierania naraz danych produktów i kategorii
    (poprzez *join*), Doctrine zwróci prawdziwy obiekt ``Category``, ponieważ nic
    nie musi być ładowane leniwie.

Łączenie powiązanych rekordów
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

W powyższych przykładach zostały wykonane dwa zapytania – jedno dla oryginalnego
obiektu (tj. ``Category``) a drugie dla obiektów powiązanych (tj. obiektów ``Product``).

.. tip::

    Pamiętaj, że możesz zobaczyć wszystkie wykonane podczas zapytania zapytania
    na pasku debugowania.

Jeśli wiesz z góry, że będziesz potrzebował dostępu do obu obiektów, to możesz
uniknąć drugiego zapytania przez zastosowanie złączenia w oryginalnym zapytaniu.
Dodamy następującą metodę do klasy ``ProductRepository``::

    // src/AppBundle/Entity/ProductRepository.php
    public function findOneByIdJoinedToCategory($id)
    {
        $query = $this->getEntityManager()
            ->createQuery('
                SELECT p, c FROM AppBundle:Product p
                JOIN p.category c
                WHERE p.id = :id'
            )->setParameter('id', $id);

        try {
            return $query->getSingleResult();
        } catch (\Doctrine\ORM\NoResultException $e) {
            return null;
        }
    }


Teraz możemy korzystać z tej metody w kontrolerze, aby pytać o obiekt ``Product``
i powiązany z nim obiekt ``Category``::


    public function showAction($id)
    {
        $product = $this->getDoctrine()
            ->getRepository('AppBundle:Product')
            ->findOneByIdJoinedToCategory($id);

        $category = $product->getCategory();

        // ...
    }


Więcej informacji o powiązaniach
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Rozdział ten jest wprowadzeniem do popularnego typu relacji encji, *jeden do wielu*.
Więcej zaawansowanych szczegółów i przykładów tego, jak używać inne typy relacji
(czyli  *jeden do jeden*, *wiele do wielu*) znajdziesz w części dokumentacji
Doctrine `Association Mapping`_.

.. note::

    Jeżeli używa się adnotacji, to trzeba poprzedzać wszystkie adnotacje przedrostkiem
    ``ORM\`` (np. ``ORM\OneToMany``), co nie zostało uwzględnione w dokumentacji
    Doctrine. Należy również dołączyć wyrażenie use ``Doctrine\ORM\Mapping as ORM;``,
    które importuje przedrostki adnotacji ORM.

Konfiguracja
------------

Doctrine jest wysoce konfigurowalna, ale prawdopodobnie nigdy nie trzeba będzie
martwić się o większość opcji konfiguracyjnych tej biblioteki. Aby dowiedzieć się
więcej o konfiguracji Doctrine, proszę przeczytać rozdział
:doc:`Informator konfiguracji </reference/configuration/doctrine>` w dokumentacji Doctrine.

Wywołania zwrotne cyklu życia encji
-----------------------------------

Czasem zachodzi potrzeba wykonania akcji zaraz przed lub po dodaniu,
zaktualizowaniu lub usunięciu encji. Tego typu akcje są nazywane **wywołaniami
zwrotnymi "cyklu życia" encji**, jako że są one metodami wywołań zwrotnych, które
trzeba wykonać na różnych etapach istnienia encji (tj. gdy encja jest dodawana,
aktualizowana, usuwana itd.).

Jeżeli używa się adnotacji dla określenia metadanych, należy rozpocząć od udostępnienia
wywołań zwrotnych cyklu życia. Nie jest to konieczne, jeśli stosuje się YAML lub XML
do odwzorowywania:

.. code-block:: php-annotations
   :linenos:

    /**
     * @ORM\Entity()
     * @ORM\HasLifecycleCallbacks()
     */
    class Product
    {
        // ...
    }

Teraz możemy powiadomić Doctrine aby wykonała metodę na każdym dostępnym zdarzeniu
w cyklu funkcjonowania encji. Przykładowo załóżmy że, chcemy ustawić utworzoną
kolumnę datową na bieżącą datę, ale tylko wtedy, gdy encja jest pierwszy raz utrwalana
(tj. dołożona):

.. configuration-block::

    .. code-block:: php-annotations
       :linenos:

        // src/AppBundle/Entity/Product.php

        /**
         * @ORM\PrePersist
         */
        public function setCreatedAtValue()
        {
            $this->createdAt = new \DateTime();
        }

    .. code-block:: yaml
       :linenos:

        # src/AppBundle/Resources/config/doctrine/Product.orm.yml
        AppBundle\Entity\Product:
            type: entity
            # ...
            lifecycleCallbacks:
                prePersist: [setCreatedAtValue]

    .. code-block:: xml
       :linenos:

        <!-- src/AppBundle/Resources/config/doctrine/Product.orm.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <doctrine-mapping xmlns="http://doctrine-project.org/schemas/orm/doctrine-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://doctrine-project.org/schemas/orm/doctrine-mapping
                http://doctrine-project.org/schemas/orm/doctrine-mapping.xsd">

            <entity name="AppBundle\Entity\Product">
                <!-- ... -->
                <lifecycle-callbacks>
                    <lifecycle-callback type="prePersist" method="setCreatedAtValue" />
                </lifecycle-callbacks>
            </entity>
        </doctrine-mapping>

.. note::

    Powyższy przykład zakłada, że wcześniej utworzyliśmy i odwzorowali właściwość
    ``creates`` (czego tu nie pokazano).


Teraz, tuż przed pierwszym utrwaleniem encji, Doctrine automatycznie wywoła tą
metodę i ustawi pole ``created`` na bieżącą datę.

Istnieje kilka innych zdarzeń cyklu życia, które można podłączyć. Więcej ogólnych
informacji o zdarzeniach i wywołaniach zwrotnych cyklu życia można znaleźć
w artykule `Lifecycle Events`_ dokumentacji Doctrine.

.. sidebar:: Wywołania zwrotne cyklu życia i nasłuchiwanie zdarzeń

    Proszę zauważyć, że metoda ``setCreatedValue()`` nie przejmuje żadnych
    argumentów. Tak jest zawsze w przypadku wywołań zwrotnych cyklu życia encji
    i jest to zamierzone – wywołania zwrotne cyklu życia encji powinny być prostymi
    metodami, które dotyczą wewnętrznego przekształcania danych encji
    (np. ustawienie tworzenia lub aktualizowania pola, generowanie wartości slug).
    
    Jeśli zachodzi potrzeba wykonania bardziej zaawansowanego kodu - takiego jak
    obsługa logowania, czy wysyłania wiadomości e-mail, powinno się zarejestrować
    zewnętrzne klasy do nasłuchiwania lub subskrybcji zdarzeń i dać im dostęp do
    wszystkich potrzebnych zasobów. W celu uzyskania więcej informacji można
    sięgnąć do artykułu :doc:`How to Register Event Listeners and Subscribers
    </cookbook/doctrine/event_listeners_subscribers>`.


.. _book-doctrine-field-types:

Informacje o typach pól Doctrine
--------------------------------

Doctrine dostarczana jest z dużą liczbą dostępnych typów pól.
Każdy z nich odwzorowuje typ danych PHP na określony typ kolumny w bazie danych.
Dla każdego typu pola można dalej skonfigurować ``Column``, ustawiając zachowanie
``length``, ``nullable``, opcję ``name`` i inne opcje. Lista wszystkich dostępnych
typów jest dostępna w artykule `Mapping Types`_ dokumentacji Doctrine.

Podsumowanie
------------

Stosując Doctrine można skupić się na obiektach i na tym jak są one potrzebne
w aplikacji, nie martwiąc się o ich utrwalenie a bazie danych. Dzieje się tak,
bo Doctrine umożliwia używanie obiektów PHP do przechowywania danych i odwzorowuje
je do określonych tabel baz danych, wykorzystując informacje metadanych odwzorowania.

Pomimo, ze Doctrine działa wg. prostej koncepcji, to jest bardzo silną biblioteką,
umożliwiająca tworzenie złożonych zapytań i wykorzystywać zdarzenia, pozwalając
na wykonywanie różnych akcji na wszystkich etapach życia encji.

Więcej informacji o Doctrine znajduje w :doc:`cookbook</cookbook/index>`,
w artykułach:

Dalsza lektura
~~~~~~~~~~~~~~

Więcej informacji o Doctrine mozna znaleźć w rozdziale *Doctrine*
:doc:`cookbook </cookbook/index>`. Kilka przydatnych artykułów, to:

* :doc:`/cookbook/doctrine/common_extensions`
* :doc:`/cookbook/doctrine/console`
* `DoctrineFixturesBundle`_
* `DoctrineMongoDBBundle`_


.. _`Doctrine`: http://www.doctrine-project.org/
.. _`MongoDB`: https://www.mongodb.org/
.. _`Basic Mapping Documentation`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/basic-mapping.html
.. _`Query Builder`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/query-builder.html
.. _`Doctrine Query Language`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/dql-doctrine-query-language.html
.. _`Association Mapping`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/association-mapping.html
.. _`DateTime`: http://php.net/manual/en/class.datetime.php
.. _`Mapping Types`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/basic-mapping.html#doctrine-mapping-types
.. _`Property Mapping`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/basic-mapping.html#property-mapping
.. _`Lifecycle Events`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/events.html#lifecycle-events
.. _`Reserved SQL keywords`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/basic-mapping.html#quoting-reserved-words
.. _`Persistent classes`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/basic-mapping.html#persistent-classes
.. _`Creating Classes for the Database`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/basic-mapping.html#creating-classes-for-the-database
.. _`DoctrineMongoDBBundle`: https://symfony.com/doc/current/bundles/DoctrineMongoDBBundle/index.html
.. _`migracji`: https://symfony.com/doc/current/bundles/DoctrineMigrationsBundle/index.html
.. _`DoctrineFixturesBundle`: https://symfony.com/doc/current/bundles/DoctrineFixturesBundle/index.html
.. _`FrameworkExtraBundle documentation`: https://symfony.com/doc/current/bundles/SensioFrameworkExtraBundle/annotations/converters.html
.. _`nowszy zestaw znakowy utf8mb4`: https://dev.mysql.com/doc/refman/5.5/en/charset-unicode-utf8mb4.html
.. _`ORM`: https://pl.wikipedia.org/wiki/Mapowanie_obiektowo-relacyjne 
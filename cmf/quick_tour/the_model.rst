.. highlight:: php
   :linenothreshold: 2

.. index::
    single: model; wprowadzenie

Model
=====

W tej części kursu dowiesz się więcej o domyślnej warstwie magazynowania danych CMF.

.. note::

    Rozdział ten dotyczy warstwy magazynowania PHPCR, ale CMF może też działać
    z innymi systemami magazynowania - nie jest uzależniony od konkretnego
    systemu magazynowania danych.

Poznajmy PHPCR
--------------

PHPCR_ przechowuje wszystkie dane w strukturze wielkiego drzewa. Można to porównać
do systemu plików, w którym każdy plik i katalog zawierają dane. Oznacz to, że
wszystkie dane przechowywane w PHPCR maja relację z co najmniej jedną inną daną -
swoim rodzicem. Istnieje także odwrotna relacja – można również uzyskać dane podrzędne
jakiegoś elementu danych.

Przyjrzyjmy się na zrzut drzewa Standard Edition, jaki pobraliśmy w poprzednim rozdziale.
Idź do katalogu instalacji Symfony CMF SE i wykonaj następujące polecenie:

.. code-block:: bash

    $ php app/console doctrine:phpcr:node:dump

W wyniku otrzymasz drzewo PHPCR:

.. code-block:: text

    ROOT:
      cms:
        content:
        blocks:
            hero_unit:
            quick_tour:
            configure:
            demo:
        simple:
        home:
        demo:
        quick_tour:
        login:
        menu:

Każda dana nosi w PHPCR nazwę *węzła*. W tym drzewie jest 13 węzłów i węzeł ROOT
(utworzony przez PHPCR). Być może przypominasz sobie dokument, jaki został utworzony
w poprzednim rozdziale – nazywa się on ``quick_tour`` (i ma ścieżkę
``/cms/simple/quick_tour``).

Każdy węzeł ma właściwości, które zawierają dane. Ustawione dla strony takie dane
jak *content*, *title* czy *label* są zapisane w właściwościach dla węzła ``quick_tour``.
Można wyświetlić te właściwości dodając przełącznik ``--props`` do polecenia zrzucającego.

.. note::

    Wcześniej drzewo PHPCR zostało porównane do systemu plików. Chociaż daje to
    dobry obraz jego działania, nie jest to do końca prawda. Lepszym porównaniem
    może być plik XML, gdzie każdy węzeł jest elementem a właściwości są atrybutami.

Doctrine PHPCR-ODM
------------------

Symfony CMF używa `Doctrine PHPCR-ODM`_ do interakcji z PHPCR.
Doctrine pozwala użytkownikowi tworzyć obiekty (zwane *dokumentami*), które są
bezpośrednio przekazywane do drzewa PHPCR i stamtąd pobierane w razie potrzeby.
Jest to podobne do Doctrine ORM używanego w Symfony2 Framework, ale działa na PHPCR.

Tworzenie strony w kodzie
-------------------------

Teraz masz już trochę więcej wiedzy o PHPCR i znasz narzędzie do interakcji z tym
repozytorium, możesz używać go samodzielnie. W poprzednim rozdziale utworzyliśmy
stronę używając pliku yaml, który parsowany był przez SimpleCmsBundle.
Teraz przyszedł czas na samodzielne utworzenie strony.

Po pierwsze, musisz utworzyć nowy obiekt DataFixture i dodać go do nowej strony.
Zrobisz to, tworząc nową klasę AcmeDemoBundle::

    // src/Acme/DemoBundle/DataFixtures/PHPCR/LoadPageData.php
    namespace Acme\DemoBundle\DataFixtures\PHPCR;

    use Doctrine\Common\Persistence\ObjectManager;
    use Doctrine\Common\DataFixtures\FixtureInterface;
    use Doctrine\Common\DataFixtures\OrderedFixtureInterface;

    class LoadPageData implements FixtureInterface, OrderedFixtureInterface
    {
        public function getOrder()
        {
            // ustalenie priorytetu kolejnosci wywołania funkcja ładującej klasę
            // (niższe wartości maja wyższy priorytet)
            return 10;
        }

        public function load(ObjectManager $documentManager)
        {
        }
    }

``$documentManager`` jest obiektem, który będzie utrwalał dokument w PHPCR.
Lecza najpierw trzeba utworzyć nowy dokument strony::

    use Symfony\Cmf\Bundle\SimpleCmsBundle\Doctrine\Phpcr\Page;

    // ...
    public function load(ObjectManager $documentManager)
    {
        $page = new Page(); // tworzy nowy obiekt (dokument) Page
        $page->setName('new_page'); // nazwa węzła
        $page->setLabel('Another new Page');
        $page->setTitle('Another new Page');
        $page->setBody('I have added this page myself!');
    }

Każdy dokument musi mieć dokument nadrzędny. W naszym przypadku będzie to węzeł
główny. Należy więc najpierw pobrać dokument główny z PHPCR i następnie ustawić
go jako węzeł nadrzędny::

    // ...
    public function load(ObjectManager $documentManager)
    {
        // ...

        // pobranie dokumentu głownego (/cms/simple)
        $simpleCmsRoot = $documentManager->find(null, '/cms/simple');

        $page->setParentDocument($simpleCmsRoot); // ustawienie dokumentu nadrzędnego jako root
    }

Na koniec musimy poinformować menadżera dokumentów aby utrwalił nasz dokument strony
w repozytorium, używając API Doctrine::

    // ...
    public function load(ObjectManager $documentManager)
    {
        // ...
        $documentManager->persist($page); // dodanie strony do kolejki
        $documentManager->flush(); // dodanie strony do PHPCR
    }

Teraz musisz wykonać polecenie ``doctrine:phpcr:fixtures:load`` i następnie odwiedzić
jeszcze raz swoją witrynę. Zobaczysz tam, że dodana została nowa strona!

.. image:: ../_images/quick_tour/the-model-new-page.png

.. seealso::

    ":doc:`../book/database_layer`" jeśli chcesz dowiedzieć sie więcej o stosowaniu
    PHPCR w aplikacjach Symfony.

Wnioski końcowe
---------------

PHPCR jest pełnowartościowym sposobem magazynowania stron w CMS. Lecz jeśli ten
sposób Ci nie odpowiada, to możesz zastosować
:doc:`inną warstwę magazynowania danych <../cookbook/database/choosing_storage_layer>`.

Patrząc wstecz na te 20 minut, to nauczyliśmy się jak pracować z nową warstwą
magazynowania danych i dodaliśmy 2 nowe strony. Czy dostrzegasz jak łatwo edytować
aplikacje CMF? CMF dostarcza większość rzeczy, które programista musiałby zrobić
sam w innym systemie.

Lecz zobaczyliśmy na razie tylko mały skrawek CMF. Jest o wiele więcej do nauki,
nie tylko o samym CMF ale też o pakietach potrzebnych do zbudowania własnej aplikacji.
Przed Tobą lektura następnego rozdziału, traktującego o
:doc:`systemie trasowania w CMF <the_router>`.
Poświęcisz następne 10 minut na tą lekturę?

.. _PHPCR: http://phpcr.github.io/
.. _`Doctrine PHPCR-ODM`: http://docs.doctrine-project.org/projects/doctrine-phpcr-odm/en/latest/

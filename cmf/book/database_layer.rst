.. highlight:: php
   :linenothreshold: 2

.. index::
    single: warstwa bazy danych
    single: PHPCR-ODM

Warstwa bazy danych: PHPCR-ODM
==============================

Symfony CMF może współdziałać z różnymi warstwami magazynowania danych. Domyślnie
Symfony CMF działa z biblioteką `Doctrine PHPCR-ODM`_.
W tym rozdziale poznamy tego szczegóły.

.. tip::

    Więcej o wyborze właściwej warstwy magazynowania danych można się dowiedziec
    w :doc:`../cookbook/database/choosing_storage_layer`

.. note::

   W tym rozdziale zakłada się, że stosujesz instalację Symfony z już skonfigurowanym
   PHPCR-ODM, tak jak to wyjaśniono w :doc:`poradniku instalacyjnym CMF Standard
   Edition <installation>` lub w :doc:`poradniku piaskownicy CMF
   <../cookbook/editions/cmf_sandbox>`.
   Przeczytaj :doc:`../bundles/phpcr_odm/introduction` w celu dowiedzenia się,
   jak skonfigurować PHPCR-ODM w swojej aplikacji.

PHPCR: struktura drzewa
-----------------------

Doctrine PHPCR-ODM jest biblioteką mapowania obiektowo-dokumentacyjnego działającą
na szczycie `PHP Content Repository`_ (PHPCR), która jest adaptacją do PHP 
`specyfikacji JSR-283`_. Najważniejsza cechą PHPCR jest struktura drzewa magazynowanych
danych. Wszystkie dane są przechowywane w elementach drzewa, zwanych węzłami. Można
myśleć o tym, jak o systemie plików, co czyni to idealnym rozwiązaniem dla CMS.

Na szczycie struktury drzewiastej PHPCR dodaje również funkcje takie jak wyszukiwanie,
kontrola wersji i kontrola dostępu.

Doctrine PHPCR-ODM ma ten sam interfejs API co inne biblioteki Doctrine, takie jak
`Doctrine ORM`_. Doctrine PHPCR-ODM dodaje jeszcze inną istotna funkcję do PHPCR:
obsługę wielojęzyczności.

.. sidebar:: Implementacje PHPCR

    W celu umożliwienia Doctrine PHPCR-ODM komunikacji z PHPCR niezbedne jest
    zaimplementowanie PHPCR. Przeczytaj
    ":doc:`../cookbook/database/choosing_phpcr_implementation`" w celu przeglądu
    dostępnych implementacji..

Prosty przyklad: Task
---------------------

Najprostszym sposobem rozpoczęcia pracy z biblioteka PHPCR-ODM jest zobaczenie
jej w akcji. W tym rozdziale utworzymy obiekt ``Task`` i nauczymy się jak go
utrwalić.

Utworzenie klasy dokumentu
~~~~~~~~~~~~~~~~~~~~~~~~~~

Można utworzyć w PHP obiekt ``Task``, bez myślenia o Doctrine lub PHPCR-ODM::

    // src/Acme/TaskBundle/Document/Task.php
    namespace Acme\TaskBundle\Document;

    class Task
    {
        protected $description;

        protected $done = false;
    }

Klasa ta, często nazywana w PHPCR-ODM "dokumentem", oznacza *podstawową klasę,
która posiada dane*. Jest prosta i pomaga spełnić wymagania biznesowe potrzebnych
w aplikacji zadań. Klasa ta nie może być jeszcze utrwalona w Doctrine PHPCR-ODM –
to tylko prosta klasa PHP.

.. note::

   Pojecie "dokumentu" jest analogiczne do pojęcia "encji" używanego w Doctrine
   ORM. Można dodać ten obiekt do ``Document`` podprzestrzeni nazw pakietu,
   w celu automatycznego zarejestrowania danych mapowania. 

Dodanie informacji mapowania
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Doctrine pozwala na pracę z PHPCR w bardziej interesujący sposób, niż tylko
pobieranie na okrągło danych jako tablicy. Zamiast tego Doctrine pozwala utrwalać
całe obiekty w PHPCR i pobierać całe obiekty z PHPCR.
Działa to przez odwzorowywanie klasy PHP i jej właściwości na drzewo PHPCR.

Dla funkcjonowania Doctrine konieczne jest utworzenie "metadanych" lub konfiguracji,
która informuje jawnie Doctrine w jaki sposób należy odwzorować do PHPCR dokument
``Task`` i jego właściwości. Te metadane mogą być określone w kilku różnych formatach,
w tym w YAML, XML lub bezpośrednio w klasie ``Task`` przy pomocy adnotacji:

.. configuration-block::

    .. code-block:: php-annotations
       :linenos:

        // src/Acme/TaskBundle/Document/Task.php
        namespace Acme\TaskBundle\Document;

        use Doctrine\ODM\PHPCR\Mapping\Annotations as PHPCR;

        /**
         * @PHPCR\Document()
         */
        class Task
        {
            /**
             * @PHPCR\Id()
             */
            protected $id;

            /**
             * @PHPCR\String()
             */
            protected $description;

            /**
             * @PHPCR\Boolean()
             */
            protected $done = false;

            /**
             * @PHPCR\ParentDocument()
             */
            protected $parentDocument;
        }

    .. code-block:: yaml
       :linenos:

        # src/Acme/TaskBundle/Resources/config/doctrine/Task.odm.yml
        Acme\TaskBundle\Document\Task:
            id: id

            fields:
                description: string
                done: boolean

            parent_document: parentDocument

    .. code-block:: xml
       :linenos:

        <!-- src/Acme/TaskBundle/Resources/config/doctrine/Task.odm.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <doctrine-mapping
            xmlns="http://doctrine-project.org/schemas/phpcr-odm/phpcr-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://doctrine-project.org/schemas/phpcr-odm/phpcr-mapping
            https://github.com/doctrine/phpcr-odm/raw/master/doctrine-phpcr-odm-mapping.xsd"
            >

            <document name="Acme\TaskBundle\Document\Task">

                <id name="id" />

                <field name="description" type="string" />
                <field name="done" type="boolean" />

                <parent-document name="parentDocument" />
            </document>

        </doctrine-mapping>

Po tym trzeba utworzyć metody getters i setters dla własciwosci.

.. note::

   Ten dokument używa nazwy dokumentu nadrzędnego i węzła do określenia swojej
   pozycji w drzewie. Ponieważ nie ma ustawionej żadnej nazwy, zostaje ona
   ustawiona automatycznie. Jeśli chce się użyć określonej nazwy węzła, takiej
   jak wersja sluggified tytułu, należy dodać odwzorowaną właściwość jako ``Nodename``.
   
   Dokument musi posiadać właściwość ``id``. Reprezentuje ona pełną ścieżkę
   (ścieżka nadrzędna + nazwa) dokumentu. Zostanie to domyślnie ustawione przez
   Doctrine. Nie zaleca się wykorzystywania tego identyfikatora do określania
   lokalizacji dokumentu.
   
   W celu uzyskania więcej informacji o strategiach generowania identyfikatora,
   prosimy przeczytać `dokumentacją Doctrine`_

.. tip::
   
   Można zaimplementować ``Doctrine\ODM\PHPCR\HierarchyInterface``, co sprawi,
   że możliwe będzie, na przykład, wykorzystanie :ref:`rozszerzenia Child Sonata
   Admin <bundle-core-child-admin-extension>`.

.. seealso::

   Można również sprawdzić `dokumentację podstaw mapowania`_ w celu poznania
   szczegółowej informacji z zakresu mapowania. Jeśli wykorzystuje się adnotacje,
   to trzeba poprzedzać wszystkie adnotacje przedrostkiem ``@PHPCR\``, który jest
   nazwą zaimportowanej przestrzeni nazw (np. ``@PHPCR\Document(..)``) - nie jest
   to omówione w dokumentacji Doctrine. Należy również dołączyć wyrażenie
   ``use Doctrine\ODM\PHPCR\Mapping\Annotations as PHPCR;`` w celu zaimportowania
   przedrostka adnotacji PHPCR.

Utrwalanie dokumentów w PHPCR
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Teraz gdy już mamy odwzorowany dokument ``Task``, z kompletem metod getter i setter,
możemy przystąpić do utrwalenia danych w PHPCR. Wykonanie tego z poziomu kontrolera
jest bardzo proste, wystarczy dodać następującą metodę do ``DefaultController``
pakietu AcmeTaskBundle::

    // src/Acme/TaskBundle/Controller/DefaultController.php

    // ...
    use Acme\TaskBundle\Document\Task;
    use Symfony\Component\HttpFoundation\Response;

    // ...
    public function createAction()
    {
        $documentManager = $this->get('doctrine_phpcr')->getManager();

        $rootTask = $documentManager->find(null, '/tasks');

        $task = new Task();
        $task->setDescription('Finish CMF project');
        $task->setParentDocument($rootTask);

        $documentManager->persist($task);

        $documentManager->flush();

        return new Response('Created task "'.$task->getDescription().'"');
    }

Przyjrzyjmy się powyższemu przykładowi bardziej szczegółowo:

* **linia 10** Linia ta pobiera obiekt *document manager* Doctrine, który jest
  odpowiedzialny za obsługę procesu utrwalania obiektów w PHPCR i pobierania ich
  z PHPCR.
* **linia 12** W tej linii pobierany jest dokument główny dla zadań – jak każdy
  dokument,dokument zadania musi mieć element nadrzędny. W celu utworzenia tego dokumentu
  głównego, można skonfigurować :ref:`inicjalizator repozytorium <phpcr-odm-repository-initializers>`,
  który zostanie wykonany podczas uruchomienia ``doctrine:phpcr:repository:init``.
* **linie 14-16** W tej sekcji utworzona została instancja i wykonywana jest praca
  z obiektem ``$task``, tak jak ze zwykłym obiektem PHP.
* **linia 18** Metoda ``persist()`` powiadamia Doctrine aby "zarządzał" obiektem
  ``$task``. To w rzeczywistości jeszcze nie wykonuje zapytania do PHPCR.
* **linia 20** Po wywołaniu metody ``flush()``, Doctrine przegląda wszystkie obiekty
  którymi zarządza, aby sprawdzić, czy nie muszą one zostać utrwalone w PHPCR. W tym
  przykładzie, obiekt ``$task`` nie został jeszcze utrwalony, tak więc menadżer dokumentu
  dokonuje zapytania do PHPCR, które doda nowy dokument.

Przepływ kodu podczas tworzenia lub aktualizowania obiektów jest zawsze taki sam.
W następnym rozdziale zobaczymy jak Doctrine jest wystarczająco inteligentna aby
zaktualizować dokumenty, jeśli one już istnieją w PHPCR.

Pobieranie obiektów z PHPCR
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Pobieranie obiektu z PHPCR jest jeszcze łatwiejsze. Załóżmy, że jest skonfigurowana
trasa dla wyświetlania przez nazwę określonego zadania::

    public function showAction($name)
    {
        $repository = $this->get('doctrine_phpcr')->getRepository('AcmeTaskBundle:Task');
        $task = $repository->find('/task/'.$name);

        if (!$task) {
            throw $this->createNotFoundException('No task found with name '.$name);
        }

        return new Response('['.($task->isDone() ? 'x' : ' ').'] '.$task->getDescription());
    }

Do pobierania obiektów z repozytorium dokumentów używa się zarówno metod ``find``
jak i ``findMany`` i wszystkich metod pomocniczych z klas specyficznych dla repozytorium.
W PHPCR, często programista nie wie, czy węzeł ma dane dla określonego dokumentu.
W takim przypadku należy użyć menadżera dokumentów do odnalezienia węzłów (na przykład,
gdy chce się pobrać dokument główny). W powyższym przykładzie wiemy, że są to dokumenty
``Task`` i dzięki temu możemy wykorzystać repozytorium.

Repozytorium zawiera różnego rodzaju przydatne metody::

    // query by the id (full path)
    $task = $repository->find($id);

    // query for one task matching be name and done
    $task = $repository->findOneBy(array('name' => 'foo', 'done' => false));

    // query for all tasks matching the name, ordered by done
    $tasks = $repository->findBy(
        array('name' => 'foo'),
        array('done' => 'ASC')
    );

.. tip::

    Jeśli używa się klasy repozytorium, można również utworzyć własne repozytorium
    dla określonego dokumentu. Pomaga to na "podział kompetencji" podczas stosowania
    bardziej złożonych zapytań. Jest to podobne do tego, co robi się w Doctrine ORM.
    W celu uzyskania więcej informacji proszę przeczytać
    "`Niestandardowe klasy repozytorium`_" w dokumentacji rdzenia.

.. tip::

    Można również wypytać obiekty używając Query Builder, dostarczony przez Doctrine
    PHPCR-ODM. Więcej informacji można znaleźć w
    `dokumentację QueryBuilder`_.

Aktualizowanie obiektu
~~~~~~~~~~~~~~~~~~~~~~

Po pobraniu obiektu z Doctrine, jego aktualizacja jest łatwa. Załóżmy, że mamy trasę,
która odwzorowuje identyfikator zadania do zaktualizowanej akcji w kontrolerze::

    public function updateAction($name)
    {
        $documentManager = $this->get('doctrine_phpcr')->getManager();
        $repository = $documentManager->getRepository('AcmeTaskBundle:Task');
        $task = $repository->find('/tasks/'.$name);

        if (!$task) {
            throw $this->createNotFoundException('No task found for name '.$name);
        }

        if (!$task->isDone()) {
            $task->setDone(true);
        }

        $documentManager->flush();

        return new Response('[x] '.$task->getDescription());
    }

Aktualizacja obiektu obejmuje tylko trzy kroki:

#. pobranie obiektu z Doctrine;
#. poprawienie obiektu;
#. wywołanie metody ``flush()`` w menadżerze dokumentów.

Proszę zwrócić uwagę, że nie jest konieczne wywołanie``$documentManger->persist($task)``.
Przypomnijmy, że metoda ta po prostu powiadamia Doctrine, aby zarządzała lub "obserwowała"
obiekt ``$task``. W tym przypadku, ponieważ pobrano z Doctrine obiekt ``$task``,
to już się stało.

Usuwanie obiektu
~~~~~~~~~~~~~~~~

Usuwanie obiektów jest bardzo podobne, ale wymaga wywołania metody ``remove()``
menadżera dokumentów po pobraniu dokumentu z PHPCR::

    $documentManager->remove($task);
    $documentManager->flush();

Jak można się spodziewać, metoda ``remove()`` powiadamia Doctrine, że chce się
usunąć określony dokument z PHPCR. Faktyczna operacja usunięcia jest wykonywana
dopiero po wywołaniu metody ``flush()``.

Podsumowanie
------------

Stosując Doctrine, można skupić się na własnych obiektach i nad tym jak są one
przydatne w aplikacji, nie martwiąc się problemami utrwalania danych w bazie danych.
Wynika to z faktu, że Doctrine pozwala wykorzystywać każdy obiekt PHP do przechowywania
danych i opierając się na informacji metadanych mapowania odwzorowuje dane obiektu
w określonej tabeli bazy danych.

Mimo, ze Doctrine działa zgodnie z prostą koncepcją, jest bardzo zaawansowaną
biblioteka, pozwalająca na `tworzenie złożonych zapytań`_ i
:doc:`subskrybowanie zdarzeń <../bundles/phpcr_odm/events>`, które umożliwiają
podejmowanie różnych akcji na dokumentach, realizując cały cykl ich utrzymania
w repozytorium.

.. _`Doctrine PHPCR-ODM`: http://docs.doctrine-project.org/projects/doctrine-phpcr-odm/en/latest/index.html
.. _`PHP Content Repository`: http://phpcr.github.io/
.. _`specyfikacji JSR-283`: http://jcp.org/en/jsr/detail?id=283
.. _`Doctrine ORM`: http://symfony.com/doc/current/book/doctrine.html
.. _`dokumentacją Doctrine`: http://docs.doctrine-project.org/projects/doctrine-phpcr-odm/en/latest/reference/basic-mapping.html#basicmapping-identifier-generation-strategies
.. _`dokumentację podstaw mapowania`: http://docs.doctrine-project.org/projects/doctrine-phpcr-odm/en/latest/reference/annotations-reference.html
.. _`dokumentację QueryBuilder`: http://docs.doctrine-project.org/projects/doctrine-phpcr-odm/en/latest/reference/query-builder.html
.. _`tworzenie złożonych zapytań`: http://docs.doctrine-project.org/projects/doctrine-phpcr-odm/en/latest/reference/query-builder.html
.. _`Niestandardowe klasy repozytorium`: http://symfony.com/doc/current/book/doctrine.html#custom-repository-classes

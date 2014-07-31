.. index::
    single: Przegląd

Przegląd
========

Rozpocznijmy używanie Symfony CMF w 10 minut! W tym rozdziale zapoznasz się z podstawowymi
koncepcjami Symfony CMF i pomożemy Ci w rozpoczęciu pracy.

Symfony CMF jest kolekcją pakietów, które zapewniają wspólną funkcjonalność potrzebną
do budowy CMS na bazie frameworka Symfony. Przed dalszą lekturą, powinno się zaznajomić
z podstawową wiedzą o frameworku Symfony. Jeśli nie znasz Symfony, przecztaj
:doc:`Krótki kurs Symfony2 </quick_tour/index>`.

Dylemat framework versus CMS
----------------------------

Przed rozpoczęciem nowego projektu trzeba podjąć trudną decyzję o tym, czy projekt
wykorzystywać będzie framework czy CMS. Wybierając framework, trzeba poświęcić dużo
czasu na stworzenie funkcjonalności zarządzania treścią. Z drugiej strony, po wyborze
CMS, trudniej będzie budować własne funkcjonalności aplikacji. Niemożliwe albo bardzo
trudne jest dostosowanie rdzenia CMS do własnych potrzeb.

CMF jest stworzony po to aby rozwiązać dylemat wyboru pomiędzy frameworkiem a CMS.
CMF dostarcza pakiety, tak więc bardzo łatwo jest dodawać funkcje CMS do swojego
projektu. Dodatkowo oferuje elastyczność i we wszystkich przypadkach wykorzystuje
framework, dzięki czemu można budować własne funkcjonalności, w sposób jaki się chce.
To nazywa się `rozdzielonym CMS`_.

Pakiety dostarczane przez Symfony CMF mogą pracować razem, ale również mogą działać
samodzielnie. Oznacza to, że nie trzeba dodawać wszystkich pakietów. Można dokonać
wyboru tylko tych potrzebnych (np. tylko RoutingBundle lub MediaBundle).

Pobieranie Symfony CMF Standard Edition
---------------------------------------

Gdy chce się rozpocząć nowy projekt z wykorzystaniem CMF można pobrać Symfony CMF
Standard Edition. Symfony CMF Standard Edition jest podobny do `Symfony Standard Edition`_,
ale zawiera i konfiguruje istotne pakiety Symfony CMF. Dodaje również bardzo prosty
pakiet umożliwiający zobaczyć podstawowe funkcje Symfony CMF.

Najprostszym sposobem pobrania Symfony CMF Standard Edition jest wykorzystanie
`programu Composer`_:

.. code-block:: bash

    $ php composer.phar create-project symfony-cmf/standard-edition cmf ~1.1

Konfiguracja bazy danych
~~~~~~~~~~~~~~~~~~~~~~~~

Teraz pozostaje tylko skonfigurowanie bazy danych. Nie jest to baza danych przeznaczona
dla aplikacji Symfony, lecz dla konfiguracji paru rzeczy przy użyciu interfejsu administratora.

Zakładamy, że masz włączone rozszerzenie sqlite. Uruchom polecenia:

.. code-block:: bash

    $ php app/console doctrine:database:create
    $ php app/console doctrine:phpcr:init:dbal
    $ php app/console doctrine:phpcr:repository:init
    $ php app/console doctrine:phpcr:fixtures:load

.. tip::

    Więcej informacji o warstwie dostępu do bazy danych Symfony CMF znajdziesz
    :doc:`w następnym rozdziale tego kursu <the_model>`.

.. seealso::

    Kompletny poradnik instalacji znajduje się w rozdziale ":doc:`../book/installation`"
    książki.

Przepływ żądania HTTP
---------------------

.. tip::

    Jeśli masz co najmniej wersję PHP 5.4, użyj polecenia ``server:run`` aby uruchomić
    lokalny serwer w celu demonstracji. W przeciwnym razie trzeba użyć localhost
    i poprzedzić adresy URL dokumentów ścieżką ``/path-to-project/web/app_dev.php/``.

Teraz Standard Edition jest gotowy do użycia. Nawiguj do strony domowej
(``http://localhost:8000/``) aby zobaczyc demo:

.. image:: ../_images/quick_tour/big-picture-home.png

Jak widzisz, mamy już kompletna stronę internetowa w naszym demo. Przyjrzyjmy się
bliżej przepływowi żądania HTTP w aplikacji Symfony CMF:

.. image:: ../_images/quick_tour/request_flow.png

Przede wszystkim widzimy na tej ilustracji typowy dla Symfony przepływ żądania
złożony z białych bloków. Tworzony jest obiekt ``Request``, który przekazywany
jest do routera. Tam wykonywany jest kod kontrolera, który wykorzystuje modele
do wygenerowania widoku umieszczanego w odpowiedzi.

Na ilustracji widać też, że w CMF dodano nowe elementy przetwarzania zaznaczone
tu na zielono. W następnych rozdziałach dowiemy się o tym więcej.

Model
~~~~~

Przed utworzeniem CMF zespół dokonał wiele badań dotyczących wykorzystania bibliotek
warstwy dostępu do baz danych. Skończyło się wyborem JCR_, repozytorium treści dla
Java. Razem z innymi programistami została utworzona biblioteka PHPCR_, port PHP
specyfikacji JCR.

PHPCR wykorzystuje strukturę opartą na drzewie. Przechowuje ona elementy w wielkim
drzewie. Elementy mają elementy nadrzędne i mogą mieć elementy potomne.

.. note::

   Chociaż PHPCR jest pierwotnym wyborem zespołu CMF, pakiety nie są przypisane
   do konkretnego systemu magazynowania. Niektóre pakiety zapewniają integrację
   z ORM i łatwo można dodawać własne modele.

Router
~~~~~~

W Symfony trasy są przechowywane w pliku konfiguracyjnym. Oznacza to, że tylko
programista może zmienić trasy. W CMS można chcieć, aby to administrator mógł
zmieniać trasy w swojej witrynie. Dlatego w Symfony CMF wprowadzono DynamicRouter.

DynamicRouter ładuje z bazy danych kilka tras, które prawdopodobnie odpowiadają
żądaniu i następnie starają się znaleźć dokładne dopasowanie. Trasy w bazie danych
można edytować, usuwać i tworzyć następne, wykorzystując interfejs administracyjny,
więc wszystko jest pod kontrola administratora.

Ponieważ można chcieć mieć również inne routery, takie jak zwykłe routery Symfony,
CMF również udostępnia ``ChainRouter``. Router łańcuchowy zawiera łańcuch innych
routerów i wykonuje je w określonej kolejności w celu znalezienia dopasowania.

Używając bazy danych do przechowywania tras, daje się również możliwość odwoływania
się do innych dokumentów z trasy. Oznacza to, że trasa może mieć obiekt Content.

.. note::

    Dowiesz się więcej o routerze :doc:`w dalszej części kursu <the_router>`.

Kontroler
~~~~~~~~~

Podczas dopasowywania trasy wykonywany jest kontroler. Kontroler zwykle tylko pobiera
obiekt Content z trasy i renderuje go. Ponieważ jest on zawsze taki sam, CMF wykorzystuje
ogólny kontroler. Może on być zastąpiony przez ustawienie konkretnego kontrolera
dla trasy lub obiektu Content.

Widok
~~~~~

Korzystając z RoutingBundle można skonfigurować obiekty Content, tak aby  były
renderowane przez określony szablon lub kontroler. Kontroler ogólny będzie następnie
renderował ten szablon.

Zobacz również jak stosuje się obiekt Menu, dostarczany przez KnpMenuBundle i jak
można go zintegrować z biblioteka Create.js dla edytowania dokumentów na żywo.

Dodawanie nowej strony
----------------------

Teraz już wiesz, że przepływ żądania może rozpocząć się od dodania nowej strony.
W Symfony CMF Standard Edition dane są przechowywane w plikach danych, które są
ładowane podczas wykonywania polecenia ``doctrine:phpcr:fixtures:load``. Aby dodać
nową stronę, wystarczy edytować taki plik, który znajduje się w katalogu
``src/Acme/DemoBundle/Resources/data``:

.. code-block:: yaml

    # src/Acme/MainBundle/Resources/data/pages.yml
    Symfony\Cmf\Bundle\SimpleCmsBundle\Doctrine\Phpcr\Page:
        # ...

        quick_tour:
            id: /cms/simple/quick_tour
            label: "Quick Tour"
            title: "Reading the Quick Tour"
            body: "I've added this page while reading the quick tour"

Następnie trzeba uruchomić ``doctrine:phpcr:fixtures:load`` w celu odzwierciedlenia
zmian w bazie danych. Po odświeżeniu, będzie można zobaczyć nowa stronę.

.. image:: ../_images/quick_tour/big-picture-new-page.png

Edytowanie na żywo
------------------

Teraz przyszedł czas aby stanąć w roli administratora witryny i edytować treść
przy użyciu interfejs sieciowego. Aby to zrobić, kliknij "Admin Login" i zastosuj
przydzielone uprawnienia.

Zobaczysz, że na stronie pojawił sie dodatkowy górny pasek:

.. image:: ../_images/quick_tour/big-picture-createjs-bar.png

Pasek ten jest zostaje wygenerowany przez bibliotekę `Create.js`_. Symfony CMF
integruje biblioteki CreatePHP_ i `Create.js`_ przy użyciu CreateBundle. Umożliwia
to edytowanie strony przy użyciu edytora WYSIWYG.

Teraz możesz zmienić treść nowej strony używając Create.js:

.. image:: ../_images/quick_tour/big-picture-edit-page.png

Po kliknięciu "save", zmiany zostaną zapisane przy użyciu CreateBundle i treść
będzie zaktualizowana.

Wnioski końcowe
---------------

Dotarliśmy do końca wprowadzenia do Symfony CMF. Jest jeszcze dużo więcej poznania,
ale już można było zobaczyć, jak Symfony CMF stara się ułatwić życie programiście
dostarczając kilka pakietów CMS. Jeśli chcesz kontynuować naukę, zapoznaj się z
następnym rozdziałem: ":doc:`the_model`".


.. seealso::
      
   * :doc:`/cmf/quick_tour/the_model`
   * :doc:`/cmf/quick_tour/the_router`
   * :doc:`/cmf/quick_tour/the_third_party_bundles`

.. _`rozdzielonym CMS`: http://decoupledcms.org
.. _`Symfony Standard Edition`: https://github.com/symfony/symfony-standard
.. _JCR: http://en.wikipedia.org/wiki/Content_repository_API_for_Java
.. _PHPCR: http://phpcr.github.io/
.. _KnpMenuBundle: http://knpbundles.com/KnpLabs/KnpMenuBundle
.. _`programu Composer`: http://getcomposer.org/
.. _`Create.js`: http://createjs.org/
.. _CreatePHP: http://demo.createphp.org/

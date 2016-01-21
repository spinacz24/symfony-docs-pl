.. index::
   double: tworzenie projektu; najlepsze praktyki

Tworzenie projektu
==================

Instalowanie Symfony
--------------------

W przeszłości, projekty Symfony tworzyło sie przy użyciu `Composer`_, menadżera
zależności dla aplikacji PHP. Obecnie zaleca się użycie **Symfony Installer**,
który trzeba zainstalować przed utorzeniem pierwszego projektu.

.. tip::

    Do tworzenia nowych projektów opartych na Symfony używaj instalatora Symfony.

Jak to zrobić? Przeczytaj :doc:`rozdział o instalacji </book/installation>`
w podręczniku Symfony.

.. _linux-and-mac-os-x-systems:
.. _windows-systems:

Tworzenie aplikacji blogu
-------------------------

Teraz, gdy wszystko jest prawidłowo skonfigurowane, można utworzyć nowy projekt
oparty na Symfony. W konsoli poleceń przechodzimy do katalogu projektów, w którym
mamy uprawnienia do tworzenia i wykonywania plików (nadrzędny katalog naszego
projektu Symfony) i wykonujemy następujące polecenia:

.. code-block:: bash

    $ cd projects/
    $ symfony new blog

    # Windows
    c:\> cd projects/
    c:\projects\> php symfony.phar new blog

Polecenie to tworzy nowy katalog o nazwie ``blog``, który zawiera świeży nowy
projekt oparty na najnowszej dostępnej stabilnej wersji Symfony. Dodatkowo,
instalator sprawdza, czy system spełnia wymagania techniczne do obsługi aplikacji
Symfony. Jeśli nie, zobaczy się na ekranie listę zmian koniecznych do przeprowadzenia.

.. tip::

    Wydania Symfony są podpisywane cyfrowo ze względów bezpieczeństwa. Jeśli chcesz
    sprawdzić integralność swojej instalacji Symfony, zobacz
    `sumę sprawdzającą`_ i wykonaj czynności opisane w tej `procedurze`_, w celu
    zweryfikowania podpisów.

Strukturyzowanie aplikacji
--------------------------

Po utworzenieu aplikacji, otwórz katalogo ``blog/`` a zobaczysz pewną liczbę
automatycznie wygenerowanych plików i katalogów:

.. code-block:: text

    blog/
    ├─ app/
    │  ├─ config/
    │  └─ Resources/
    ├─ bin
    │  └─ console
    ├─ src/
    │  └─ AppBundle/
    ├─ var/
    │  ├─ cache/
    │  ├─ logs/
    │  └─ sessions/
    ├─ tests/
    │  └─ AppBundle/
    ├─ vendor/
    └─ web/

Ta hierarchia plików i katalogów jest konwencją przyjetą w Symfony w celu
ustrukturyzowania aplikacji. Zalecane przeznaczenie poszczególnych katalogów
jest następujące:

* ``app/config/``: przchowuje wszystkie konfiguracje zdefiniowane dla każdego środowiska;
* ``app/Resources/``: przechowuje wszystkie szablony i pliki tłumaczeń aplikacji;
* ``src/AppBundle/``: przechowuje specyficzny kod Symfony (kontrolery i trasy),
  kod domenowy (np. klasy Doctrine) i całą logikę biznesową;
* ``var/cache/``: przechowuje wszystkie pliki pamięci podręcznej wygenerowane w aplikacji;
* ``var/logs/``: przechowuje wszystkie pliki dziennikowe wygenerowane w aplikacji;
* ``var/sessions/``: przechowuje wszystkie pliki sesji wygenerowane w aplikacji;
* ``tests/AppBundle/``: przechowuje testy automatyczne (np. testy jednostkowe aplikacji.  
* ``vendor/``: jest to katalog, w którym Composer instaluje zależności aplikacji
  i nie powinno się nigdy zmieniać zawartości tego katalogu;
* ``web/``: Przechowuje wszystkie pliki kontrolera wejściowego (*ang. front controller*)
  i wszystkie aktywa internetowe, takie jak arkusze stylów, pliki JavaScript i obrazy.

Pakiety aplikacji
~~~~~~~~~~~~~~~~~

Po wydaniu Symfony 2.0, większość programistów naturalnie adaptowała sposób dzielenia
aplikacji na logiczne moduły, występujący w Symfony 1.x. Dlatego wiele aplikacji
Symfony wykorzystuje pakiety (*ang. bundles*) do dzielenia swojego kodu na logiczne
funkcjonalności: UserBundle, ProductBundle, InvoiceBundle itd.

Jednakże pakiet oznacza też coś, co można wielokrotnie zastosować jako samodzielny
fragment kodu. Jeśli UserBundle nie może zostać użyty *"taki jaki jest"* w innych
aplikacjach Symfony, to nie można go traktować jako pakietu Symfony. Ponadto, jeśli
InvoiceBundle zależy od ProductBundle, to traci sens uzywanie dwóch oddzielnych
pakietów.

.. tip::

    Dla logiki swojej aplikacji twórz tylko jeden pakiet o nazwie AppBundle.

Implementując pojedynczy pakiet AppBundle w swoim projekcie powoduje się, że kod
jest bardziej zwarty i łatwiejszy do zrozumienia. Począwszy od Symfony 2.6, oficjalna
dokumentacja Symfony używa dla takiego pakietu nazwę AppBundle.

.. tip::

    Nie poprzedzaj nazwy AppBundle przedrostkiem wskazującym na dostawcę
    (np. AcmeAppBundle), ponieważ ten pakiet aplikacji nigdy nie będzie używany
    w innych aplikacjach, Tak więc nazwa AppBundle bedzie zawsze unikatowa w ramach
    danej aplikacji i nie bedzie powodować konfliktu nazewniczego.
    
.. note::
    
    Inny powód do tworzenia nowego pakietu dostarcza sytuacja, gdy przesłaniamy
    coś w pakiecie dostawcy (np. kontroler). Czytaj :doc:`/cookbook/bundles/inheritance`.

Podsumowując, oto typowa struktura katalogowa aplikacji Symfony, która spełnia 
najlepsze praktyki Symfony:

.. code-block:: text

    blog/
    ├─ app/
    │  ├─ config/
    │  └─ Resources/
    ├─ bin/
    │  └─ console
    ├─ src/
    │  └─ AppBundle/
    ├─ tests/
    │  └─ AppBundle/
    ├─ var/
    │  ├─ cache/
    │  ├─ logs/
       └─ sessions/
    ├─ vendor/
    └─ web/
       ├─ app.php
       └─ app_dev.php

.. tip::

    Jeśli instalacja Symfony nie jest dostarczana ze wstępnie wygenerowanym
    pakietem AppBundle, można go wygenerować samemu, wykonując następujące
    polecenie konsolowe:

    .. code-block:: bash

        $ php bin/console generate:bundle --namespace=AppBundle --dir=src --format=annotation --no-interaction

Rozszerzanie struktury katalogowej
----------------------------------

Jeśli projekt lub infrastruktura wymaga pewnych zmian w domyślnej strukturze
katalogowej, to można
:doc:`nadpisać lokalizację głównych katalogów </cookbook/configuration/override_dir_structure>`:
``cache/``, ``logs/`` i ``web/``.

Zmiany te są dość powierzchowne, ale teraz zalecamy, aby używać opisaną wcześniej
strukturę katalogową Symfony.

.. _`Composer`: https://getcomposer.org/
.. _`Get Started`: https://getcomposer.org/doc/00-intro.md
.. _`Composer download page`: https://getcomposer.org/download/
.. _`sumę sprawdzajacą`: https://github.com/sensiolabs/checksums
.. _`procedurze`: http://fabien.potencier.org/signing-project-releases.html

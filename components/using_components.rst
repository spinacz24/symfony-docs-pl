.. index::
   single: Components; Installation
   single: Components; Usage

.. _how-to-install-and-use-the-symfony2-components:

Jak zainstalować i stosować komponety Symfony
=============================================

Jeśli rozpoczyna się nowy projekt (lub już ma się rozpczęty projekt). który wykorzystuje
jeden lub więcej komponentów, to najłatwiejszym sposobem zintegrowania wszystkiego
jest `Composer`_.
Composer jest na tyle inteligentny, że umożliwia pobranie komponentów, które są
potrzebne i dba o automatyczne ładowanie komponentów tak, że można użyć tych
bibliotek natychmiast.

Artykuł ten zaznajamia Czytelnika z używaniem :doc:`/components/finder`, który
ma zastosowanie do wszystkich komponentów.

Używanie Finder Component
-------------------------

**1.** Jeśli tworzysz nowy projekt, utwórz dla niego nowy pusty katalog.

**2.** Otwórz terminal i użyj Composer w celu pobrania bibliotek.

.. code-block:: bash

    $ composer require symfony/finder

Nazwa ``symfony/finder`` jest napisana na górze dokumentacji każdego potrzebnego
komponentu.

.. tip::

    `Zainstaluj composer`_, jeśli nie masz go jeszcze w systemie.
    Ostatecznie możesz poprzestać na pliku ``composer.phar`` w swoim katalogu.
    W takim przypadku nie martw się! Wystarczy uruchomić
    ``php composer.phar require symfony/finder``.

**3.** Pisz swój kod!

Po pobraniu przez Composer komponentu (komponentów), wszystko co trzeba zrobić,
to dołączyć plik ``vendor/autoload.php``, który został wugenerowany przez Composer.
Plik ten ładuje automatycznie wszystkie potrzebne biblioteki, dzięki czemu możesz
je użyć w projekcie natychmiast::

    // Przykładowy plik: src/script.php

    // aktualizuje to ścieżkę do katalogu "vendor/",
    // względem tego skryptu
    require_once __DIR__.'/../vendor/autoload.php';

    use Symfony\Component\Finder\Finder;

    $finder = new Finder();
    $finder->in('../data/');

    // ...

Użycie wszystkich komponentów
-----------------------------

Jeśłi chcesz używać wszystkie komponenty Symfony, to zamiast pobierać każdy
komponent z osobna, pobierz cały pakiet instalacyjny ``symfony/symfony``:

.. code-block:: bash

    $ composer require symfony/symfony

Dołączy to również pakiety aplikacyjne (*ang. bundles*) i biblioteki pomostowe
(*ang. bridge libraries*), które mogą ale nie musza byc potrzebne.

Co teraz?
---------

Teraz, gdy juz komponent jest zainstalowany i automatycznie ładowany, przeczytaj
dokumentację komponentu, aby znaleźć więcej informacji na jego temat.

Powodzenia!

.. _Composer: https://getcomposer.org
.. _Zainstaluj composer: https://getcomposer.org/download/

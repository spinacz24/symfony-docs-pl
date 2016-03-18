.. highlight:: php
   :linenothreshold: 2

.. index::
   double: komponent Security; narzędzia

Bezpieczne porownywanie ciągów i generowanie liczb losowych
===========================================================

Komponent Security dostarczany jest z kolekcją przydatnych narzędzi
związanych z bezpieczeństwem. Narzędzia te są wykorzystywane przez Symfony,
ale powinny być też stosowane, jeśli chce się rozwiązać problemy, dla których
są adresowane.

.. index::
   single: bezpieczeństwo; porównywanie ciągów

Porównywanie ciągów
-------------------

Czas potrzebny na porównanie dwóch ciągów znakowych zależy od ich różnicy.
Może to zostać wykorzystane przez atakujacego, gdy sa porównywane dwa ciagi reprezentujące
hasło. Ten rodzaj atakuzwany jest `atakiem czasowym`_.

Przy porównywaniu dwóch haseł, Symfony używa wewnetrznie algorytmu o stałym czasie.
Mozna wykorzystać tą samą startegię we własnym kodzie, dzięki klasie
:class:`Symfony\\Component\\Security\\Core\\Util\\StringUtils`::

    use Symfony\Component\Security\Core\Util\StringUtils;

    // is some known string (e.g. password) equal to some user input?
    $bool = StringUtils::equals($knownString, $userInput);

.. index::
   single: bezpieczeństwo; generowanie liczb losowych

Generowanie bezpiecznej liczby losowej
--------------------------------------

Gdy zachodzi konieczność wygenerowania bezpiecznej liczby losowej, bardzo zachęcamy
do użycia klasy Symfony
:class:`Symfony\\Component\\Security\\Core\\Util\\SecureRandom`::

    use Symfony\Component\Security\Core\Util\SecureRandom;

    $generator = new SecureRandom();
    $random = $generator->nextBytes(10);

Metoda
:method:`Symfony\\Component\\Security\\Core\\Util\\SecureRandom::nextBytes`
zwraca losowy ciąg składajacy się ze znaków w liczbie określonej w przekazanym
argumencie (10 w powyższym przykładzie).

Klasa SecureRandom działa lepiej, gdy zainstalowana jest biblioteka OpenSSL.
Gdy nie jest ona dostępna, awaryjnie wykorzystywany jest wewnętrzny algorytm,
który potrzebuje do poprawnego działania pliku zalążkowego. Wystarczy przekazać
nazwę pliku, aby to udostępnić::

    use Symfony\Component\Security\Core\Util\SecureRandom;

    $generator = new SecureRandom('/some/path/to/store/the/seed.txt');

    $random = $generator->nextBytes(10);
    $hashedRandom = md5($random); // zobacz poniższą wskazówkę

.. note::

    Jeśli używa się frameworka Symfony, można pobrać bezpieczną liczbę losową
    generując ją poprzez usługę ``security.secure_random``.

.. tip::

    Metoda ``nextBytes()`` zwraca ciąg binarny, który może zawierać znak ``\0``.
    Może to powodować problem w kilku typowych scenariuszach, takich, które
    przechowują tą wartość w bazie danych lub dołączają ją jako część adresu URL.
    Rozwiazaniem jest haszowanie wartości zwracanej przez ``nextBytes()``
    (do zrobienia tego można uzyć prostej funkcji ``md5()`` PHP).

.. _`atakiem czasowym`: https://pl.wikipedia.org/wiki/Atak_czasowy

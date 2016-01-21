.. index::
   single: najlepsze praktyki

Najlepsze praktyki dla frameworka Symfony
=========================================

Framework Symfony jest dobrze znany jako *naprawdę* elastyczna platforma do budowy
mikro-witryn, korporacyjnych aplikacji obsługujących miliardy połączeń a nawet
jako podstawa dla *innych* frameworków. Od premiery w lipcu 2011 roku,
jego społeczność nauczyła się wiele na temat tego, co jest możliwe i jak robić
rzeczy *najlepiej*.

Te społeczne zasoby - takie jak wpisy na blogu lub prezentacje - zostały stworzone
według nieoficjalnego zbioru zaleceń dla tworzenia aplikacji Symfony.
Niestety, część tych zaleceń jest zbyteczna dla aplikacji internetowych.
Często komplikowały one niepotrzbnie rzeczy i nie były zgodne z oryginalną
pragmatyczną filozofia Symfony.

O czym jest ten przewodnik?
---------------------------

Przewodnik ten ma na celu ujednolicenie i spisanie **najlepszych praktyk tworzenia
aplikacji internetowych w jednolitym frameworku aplikacyjnym Symfony**. Są to najlepsze
praktyki, zgodne z filozofiia tego frameworka, jaką wyznawał pierwotny jego
twórca, `Fabien Potencier`_.

.. note::

    **Najlesza praktyka** jest pojęciem oznaczajacym *"dobrze określoną procedurę,
    która jest znana z tego, że daje najbardziej optymalne rezultaty"*. Jest to
    dokładnie to, co ma na celu ten przewodnik. Jeśli nawet nie zgadzasz się
    z każdym zaleceniem, wierzymy, że pomoże on Tobie tworzyć wspaniałe aplikacje,
    przy jak najmniejszym komplikowaniu rzeczy.

Przewodnik ten jest **szczególnie przeznaczony** dla:

* Twórców witryn i aplikacji internetowych na bazie jednolitego frameworka Symfony.

W innych sytuacjch, przewodnik ten może być dobrym **punktem wyjścia**, przy:

* rozpowszechnianiu pakietów dla spolecznosci Symfony community;
* pracy zaawansowanych programistów i zespołów, które tworza własne standardy;
* niektórych aplikacjach, które maja złożone wymagania dostosowawcze;
* tworzeniu pakietów, które maja być współdzielone wewnętrznie w przedsiębiorstwie.

Rozumiemy, że stare nawyki nie umierają i niektórzy będą w szoku przy lekturze
tych najlepszych praktyk. Jednak postępując zgodnie z nimi, będzie sie mogło
tworzyć aplikacje szybciej, z wiekszą prostotą i z ta sama a nawet większą jakością.
Zasady te będziemy stale polepszać.

Należy pamiętać, że są to **tylko zalecenia**, które Ty lub Twój zespół możecie,
ale nie musicie przyjąć przy pracy nad aplikacją Symfony. Jeśli chcesz pozostać
przy swoich starych praktykach i metodologiach, możesz oczywiście to zrobić.
Symfony jest na tyle elestyczne, że dostosuje się do Twoich potrzeb. To nigdy
nie ulegnie zmianie.

Dla kogo jest ten przewodnik (wskazówka: to nie jest samouczek)
---------------------------------------------------------------

Przewodnik ten może przeczytać każdy programista Symfony, niezależnie od tego,
czy jest on ekspertem czy początkującym. Ponieważ nie jest to samouczek, musi się
posiadać wiedzę o Symfony w zakresie tu poruszanym. Jeśli jesteś nowicjuszem
w Symfony, to witamy!
Najpierw rozpocznij od :doc:`"Krótkiego kursu" </quick_tour/the_big_picture>`.

Utrzymujemy celowo ten przewodnik krótkim. Nie będziemy powtarzać tu wyjaśnień,
jakie można znaleźć w rozległej dokumentacji Symfony, takich jak omówienie
wstrzykiwania zależnosci lub wyjaśnienie roli kontrolera wejściowego (*ang. front
controller*). Skupiamy sie jedynie na wyjaśnieniu "jak to zrobić", zakładając, że
Czytelnik ma dostateczną wiedzę.

Aplikacja
---------

Oprócz tego przewodnika, została stworzona przykładowa aplikacja, uwzgledniająca
wszystkie opisane tu najlepsze praktyki. Projekt ten, nazwany aplikacją Symfony
Demo, można sobie zainstalować za pomoca instalatora Symfony. Najpierw,
`pobierz i zainstaluj`_ instalator i następnie wykonaj polecenie pobierające
aplikację demonstracyjną:

.. code-block:: bash

    $ symfony demo

**Aplikacja demo jest prostym silnikiem bloga**, bo pozwala to skupić się na
koncepcji Symfony i jego funkcjonalnościach, bez konieczności wnikania w ukryte
szczegóły implementacji. Zamiast tworzenia aplikacji, krok po kroku w tym przewodniku,
znajdziesz tam wybrane fragmenty kodu dla każdego rozdziału niniejszego przewodnika.

Nie aktualizuj istniejących aplikacji
-------------------------------------

Po przeczytaniu tego podręcznika, niektórzy Czytelnicy moga rozważać przerobienie
swoich już istniejacych aplikacji Symfony. Nasza rekomendacja jest wyraźna:
**nie powinno sie przerabiać istniejacych aplikacji w celu dostosowania ich do
zgodności z opisanymi tu najlepszymi praktykami**. Powodów jest kilka:

* Istniejące aplikacji mogą wcale nie być złe, po prostu zostały wykonane według
  innego zbioru wytycznych;
* Pełna przeróbka kodu jest podatna na wprowadzenie nowych błedów w aplikacji;
* Nakład pracy lepiej przeznaczyć na przeprowadzenie testów i dodanie nowych
  funkcji, które przełożą sie na realna korzyść dla końcowych uzytkowników.

.. _`Fabien Potencier`: https://connect.sensiolabs.com/profile/fabpot
.. _`pobierz i zainstaluj`: https://symfony.com/download

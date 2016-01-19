.. index::
   double: aktywa; najlepsze praktyki

Aktywa siweciowe
================

:term:`Aktywa sieciowe <aktywa>`, to takie rzeczy jak skrypty CSS i JavaScript,
pliki obrazów itd., które tworzą część frontową witryny i sprawiają, że wygląda
ona świetnie. Programiści Symfony zwykle przechowują aktywa (zasoby aplikacji)
w katalogu ``Resources/public/`` każdego pakietu.

.. best-practice::

    Przechowuj aktywa (zasoby aplikacji) w katalogu``web/``.

Rozrzucając aktywa sieciowe po wszystkich pakietach, sprawia się, że zarządzanie
aktywami jest trudniejsze. Projektanci części frontowej będą mieli znacznie uproszczone
zadanie, jeśli aktywa sieciowe zostaną umieszczone w jednej lokalizacji.

Szablony również korzystają na zcentralizowaniu aktywów, ponieważ odnośniki są
znacznie prostsze:

.. code-block:: html+twig
   :linenos:

    <link rel="stylesheet" href="{{ asset('css/bootstrap.min.css') }}" />
    <link rel="stylesheet" href="{{ asset('css/main.css') }}" />

    {# ... #}

    <script src="{{ asset('js/jquery.min.js') }}"></script>
    <script src="{{ asset('js/bootstrap.min.js') }}"></script>

.. note::

    Trzeba mieć na uwadze, że katalog ``web/`` jest katalogiem publicznym, więc
    wszystkie tutaj umieszczone pliki będą dostępne publicznie, włączając w to
    wszystkie oryginalne pliki zasobów aplikacji (np. pliki Sass, LESS i CoffeeScript).

Stosowanie aktywów
------------------

Obecnie już unika się już tworzenia statycznych plików CSS i JavaScript oraz
ręcznego dołączania tych plików do szablonów. Zamiast tego najczęściej, stosuje
się rozwiązania polegajacego na łączeniu i minimalizacji tych plików po stronie
klienta, w celu zwiększenia wydajności. Można zdecydować się na, na przykład,na
użycie LESS lub Sass, co oznacza, że trzeba w jakiś sposób przetwarzać pliki CSS.

Istnieje wiele narzędzi pozwalajacych rozwiązać te problemy, w tym narzędzia dla
tworzenia "front-endu" (nie w PHP), takich jak GruntJS.

.. best-practice::

    Używaj Assetic do kompilowania, łączenia i minimalizacji aktywów sieciowych,
    chyba że wygodniej jest stosować narzędzia do tworzenia części frontowej
    aplikacji (front-endu), takie jak GruntJS.

:doc:`Assetic </cookbook/assetic/asset_management>` jest menadżerem aktywów,
umożliwiającym kompilowanie aktywów opracowanych w wielu różnych technologiach
front-endu, takich jak LESS, Sass i CoffeeScript. Łączenie aktywów w Assetic
sprowadza się do opakowania wszystkich aktywów pojednynczym znaczniku Twig:

.. code-block:: html+twig
   :linenos:

    {% stylesheets
        'css/bootstrap.min.css'
        'css/main.css'
        filter='cssrewrite' output='css/compiled/app.css' %}
        <link rel="stylesheet" href="{{ asset_url }}" />
    {% endstylesheets %}

    {# ... #}

    {% javascripts
        'js/jquery.min.js'
        'js/bootstrap.min.js'
        output='js/compiled/app.js' %}
        <script src="{{ asset_url }}"></script>
    {% endjavascripts %}

Aplikacje frontendowe
---------------------

Ostatnio, coraz większą popularność zyskuje stosowanie frameworków tzw. "aplikacji
frontendowych", opartych o kod JavaScript, takich jak AngularJS. Aplikacje takie
działają po stronie klienta, w przeglądarce internetowej i wykorzystują aplikację
zaplecza ("backend"), działającą po stronie serwera, która może być utworzona
w innej technologii, na przykład w Symfony.   

Jeśli wykorzystuje się takie rozwiązanieprzy tworzeniu swoich aplikacji, powinno
się używać narzędzi, które są zalecane przez dany framework, takich jak Bower,
GruntJS, czy Yeoman. Powinno się tworzyć aplikację frontendową oddzielnie od
backendu Symfony (nawet używając oddzielnych repozytoriów).

Więcej informacji o Assetic
---------------------------

Assetic może również ninimalizować aktywa CSS i JavaScript uzywając bibliotek 
:doc:`UglifyCSS i UglifyJS </cookbook/assetic/uglifyjs>`, co pozwala przyśpieszyć
działanie aplikacji. W Assetic można nawet
:doc:`kompresować obrazy </cookbook/assetic/jpeg_optimize>`
w celu zmniejszenia ich wielkości, przed zaserwowaniem ich do użytkownika.
Proszę zapoznać się z `oficjalną dokumentacją Assetic`_ w celu poznania szczegółów.

.. _`oficjalną dokumentacją Assetic`: https://github.com/kriswallsmith/assetic

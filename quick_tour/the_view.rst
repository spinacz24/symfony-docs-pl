.. highlight:: php
   :linenothreshold: 2

Widok
=====

Po przeczytaniu pierwszej części tego poradnika, dobrze jest poświęcić
kolejne 10 minut dla Symfony. W drugiej części, nauczysz się
więcej na temat silnika szablonów Symfony, `Twig`_ - elastycznego,
szybkiego oraz bezpiecznnego systemem szablonów PHP. Sprawia że szablony
są bardo czytelne oraz zwięzłe. Czyni je także bardziej przyjaznymi dla
projektantów stron internetowych.

Zapoznanie się z Twig
---------------------

Najlepszym źródłem informacji o tym silniku szablonowania jest oficjalna
`dokumentacja Twig`_. Niniejszy rozdział daje tylko krótki przegląd najważniejszych
pojęć.

Szablon Twig jest tekstowym plikiem który może generować każdy rodzaj
treści (HTML, XML, CSV, LaTeX, ...). Elementy szablonu Twig są oddzielana od reszt
zawartości przy użyciu tych ograniczników:

* ``{{ ... }}``: wypisuje zmienną lub też wynik wyrażenia;

* ``{% ... %}``: kontroluje logikę szablonu. Jest na przykład używany do pętli
  ``for`` lub  wyrażeń ``if``.

* ``{# ... #}``: Umożliwia dołączanie wewnątrz szablonu komentarzy.

Poniżej znajduje się kod minimalnego szablonu, ilustrujący kilka podstawowych rzeczy:
użyte zostały dwie zmienne ``page_title`` oraz ``navigation``, które będą
przekazane do szablonu:

.. code-block:: html+jinja
   :linenos:

    <!DOCTYPE html>
    <html>
        <head>
            <title>{{ page_title }}</title>
        </head>
        <body>
            <h1>{{ page_title }}</h1>

            <ul id="navigation">
                {% for item in navigation %}
                    <li><a href="{{ item.url }}">{{ item.label }}</a></li>
                {% endfor %}
            </ul>
        </body>
    </html>

Do renderowania szablonu w Symfony używa się metody ``render`` w kontrolerze
i przekazuje potrzebne zmienne jako tablicę, wykorzystując opcjonalny drugi argument::

    $this->render('AcmeDemoBundle:Demo:hello.html.twig', array(
        'name' => $name,
    ));

Zmienne przekazane do szablonu mogą być ciągami znaków, tablicami lub nawet obiektami.
Twig rozpoznaje różnice między nimi i umożliwia łatwy dostęp do "atrybutów" zmiennej
poprzez notację kropkową (``.``).
Poniższy kod pokazuje jak wyświetlana jest treść zmiennych w zależności od zmiennej
przekazanej przez kontroler:

.. code-block:: jinja
   :linenos:

    {# 1. Simple variables #}
    {# array('name' => 'Fabien') #}
    {{ name }}

    {# 2. Arrays #}
    {# array('user' => array('name' => 'Fabien')) #}
    {{ user.name }}

    {# alternative syntax for arrays #}
    {{ user['name'] }}

    {# 3. Objects #}
    {# array('user' => new User('Fabien')) #}
    {{ user.name }}
    {{ user.getName }}

    {# alternative syntax for objects #}
    {{ user.name() }}
    {{ user.getName() }}

Dekorowanie szablonów
---------------------

Przeważnie szablony w projekcie posiadają współdzielone elementy takie jak dobrze
znany nagłówek czy stopka. Twig rozwiązuje ten problem elegancko wykorzystując
tzw. "dziedziczenie szablonów". Ta funkcjonalność umożliwia zbudowanie bazowego
szablonu "layout", który zawiera wszystkie elementy strony i definiuje "bloki",
które szablony potomne mogą przesłaniać.

Szablon ``index.html.twig`` używa znacznika ``extends`` do wskazania, że dziedziczy
ze wspólnego szblonu ``base.html.twig``:

.. code-block:: html+jinja
   :linenos:

    {# app/Resources/views/default/index.html.twig #}
    {% extends 'base.html.twig' %}

    {% block body %}
        <h1>Welcome to Symfony!</h1>
    {% endblock %}

Otwórz plik ``app/Resources/views/base.html.twig``, który odpowiada szablonowi
``base.html.twig`` i znajdź następujący kod Twig code:

.. code-block:: html+jinja
   :linenos:

    {# app/Resources/views/base.html.twig #}
    <!DOCTYPE html>
    <html>
        <head>
            <meta charset="UTF-8" />
            <title>{% block title %}Welcome!{% endblock %}</title>
            {% block stylesheets %}{% endblock %}
            <link rel="icon" type="image/x-icon" href="{{ asset('favicon.ico') }}" />
        </head>
        <body>
            {% block body %}{% endblock %}
            {% block javascripts %}{% endblock %}
        </body>
    </html>

Znaczniki ``{% block %}`` powiadamiają silnik szablonowania, że szablon potomny
może przesłaniać ten fragment szablonu. W naszym przykładzie, szablon ``index.html.twig``
przesłania blok ``body``, alenie blok ``title``, który będzie wyswietlał domyślną
treść zdefiniowana w szablonie ``base.html.twig``.

Używanie znaczników, filtrów i funkcji
--------------------------------------

Jedną z najlepszych cech systemu Twig jest jego rozszerzalność poprzez znaczniki,
filtry i funkcje. Proszę spojrzeć na poniższy przykładowy szablon, w którym stosuje
się filtry znacznie modyfikujące informacje przed ich wyświetleniem:

.. code-block:: jinja
   :linenos:
   
   <h1>{{ article.title|trim|capitalize }}</h1>
   
   <p>{{ article.content|striptags|slice(0, 1024) }}</p>
   
   <p>Tags: {{ article.tags|sort|join(", ") }}</p>
   
   <p>Następny artykuł zostanie opublikowany w {{ 'następny poniedziałek'|date('d-m-Y')}}</p>

Zapoznaj się z oficjalną `dokumentacją Twig`_, aby nauczyć się wszystkiego o filtrach,
funkcjach i znacznikach.

Dołączenie innych szablonów
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Najlepszym sposobem, aby podzielić się fragmentem kodu pomiędzy różnymi
szablonami jest stworzenie nowego szablonu który może zostać dołączony
przez inne szablony.

Proszę sobie wyobrazić, że chcemy wyświetlić reklamy na niektórych stronach
naszej aplikacji. Najpierw utwórzmy szablon ``banner.html.twig``:

.. code-block:: jinja
   :linenos:

    {# app/Resources/views/ads/banner.html.twig #}
    <div id="ad-banner">
        ...
    </div>

Dla wyświetleniatej reklamy na stronie, dołączymy szablon ``banner.html.twig``
używając funkcję ``include()``:

.. code-block:: html+jinja
   :linenos:

    {# app/Resources/views/default/index.html.twig #}
    {% extends 'base.html.twig' %}

    {% block body %}
        <h1>Welcome to Symfony!</h1>

        {{ include('ads/banner.html.twig') }}
    {% endblock %}


Osadzanie innych kontrolerów
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Co, jeśli chce się osadzić wynik innego kontrolera w szablonie? To jest bardzo
przydatne podczas pracy z Ajax, lub gdy osadzony szablon potrzebuje niektórych
zmiennych niedostępnych w głównym szablonie.

Załóżmy, że utworzyliśmy metodę kontrolera ``topArticlesAction`` dla wyświetlania
najbardziej popularnych artykułów na swojej witrynie. Jeśli chce się "renderować"
wynik tej metody (np. ``HTML``) wewnątrz szablonu ``index``, trzeba zastosować
funkcję``render``:

.. code-block:: jinja

    {# app/Resources/views/index.html.twig #}
    {{ render(controller('AppBundle:Default:topArticles')) }}

tutaj, funkcje ``render()`` i ``controller()`` używają specjalnej składni
``AppBundle:Default:topArticles`` do odwoływania sie do akcji ``topArticlesAction``
kontrolera ``Default`` (część ``AppBundle`` zostanie wyświetlona dalej)::

    // src/AppBundle/Controller/DefaultController.php

    class DefaultController extends Controller
    {
        public function topArticlesAction()
        {
            // wyszukanie najbardziej popularnych artykułów w bazie danych
            $articles = ...;

            return $this->render('default/top_articles.html.twig', array(
                'articles' => $articles,
            ));
        }

        // ...
    }
    
Tutaj ciąg ``AcmeDemoBundle:Demo:topArticles`` odnosi się do akcji ``topArticlesAction``
kontrolera ``Demo`` a argument ``num`` jest dostępny dla kontrolera::

   // src/Acme/DemoBundle/Controller/DemoController.php

    class DemoController extends Controller
    {
        public function topArticlesAction($num)
        {
            // look for the $num most popular articles in the database
            $articles = ...;

            return $this->render('AcmeDemoBundle:Demo:topArticles.html.twig', array(
                'articles' => $articles,
            ));
        }

        // ...
    }    

Tworzenie odnośników pomiędzy stronami
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Tworzenie odnośników pomiędzy stronami jest koniecznością w aplikacjach internetowych.
Zamiast umieszczania w szablonach sztywnych lokalizatorów URL, można zastosować funkcję
``path``, która wie jak wygenerować adres URL na podstawie konfiguracji trasowania.
W ten sposób wszystkie lokalizatory URL mogą być łatwo aktualizowane tylko przez
zmianę konfiguracji:

.. code-block:: html+jinja

    <a href="{{ path('homepage') }}">Return to homepage</a>

Funkcja ``path`` pobiera jako pierwszy argument nazwę trasy i może opcjonalnie
przekazywać jako drugi argument tablicę parametrów.

.. tip::

    Funkcja ``url`` jest bardzo podobna do funkcji ``path``, ale generuje *bezwzgledne*
    adresy URL, które są bardzo pomocne przy renderowaniu adresów email i plików RSS:
    ``<a href="{{ url('homepage') }}">Visit our website</a>``.

Dołączanie zasobów: obrazów, skryptów JavaScript i arkuszy stylów
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Co to byłby za Internet bez zdjęć, skryptów JavaScript i arkuszy stylów? Symfony
oferuje funkcję ``asset`` radzącą sobie łatwo z tym zagadnieniem:

.. code-block:: html+jinja

    <link href="{{ asset('css/blog.css') }}" rel="stylesheet" type="text/css" />

    <img src="{{ asset('images/logo.png') }}" />

Głównym zadaniem funkcji ``asset`` jest umożliwienie lepszej przenośności aplikacji.
Dzięki tej funkcji, możesz przenieść główny katalog aplikacji w dowolne miejsce bez
konieczności dokonywania zmian w kodzie szablonu.

Korzystanie z funkcji ``asset`` sprawia, że aplikacja staje się bardziej przenośna.
Powodem jest to, że można przenieść katalog główny aplikacji gdziekolwiek w ramach
głównego katalogu serwera internetowego, bez konieczności zmieniania czegokolwiek
w kodzie szablonów.

Podsumowanie
------------

Twig jest prosty ale skuteczny. Dzięki możliwości stosowania formatek (*ang. layout*),
bloków, dziedziczenia szablonów i akcjom, bardzo łatwo można zorganizować swój
szablon, w sposób logiczny i rozszerzalny. Jeśli jednak nie odpowiada Ci Twig,
to zawsze, bez żadnych problemów, możesz użyć w Symfony zwykłych szablonów PHP.

Pracujesz z Symfony od około 20 minut, ale już teraz możesz zrobić z nim
sporo niesamowitych rzeczy. To jest siła Symfony. Nauka podstaw jest bardzo
prosta. Już niedługo odkryjesz, że prostota jest ukryta pod bardzo elastyczną
architekturą.

Ale coraz bardziej odbiegam od tematu. Po pierwsze, musisz dowiedzieć się więcej
o kontrolerach i to jest tematem :doc:`kolejnej części przewodnika <the_controller>`.
Gotowy na kolejne 10 minut z Symfony?

.. _`Twig`:               http://twig.sensiolabs.org/
.. _`dokumentacją Twig`: http://twig.sensiolabs.org/documentation
.. _`dokumentacja Twig`: http://twig.sensiolabs.org/documentation


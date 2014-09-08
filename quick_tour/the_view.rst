.. highlight:: php
   :linenothreshold: 2

Widok (View)
============

Po przeczytaniu pierwszej części tego poradnika, dobrze jest poświęcić
kolejne 10 minut dla Symfony2. W drugiej części, nauczysz się
więcej na temat silnika szablonów Symfony2, `Twig`_ - elastycznego,
szybkiego oraz bezpiecznnego systemem szablonów PHP. Sprawia że szablony
są bardo czytelne oraz zwięzłe. Czyni je także bardziej przyjaznymi dla
projektantów stron internetowych.

Zapoznanie się z Twig
---------------------

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

.. tip::

    Komentarze mogą być dołączone do szablonu poprzez użycie ogranicznika ``{# ... #}``.

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

Szablon ``hello.html.twig`` uzywa znacznika ``extends`` do wskazania, że dziedziczy
ze wspólnego szblonu ``layout.html.twig``:

.. code-block:: html+jinja
   :linenos:

    {# src/Acme/DemoBundle/Resources/views/Demo/hello.html.twig #}
    {% extends "AcmeDemoBundle::layout.html.twig" %}

    {% block title "Hello " ~ name %}

    {% block content %}
        <h1>Hello {{ name }}!</h1>
    {% endblock %}

Zapis ``AcmeDemoBundle::layout.html.twig`` wygląda znajomo, prawda? Jest to ta sama
notacja, jaka była zastosowana do regularnego szablonu. Część ``::`` oznacza, że
element kontrolera jest pusty, tak więc odpowiedni plik znajduje się w katalogu
pakietu ``Resources/views/``.

Przyjrzyjmy sie uproszczonej wersji ``layout.html.twig``:

.. code-block:: jinja
   :linenos:
   
    {# src/Acme/DemoBundle/Resources/views/layout.html.twig #}
    <div>
        {% block content %}
        {% endblock %}
    </div>

Znaczniki ``{% block %}`` powiadamiają silnik szablonowania, że szablon potomny
może przesłaniać tą porcje szablonu. W naszym przykładzie, szablon ``hello.html.twig``
przesłania blok ``content``, co oznacza, że tekst "Hello Fabien" jest renderowany
wewnątrz elementu ``<div>``.

W tym przykładzie, szablon ``hello.html.twig`` zastępuje blok ``content``,
co oznacza że tekst "Hello Fabien" jest renderowany w środku elementu
``div.symfony-content``.

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

Najpierw, utworzymy szablon ``embedded.html.twig``:

.. code-block:: jinja
    :linenos:
    
    {# src/Acme/DemoBundle/Resources/views/Demo/embedded.html.twig #}
    Hello {{ name }}

i zmienimy szablon ``index.html.twig``, tak aby dołączał nasz nowo utworzony szablon:

.. code-block:: jinja
   :linenos:

    {# src/Acme/DemoBundle/Resources/views/Demo/hello.html.twig #}
    {% extends "AcmeDemoBundle::layout.html.twig" %}

    {# override the body block from embedded.html.twig #}
    {% block content %}
        {% include "AcmeDemoBundle:Demo:embedded.html.twig" %}
    {% endblock %}

Osadzanie innych kontrolerów
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A co, jeśli chcesz osadzić wynik innego kontrolera w szablonie? To bardzo przydatne
podczas pracy z Ajax, lub gdy osadzony szablon potrzebuje niektórych zmiennych
niedostępnych w głównym szablonie.

Załóżmy, że utworzyliśmy metodę kontrolera ``topArticlesAction`` dla wyświetlania
najbardziej popularnych artykułów na swojej witrynie. Jeśli chce się "renderować"
wynik tej metody (np. ``HTML``) wewnątrz szablonu ``index``, trzeba zastosować
funkcję``render``:

.. code-block:: jinja

    {# src/Acme/DemoBundle/Resources/views/Demo/index.html.twig #}
    {{ render(controller("AcmeDemoBundle:Demo:fancy", {'name': name, 'color': 'green'})) }}

Załóżmy, że utworzyliśmy metodę kontrolera ``fancyAction`` i chcemy "renderować"
ją w szablonie ``index``, co oznacza osadzenie wyniku kontrolera (np. ``HTML``)
w renderowanej z szablonu stronie. Aby to zrobić, użyjemy funkcji``render``::

    // src/Acme/DemoBundle/Controller/DemoController.php

    class DemoController extends Controller
    {
        public function fancyAction($name, $color)
        {
            // utworzenie jakiegoś obiektu, na podstawie zmiennej $color
            $object = ...;

            return $this->render('AcmeDemoBundle:Demo:fancy.html.twig', array(
                'name' => $name,
                'object' => $object,
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
Zamiast umieszczania w szablonach sztywnych lokalizatorów URL,
można zastosować funkcję ``path``, która wie jak wygenerować URL na podstawie
konfiguracji trasowania. W ten sposób wszystkie lokalizatory URL mogą być łatwo
aktualizowane tylko przez zmianę konfiguracji:

.. code-block:: html+jinja

    <a href="{{ path('_demo_hello', { 'name': 'Thomas' }) }}">Greet Thomas!</a>

Funkcja ``path`` pobiera jako argumenty nazwę trasy i tablicę parametrów.
Nazwa trasy jest kluczem w którym zdefiniowane są trasy a parametry są wartościami
zmiennych zdefiniowanych we wzorcu trasy::

    // src/Acme/DemoBundle/Controller/DemoController.php
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Template;

    /**
     * @Route("/hello/{name}", name="_demo_hello")
     * @Template()
     */
    public function helloAction($name)
    {
        return array('name' => $name);
    }

.. tip::

    Funkcja ``url`` jest bardzo podobna do funkcji ``path``, ale generuje *bezwzgledne*
    adresy URL, które są bardzo pomocne przy renderowaniu adresów email i plików RSS:
    ``{{ url('_demo_hello', {'name': 'Thomas'}) }}``.

Dołączanie zasobów: obrazów, skryptów JavaScript i arkuszy stylów
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Co to byłby za Internet bez zdjęć, skryptów JavaScript i arkuszy stylów? Symfony2
oferuje funkcję ``asset`` radzącą sobie łatwo z tym zagadnieniem:

.. code-block:: html+jinja

    <link href="{{ asset('css/blog.css') }}" rel="stylesheet" type="text/css" />

    <img src="{{ asset('images/logo.png') }}" />

Głównym zadaniem funkcji ``asset`` jest umożliwienie lepszej przenośności aplikacji.
Dzięki tej funkcji, możesz przenieść główny katalog aplikacji w dowolne miejsce bez
konieczności dokonywania zmian w kodzie szablonu.

Podsumowanie
------------

Twig jest prosty ale skuteczny. Dzięki możliwości stosowania formatek (*ang. layout*),
bloków, dziedziczenia szablonów i akcjom, bardzo łatwo można zorganizować swój
szablon, w sposób logiczny i rozszerzalny. Jeśli jednak nie odpowiada Ci Twig,
to zawsze, bez żadnych problemów, możesz użyć w Symfony zwykłych szablonów PHP.

Pracujesz z Symfony2 od około 20 minut, ale już teraz możesz zrobić z nim
sporo niesamowitych rzeczy. To jest siła Symfony2. Nauka podstaw jest bardzo
prosta. Już niedługo odkryjesz, że prostota jest ukryta pod bardzo elastyczną
architekturą.

Ale coraz bardziej odbiegam od tematu. Po pierwsze, musisz dowiedzieć się więcej
o kontrolerach i to jest tematem :doc:`kolejnej części przewodnika <the_controller>`.
Gotowy na kolejne 10 minut z Symfony2?

.. _`Twig`:               http://twig.sensiolabs.org/
.. _`dokumentacją Twig`: http://twig.sensiolabs.org/documentation

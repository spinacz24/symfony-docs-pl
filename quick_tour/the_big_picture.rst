.. highlight:: php
   :linenothreshold: 2

Przegląd
========

Zacznij korzystać z Symfony2 w 10 minut! Ten rozdział wyjaśni Ci kilka
najważniejszych pojęć Symfony2 i to, jak można rozpocząć
szybko pracę, pokazując w działaniu prosty przykład aplikacji.

Jeśli czytelnik kiedykolwiek używał frameworka aplikacji internetowej, to z Symfony2
powinien czuć się jak w domu.
Jeśli nie, to zapraszamy do poznania zupełnie nowego sposobu tworzenia aplikacji
internetowych.

    
Instalowanie Symfony2
---------------------

Najpierw, sprawdź zainstalowaną na swoim komputerze wersję wersję PHP, gdyż Symfony2
wymaga wersji 5.3.3 lub wyższej. Otwórz konsolę i wykonaj następujące polecenie
instalujące najnowszą wersję Symfony2 w katalogu ``myproject/``:

.. code-block:: bash

    $ composer create-project symfony/framework-standard-edition myproject/ '~2.5'

.. note::

    `Composer`_, to menadżer pakietów wykorzystywany w nowoczesnych aplikacjach
    PHP i jedyny zalecany sposób instalacji Symfony2. W celu zainstalowania
    programu Composer w systemie Linux lub Mac, wykonaj następujące polecenia:

    .. code-block:: bash

        $ curl -sS https://getcomposer.org/installer | php
        $ sudo mv composer.phar /usr/local/bin/composer

    Dla zainstalowania programu Composer w systemie Windows, trzeba
    pobrac `executable installer`_.

Pamiętaj, że w czasie pierwszej instalacji Symfony2 może minąć kilka minut zanim
zostaną pobrane wszystkie komponenty. Pod koniec procesu instalacji instalator
poprosi o odpowiedź na kilka pytań:

1. **Would you like to use Symfony 3 directory structure? [y/N]**
   (*Czy chcesz zastosować strukturę katalogową Symfony 3? [y/N]*) Nadchodząca
   wersja Symfony 3 zmieni domyślną strukturę katalogową aplikacji Symfony.
   Jeśli chcesz przetestować tą nową strukturę, wprowadź ``y``. Na użytek niniejszego
   poradnika, wciśnij klawisz ``<Enter>``, aby zaakceptować domyślną wartość ``N``
   i utrzymać domyślna strukturę Symfony2.
2. **Would you like to install Acme demo bundle? [y/N]**
   (*Czy chcesz zainstalować demonstracyjny pakiet Acme? [y/N]*)
   Wcześniejsze wersje Symfony niż 2.5 zawierały demonstracyjną aplikację do testowania
   niektórych możliwości frameworka. Ponieważ ta aplikacja jest przydatna tylko dla
   początkujących, jej instalacja jest teraz opcjonalna. W celu wykonywania ćwiczeń
   zawartych w tym poradniku, zainstaluj aplikację demonstracyjną wprowadzając
   odpowiedź ``y``.
3. **Some parameters are missing. Please provide them.**
   (*Niektóre parametry są puste. Proszę wprowadzić je.*)
   Symfony2 poprosi o podanie wartości wszystkich parametrów konfiguracyjnych.
   W naszym pierwszym projekcie można je bezpiecznie zignorować wciskając wielokrotnie
   klawisz ``<Enter>``.
4. **Do you want to remove the existing VCS (.git, .svn..) history? [Y,n]?**
   (*Czy chcesz usunąć istniejąca historię VCS (.git, .svn..)? [Y,n]?*)
   Historia tworzenia dużych projektów, takich jak Symfony, może zajmować sporą
   przestrzeń dyskową. Wciśnij klawisz ``<Enter>``, aby bezpiecznie usunąć wszystkie
   dane historii.

Uruchomienie Symfony2
---------------------

Przed pierwszym uruchomieniem Symfony2, należy wykonać następujące polecenie,
aby upewnić się, że system spełnia wszystkie techniczne wymagania:

.. code-block:: bash

    $ cd myproject/
    $ php app/check.php

Napraw błędy zgłoszone przez polecenie i następnie wykorzystaj wbudowany w PHP
serwer internetowy do uruchomienia aplikacji Symfony:

.. code-block:: bash

    $ php app/console server:run

.. seealso::

   Więcej informacji o wbudowanycm serwerze znajduje się w
   :doc:`in the cookbook </cookbook/web_server/built_in>`.

Jeśli pojawi się błąd `There are no commands defined in the "server" namespace.`,
to prawdopodobnie używasz PHP 5.3. Nie jest to nic strasznego, ale wbudowany serwer
jest tylko dostępny w PHP 5.4.0 lub wersji wyższej. Jeśli masz starszą wersję PHP
lub jeśli preferujesz tradycyjny serwer, taki jak Apache lub Nginx, przeczytaj
artykuł :doc:`/cookbook/configuration/web_server_configuration`.

Otwórz przeglądarkę i odwiedź adres URL ``http://localhost:8000``, aby zobaczyć
stronę powitalną Symfony2:

.. image:: /images/quick_tour/welcome.png
   :align: center
   :alt:   Symfony2 Welcome Page

Podstawy
--------

Jednym z głównych celów frameworka (platformy aplikacyjnej) jest dobre zorganizowanie
kodu i umożliwienie łatwego rozwoju aplikacji, z uniknięciem mieszania w jednym
skrypcie wywołań bazy danych, znaczników HTML i biznesowej logiki. Dla osiągnięcia
tego celu z Symfony, najpierw musisz poznać kilka podstawowych pojęć i terminów.

Symfony dostarczane jest z przykładowym kodem, który można wykorzystać do nauczenia
się więcej o podstawowych pojęciach. Przejdź do następującego adresu URL, aby zobaczyć
stronę powitalną Symfony2 (zamień *Fabien* na swoje imię):

.. code-block:: text

    http://localhost:8000/app_dev.php/demo/hello/Fabien

.. image:: /images/quick_tour/hello_fabien.png
   :align: center

.. note::

    Może się zdarzyć, że zamiast strony powitalnej, zobaczy się pustą stronę lub
    stronę błędu. Jest to spowodowane błędem uprawnień do katalogu aplikacji.
    Jest kilka możliwych rozwiązań tego problemu. Wszystkie z nich omówione są
    w rozdziale :ref:`Ustawienie uprawnień <book-installation-permissions>`
    oficjalnego podręcznika Symfony 2.


Co się tutaj dzieje? Spróbujmy przeanalizować adres URL:

* ``app_dev.php``: Jest to :term:`kontroler wejścia` - unikalny punkt wejścia
  aplikacji, który odpowiedzialny za wszystkie żądania użytkownika;

* ``/demo/hello/Fabien``: Jest to *wirtualna ścieżka* do zasobu jaki chce uzyskać
  użytkownik.

Twoim zadaniem, jako programisty jest napisanie takiego kodu, który odwzorowuje
*żądanie* (``/demo/hello/Fabien``) użytkownika na *zasób* z nim związany (``Hello Fabien!``).

Trasowanie
~~~~~~~~~~

System trasowania (*ang. routing*), nazywany też w polskiej literaturze "systemem przekierowań",
w Symfony2 obsługuje żądania klienta, dopasowując ścieżkę dostępu (zawartą w adresie URL)
do skonfigurowanych wzorców tras i przekazaniu sterowania właściwemu kontrolerowi.
Domyślnie wzorce te są zdefiniowane w pliku ``app/config/routing.yml``:


.. code-block:: yaml
   :linenos:

    # app/config/routing_dev.yml
    _welcome:
        pattern:  /
        defaults: { _controller: AcmeDemoBundle:Welcome:index }

    _demo:
        resource: "@AcmeDemoBundle/Controller/DemoController.php"
        type:     annotation
        prefix:   /demo

    # ...

Pierwsze trzy linie (po komentarzu) określają kod, który jest wykonywany
gdy użytkownik zażąda zasobu "``/``" (tj. strony powitalnej, którą widziałeś
wcześniej). Żądanie wywoła kontroler ``AcmeDemoBundle:Welcome:index``. 
W kolejnym rozdziale dowiesz się dokładnie co to oznacza.

.. tip::

    Oprócz plików YAML, trasy można skonfigurować w plikach XML lub PHP oraz
    można je osadzić w postaci adnotacji PHP w kontrolerach. Ta elastyczność
    jest jedną z głównych zalet Symfony2, frameworka, który nie narzuca
    programiście formatu konfiguracji.

Kontrolery
~~~~~~~~~~

Kontroler jest to jakaś funkcja lub metoda PHP obsługująca przychodzące
*żądania* i zwracająca *odpowiedzi* (często kod HTML). Zamiast wykorzystywać
zmienne globalne PHP i funkcje (np. ``$_GET`` lub ``header()``) do zarządzania
komunikatami HTTP, Symfony używa obiekty :class:`Symfony\\Component\\HttpFoundation\\Request`
i :class:`Symfony\\Component\\HttpFoundation\\Response`. Możliwie najprostszy 
kontroler można utworzyć ręcznie, na podstawie żądania::

    use Symfony\Component\HttpFoundation\Response;

    $name = $request->get('name');

    return new Response('Hello '.$name);
    
    
Symfony2 wybiera kontroler na podstawie wartości ``_controller`` z 
konfiguracji trasowania: ``AcmeDemoBundle:Welcome:index``. Ten ciąg znaków jest
*logiczną nazwą* kontrolera i odwołuje się do metody ``indexAction`` z
:class:`Acme\DemoBundle\Controller\WelcomeController`::

    // src/Acme/DemoBundle/Controller/WelcomeController.php
    namespace Acme\DemoBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\Controller;

    class WelcomeController extends Controller
    {
        public function indexAction()
        {
            return $this->render('AcmeDemoBundle:Welcome:index.html.twig');
        }
    }

.. tip::

    Można używać pełnej nazwy klasy i metody - 
    ``Acme\DemoBundle\Controller\WelcomeController::indexAction`` dla wartości
    ``_controller``. Jeśli jendnak chcesz wykorzystywać proste konwencje, używaj
    nazwy logicznej, która jest krótsza i pozwala na większą elastyczność.

Klasa ``WelcomeController`` rozszerza wbudowaną klasę :class:`Controller`,
która dostarcza użytecznych skrótowych metod, takich jak metoda
`render() <http://api.symfony.com/2.0/Symfony/Bundle/FrameworkBundle/Controller/Controller.html#render()>`_
ładującą i renderującą szablon (``AcmeDemoBundle:Welcome:index.html.twig``).
Zwracaną wartością jest obiekt ``Response`` wypełniony zrenderowaną zawartością strony.
Jeżeli wystąpi taka potrzeba, to obiekt ``Response`` może zostać zmodyfikowany przed
przesłaniem go do przeglądarki::

   public function indexAction()
   {
      $response = $this->render('AcmeDemoBundle:Welcome:index.txt.twig');
      $response->headers->set('Content-Type', 'text/plain');
      
      return $response;
   }

Nie ważne jak to jest robione, ostatecznym celem kontrolera jest zawsze zwrócenie
obiektu ``Response``, który następnie powinien być dostarczony do użytkownika.
Ten obiekt może być wypełniony kodem HTML, reprezentować przekierowanie klienta
lub nawet zwracać zawartość obrazu JPG nagłówka z ``Content-Type image/jpg``.

Nazwa szablonu ``AcmeDemoBundle:Welcome:index.html.twig``, to logiczna nazwa
odwołująca się do pliku ``Resources/views/Welcome/index.html.twig`` wewnątrz
``AcmeDemoBundle` (umieszczonego w ``src/Acme/DemoBundle``). Dalszy rozdział
o pakietach wyjaśnia, dlaczego jest to takie użyteczne.

Teraz ponownie zajrzyj do konfiguracji tras i znajdź klucz ``_demo``:

.. code-block:: yaml
   :linenos:
   
   # app/config/routing_dev.yml
   _demo:
      resource: "@AcmeDemoBundle/Controller/DemoController.php"
      type:     annotatio
      prefix:   /demo
   
*Nazwa logiczna* pliku zawierajacego trasę ``_demo``, to
``@AcmeDemoBundle/Controller/DemoController.php`` i odnosi się do pliku
``src/Acme/DemoBundle/Controller/DemoController.php``.
W pliku tym trasy są określone jako adnotacje o metodach działania::

   // src/Acme/DemoBundle/Controller/DemoController.php
   use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
   use Sensio\Bundle\FrameworkExtraBundle\Configuration\Template;
   
   class DemoController extends Controller
   {
      /**
      * @Route("/hello/{name}", name="_demo_hello")
      * @Template()
      */
      public function helloAction($name)
      {
         return array('name' => $name);
      }
      
      // ...
   }

Adnotacja ``@Route()`` tworzy nową trasę dopasowująca ścieżkę ``/hello/{name}``
w metodzie ``helloAction()``. Dowolny ciąg znakowy w nawiasach klamrowych, taki
jak ``{name}``, jest traktowany jako zmienna, która może być pobrana bezpośrednio
z argumentu metody i o tej samej nazwie.

Jeżeli przyjrzeć się dokładniej kodowi kontrolera, to można zauważyć, ze zamiast
renderowania szablonu i zwrócenia obiektu ``Response``, jak poprzednio, zwracana
teraz jest tablica parametrów. Adnotacja ``@Template()`` powiadamia Symfony aby
renderował szablon, przechodząc w każdej z tych zmiennych z tablicy do szablonu.
Nazwa renderowanego szablonu występuje po nazwie kontrolera. Tak więc w tym przykładzie,
renderowany jest szablon ``AcmeDemoBundle:Demo:hello.html.twig`` (znajduje się w
``src/Acme/DemoBundle/Resources/views/Demo/hello.html.twig``).

Szablony
~~~~~~~~

Kontroler renderuje szablon ``src/Acme/DemoBundle/Resources/views/Demo/hello.html.twig``
(lub ``AcmeDemoBundle:Demo:hello.html.twig`` jeśli używa się logicznej nazwy):

.. code-block:: html+jinja
   :linenos:
      
   {# src/Acme/DemoBundle/Resources/views/Demo/hello.html.twig #}
   {% extends "AcmeDemoBundle::layout.html.twig" %}
   
   {% block title "Hello " ~ name %}
   
   {% block content %}
      <h1>Hello {{ name }}!</h1>
   {% endblock %}

Symfony2 stosuje domyślnie silnik szablonów `Twig`_,
ale można również korzystać z tradycyjnych szablonów PHP.
:doc:`Natępny rozdział opisuje </quick_tour/the_view>`
jak działają szablony w Symfony2.

Pakiety
~~~~~~~

Może zastanawiałeś się, do czego odnosi się słowo :term:`pakiet` (*ang. bundle*),
które już kilkakrotnie zostało użyte wcześniej? Cały kod tworzony dla jakiejś aplikacji
jest zorganizowany w pakiety. W Symfony2 mówi się, że pakiet, to uporządkowany zestaw plików
(plików PHP, arkuszy stylów, skryptów JavaScript, obrazów, ...), które implementują
pojedyńczą funkcjonalność (blog, forum, ...) i które mogą być łatwo udostępniane
innym programistom. Dotąd manipulowaliśmy jednym pakietem - ``AcmeDemoBundle``.
Dowiesz się więcej na temat pakietów w :doc:`ostatniej części tego przewodnika
</quick_tour/the_architecture>`.

.. _quick-tour-big-picture-environments:

Praca ze środowiskami
---------------------

Teraz, gdy już lepiej rozumiemy działanie Symfony2, przyjrzymy sie bliżej stopce
renderowanej na każdej stronie Symfony2. Możesz tam zauważyć mały pasek z logo Symfony2.
Jest on nazywany "paskiem debugowania" (*ang. "Web Debug Toolbar"*) i jest to najlepszy
przyjaciel programisty.

.. image:: /images/quick_tour/web_debug_toolbar.png
   :align: center
   
To co teraz można zobaczyć, jest tylko „wierzchołkiem góry lodowej”.
Klikniecie na jakąkolwiek sekcję paska otworzy profiler i będzie można uzyskać
znacznie więcej informacji o żądaniu, parametrach zapytania, szczegółach zabezpieczeń
i kwerendach bazy danych:

.. image:: /images/quick_tour/profiler.png
   :align: center

Oczywiście nie będziesz chciał pokazywać tych narzędzi w środowisku produkcyjnym witryny.
Dlatego znajdziesz w katalogu ``web/`` inny kontroler wejścia (``app.php``), który
jest zoptymalizowany dla środowiska produkcyjnego.

.. _quick-tour-big-picture-environments-intro:

Co to jest środowisko?
~~~~~~~~~~~~~~~~~~~~~~

:term:`Środowisko <środowisko>` reprezentuje grupę konfiguracji wykorzystywanych
podczas uruchamiania aplikacji. Symfony2 definiuje domyślnie dwa środowiska:
``dev`` (wykorzystywane lokalnie przy pracach programistycznych nad aplikacją)
i ``prod`` (zoptymalizowane dla wykonywania aplikacji na serwerze produkcyjnym).

Zazwyczaj, środowiska udostępniają dużą ilość opcji konfiguracyjnych. Z tego powodu,
wspólne opcje konfiguracyjne wstawiane są do wspólnego pliku ``config.yml``
i nadpisywane w razie potrzeby przez opcje umieszczane w pliku konfiguracyjnym
specyficznym dla środowiska:


.. code-block:: yaml
   :linenos:

    # app/config/config_dev.yml
    imports:
        - { resource: config.yml }

    web_profiler:
        toolbar: true
        intercept_redirects: false

W tym przykładzie, środowisko ``dev`` ładuje plik konfiguracyjny ``config_dev.yml``,
który sam importuje wspólny plik ``config.yml`` i modyfikuje go, udostępniając
pasek narzędziowy debugowania.

Po odwiedzeniu w przegladarce pliku``app_dev.php``, wykonuje się aplikację Symfony
w środowisku ``dev``. W celu uruchomienia aplikacji w środowisku ``prod``, trzeba
odwiedzić zamiast plik ``app.php``. 

Trasy demo są dostęþne w naszej aplikacji tylko w śodowisku ``dev``.
Dlatego, jeśli spróbuje się uzyskać dostęp z adresu
``http://localhost/app.php/demo/hello/Fabien``, otrzyma się błąd 404.

.. tip::
   
   Jeśli zamiast wbudowanego serwera internetowego PHP użyje się Apache z włączonym
   modułem ``mod_rewrite`` i wykorzysta się plik ``.htaccess``, zapewni to dostęp
   do aplikacji Symfony2 w katalogu ``web/``, nawet z możliwością pominięcia w adresie
   URL nazwy pliku``app.php``. Domyślnie ``.htaccess`` wskazuje wszystkie żądania
   kontrolerowi wejścia ``app.php``:

   .. code-block:: text
      
      http://localhost/demo/hello/Fabien   

Więcej szczegółów o środowiskach można znaleźć w rozdziale
":ref:`Środowidka i kontroler wejścia <page-creation-environments>`".

Podsumowanie
------------

Gratulacje! Miałeś Czytelniku przedsmak kodowania Symfony2. To nie było tak trudne, prawda?
Jest dużo więcej do odkrycia, ale teraz trzeba zobaczyć, jak Symfony2 sprawia,
że ​​naprawdę łatwo jest wdrożyć strony internetowe. Jeśli chcesz się dowiedzieć
więcej o Symfony2, zacznij lekturę następnej część przewodnika: ":doc:`the_view`.

.. _Composer:             https://getcomposer.org/
.. _executable installer: http://getcomposer.org/download
.. _Twig:                 http://twig.sensiolabs.org/

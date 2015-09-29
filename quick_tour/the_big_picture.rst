... highlight:: php
   :linenothreshold: 2

Przegląd
========

<<<<<<< HEAD
Zacznij korzystać z Symfony w 10 minut! Ten rozdział wyjaśnia kilka
najważniejszych pojęć Symfony i to, jak można rozpocząć
szybko pracę, demonstrując prosty przykład aplikacji.
=======
Zacznij korzystać z Symfony2 w 10 minut! Ten rozdział wyjaśni Ci kilka
najważniejszych pojęć Symfony2 i to, jak można rozpocząć
szybko pracę, pokazując w działaniu prosty przykład aplikacji.
>>>>>>> refs/heads/nowe_rozdz

Jeśli Czytelnik kiedykolwiek używał frameworka aplikacji internetowej, to z Symfony
powinien czuć się jak w domu.
Jeśli nie, to zapraszamy do poznania zupełnie nowego sposobu tworzenia aplikacji
internetowych.

<<<<<<< HEAD
Jedynym wymaganiem do śledzenia wykonywanego tu przykładu jest posiadanie **na swoim
komputerze instalacji PHP 5.4 lub wersji wyżej**. Jeśli używasz jakieś rozwiązanie
pakietowe PHP, takie jak WAMP, XAMP lub MAMP, sprawdź, czy ta instalacja używa 
PHP 5.4 lub wersji wyższej. Możesz również wykonać następujace polecenie w swoim
terminalu lub konsoli poleceń, aby wyświetlić informację o zaistalowanej wersji
PHP:

.. code-block:: bash

    $ php --version

.. _installing-symfony2:
=======
>>>>>>> refs/heads/nowe_rozdz
    
<<<<<<< HEAD
Instalowanie Symfony
--------------------
=======
Instalowanie Symfony2
---------------------
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
W przeszłości musieliśmy instalować Symfony ręcznie, dla każdego nowego projektu.
Teraz możemy wykorzystać **Symfony Installer**, który musi być najpierw zainstalowany
na komputerze przed pierwszym użyciem Symfony.
=======
Najpierw, sprawdź zainstalowaną na swoim komputerze wersję wersję PHP, gdyż Symfony2
wymaga wersji 5.3.3 lub wyższej. Otwórz konsolę i wykonaj następujące polecenie
instalujące najnowszą wersję Symfony2 w katalogu ``myproject/``:
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
Na systemach **Linux** i **Mac OS X**, trzeba wykonać następujace polecenia konsolowe:
=======
.. code-block:: bash
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
.. code-block:: bash

    $ curl -LsS http://symfony.com/installer > symfony.phar
    $ sudo mv symfony.phar /usr/local/bin/symfony
    $ chmod a+x /usr/local/bin/symfony

Po zainstalowaniu instalatora Symfony, trzeba otworzyć nowe okno konsoli, aby
wykonać nowe polecenie ``symfony``:

.. code-block:: bash

    $ symfony

Na systemach **Windows**, trzeba wykonać następujące polecenie konsolowe:

.. code-block:: bash

    c:\> php -r "readfile('http://symfony.com/installer');" > symfony.phar

Polecenie to pobierze plik o nazwie ``symfony.phar``, który zawiera instalatora
Symfony. Zapisz lub przenieś ten plik do katalogu, w którym mają być tworzone
projekty Symfony i następnie uruchom instalator Symfony, takim
poleceniem:

.. code-block:: bash
=======
    $ composer create-project symfony/framework-standard-edition myproject/ '~2.3'
>>>>>>> refs/heads/nowe_rozdz

    c:\> php symfony.phar

<<<<<<< HEAD
Utworzenie pierwszego projektu Symfony
--------------------------------------
=======
    `Composer`_, to menadżer pakietów wykorzystywany w nowoczesnych aplikacjach
    PHP i jedyny zalecany sposób instalacji Symfony2. W celu zainstalowania
    programu Composer w systemie Linux lub Mac, wykonaj następujące polecenia:
>>>>>>> refs/heads/nowe_rozdz

Gdy już Symfony Installer jest ustawiony, można utworzyć nowy projekt Symfony,
stosujac polecenie ``new``. Utwórzmy nowy projekt o nazwie ``myproject``:

<<<<<<< HEAD
.. code-block:: bash
=======
        $ curl -sS https://getcomposer.org/installer | php
        $ sudo mv composer.phar /usr/local/bin/composer
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
    # Linux and Mac OS X
    $ symfony new myproject
=======
    Dla zainstalowania programu Composer w systemie Windows, trzeba
    pobrac `executable installer`_.
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
    # Windows
    c:\> php symfony.phar new myproject
=======
Pamiętaj, że w czasie pierwszej instalacji Symfony2 może minąć kilka minut zanim
zostaną pobrane wszystkie komponenty. Pod koniec procesu instalacji instalator
poprosi o odpowiedź na kilka pytań. W tym przykładowym projekcie nie jest ona istotna,
więc prosze wcisnąc klawisz ``<Enter>`` przy każdym pytaniu, akceptujac odpowiedzi
domyślne.
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
Polecenie to pobierze najnowszą stabilną wersję Symfony i utworzy pusty projekt
w katalogu ``myproject/``, dzięki czemu od razu można rozpocząć tworzenie swojej
aplikacji.
=======
Uruchomienie Symfony2
---------------------
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
.. _running-symfony2:
=======
Przed pierwszym uruchomieniem Symfony2, należy wykonać następujące polecenie,
aby upewnić się, że system spełnia wszystkie techniczne wymagania:
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
Uruchomienie Symfony
--------------------
=======
.. code-block:: bash
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
W tym poradniku wykorzytujemy do uruchamiania aplikacji Symfony wewnętrzny serwer
internetowy dostarczany przez PHP. Dlatego, uruchamianie aplikacji serwera sprowadza
się do skonfigurowania się w katalogu projektu i wykonaniu polecenia:
=======
    $ cd myproject/
    $ php app/check.php
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
.. code-block:: bash
=======
Napraw błędy zgłoszone przez polecenie i następnie wykorzystaj wbudowany w PHP
serwer internetowy do uruchomienia aplikacji Symfony:
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
    $ cd myproject/
    $ php app/console server:run
=======
.. code-block:: bash
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
Otwórz przegladarkę i przejdź do adresu URL ``http://localhost:8000/app/example``,
co powinno wyświetlić stronę powitalną :
=======
    $ php app/console server:run
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
=======
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

>>>>>>> refs/heads/nowe_rozdz
.. image:: /images/quick_tour/welcome.png
   :align: center
<<<<<<< HEAD
   :alt: Symfony Welcome Page
=======
   :alt:   Symfony2 Welcome Page
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
Gratulujemy! Twój pierwszy projekt Symfony jest gotowy do pracy.
=======
Podstawy
--------
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
.. note::

    Zamiast strony powitalnej można czasem zobaczyć pustą stronę lub stronę błędu.
    Jest to spowodowane błędem uprawnień dostępu do katalogu. Jest kilka
    możliwych rozwiązań, w zależności od systemu operacyjnego. Sposoby te są
    omówione w rozdziale :ref:`Ustawianie uprawnień <book-installation-permissions>`
    podręcznika.

    Jeśli strona powitalna nie została zrenderowana z aktywami CSS lub obrazów,
    zainstaluj je:

    .. code-block:: bash

        $ php app/console assets:install

Serwer internetowy można zatrzymać, po zakończeniu pracy z aplikacją Symfony,
wciskając klucz Ctrl+C.
=======
Jednym z głównych celów frameworka (platformy aplikacyjnej) jest dobre zorganizowanie
kodu i umożliwienie łatwego rozwoju aplikacji, z uniknięciem mieszania w jednym
skrypcie wywołań bazy danych, znaczników HTML i biznesowej logiki. Dla osiągnięcia
tego celu z Symfony, najpierw musisz poznać kilka podstawowych pojęć i terminów.
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
.. tip::

    Jeśli wolisz pracować z tradycyjnym serwerem internetowym, takim jak Apache
    lub Nginx, przeczytaj artykuł
    :doc:`/cookbook/configuration/web_server_configuration`.

=======
Symfony dostarczane jest z przykładowym kodem, który można wykorzystać do nauczenia
się więcej o podstawowych pojęciach. Przejdź do następującego adresu URL, aby zobaczyć
stronę powitalną Symfony2 (zamień *Fabien* na swoje imię):
>>>>>>> refs/heads/nowe_rozdz

Podstawy
--------

<<<<<<< HEAD
Jednym z głównych celów frameworka (platformy aplikacyjnej) jest dobre zorganizowanie
kodu i umożliwienie łatwego rozwoju aplikacji, z uniknięciem mieszania w jednym
skrypcie wywołań bazy danych, znaczników HTML i logiki biznesowej. W celu zrozumienia,
jak to działa w Symfony, najpierw musisz poznać kilka podstawowych pojęć i terminów.
=======
    http://localhost:8000/app_dev.php/demo/hello/Fabien
>>>>>>> refs/heads/nowe_rozdz

Podczas tworzenia aplikacji Symfony, rola programisty polaga na napisaniu kodu,
który odwzorowuje *żądanie* użytkownika (np.  ``http://localhost:8000/app/example``)
na *zasób* związany z tym żądaniem (stron HTML ``Homepage``).

<<<<<<< HEAD
Kod, który ma być wykonany, jest zdefiniowany w **akcjach** i **kontrolerach**.
Odzwzorowanie pomiędzy żądaniem użytkownika a tym kodem jest zdefiniowane w
konfiguracji **trasowania** .
Treści wyświetlane w przeglądarce są zazwyczaj renderowane przy użyciu **szablonów**.
=======
.. note::

    Może się zdarzyć, że zamiast strony powitalnej, zobaczy się pustą stronę lub
    stronę błędu. Jest to spowodowane błędem uprawnień do katalogu aplikacji.
    Jest kilka możliwych rozwiązań tego problemu. Wszystkie z nich omówione są
    w rozdziale :ref:`Ustawienie uprawnień <book-installation-permissions>`
    oficjalnego podręcznika Symfony 2.


Co się tutaj dzieje? Spróbujmy przeanalizować adres URL:
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
Kiedy wywołało się ``http://localhost:8000/app/example``, Symfony wykonał kod
kontrolera, zdefiniowany w pliku ``src/AppBundle/Controller/DefaultController.php``
i zrenderował szablon ``app/Resources/views/default/index.html.twig``.
W następnym rozdziale dowiesz się o szczegółach wewnętrznego funkcjonowania kontrolerów
Symfony, trasach i szablonach.
=======
* ``app_dev.php``: Jest to :term:`kontroler wejścia` - unikalny punkt wejścia
  aplikacji, który odpowiedzialny za wszystkie żądania użytkownika;
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
Akcje i kontrolery
~~~~~~~~~~~~~~~~~~
=======
* ``/demo/hello/Fabien``: Jest to *wirtualna ścieżka* do zasobu jaki chce uzyskać
  użytkownik.
>>>>>>> refs/heads/nowe_rozdz

Otwórz plik ``src/AppBundle/Controller/DefaultController.php`` i obejrzyj zawarty
tam kod (na razie nie będziemy zajmować się konfiguracją ``@Route``, ponieważ
zostanie to wyjaśnione w następnym rozdziale)::

    namespace AppBundle\Controller;

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;

    class DefaultController extends Controller
    {
        /**
         * @Route("/app/example", name="homepage")
         */
        public function indexAction()
        {
            return $this->render('default/index.html.twig');
        }
    }

W aplikacji Symfony, **kontrolery**, to klasy PHP, których nazwy są
zakończone słowem ``Controller``. W tym przykładzie kontroler nosi nazwę
``Default`` a klasa PHP ma nazwę ``DefaultController``.

Metody zdefiniowane w kontrolerze są nazywane **akcjami** - zwykle związane są
z jakimś jedym adresem URL aplikacji i ich nazwy kończą się słowem ``Action``.
W naszym przykładzie kontroler ``Default`` ma tylko jedną akcję o nazwie ``index``
i definiuje metodę ``indexAction``.

.. note::
   W niniejszym podręczniku, można się spotkać (jeszcze) ze stosowaniem słowa
   *kontroler* w znaczeniu *akcja*, co zostało przeniesione z dokumentacji
   anglojezycznej. Np. mówiąc, że "trasa odwzorowuje żądanie użytkownika na kontroler",
   trzeba to rozumieć, że "trasa odwzorowuje żądanie użytkownika na odpowiednią
   metodę kontrolera, czyli na akcję. Tak więc, czytajac o "kontrolerze" proszę
   zwracać uwagę na kontekst.    

Kod akcji jest zazwyczaj bardzo krótki - około 10-15 linii kodu - ponieważ akcje
odwołują się do innych części aplikacji, w celu pobrania lub wygenerowania
potrzebnych informacji i renderowania szablonu, aby ostatecznie pokazać wynik
użytkownikowi.

W tym przykładzie akcja ``index`` jest praktycznie pusta, ponieważ nie ma potrzeby
wywoływania innych metod. Akcja ta tylko renderuje szablon z treścią *Homepage*.

Trasowanie
~~~~~~~~~~

<<<<<<< HEAD
System trasowania (*ang. routing*), nazywany też w polskiej literaturze "systemem
przekierowań", w Symfony obsługuje żądania klienta, dopasowując ścieżkę dostępu
(zawartą w adresie URL) do skonfigurowanych wzorców tras i przekazuje sterowanie
właściwej akcji.
Otwórzmy ponownie plik ``src/AppBundle/Controller/DefaultController.php`` i skupmy
się na trzech pierwszych liniach metody ``indexAction``::
   
   // src/AppBundle/Controller/DefaultController.php
    namespace AppBundle\Controller;
=======
System trasowania (*ang. routing*), nazywany też w polskiej literaturze "systemem przekierowań",
w Symfony2 obsługuje żądania klienta, dopasowując ścieżkę dostępu (zawartą w adresie URL)
do skonfigurowanych wzorców tras i przekazaniu sterowania właściwemu kontrolerowi.
Domyślnie wzorce te są zdefiniowane w pliku ``app/config/routing.yml``:
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
=======

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
:class:`Acme\\DemoBundle\\Controller\\WelcomeController`::

    // src/Acme/DemoBundle/Controller/WelcomeController.php
    namespace Acme\DemoBundle\Controller;

>>>>>>> refs/heads/nowe_rozdz
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;

    class DefaultController extends Controller
    {
        /**
         * @Route("/app/example", name="homepage")
         */
        public function indexAction()
        {
            return $this->render('default/index.html.twig');
        }
    }

Te trzy pierwsze linie definiują konfigurację trasowania przy użyciu adnotacji
``@Route()``. **Adnotacja PHP** jest wygodnym sposobem konfigurowania metody bez
konieczności pisania zwykłego kodu PHP. Trzeba pamiętać, że bloki adnotacji
rozpoczynają się od ``/**``, natomiast zwykłe komentarze od ``/*``.

<<<<<<< HEAD
Pierwsza wartość adnoacji ``@Route()`` określa adres URL, która spowoduje wykonanie
określonej akcji. ponieważ nie trzeba dodawać schematu i hosta z adresu URL
(np. ``http://example.com``), te adresy URL są zawsze względne i nazywamy je
*ściezkami*. W naszym przypadku, ścieżka ``/app/example`` odnosi się do aplikacji
homepage. Druga wartość adnotacji ``@Route()`` (tj. ``name="homepage"``) jest
opcjonalna i ustawia nazwę tej trasy. Na razie ta nazwa jest nieprzydatna, ale
później stanie się potrzebna do linkowania stron.
=======
    Można używać pełnej nazwy klasy i metody - 
    ``Acme\DemoBundle\Controller\WelcomeController::indexAction`` dla wartości
    ``_controller``. Jeśli jendnak chcesz wykorzystywać proste konwencje, używaj
    nazwy logicznej, która jest krótsza i pozwala na większą elastyczność.
>>>>>>> refs/heads/nowe_rozdz

Uwzględniajac to wszystko, adnotacja ``@Route("/app/example", name="homepage")``
tworzy nową trasę o nazwie ``homepage``, co powoduje, że Symfony wykonuje akcję
``index`` kontrolera ``Default``, gdy użytkownik odwiedzi adres URL aplikacji ze
ścieżką ``/app/example``.

<<<<<<< HEAD
.. tip::
=======
Nazwa szablonu ``AcmeDemoBundle:Welcome:index.html.twig``, to logiczna nazwa
odwołująca się do pliku ``Resources/views/Welcome/index.html.twig`` wewnątrz
``AcmeDemoBundle` (umieszczonego w ``src/Acme/DemoBundle``). Dalszy rozdział
o pakietach wyjaśnia, dlaczego jest to takie użyteczne.
>>>>>>> refs/heads/nowe_rozdz

    Oprócz adnotacji PHP, trasy można konfigurować w plikach YAML, XML lub
    PHP, tak jak wyjaśniono to w rozdziale :doc:`Trasowanie </book/routing>`
    podręcznika Symfony. ta elastyczność jest jedną z głównych cech frameworka
    Symfony, który nigdy nie narzuca konkrenego formatu konfiguracji.

<<<<<<< HEAD
=======
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
W pliku tym trasy są określone jako adnotacje w metodach akcji::

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

>>>>>>> refs/heads/nowe_rozdz
Szablony
~~~~~~~~

Jedyną zawartością akcji ``index`` jest instrukcja PHP::

    return $this->render('default/index.html.twig');

Metoda ``$this->render()`` jest wygodnym skrótem renderującym szablon.
Symfony dostarcza kilka przydatnych skrótów do każdego kontrolera rozszerzającego
klasę ``Controller``.

Domyślnie, szablony aplikacji są przechowywane w katalogu ``app/Resources/views/``.
Dlatego szablon ``default/index.html.twig``, to to samo co
``app/Resources/views/default/index.html.twig``. Otwórz ten plik i przyjrzyj się
temu kodowi:

.. code-block:: html+jinja

<<<<<<< HEAD
    {# app/Resources/views/default/index.html.twig #}
    {% extends 'base.html.twig' %}
=======
Symfony2 stosuje domyślnie silnik szablonów `Twig`_,
ale można również korzystać z tradycyjnych szablonów PHP.
:doc:`Natępny rozdział opisuje </quick_tour/the_view>`
jak działają szablony w Symfony2.
>>>>>>> refs/heads/nowe_rozdz

    {% block body %}
        Homepage.
    {% endblock %}

<<<<<<< HEAD
Szablon ten jest utworzony w `Twig`_, nowym silniku szablonowania, przeznaczonym
dla nowoczesnych aplikacji PHP.
:doc:`Druga część tego poradnika </quick_tour/the_view>` wyjaśni Ci, jak działają
szablony w Symfony.
=======
Może zastanawiałeś się, do czego odnosi się słowo :term:`pakiet` (*ang. bundle*),
które już kilkakrotnie zostało użyte wcześniej? Cały kod tworzony dla jakiejś aplikacji
jest zorganizowany w pakiety. W Symfony2 mówi się, że pakiet, to uporządkowany zestaw plików
(plików PHP, arkuszy stylów, skryptów JavaScript, obrazów, ...), które implementują
pojedyńczą funkcjonalność (blog, forum, ...) i które mogą być łatwo udostępniane
innym programistom. Dotąd manipulowaliśmy jednym pakietem - ``AcmeDemoBundle``.
Dowiesz się więcej na temat pakietów w :doc:`ostatniej części tego przewodnika
</quick_tour/the_architecture>`.
>>>>>>> refs/heads/nowe_rozdz

.. _quick-tour-big-picture-environments:

Praca ze środowiskami
---------------------

Teraz, gdy już lepiej rozumiemy działanie Symfony, przyjrzymy się bliżej stopce
renderowanej na każdej stronie Symfony. Możesz tam zauważyć mały pasek z logo Symfony.
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

<<<<<<< HEAD
Narzędzie to dostrcza bardzo dużo wewnetrznej informacji o aplikacji, dlatego z
pewnością nie można jej pokazywać publicznie. Symfony nie pokazuje tego paska
narzędziowego, gdy aplikacja jest uruchamiana na serwerze produkcyjnym w "trybie
publicznym".
=======
Oczywiście nie będziesz chciał pokazywać tych narzędzi w środowisku produkcyjnym witryny.
Dlatego znajdziesz w katalogu ``web/`` inny kontroler wejścia (``app.php``), który
jest zoptymalizowany dla środowiska produkcyjnego.
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
Skąd Symfony wie, czy aplikacja ma być uruchomiona w "trybie publicznym"? Dowiesz
się tego czytając o pojeciu **środowisko wykonawcze**.
=======
.. _quick-tour-big-picture-environments-intro:
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
.. _quick-tour-big-picture-environments-intro:
=======
Co to jest środowisko?
~~~~~~~~~~~~~~~~~~~~~~
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
Co to jest środowisko?
~~~~~~~~~~~~~~~~~~~~~~
=======
:term:`Środowisko <środowisko>` reprezentuje grupę konfiguracji wykorzystywanych
podczas uruchamiania aplikacji. Symfony2 definiuje domyślnie dwa środowiska:
``dev`` (wykorzystywane lokalnie przy pracach programistycznych nad aplikacją)
i ``prod`` (zoptymalizowane dla wykonywania aplikacji na serwerze produkcyjnym).
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
:term:`Środowisko <środowisko>` reprezentuje grupę konfiguracji wykorzystywanych
podczas uruchamiania aplikacji. Symfony definiuje domyślnie dwa środowiska:
``dev`` (wykorzystywane lokalnie przy pracach programistycznych nad aplikacją)
i ``prod`` (zoptymalizowane dla wykonywania aplikacji na serwerze produkcyjnym).
=======
Zazwyczaj, środowiska udostępniają dużą ilość opcji konfiguracyjnych. Z tego powodu,
wspólne opcje konfiguracyjne wstawiane są do wspólnego pliku ``config.yml``
i nadpisywane w razie potrzeby przez opcje umieszczane w pliku konfiguracyjnym
specyficznym dla środowiska:
>>>>>>> refs/heads/nowe_rozdz

<<<<<<< HEAD
Gdy odwiedza się w przeglądarce adres ``http://localhost:8000``, wykonuje się
aplikację Symfony w środowisku ``dev``. W celu odwiedzenia tej aplikacji w środowisku
``prod``, trzeba zamiast tego odwiedzić adres ``http://localhost:8000/app.php``.
Jeśli chce się, aby zawsze środowisko ``dev`` było pokazywane w adresie URL,
trzeba odwiedzić adres ``http://localhost:8000/app_dev.php``.

Zasadniczą różnicą pomiędzy tymi środowiskami jest to, że środowisko ``dev`` jest
zoptymalizowane pod względem dostarczania dużej ilości informacji dla programisty,
co oznacza gorszą wydajność. Natomiast środowisko ``prod`` jest zoptymalizowane
pod kątem jak największej wydajności, co oznacza, że debugowanie nie jest realizowane
i niedostępna jest tego typu informacja, jak też cały pasek debugowania.

Druga różnica między środowskami polega na używaniu przez aplikację innych opcji
konfiguracyjnych w poszczególnych środowiskach. Gdy ma się dostęp do środowiska
``dev``, Symfony ładuje plik konfiguracyjny ``app/config/config_dev.yml``,
a w środowisku ``prod``, plik ``app/config/config_prod.yml``.

Zazwyczaj, środowiska udostępniają dużą ilość opcji konfiguracyjnych. Z tego powodu,
wspólne opcje konfiguracyjne wstawiane są do wspólnego pliku ``config.yml``
i nadpisywane w razie potrzeby przez opcje umieszczane w pliku konfiguracyjnym
specyficznym dla środowiska:

=======
>>>>>>> refs/heads/nowe_rozdz

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

<<<<<<< HEAD
Więcej szczegółów o środowiskach można znaleźć w artykule
=======
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
>>>>>>> refs/heads/nowe_rozdz
":ref:`Środowidka i kontroler wejścia <page-creation-environments>`".

Podsumowanie
------------

Gratulacje! Miałeś Czytelniku przedsmak kodowania Symfony. To nie było tak trudne, prawda?
Jest dużo więcej do odkrycia, ale teraz trzeba zobaczyć, jak Symfony sprawia,
że ​​naprawdę łatwo jest wdrożyć strony internetowe. Jeśli chcesz się dowiedzieć
więcej o Symfony, zacznij lekturę następnej części przewodnika: ":doc:`the_view`.

.. _Composer:             https://getcomposer.org/
.. _executable installer: http://getcomposer.org/download
.. _Twig:                 http://twig.sensiolabs.org/

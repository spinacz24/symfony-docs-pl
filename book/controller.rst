.. highlight:: php
   :linenothreshold: 2

.. index::
   kontroler, akcja

.. _book-controller:

Kontroler i akcje
=================

Ogólnie rzecz biorąc, kontrolerem (w znaczeniu `wzorca projektowego Model-Widok-Kontroler`_)
może być dowolny, możliwy do wywołania, kod PHP (funkcja, metoda obiektu czy domknięcie).
Jednakże w Symfony kontrolerem (w tym znaczeniu) jest pojedyncza metoda w klasie
:term:`kontrolera <kontroler>`. W takim znaczeniu *kontroler ze wzorca MVC* nazywany
jest powszechnie **akcją**. Klasa kontrolera, to po prostu wygodny sposób na
grupowanie kontrolerów. Używając słowa "kontroler", trzeba zwracać uwagę na kontekst,
bo może ono oznaczać klasę kontrolera albo metodę tej klasy, czyli akcję. 

W Symfony, *akcje* są odpowiedzialne za przetworzenie obiektu ``Request`` oraz
utworzenie i zwrócenie odpowiedzi HTTP w postaci obiektu ``Response``.
Odpowiedź może być stroną HTML, dokumentem XML, serializowaną tablicą JSON, obrazem,
przekierowaniem, stroną błędu 404 lub czymkolwiek innym. Akcje mogą zawierać dowolną
logikę potrzebną do wyrenderowania zawartości strony.

Aby zobaczyć, jakie to proste, spójrzmy jak działa akcja Symfony.
Poniższa akcja wygeneruje stronę wyświetlającą ``Hello world!``::

    use Symfony\Component\HttpFoundation\Response;

    public function helloAction()
    {
        return new Response('Hello world!');
    }

Zadanie akcji jest zawsze takie samo: stworzyć i zwrócić obiekt ``Response``.
Po drodze moża ona odczytywać informacje z żądania, wczytywać dane z bazy danych,
wysłać email, czy zapisać informacje w sesji użytkownika. Jednakże w każdym przypadku
kontroler ostatecznie zwróci obiekt ``Response``, który będzie dostarczony z powrotem
do użytkownika.

Nie ma tutaj żadnej magii czy innych wymagań, o które trzeba się martwić. Oto kilka
najczęstszych przypadków:

- *Akcja A* przygotowuje obiekt ``Response`` reprezentujący zawartość strony głównej
  witryny.

- *Akcja B* odczytuje parametr ``slug`` z żądania, aby pobrać wpis blogu
  z bazy danych i utworzyć obiekt ``Response``, który wyświetli tego blogu. Jeśli
  ``slug`` nie zostanie znaleziony w bazie danych, akcja utworzy obiekt ``Response``
  zawierający kod błędu 404.

- *Akcja C* obsługuje formularz kontaktowy. Odczytuje dane formularza z żądania HTTP,
  zapisuje dane kontaktowe do bazy danych i wysyła email do administratora. W końcu tworzy
  obiekt ``Response``, który przekieruje przeglądarkę klienta do strony z podziękowaniami.

.. index::
   single: cykl żądanie-akcja-odpowiedź

Cykl żądanie-akcja-odpowiedź
----------------------------

Każde żądanie obsługiwane przez projekt Symfony przechodzi ten sam cykl.
Framework zajmuje się powtarzającymi się czynnościami i w końcu wykonuje akcję
(metodę kontrolera), która zawiera indywidualny kod aplikacji:

#. Każde żądanie jest obsługiwane przez pojedynczy plik
   :term:`kontrolera wejścia <kontroler wejścia>` 
   (np. ``app.php`` czy ``app_dev.php``), który inicjuje aplikację i tworzy
   obiekt ``Request``;

#. ``Router`` odczytuje informacje z obiekt ``Request`` (np. URI), znajduje trasę,
   która pasuje do tej informacji oraz odczytuje parametr ``_controller`` dopasowanej
   trasy;

#. Wykonywana jest określona akcja z dopasowaną trasą a kod w niej
   zawarty tworzy i zwraca obiekt ``Response``;

#. Do klienta odsyłane są nagłówki HTTP i zawartość obiektu ``Response``.

Tworzenie strony sprowadza się do utworzenie kontrolera i akcji (#3) oraz odpowiedniego
zdefiniowania trasy, która odwzorowuje ścieżkę URL na akcję kontrolera (#2).

.. note::

    Pomimo podobnej nazwy, "kontroler wejścia" (zwany też kontrolerem fasady)
    zupełnie różni się od "kontrolerów", o których mówimy w tym rozdziale.
    :term:`Kontroler wejścia <kontroler wejścia>`
    to krótki plik PHP, który znajduje się w katalogu `web` aplikacji i do którego
    kierowane są wszystkie przechodzące żądania HTTP. Typowa aplikacja posiada
    kontroler wejścia dla środowiska produkcyjnego (np. ``app.php``) i kontroler
    wejścia dla środowiska programistycznego (np. ``app_dev.php``). Kontroler wejścia
    jest implemetacją wzorca projektowego *Front Controller*.
    Prawdopodobnie nigdy nie będziesz musiał edytować, przeglądać czy martwić się
    o kontrolery wejścia swojej aplikacji.

.. index::
   single: kontroler; prosty przykład
   single: kontroler; wzorzec Model-Widok-Kontroler

Prosty kontroler
----------------

Przyjmijmy do analizy bardzo prosty kod kotrolera: 

.. code-block:: php
    :linenos:

    // src/AppBundle/Controller/HelloController.php
    namespace AppBundle\Controller;
    use Symfony\Component\HttpFoundation\Response;

    class HelloController
    {
        public function indexAction($name)
        {
            return new Response('<html><body>Hello '.$name.'!</body></html>');
        }
    }

* *linie 2*: Symfony wykorzystuje przestrzeń nazewniczą PHP,
  aby nazwać całą klasę kontrolera. Tak przyjęta nazwa, pozwala uniknąć konfliktów
  nazewniczych;
* *linia 3*: Słowo kluczowe ``use`` importuje klasę ``Response``, którą nasz
  kontroler musi przetworzyć i zwrócić.
* *linia 6*: Nazwa klasy to połączenie nazwy kontrolera (np. ``Hello``)
  i słowa ``Controller``. Jest to konwencja zapewniająca zgodność nazewniczą kontrolerów
  i pozwalająca na odwoływanie się do nich w konfiguracji trasowania wyłącznie przez
  pierwszą część ich nazwy (np. ``Hello``).
* *linia 8*: Każda nazwa akcji w klasie kontrolera posiada przyrostek ``Action``
  i odwołuje się do konfiguracji trasowania poprzez nazwę akcji (``index``).
  W następnym rozdziale utworzymy trasę, która będzie odwzorowywać ścieżkę URL na
  akcję. Nauczysz się jak wieloznaczniki (*ang. placeholders*) trasy (``{name}``)
  stają się argumentami metody akcji (``$name``).
* *linia 10*: Kontroler tworzy i zwraca obiekt ``Response``.

.. index::
   single: kontroler; trasa
   single: akcja; trasa

Odwzorowanie URL na kontroler
-----------------------------

Nowy kontroler zwraca prostą stronę HTML. Aby móc zobaczyć tą stronę w przeglądarce,
trzeba utworzyć trasę (*ang. route*) odwzorowującą wzorzec ścieżki URL na kontroler:

.. configuration-block::

    .. code-block:: php-annotations

        // src/AppBundle/Controller/HelloController.php
        namespace AppBundle\Controller;

        use Symfony\Component\HttpFoundation\Response;
        use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;

        class HelloController
        {
            /**
             * @Route("/hello/{name}", name="hello")
             */
            public function indexAction($name)
            {
                return new Response('<html><body>Hello '.$name.'!</body></html>');
            }
        }

    .. code-block:: yaml

        # app/config/routing.yml
        hello:
            path:      /hello/{name}
<<<<<<< HEAD
            # uses a special syntax to point to the controller - see note below
            defaults:  { _controller: AppBundle:Hello:index }
=======
            defaults:  { _controller: AcmeHelloBundle:Hello:index }
>>>>>>> refs/heads/nowe_rozdz

    .. code-block:: xml

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="hello" path="/hello/{name}">
<<<<<<< HEAD
                <!-- uses a special syntax to point to the controller - see note below -->
                <default key="_controller">AppBundle:Hello:index</default>
=======
                <default key="_controller">AcmeHelloBundle:Hello:index</default>
>>>>>>> refs/heads/nowe_rozdz
            </route>
        </routes>

    .. code-block:: php

        // app/config/routing.php
        use Symfony\Component\Routing\Route;
        use Symfony\Component\Routing\RouteCollection;

        $collection = new RouteCollection();
        $collection->add('hello', new Route('/hello/{name}', array(
            // uses a special syntax to point to the controller - see note below
            '_controller' => 'AppBundle:Hello:index',
        )));

        return $collection;

        return $collection;


Teraz, po wprowdzeniu ścieżki ``/hello/ryan`` (np. ``http://localhost:8000/hello/ryan``
gdy stosuje się :doc:`wbudowany serwer internetowy </cookbook/web_server/built_in>`)
wykonany zostanie kontroler ``HelloController::indexAction()`` i do zmiennej
``$name`` zostanie przekazana wartość ``ryan``. Tworzenie "strony" sprowadza się
do utworzenie akcji i powiązania jej z trasą.

Proste, prawda?

.. sidebar:: Skrótowa nazwa akcji: AppBundle:Hello:index

    Jeśli przy trasowaniu stosuje sie formaty YML lub XML, trzeba odnieść się do
    kontrolera używając specjalną składnię skrótu: ``AppBundle:Hello:index``.
    Wiecej szczegółów o formacie kontrolera mozna znaleźć w :ref:`controller-string-syntax`.

.. seealso::

    Możesz dowiedzieć się więcej o systemie trasowania w rozdziale
    :doc:`routing`.


.. index::
   single: kontroler; argumenty akcji
   single: akcje; argumenty

.. _route-parameters-controller-arguments:

Parametry trasy jako argumenty akcji
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Już wiemy, że trasa wskazuje na metodę ``HelloController::indexAction()``
znajdującą się wewnątrz ``AcmeHelloBundle``. Co ciekwsze, jest też argument
przekazywany do tej metody::

    // src/AppBundle/Controller/HelloController.php
    // ...
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;

<<<<<<< HEAD
    /**
     * @Route("/hello/{name}", name="hello")
     */
    public function indexAction($name)
=======
    namespace Acme\HelloBundle\Controller;
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;

    class HelloController extends Controller
    {
        public function indexAction($name)
        {
          // ...
        }
    }

Kontroler ma pojedynczy argument ``$name``, który odpowiada parametrowi ``{name}``
z dopasowanej trasy (w naszym przykładzie ma on wartość ``ryan``). W rzeczywistości
podczas wykonywania kontrolera Symfony2 dopasowuje każdy argument kontrolera
do parametru trasy. Rozważmy następujący przykład:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/routing.yml
        hello:
            path:      /hello/{firstName}/{lastName}
            defaults:  { _controller: AcmeHelloBundle:Hello:index, color: green }

    .. code-block:: xml
       :linenos:

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="hello" path="/hello/{firstName}/{lastName}">
                <default key="_controller">AcmeHelloBundle:Hello:index</default>
                <default key="color">green</default>
            </route>
        </routes>

    .. code-block:: php
       :linenos:

        // app/config/routing.php
        use Symfony\Component\Routing\Route;
        use Symfony\Component\Routing\RouteCollection;

        $collection = new RouteCollection();
        $collection->add('hello', new Route('/hello/{firstName}/{lastName}', array(
            '_controller' => 'AcmeHelloBundle:Hello:index',
            'color'       => 'green',
        )));

        return $collection;

W tym przykładzie kontroler może przyjąć kilka argumentów::

    public function indexAction($first_name, $last_name, $color)
>>>>>>> refs/heads/nowe_rozdz
    {
        // ...
    }


Akcja ta ma pojedynczy argument ``$name``, który odpowiada parametrowi ``{name}``
z dopasowanej trasy (w naszym przykładzie ma on wartość ``ryan``). W rzeczywistości
podczas wykonywania kontrolera Symfony dopasowuje każdy argument akcji
do parametru trasy. Rozważmy bardziej interesujacy przykład:

.. configuration-block::

    .. code-block:: php-annotations

        // src/AppBundle/Controller/HelloController.php
        // ...

        use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;

        class HelloController
        {
            /**
             * @Route("/hello/{firstName}/{lastName}", name="hello")
             */
            public function indexAction($firstName, $lastName)
            {
                // ...
            }
        }

    .. code-block:: yaml

        # app/config/routing.yml
        hello:
            path:      /hello/{firstName}/{lastName}
            defaults:  { _controller: AppBundle:Hello:index }

    .. code-block:: xml

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="hello" path="/hello/{firstName}/{lastName}">
                <default key="_controller">AppBundle:Hello:index</default>
            </route>
        </routes>

    .. code-block:: php

        // app/config/routing.php
        use Symfony\Component\Routing\Route;
        use Symfony\Component\Routing\RouteCollection;

        $collection = new RouteCollection();
        $collection->add('hello', new Route('/hello/{firstName}/{lastName}', array(
            '_controller' => 'AppBundle:Hello:index',
        )));

        return $collection;

Teraz akcja może mieć dwa argumenty::

    public function indexAction($firstName, $lastName)
    {
        // ...
    }

Odwzorowanie parametrów trasy na argumenty akcji jest łatwe i elastyczne.
Należy pamiętać o następujących wskazówkach:

* **Kolejność argumentów akcji nie ma znaczenia**

<<<<<<< HEAD
    Symfony potrafi dopasować nazwy parametrów z trasy do nazw zmiennych z sygnatury
    metody kontrolera. Innymi słowy, Symfony rozumie, że parametr ``{last_name}``
    pasuje do argumentu ``$last_name``. Argumenty akcji mogą być kompletnie
    pomieszane i nadal będą działać poprawnie::
=======
  Symfony potrafi dopasować nazwy parametrów z trasy do nazw zmiennych z sygnatury
  metody kontrolera. Innymi słowy, Symfony rozumie, że parametr ``{last_name}``
  pasuje do argumentu ``$last_name``. Argumenty kontrolera mogą być kompletnie
  pomieszane i nadal będą działać poprawnie::
>>>>>>> refs/heads/nowe_rozdz

        public function indexAction($last_name, $color, $first_name)
        {
            // ..
        }

* **Każdy wymagany argument akcji musi pasować do parametru trasowania**

  Poniższy kod zgłosi wyjątek ``RuntimeException``, ponieważ parametr ``foo``
  nie został określony w trasie::

        public function indexAction($first_name, $last_name, $color, $foo)
        {
            // ..
        }
<<<<<<< HEAD
    
    Rozwiązaniem problemu może być przypisanie wartości domyślnej do argumentu.
    Poniższy przykład nie zgłosi wyjątku::
=======

  Rozwiązaniem problemu może być przypisanie wartości domyślnej do argumentu.
  Poniższy przykład nie zgłosi wyjątku::
>>>>>>> refs/heads/nowe_rozdz

        public function indexAction($first_name, $last_name, $color, $foo = 'bar')
        {
            // ..
        }

* **Nie wszystkie parametry trasowania muszą być argumentami akcji**

<<<<<<< HEAD
    Jeśli, na przykład, ``last_name`` nie jest istotny dla akcji,
    można go całkowicie pominąć::
=======
  Jeśli, na przykład, ``last_name`` nie jest istotny dla kontrolera,
  można go całkowicie pominąć::
>>>>>>> refs/heads/nowe_rozdz

        public function indexAction($first_name, $color)
        {
            // ..
        }

.. tip::

    Każda trasa posiada również specjalny parametr ``_route``, który przyjmuje
    wartość nazwy dopasowanej trasy (np. ``hello``). Parametr ten dostępny jest
    jako argument akcji, ale jest mało przydatny.
    Można również przekazywać inne zmienne z trasy do argumentów kontrolera.
    Proszę zapoznać się z :doc:`/cookbook/routing/extra_information`.

.. _book-controller-request-argument:

Obiekt Request jako argument akcji
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Co jeśli trzeba odczytać parametry zapytania, przejąć nagłówek lub uzyskać dostęp
do przesłanego pliku? Wszystkie te informacje są przechowywane w obiekcie ``Request``.
W celu pobrania tych informacji z akcji wystarczy wykorzystać argument
i **z podpowiadanym typem Request**::

    use Symfony\Component\HttpFoundation\Request;

    public function indexAction($firstName, $lastName, Request $request)
    {
        $page = $request->query->get('page', 1);

        // ...
    }
    
.. seealso::

    Chcesz wiedzieć więcej o uzyskiwaniu informacji z żądania? Zapoznaj się z
    :ref:`"Dostęp do informacji z żądania" <component-http-foundation-request>`.

.. index::
   single: kontroler; podstawowa klasa kontrolera

Podstawowa klasa kontrolera
---------------------------

<<<<<<< HEAD
Symfony udostępnia klasę ``Controller`` będącą klasą bazową dla kontrolerów
=======
Symfony2 udostępnia klasę ``Controller`` będącą klasą podstawową (bazową) dla kontrolerów
>>>>>>> refs/heads/nowe_rozdz
aplikacji. Pomaga ona w najbardziej typowych zadaniach kontrolera i daje klasie
kontrolera dostęp do każdego potrzebnego zasobu. Rozszerzając klasę ``Controller``
można skorzystać z kilku metod pomocniczych a za pośrednictwem kontenera ze wszystkich
obiektów usługowych.

Dodajmy instrukcję ``use`` na początku pliku kontrolera, a później zmodyfikujmy
klasę ``HelloController`` tak, aby była rozszerzeniem klasy ``Controller``::
   
   // src/AppBundle/Controller/HelloController.php
    namespace AppBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\Controller;

    class HelloController extends Controller
    {
        // ...
    }
    
W rzeczywistości niczego to nie zmienia w sposobie działania kontrolera.
W następnym rozdziale dowiesz się o metodach pomocniczych (helperach), które są
<<<<<<< HEAD
udostępnione przez klasę kontrolera bazowego. Te metody to po prostu skróty
do rdzennych funkcji Symfon, które są dostępne niezależnie od tego, czy używa
się klasy ``Controller``, czy nie. Dobrym sposobem na zobaczenie rdzennej funkcjonalności
w działaniu jest zapoznanie sie z `klasą Controller`_.
=======
udostępnione przez klasę kontrolera podstawowego. Te metody to po prostu skróty
do rdzennych funkcji Symfony2, które są dla dostępne niezależnie od tego, czy używa
się klasy ``Controller``, czy nie. Dobrym sposobem na zapoznanie się z klasą
:class:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller` jest zobaczenie
jak ona działa.
.
>>>>>>> refs/heads/nowe_rozdz

.. seealso::

<<<<<<< HEAD
    Informacje o tym jak działać będzie kontroler, który nie rozszerza klasy bazowej,
    można znaleźć w artykule :doc:`Kontrolery jako usługi </cookbook/controller/service>`.
    Stosując kontroler, który nie rozszerza kontrolera ``Controler``, można
    uzyskać więcej kontroli nad jawnymi obiektami i zależnościami,
    które sa wstrzykiwane do kontrolera.
=======
    Rozszerzenie klasy bazowej w Symfony jest *opcjonalne* - zawiera ona użyteczne
    skróty, ale nie są one obowiązkowe. Można również rozszerzyć klasę
    :class:`Symfony\\Component\\DependencyInjection\\ContainerAware`. Spowoduje to
    dostępność obiektu kontenera usług poprzez właściwość ``container``.

.. note::

    Można również zdefiniować własne
    :doc:`kontrolery jako usługi</cookbook/controller/service>`.

.. index::
   single: kontroler; typowe zadania

Typowe zadania kontrolera
-------------------------

Choć kontroler może praktycznie wykonywać prawie wszystko, większość kontrolerów
będzie wykonywać te same podstawowe zadania w kółko. Zadania takie jak jak przekierowania, forwardowanie,
przetwrzanie szablonów i udostępnianie rdzennych usług są w Symfony2 bardzo
łatwe w użyciu.
>>>>>>> refs/heads/nowe_rozdz

.. index::
   single: kontroler; przekierowania
   single: akcja; przekierowania
   przekierowania

Przekierowania
~~~~~~~~~~~~~~

Jeśli chce się przekierować użytkownika do innej strony, należy użyć metody ``redirectToRoute()``::

    public function indexAction()
    {
        return $this->redirectToRoute('homepage');

        // redirectToRoute jest zamiennikiem równocześnie fla redirect() i generateUrl():
        // return $this->redirect($this->generateUrl('homepage'), 301);
    }

.. versionadded:: 2.6
    Metod ``redirectToRoute()`` została dodana w Symfony 2.6. Poprzednio w tym celu
    używało się razem ``redirect()`` i ``generateUrl()``, co jest jeszcze obsługiwane 
    (zobacz powyższy przykład).

Jeśli chce się wykonać przekierowanie zewnętrzne, wystarczy użyć ``redirect()``
i przekazać to w adresie URL::
   
   public function indexAction()
    {
        return $this->redirect('http://symfony.com/doc');
    }

Domyślnie metoda ``redirectToRoute()`` realizuje przekierowanie 302 (tymczasowe,
*ang. temporary*). W celu wykonania przekierowania 301 (trwałe, *ang. permanent*),
należy zmodyfikować trzeci argument::

    public function indexAction()
    {
        return $this->redirectToRoute('homepage', array(), 301);
    }

.. tip::

    Metoda ``redirect()`` jest skrótem tworzącym obiekt ``Response``,
    którego zadaniem jest przekierowanie użytkownika. Jest to równoznaczne z::

      use Symfony\Component\HttpFoundation\RedirectResponse;

<<<<<<< HEAD
        public function indexAction()
        {
            return new RedirectResponse($this->generateUrl('homepage'));
        }
=======

.. index::
   single: kontroler; przekazywania

Przekazywanie (forwarding)
~~~~~~~~~~~~~~~~~~~~~~~~~~

Można również łatwo dokonać przekazania do innego wewnętrznego kontrolera przy
użyciu metody ``forward()``. Metoda ta sprawia, że zamiast przekierowywać przegladarkę
użytkownika wykonywane jest wewnętrzne podżądanie i wywoływany jest określony kontroler.
Metoda ``forward()`` zwraca obiekt ``Response``, który jest zwracany przez ten kontroler::

    public function indexAction($name)
    {
        $response = $this->forward('AcmeHelloBundle:Hello:fancy', array(
            'name'  => $name,
            'color' => 'green'
        ));

        // ... zmodyfikowanie odpowiedzi lub zwrócenie jej bezpośrednio

        return $response;
    }

Należy zwrócić uwagę, że metoda ``forward()`` wykorzystuje tą samą reprezentację
znakową kontrolera jaka jest używana w konfiguracji trasowania. W powyższym przykładzie
klasa docelowego kontrolera ``HelloController`` będzie się znajdować wewnątrz pakietu
``AcmeHelloBundle``. Tablica przekazana do metody staje sie argumentami wynikowego
kontrolera. Taki sam interfejs jest stosowany podczas osadzania kontrolerów w szablonach
(zobacz do rozdziału :ref:`templating-embedding-controller`). Metoda docelowego
kontrolera musi wyglądać następująco::

    public function fancyAction($name, $color)
    {
        // ... utworzenie i zwrócenie obiektu Response
    }

Kolejność argumentów ``fancyAction`` nie ma znaczenia, podobnie jak w przypadku
tworzenia kontrolera dla trasy. Symfony2 dopasowuje nazwy indeksów (np. ``name``)
do nazw argumentów metody (np. ``$name``). Jeśli zmieni się kolejność argumentów,
Symfony2 wciąż będzie w stanie przekazywać właściwą wartości do każdej zmiennej.

.. tip::

    Podobnie jak inne metody podstawowej klasy ``Controller``, metoda ``forward``
    jest skrótem do rdzennej funkcjonalności Symfony2. Przekazanie może być też
    dokonane bezpośrednio przez usługę ``http_kernel`` zawracajac obiekt
    ``Response``::

        use Symfony\Component\HttpKernel\HttpKernelInterface;

        $path = array(
            '_controller' => 'AcmeHelloBundle:Hello:fancy',
            'name'        => $name,
            'color'       => 'green',
        );
        $request = $this->container->get('request');
        $subRequest = $request->duplicate(array(), null, $path);

        $httpKernel = $this->container->get('http_kernel');
        $response = $httpKernel->handle(
            $subRequest,
            HttpKernelInterface::SUB_REQUEST
        );
        
>>>>>>> refs/heads/nowe_rozdz

.. index::
   single: kontroler; renderowanie szablonów
   single: akcja; renderowanie szablonów

.. _controller-rendering-templates:

Renderowanie szablonów
~~~~~~~~~~~~~~~~~~~~~~

Jeśli z akcji wyprowadza się kod HTML można wykorzystać renderowanie szablonu.
Metoda ``render()`` renderuje szablon i wstawia zawartość do obiektu ``Response``::
   
    // renderowanie szablonu app/Resources/views/hello/index.html.twig
    return $this->render('hello/index.html.twig', array('name' => $name));

Można również wstawić szablony z bardziej zagłębionych podkatalogów. Wystarczy
to wypróbować, aby uniknąć tworzenia niepotrzebnie zagłebionej struktury::
   
    // renders app/Resources/views/hello/greetings/index.html.twig
    return $this->render('hello/greetings/index.html.twig', array(
        'name' => $name
    ));

   
Silnik szablonowania Symfony jest szczegółowo wyjaśniony w rozdziale
:doc:`templating`.

.. sidebar:: Odwoływanie się do szablonów umieszczonych w pakietach

<<<<<<< HEAD
    Można również umieszczać szablony w katalogu ``Resources/views`` pakietu
    i odwoływać się do nich stosując składnię ``BundleName:DirectoryName:FileName``.
    Na przykład, ``AppBundle:Hello:index.html.twig`` będzie odwoływał się do szablonu
    umieszczonego w  ``src/AppBundle/Resources/views/Hello/index.html.twig``.
    Zobacz :ref:`template-referencing-in-bundle`.
=======
    Można nawet uniknąć wywoływania metody ``render`` stosujac adnotację ``@Template``.
    Zobacz do dokumentacji :doc:`FrameworkExtraBundle</bundles/SensioFrameworkExtraBundle/annotations/view>`
    w celu poznania szczegółów.
    

.. tip::

    Metoda ``renderView`` jest skrótem usługi ``templating``.
    Usługa ``templating`` może być również użyta bezpośrednio::

        $templating = $this->get('templating');
        $content = $templating->render(
            'AcmeHelloBundle:Hello:index.html.twig',
            array('name' => $name)
        );

.. note::

    Możliwe jest także renderowanie szablonów znajdujących się w głębszych podkatalogach,
    jednak należy uważać, aby nie wpaść w pułapkę nadmiernie rozbudowanej struktury
    katalogów:::

        $templating->render(
            'AcmeHelloBundle:Hello/Greetings:index.html.twig',
            array('name' => $name)
        );
        // renderowany jest index.html.twig znajdujacy się w Resources/views/Hello/Greetings.
        
>>>>>>> refs/heads/nowe_rozdz

.. index::
   single: kontroler; dostęp do usług
   single: akcja; dostęp do usług

.. _controller-accessing-services:

Dostęp do innych usług
~~~~~~~~~~~~~~~~~~~~~~

<<<<<<< HEAD
Symfony dostarczane jest z wieloma przydatnymi obiektami nazywanymi usługami.
Są one używane do renderowania szablonów, wysyłania wiadomości email, wykonywania
zapytań do bazy danych i innych wymyślnych "działań".

Rozszerzając klasę kontrolera podstawowego, można uzyskać dostęp do
każdej usługi Symfony poprzez metodę ``get()``. Poniżej znajduje się kilka
popularnych usług, jakie można wykorzystać::
=======
Rozszerzając klasę kontrolera podstaowego, można uzyskać dostęp do
każdej usługi Symfony2 poprzez metodę ``get()``. Poniżej znajduje się kilka
popularnych usług, jakie mogą być potrzebne::

    $request = $this->getRequest();
>>>>>>> refs/heads/nowe_rozdz

    $templating = $this->get('templating');

    $router = $this->get('router');

    $mailer = $this->get('mailer');

Istnieje wiele dostępnych usług i zachęca się do tworzenia własnych.
Aby wyświetlić listę wszstkich dostępnych usług, nalezy użyć polecenia konsoli
``debug:container``:

.. code-block:: bash

    $ php app/console debug:container

.. versionadded:: 2.6
    W wersjach wcześniejszych niż Symfony 2.6 polecenie to wywoływane było
    wyrażeniem ``container:debug``.
    

Więcej informacji można znaleźć w rozdziale :doc:`service_container`.

.. index::
   single: kontroler; zarządzanie stronami błędów
   single: kontroler; strona 404

Zarządzanie błędami i strona 404
--------------------------------

Gdy zasób nie może być znaleziony, to protokół HTTP zwraca odpowiedź 404. Aby to
obsłużyć trzeba zrzucić specjalny wyjątek. Jeśli rozszerza się klasę kontrolera
bazowego, można postąpić następująco::

    public function indexAction()
    {
        $product = // pobieramy obiekt z bazy danych
        if (!$product) {
            throw $this->createNotFoundException('Produkt nie istnieje');
        }

        return $this->render(...);
    }

Metoda ``createNotFoundException()`` tworzy specjalny obiekt
:class:`Symfony\\Component\\HttpKernel\\Exception\\NotFoundHttpException`,
który w efekcie końcowym wyzwala odpowiedź HTTP z kodem statusu 404.

Oczywiście w kontrolerze można zrzucić dowolną klasę ``Exception`` - Symfony będzie
wówczas automatycznie zwracać kod odpowiedzi HTTP 500, który interpretowany jest
jako wewnętrzny, niezidentyfikowany błąd serwera.

.. code-block:: php

    throw new \Exception('Coś poszło źle!');

W każdym przypadku użytkownikowi końcowemu jest wyświetlana wystylizowana strona
błędu a programiście strona pełnego raportu z debugowania (gdy strona jest wyświetlana
w trybie debugowania). Obie te strony błędu mogą być dostosowane do indywidualnych
potrzeb. Więcej szczegółów można znaleźć w artykule
":doc:`/cookbook/controller/error_pages`".

.. index::
   pair: kontroler; sesja

Zarządzanie sesją
-----------------

Symfony zapewnia świetny obiekt sesji, który można użyć do przechowywania informacji
o użytkowniku między poszczególnymi żądaniami (zarówno prawdziwej osoby używającej
przeglądarki, jak i użytkownika w postacji usługi internetowej). Domyślnie Symfony
zapamiętuje atrybuty w pliku cookie, używając natywnych sesji PHP.

Przechowywanie i pobieranie informacji z sesji może być wykonać w każdej akcji::

    $session = $this->getRequest()->getSession();

    // zapisanie atrybutu do odczytania w kolejnym żądaniu
    $session->set('foo', 'bar');

    // w innym kontrolerze i innym żądaniu
    $foo = $session->get('foo');

<<<<<<< HEAD
        // get the attribute set by another controller in another request
        $foobar = $session->get('foobar');

        // use a default value if the attribute doesn't exist
        $filters = $session->get('filters', array());
    }
    
=======
    // użycie domyślnej wartości, jeśli nie istnieje klucz
    $filters = $session->get('filters', array());

>>>>>>> refs/heads/nowe_rozdz
Atrybuty te pozostają przypisane użytkownikowi przez pozostałą część sesji.

.. index::
   single sesja; komunikaty fleszowe
   komunikaty fleszowe

Komunikaty fleszowe
~~~~~~~~~~~~~~~~~~~

W sesji użytkownika można również przechowywać małe komunikaty dla dokładnie
jednego dodatkowego żądania. Jest to przydatne w przetwarzaniu formularzy:
gdy chce się przekierować stronę i mieć specjalny komunikat wyświetlający
następne żądanie. Tego typu komunikaty nazywane są "fleszowymi".

Na przykład, wyobraźmy sobie, że przetwarzane jest zgłoszenie formularza::

    public function updateAction()
    {
        $form = $this->createForm(...);

        $form->handleRequest($this->getRequest());
        
        if ($form->isValid()) {
            // obsługa formularza

<<<<<<< HEAD
            $this->addFlash(
                'notice',
                'Your changes were saved!'
            );
=======
            $this->get('session')->getFlashBag()->add('notice', 'Zmiany zostały zapisane!');
>>>>>>> refs/heads/nowe_rozdz

            // $this->addFlash is equivalent to $this->get('session')->getFlashBag()->add

            return $this->redirectToRoute(...);
        }

        return $this->render(...);
    }

Po obsłużeniu żądania, kontroler ustawia komunikat fleszowy ``notice``, a następnie
wykonuje przekierowanie. Nazwa (``notice``) nie ma znaczenia - używa sie ją tylko
do zidentyfikowania typu komunikatu.

W szablonie następnej akcji poniższy kod jest użyty do wyrenderowania
komunikatu ``notice``:

.. configuration-block::

    .. code-block:: html+jinja

        {% if app.session.started %}
            {% for flashMessage in app.session.flashbag.get('notice') %}
                <div class="flash-notice">
                    {{ flashMessage }}
                </div>
            {% endfor %}
        {% endif %}

    .. code-block:: html+php

<<<<<<< HEAD
        <?php foreach ($view['session']->getFlash('notice') as $message): ?>
            <div class="flash-notice">
                <?php echo "<div class='flash-error'>$message</div>" ?>
            </div>
        <?php endforeach ?>
=======
        <?php if ($view['session']->isStarted()): ?>
            <?php foreach ($view['session']->getFlashBag()->get('notice') as $message): ?>
                <div class="flash-notice">
                    <?php echo "<div class='flash-error'>$message</div>" ?>
                </div>
            <?php endforeach; ?>
        <?php endif; ?>
>>>>>>> refs/heads/nowe_rozdz

Zgodnie z założeniem, komunikaty fleszowe są przeznaczone do użycia dokładnie
przy jednym żądaniu (są one wyświetlane natychmiast). Zostały zaprojektowane tak,
aby stosować przekierowania w sposób, jaki użyliśmy w tym przykładzie.

.. index::
   single: kontroler; obiekt Response
   single: akcja; obiekt Response
   single: obiekt Response

Obiekt Response
---------------

Jedyny wymóg dla kontrolera, to zwrócić obiekt ``Response``. Klasa
:class:`Symfony\\Component\\HttpFoundation\\Response` to abstrakcja PHP dla
odpowiedzi HTTP - tekstowa wiadomość zawierająca nagłówki HTTP i treść, która
jest zwracana klientowi::

    use Symfony\Component\HttpFoundation\Response;

<<<<<<< HEAD
    // utworzenie prostego obiektu Response z kodem stanu 200 (domyślny)
    $response = new Response('Hello '.$name, Response::HTTP_OK);
=======
    // utworzenie prostego obiektu Response z kodem statusu 200 (domyślnie)
    $response = new Response('Hello '.$name, 200);
>>>>>>> refs/heads/nowe_rozdz

    // utworzenie odpowiedzi w formacie JSON z kodem stanu 200
    $response = new Response(json_encode(array('name' => $name)));
    $response->headers->set('Content-Type', 'application/json');
<<<<<<< HEAD

Właściwość ``headers`` jest obiektem :class:`Symfony\\Component\\HttpFoundation\\HeaderBag`
i ma kilka ciekawych metod do pobierania i ustawiania nagłówków. Nazwy nagłówków
są normalizowane, tak więc używanie ``Content-Type`` jest równoważne z ``content-type``
lub nawet ``content_type``.

Istnieja również specjalne klasy do łatwiejszego wykonywania pewnego rodzaju
odpowiedzi:
=======
>>>>>>> refs/heads/nowe_rozdz

* dla JSON istnieje :class:`Symfony\\Component\\HttpFoundation\\JsonResponse`.
  Czytaj :ref:`component-http-foundation-json-response`.

* dla plików istnieje :class:`Symfony\\Component\\HttpFoundation\\BinaryFileResponse`.
  Patrz :ref:`component-http-foundation-serving-files`.

* dla odpowiedzi strumieniowanych istnieje :class:`Symfony\\Component\\HttpFoundation\\StreamedResponse`.
  Czytaj :ref:`streaming-response`.

.. seealso::

    Wiecej informacji o obiekcie Response mozna znaleźć w
    w dokumentacji komponentu. Czytaj :ref:`component-http-foundation-response`.


.. index::
   single: kontroler; obiekt Request

Obiekt Request
--------------

<<<<<<< HEAD
Poza wartościami wieloznaczników trasowania, akcja również uzyskuje dostęp
do obiektu ``Request``.
Framework wstrzykuje obiekt ``Request`` do akcji, jeśli argument ma podpowiadany
typ :class:`Symfony\\Component\\HttpFoundation\\Request`::
=======
Rozszerzając podstawową klasę ``Controller``, kontroler uzyskuje również dostęp
do obiektu``Request``. Framework wstrzykuje obiekt ``Request`` w kontroler, jeśli
zmienna jest typu odgadywanego w `Symfony\Component\HttpFoundation\Request`::
>>>>>>> refs/heads/nowe_rozdz

    $request = $this->getRequest();

    $request->isXmlHttpRequest(); // żądanie Ajax?

    $request->getPreferredLanguage(array('en', 'fr'));

    $request->query->get('page'); // pobieramy parametr $_GET

    $request->request->get('page'); // pobieramy parametr $_POST

Podobnie jak w przypadku obiektu ``Response``, nagłówki żądania są przechowywane w
obiekcie ``HeaderBag`` i są równie łatwo dostępne.

.. seealso::

    Wiecej informacji o obiekcie Request można znaleźć w dokumentacji komponentu.
    Czytaj :ref:`component-http-foundation-request`.


Tworzenie stron statycznych
---------------------------

Można utworzyć stronę statyczną bez tworzenia kontrolera (potrzebne są tylko
trasa i szablon).

Patrz :doc:`/cookbook/templating/render_without_controller`.

.. index::
   single: kontroler; przekazywanie

Przekazywanie do innych akcji
-----------------------------

Choć nie często, można również dokonać przekazania do innej akcji, wykorzystując
metode :method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller::forward`.
Sprawia to, że zamiast przekierowywać przegladarkę użytkownika, wykonywane jest
wewnętrzne pod-żądanie i wywoływana jest akcja. Metoda ``forward()`` zwraca
obiekt ``Response``, który został zwrócony z *tamtego* kontrolera::

    public function indexAction($name)
    {
        $response = $this->forward('AppBundle:Something:fancy', array(
            'name'  => $name,
            'color' => 'green',
        ));

        // ... dalsza modyfikacja odpowiedzi lub jej bezposrednie zwrócenie

        return $response;
    }

Proszę zwrócić uwagę, że metoda ``forward()`` używa specjalnego łańcucha reprezentującego
akcję (zobacz :ref:`controller-string-syntax`). W tym przypadku, docelową
funkcją kontrolera będzie ``SomethingController::fancyAction()`` wewnatrz AppBundle.
Tablica przekazywana do metody staje się argumentami wynikowej akcji.
Jest to ten sam pomysł jak przy osadzaniu kontrolerów w szablonach (patrz
:ref:`templating-embedding-controller`). Ta docelowa metoda kontrolera będzie
wygladać mniej więcej tak::

    public function fancyAction($name, $color)
    {
        // ... create and return a Response object
    }

Podobnie jak w przypadku tworzenia akcji dla trasy, kolejność argumentów
``fancyAction`` nie ma znaczenia. Symfony dopasowuje nazwy kluczy indeksu
(np. ``name``) do nazw argumentów metody (np. ``$name``). Jeśli zmieni się kolejność
argumentów, Symfony wciąż będzie w stanie przekazywać właściwą wartości do każdej
zmiennej.


Wnioski końcowe
---------------

Za każdym razem, kiedy tworzy sie stronę, musi się napisać kod, który zawiera logikę
tej strony. W Symfony nazywa się ten kod akcją i jest to funkcja PHP, która
może robić wszystko co jest potrzebne, aby w efekcie końcowym został zwrócony
obiekt ``Response``, który zostaje wysłany do użytkownika.

Aby ułatwić sobie życie, możesz rozszerzyć podstawową klasę ``Controller``,
która zawiera skrótowe metody wielu typowych zadań kontrolera. Na przykład,
jeśli nie chce się umieszczać kodu HTML w swojej akcji, można użyć metody
``render()``, aby wyrenderować zawartość szablonu.

W kolejnych rozdziałach zobaczysz jak akcja może być wykorzystana do umieszczania
i pobierania obiektów z bazy danych, przetwarzania formularzy, wykorzystywania
pamięci podręcznej i wiele więcej.

Dalsza lektura
--------------

* :doc:`/cookbook/controller/error_pages`
* :doc:`/cookbook/controller/service`


.. _`klasą Controller`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bundle/FrameworkBundle/Controller/Controller.php
.. _`wzorca projektowego Model-Widok-Kontroler`: https://pl.wikipedia.org/wiki/Model-View-Controller
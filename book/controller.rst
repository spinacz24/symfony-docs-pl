.. highlight:: php
   :linenothreshold: 2

.. index::
   single: kontroler

Kontroler
=========

Kontroler to wywoływalny kod PHP (głównie metoda), którą tworzy się, aby pobierała
informacje z żądania HTTP, a następnie konstruowała i zwracała odpowiedź HTTP (jako
obiekt ``Response`` Symfony2). Odpowiedź może być stroną HTML, dokumentem XML,
serializowaną tablicą JSON, obrazem, przekierowaniem, stroną błędu 404
lub czymkolwiek innym. Kontroler może zawierać dowolną logikę potrzebną do
wyrenderowania zawartości strony.

Aby zobaczyć, jakie to proste, spójrzmy jak działa kontroler Symfony2.
Poniższy kontroler wygeneruje stronę, która wyświetlającą ``Hello world!``::

    use Symfony\Component\HttpFoundation\Response;

    public function helloAction()
    {
        return new Response('Hello world!');
    }

Zadanie kontrolera jest zawsze takie samo: stworzyć i zwrócić obiekt ``Response``.
Po drodze może odczytywać informacje z żądania, wczytywać dane z bazy danych,
wysłać email, czy zapisać informacje w sesji użytkownika. Jednakże w każdym przypadku
kontroler ostatecznie zwróci obiekt ``Response``, który będzie dostarczony z powrotem
do użytkownika.

Nie ma tutaj żadnej magii czy innych wymagań, o które trzeba się martwić. Oto kilka
najczęstszych przypadków:

- *Kontroler A* przygotowuje obiekt ``Response`` reprezentujący zawartość strony głównej
  witryny.

- *Kontroler B* odczytuje parametr ``slug`` z żądania, aby pobrać wpis blogu
  z bazy danych i utworzyć obiekt ``Response``, który wyświetli tego blogu. Jeśli
  ``slug`` nie zostanie znaleziony w bazie danych, kontroler utworzy obiekt ``Response``
  zawierający kod błędu 404.

- *Kontroler C* obsługuje formularz kontaktowy. Odczytuje dane formularza z żądania HTTP,
  zapisuje dane kontaktowe do bazy danych i wysyła email do administratora. W końcu tworzy
  obiekt ``Response``, który przekieruje przeglądarkę klienta do strony z podziękowaniami.

.. index::
   single: kontroler; cykl żądanie-kontroler-odpowiedź

Cykl żądanie-kontroler-odpowiedź
--------------------------------

Każde żądanie obsługiwane przez projekt Symfony2 przechodzi ten sam cykl.
Framework zajmuje się powtarzającymi się czynnościami i w końcu wykonuje kontroler,
który przechowuje indywidualny kod aplikacji:

#. Każde żądanie jest obsługiwane przez pojedynczy plik kontrolera wejścia
   (np. ``app.php`` czy ``app_dev.php``), który inicjuje aplikację;

#. ``Router`` odczytuje informacje z żądania (np. URI), znajduje trasę, która
    pasuje do tej informacji oraz odczytuje parametr ``_controller`` dopasowanej
    trasy;

#. Wykonywany jest kontroler z dopasowana trasą i kod w nim zawarty
   tworzy i zwraca obiekt ``Response``;

#. Odsyłane są do klienta nagłówki HTTP i zawartość obiektu ``Response`.

Tworzenie strony sprowadza się do utworzenie kontrolera (#3) i wykonania trasy,
która odwzorowuje ścieżkę URL do tego kontrolera (#2).

.. note::

    Pomimo podobnej nazwy, "kontroler wejścia" zupełnie różni się od "kontrolerów",
    o których mówimy w tym rozdziale. :term:`Kontroler wejścia<kontroler wejscia>`
    to krótki plik PHP, który znajduje się w katalogu `web` aplikacji i do którego
    kierowane są wszystkie przechodzące żądania. Typowa aplikacja posiada
    kontroler wejścia dla środowiska produkcyjnego (np. ``app.php``) i kontroler
    wejścia dla środowiska programistycznego (np. ``app_dev.php``).
    Prawdopodobnie nigdy nie będziesz musiał edytować, przeglądać czy martwić się
    o kontrolery wejścia swojej aplikacji.

.. index::
   single: kontroler; prosty przykład

Prosty kontroler
----------------

Ogólnie rzecz biorąc, kontrolerem może być dowolne wywołanie kodu PHP (funkcja,
metoda obiektu czy domknięcie), to jednak w Symfony2 :term:`kontroler` jest zazwyczaj
pojedynczą metodą w obiekcie kontrolera. W tym znaczeniu kontrolery są też nazywane
*akcjami*.

.. code-block:: php
    :linenos:

    // src/Acme/HelloBundle/Controller/HelloController.php
    namespace Acme\HelloBundle\Controller;

    use Symfony\Component\HttpFoundation\Response;

    class HelloController
    {
        public function indexAction($name)
        {
            return new Response('<html><body>Hello '.$name.'!</body></html>');
        }
    }


.. tip::

    Prosze zauważyć, że *kontrolerem* jest metoda ``indexAction``, która zawarta
    jest wewnatrz *klasy kontrolera* (``HelloController``). Nie należy sugerowac się
    nazewnictwem. Klasa kontrolera to po prostu wygodny sposób na grupowanie kilku
    kontrolerów (akcji). Zazwyczaj klasa kontrolera przechowuje kilka kontrolerów
    (akcji) (np. ``updateAction``, ``deleteAction`` itd.)

Kontroler jest bardzo prosty, przeanalizujmy go:

* *linia 3*: Symfony2 korzysta z funkcjonalności przestrzeni nazw PHP 5.3, aby
  nazwać całą klasę kontrolera. Słowo kluczowe ``use`` importuje klasę ``Response``,
  którą nasz kontroler musi zwrócić.

* *linia 6*: Nazwa klasy to połączenie nazwy kontrolera (np. ``Hello``) i słowa
  ``Controller``. Jest to konwencja zapewniająca zgodność nazewniczą kontrolerów
  i pozwalająca na odwoływanie się do nich wyłącznie przez pierwszą część ich nazwy
  (np. ``Hello``) w konfiguracji trasowania.

* *linia 8*: Każda nazwa akcji w klasie kontrolera posiada przyrostek ``Action``
  i odwołuje się do konfiguracji trasowania poprzez nazwę akcji (``index``).
  W następnym rozdziale utworzymy trasę, która będzie odwzorowywać URL do
  akcji. Nauczysz się jak wieloznaczniki (*ang. placeholders*) trasy (``{name}``)
  stają się argumentami metody akcji (``$name``).

* *linia 10*: Kontroler tworzy i zwraca obiekt ``Response``.

.. index::
   single: kontroler; trasa

Odwzorowanie URL do kontrolera
------------------------------

Nowy kontroler zwraca prostą stronę HTML. Aby móc zobaczyć tą stronę w przeglądarce,
trzeba utworzyć trasę (*ang. route*) odwzorowującą wzorzec ścieżki URL do kontrolera:

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
            # uses a special syntax to point to the controller - see note below
            defaults:  { _controller: AppBundle:Hello:index }

   .. code-block:: xml

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="hello" path="/hello/{name}">
                <!-- uses a special syntax to point to the controller - see note below -->
                <default key="_controller">AppBundle:Hello:index</default>
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

Teraz, po wprowdzeniu ścieżki ``/hello/ryan`` (tj. ``http://localhost:8000/hello/ryan``
gdy stosuje się :doc:`wbudowanego serwera internetowego </cookbook/web_server/built_in>`)
wykonany zostanie kontroler ``HelloController::indexAction()`` i do zmiennej
``$name`` zostanie przekazana wartość ``ryan``. Tworzenie "strony" sprowadza się
do utworzenie metody kontrolera i powiązania jej z trasą.

Proste, prawda?

.. sidebar:: Składnia kontrolera AppBundle:Hello:index

    Jeśli przy trasowaniu stosuje sie formaty YML lub XML, trzeba odnieść się do
    kontrolera używając spesjalną składnię skrótu: ``AppBundle:Hello:index``.
    Wiecej szczegółów o formacie kontrolera mozna znaleźć w :ref:`controller-string-syntax`.

.. seealso::

    Możesz dowiedzieć się więcej o systemie trasowania w rozdziale
    :doc:`routing`.


.. index::
   single: kontroler; argumenty kontrolera

.. _route-parameters-controller-arguments:

Parametry trasy jako argumenty kontrolera
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Już wiemy, że trasa wskazuje na metodę ``HelloController::indexAction()``
znajdującą się wewnatrz ``AcmeHelloBundle``. Co ciekwsze, jest też argument
przekazywany do tej metody::

    // src/Acme/HelloBundle/Controller/HelloController.php

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

Teraz kontroler może mieć dwa argumenty::

    public function indexAction($firstName, $lastName)
    {
        // ...
    }

Odwzorowanie parametrów trasy na argumenty kontrolera jest łatwe i elastyczne.
Należy pamiętać o następujących wskazówkach:

* **Kolejność argumentów kontrolera nie ma znaczenia**

    Symfony potrafi dopasować nazwy parametrów z trasy do nazw zmiennych z sygnatury
    metody kontrolera. Innymi słowy, Symfony rozumie, że parametr ``{last_name}``
    pasuje do argumentu ``$last_name``. Argumenty kontrolera mogą być kompletnie
    pomieszane i nadal będą działać poprawnie::

        public function indexAction($last_name, $color, $first_name)
        {
            // ..
        }

* **Każdy wymagany argument kontrolera musi pasować do parametru trasowania**

    Poniższy kod zgłosi wyjątek ``RuntimeException``, ponieważ parametr ``foo``
    nie został określony w trasie::

        public function indexAction($first_name, $last_name, $color, $foo)
        {
            // ..
        }
    
    Rozwiązaniem problemu może być przypisanie wartości domyślnej do argumentu.
    Poniższy przykład nie zgłosi wyjątku::

        public function indexAction($first_name, $last_name, $color, $foo = 'bar')
        {
            // ..
        }

* **Nie wszystkie parametry trasowania muszą być argumentami kontrolera**

    Jeśli, na przykład, ``last_name`` nie jest istotny dla kontrolera,
    można go całkowicie pominąć::

        public function indexAction($first_name, $color)
        {
            // ..
        }

.. tip::

    Każda trasa posiada również specjalny parametr ``_route``, który przyjmuje
    wartość nazwy dopasowanej trasy (np. ``hello``). Parametr ten dostępny jest
    jako argument kontrolera, ale jest mało przydatny.
    Można również przekazywać inne zmienne z trasy do argumentów kontrolera.
    Proszę zapoznać się z :doc:`/cookbook/routing/extra_information`.

.. _book-controller-request-argument:

Obiekt Request jako argument kontrolera
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Co jeśli trzeba odczytać parametry zapytania, przejąc nagłówek lub uzyskać dostęp
do przesłanego pliku? Wszystkie te informacje są przechowywane w obiekcie ``Request``
Symfony. W celu pobrania tych informacji z kontrolera wystarczy wykorzystać argumenty
i **podpowiedzieć je w klasie Request**::

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

Bazowa klasa kontrolera
-----------------------

Symfony2 udostępnia klasę ``Controller`` będącą klasą bazową dla kontrolerów
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
udostępnione przez klasę kontrolera podstawowego. Te metody to po prostu skróty
do rdzennych funkcji Symfony2, które są dostępne niezależnie od tego, czy używa
się klasy ``Controller``, czy nie. Dobrym sposobem na zobaczenie rdzennej funkcjonalności
w działaniu jest zapoznanie sie z `Controller class`_.

.. seealso::

    Informacje o tym jak działać będzie kontroler, który nie rozszerza klasy bazowej,
    można znaleźć w artykule :doc:`Kontrolery jako usługi </cookbook/controller/service>`.
    Można ewentualnie uzyskać więcej kontroli nad jawnymi objects i zależnościami,
    które sa wstrzykiwane do kontrolera.

.. index::
   single: kontroler; przekierowania

Przekierowania
~~~~~~~~~~~~~~

Jeśli chce się przekierować użytkownika do innej strony, należy użyć metody ``redirect()``::

    public function indexAction()
    {
        return $this->redirectToRoute('homepage');

        // redirectToRoute is equivalent to using redirect() and generateUrl() together:
        // return $this->redirect($this->generateUrl('homepage'), 301);
    }

.. versionadded:: 2.6
    Metod ``redirectToRoute()`` została dodana w Symfony 2.6. Poprzednio w tym celu
    używało się razem ``redirect()`` i ``generateUrl()``, co jest jeszcze obsługiwane 
    (zobacz powyższy przykład).

Jeśli chce się wykonać przekierowanie zewnętrzne, wystarczy uzyć ``redirect()``
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

        public function indexAction()
        {
            return new RedirectResponse($this->generateUrl('homepage'));
        }

.. index::
   single: kontroler; renderowanie szablonów

.. _controller-rendering-templates:

Renderowanie szablonów
~~~~~~~~~~~~~~~~~~~~~~

Jeśli wyprowadza się HTML można wykorzystać renderowanie szablonu. Metoda ``render()``
renderuje szablon i wstawia zawartość do obiektu ``Response``::
   
   // renders app/Resources/views/hello/index.html.twig
    return $this->render('hello/index.html.twig', array('name' => $name));

Można również wstawić szablony w bardziej zagłębionych podkatalogach. Wystarczy
wypróbować, a by uniknąć tworzenia niepotrzebnie zagłebionej struktur::
   
   // renders app/Resources/views/hello/greetings/index.html.twig
    return $this->render('hello/greetings/index.html.twig', array(
        'name' => $name
    ));      
   
Silnik szablonowania Symfony jest szczegółowo wyjaśniony w rozdziale
:doc:`templating`.

.. sidebar:: Odwoływanie się do szablonów umieszczonych w pakietach

    Można również umieszczać szablony w katalogu ``Resources/views`` pakietu
    i odwoływać się do nich stosując składnię ``BundleName:DirectoryName:FileName``.
    Na przykład, ``AppBundle:Hello:index.html.twig`` będzie odwoływał się do szablonu
    umieszczonego w  ``src/AppBundle/Resources/views/Hello/index.html.twig``.
    Zobacz :ref:`template-referencing-in-bundle`.

.. index::
   single: kontroler; dostęp do usług

Dostęp do innych usług
~~~~~~~~~~~~~~~~~~~~~~

Symfony jest spakowany w wiele przydatnych obiektów nazywanych usługami. Są one
używane do renderowania szablonów, wysyłania wiadomości email, zapytań do bazy
danych i innych wymyśłonych "działań".

Rozszerzając klasę kontrolera podstawowego, można uzyskać dostęp do
każdej usługi Symfony2 poprzez metodę ``get()``. Poniżej znajduje się kilka
popularnych usług, jakie można wykorzystać::

    $templating = $this->get('templating');

    $router = $this->get('router');

    $mailer = $this->get('mailer');

Istnieje wiele dostępnych usług i zachęca się do tworzenia własnych.
Aby wyświetlić listę wszstkich dostępnych usług, nalezy użyć polecenia konsoli
``container:debug``:

.. code-block:: bash

    $ php app/console container:debug

.. versionadded:: 2.6
    W wersjach wcześniejszych niż Symfony 2.6 polecenie to wywoływane było
    wyrazeniem ``container:debug``.
    

Więcej informacji można znaleźć w rozdziale :doc:`service_container`.

.. index::
   single: kontroler; zarządzanie stronami błędów
   single: kontroler; strona 404

Zarządzanie stronami błędów i strona 404
----------------------------------------

Gdy zasób nie może być znaleziony, to protokół HTTP zwraca odpowiedź 404. Aby to
obsłużyć trzeba zrzucić specjalny wyjątek. Jeśli rozszerza się klasę kontrolera
podstawiwego, można postąpić następująco::

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

Oczywiście w kontrolerze można zrzucić dowolną klasę ``Exception`` - Symfony2 będzie
wówczas automatycznie zwracać kod odpowiedzi HTTP 500, który interpretowany jest
jako wewnętrzny, niezidentyfikowany błąd serwera.

.. code-block:: php

    throw new \Exception('Coś poszło źle!');

W każdym przypadku użytkownikowi końcowemu jest wyświetlana wystylizowana strona
błędu a programiście strona pełnego raportu z debugowania (gdy strona jest wyswietlana
w trybie debugowania). Obie te strony błędu mogą być dostosowane do indywidualnych
potrzeb. Więcej szczegółów można znaleźć w artykule
":doc:`/cookbook/controller/error_pages`".

.. index::
   pair: kontroler; sesja

Zarządzanie sesją
-----------------

Symfony2 zapewnia świetny obiekt sesji, który można użyć do przechowywania informacji
o użytkowniku między poszczególnymio żądaniami (zarówno prawdziwej osoby używającej
przeglądarki, jak i użytkownika w postacji serwisu web). Domyślnie Symfony2 zapamiętuje
atrybuty w pliku cookie, używając natywnych sesji PHP.

Przechowywanie i pobieranie informacji z sesji może być łatwo osiągnięte z dowolnego
kontrolera::

    use Symfony\Component\HttpFoundation\Request;

    public function indexAction(Request $request)
    {
        $session = $request->getSession();

        // store an attribute for reuse during a later user request
        $session->set('foo', 'bar');

        // get the attribute set by another controller in another request
        $foobar = $session->get('foobar');

        // use a default value if the attribute doesn't exist
        $filters = $session->get('filters', array());
    }
    

Atrybuty te pozostają przypisane użytkownikowi przez pozostałą część sesji.

.. index::
   single sesja; wiadomości fleszowe

Komunikaty fleszowe
~~~~~~~~~~~~~~~~~~~

W sesji uzytkownika można również przechowywać małe komunikaty dla dokładnie
jednego dodatkowego żądania. Jest to przydatne w przetwarzaniu formularzy:
gdy chce się przekierować stronę i mieć specjalny komunikat wyświetlający
następne żądanie. Tego typu komunikaty nazywane są "fleszowymi".

Na przykład, wyobraźmy sobie, że przetwarzane jest zgłoszenie formularza::

    use Symfony\Component\HttpFoundation\Request;

    public function updateAction(Request $request)
    {
        $form = $this->createForm(...);

        $form->handleRequest($request);

        if ($form->isValid()) {
            // do some sort of processing

            $this->addFlash(
                'notice',
                'Your changes were saved!'
            );

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

        {% for flashMessage in app.session.flashbag.get('notice') %}
            <div class="flash-notice">
                {{ flashMessage }}
            </div>
        {% endfor %}

    .. code-block:: html+php

        <?php foreach ($view['session']->getFlash('notice') as $message): ?>
            <div class="flash-notice">
                <?php echo "<div class='flash-error'>$message</div>" ?>
            </div>
        <?php endforeach ?>

Zgodnie z założeniem, komunikaty fleszowe są przeznaczone do użycia dokładnie
przy jednym żądaniu (są one wyświetlane natychmiast). Zostały zaprojektowane tak,
aby stosować przekierowania tak, jak zrobiliśmy to w tym przykładzie.

.. index::
   single: kontroler; obiekt Response

Obiekt Response
---------------

Jedyny wymóg dla kontrolera to zwrócić obiekt ``Response``. Klasa
:class:`Symfony\\Component\\HttpFoundation\\Response` to abstrakcja PHP dla
odpowiedzi HTTP - tekstowa wiadomość zawierająca nagłówki HTTP i treść, która
jest zwracana klientowi::

    use Symfony\Component\HttpFoundation\Response;

    // create a simple Response with a 200 status code (the default)
    $response = new Response('Hello '.$name, Response::HTTP_OK);

    // create a JSON-response with a 200 status code
    $response = new Response(json_encode(array('name' => $name)));
    $response->headers->set('Content-Type', 'application/json');

Właściwość ``headers`` jest obiektem :class:`Symfony\\Component\\HttpFoundation\\HeaderBag`
i ma kilka ciekawych metod do pobierania i ustawiania nagłówków. Nazwy nagłówków
są normalizowane, tak więc używanie ``Content-Type`` jest równoważne z ``content-type``
lub nawet ``content_type``.

Istnieja również specjalne klasy do łatwiejszego wykonywania pewnego rodzaju
odpowiedzi:

* dla JSON istnieje :class:`Symfony\\Component\\HttpFoundation\\JsonResponse`.
  Patrz :ref:`component-http-foundation-json-response`.

* dla plików istnieje :class:`Symfony\\Component\\HttpFoundation\\BinaryFileResponse`.
  Patrz :ref:`component-http-foundation-serving-files`.

* dla odpowiedzi strumieniowanych istnieje :class:`Symfony\\Component\\HttpFoundation\\StreamedResponse`.
  Patrz :ref:`streaming-response`.

.. seealso::

    Nie martw się! Istnieje o wiele więcej informacji o obiekcie Response
    w dokumentacji komponentu. Patrz :ref:`component-http-foundation-response`.


.. index::
   single: kontroler; obiekt Request

Obiekt Request
--------------

Poza wartościami wieloznaczników trasowania, kontroler również uzyskuje dostęp
do obiektu ``Request``. Framework wstrzykuje obiekt ``Request`` do kontrolera,
jeśli zmienna w :class:`Symfony\\Component\\HttpFoundation\\Request` jest typu
podpowiedzi (type-hinted)::

    use Symfony\Component\HttpFoundation\Request;

    public function indexAction(Request $request)
    {
        $request->isXmlHttpRequest(); // is it an Ajax request?

        $request->getPreferredLanguage(array('en', 'fr'));

        $request->query->get('page'); // get a $_GET parameter

        $request->request->get('page'); // get a $_POST parameter
    }

Podobnie jak w przypadku obiektu ``Response``, nagłówki żądania są przechowywane w
obiekcie ``HeaderBag`` i są równie łatwo dostępne.

Tworzenie stron statycznych
---------------------------

Można utworzyć stronę statyczną bez tworzenia kontrolera (potrzebne są tylko
trasa i szablon).

Patrz :doc:`/cookbook/templating/render_without_controller`.

.. index::
   single: kontroler; przekazywanie

Przekazywanie do innych kontrolerów
-----------------------------------

Choć nie często, można również dokonać przekazania do innego wewnetrznego kontrolera
w metodzie :method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller::forward`.
Sprawia to, że zamiast przekierowywać przegladarkę użytkownika, wykonywany jest
wewnętrzne pod-żądanie i wywoływany jest kontroler. Metoda ``forward()`` zwraca
obiekt ``Response``, który jest zwracany z kontrolera *that*::

    public function indexAction($name)
    {
        $response = $this->forward('AppBundle:Something:fancy', array(
            'name'  => $name,
            'color' => 'green',
        ));

        // ... further modify the response or return it directly

        return $response;
    }

Proszę zwrócić uwagę, że metoda ``forward()`` używa specjalnego łańcucha reprezentującego
kontroler (zobacz :ref:`controller-string-syntax`). W tym przypadku, docelową
funkcją kontrolera będzie ``SomethingController::fancyAction()`` wewnatrz AppBundle.
Tablica przekazywana do metody staje się argumentami wynikowego kontrolera.
Jest to ten sam pomysł jak przy osadzaniu kontrolerów w szablonach (patrz
:ref:`templating-embedding-controller`). Ta docelowa metoda kontrolera będzie
wygladać mniej więcej tak::

    public function fancyAction($name, $color)
    {
        // ... create and return a Response object
    }

Podobnie jak w przypadku tworzenia kontrolera dla trasy, kolejność argumentów
``fancyAction`` nie ma znaczenia. Symfony dopasowuje nazwy kluczy indeksu
(np. ``name``) do nazw argumentów metody (np. ``$name``). Jeśli zmieni się kolejność
argumentów, Symfony2 wciąż będzie w stanie przekazywać właściwą wartości do każdej
zmiennej.


Wnioski końcowe
---------------

Za każdym razem, kiedy tworzy sie stronę, musi się napisać kod, który zawiera logikę
tej strony. W Symfony nazywa się ten kod kontrolerem i jest to funkcja PHP, która
może robić wszystko co jest potrzebne, aby w efekcie końcowym został zwrócony
obiekt ``Response``, który zostaje wysłany do użytkownika.

Aby ułatwić sobie życie, możesz rozszerzyć podstawową klasę ``Controller``,
która zawiera skrótowe metody wielu typowych zadań kontrolera. Na przykład,
jeśli nie chce się umieszczać kodu HTML w swoim kontrolerze, można użyć metody
``render()``, aby wyrenderować zawartość szablonu.

W kolejnych rozdziałach zobaczysz jak kontroler może być wykorzystany do umieszczania
i pobierania obiektów z bazy danych, przetwarzania formularzy, wykorzystywania pamieci
podręcznej i wiele więcej.

Dalsza lektura
--------------

* :doc:`/cookbook/controller/error_pages`
* :doc:`/cookbook/controller/service`


.. _`Controller class`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bundle/FrameworkBundle/Controller/Controller.php
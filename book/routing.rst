.. highlight:: php
   :linenothreshold: 2

.. index::
   single: trasowanie

Trasowanie
==========

Przyjazne adresy URL to absolutna konieczność dla każdej poważnej aplikacji
internetowej. Oznacza to porzucenie "brzydkich" (nieprzyjaznych) ścieżek, takich
jak ``index.php?article_id=57`` na rzecz czegoś takiego jak ``/read/intro-to-symfony``.

Jeszcze ważniejsza jest elastyczność. Co, jeśli musi się zmienić ścieżkę URL strony
z ``/blog`` na ``/news``? Ile odnośników trzeba odnaleźć i poprawić,
aby dokonać takiej zmiany? Jeśli korzysta się z mechanizmu trasowania Symfony,
taka zmiana jest bardzo prosta.

Mechanizm trasowania Symfony pozwala na dynamiczne odwzorowywanie ścieżek URL (czyli
ścieżek dostępu, zawartych w adresie URL żądania HTTP) na różne obszary aplikacji.
Pod koniec tego rozdziału, będziesz w stanie:

* tworzyć złożone trasy odwzorowujące akcje;
* generować ścieżki URL wewnątrz szablonów i akcji;
* ładować zasoby trasowania z pakietów (lub z czegokolwiek innego);
* debugować swoje trasowania.


.. index::
   single: trasowanie; podstawy

Działanie trasowania
--------------------

**Trasa** (*ang. route*) jest mapą od ścieżki URL do akcji. Na przykład, załóżmy,
że chcemy dopasować ścieżkę URL jak ``/blog/moj-post`` czy ``/blog/wszystko-o-symfony``
i wysłać go do akcji, która może odnaleźć i wyświetlić dany wpis blogu. Trasa
jest prosta:

.. configuration-block::

    .. code-block:: php-annotations
       :linenos:

        // src/AppBundle/Controller/BlogController.php
        namespace AppBundle\Controller;

        use Symfony\Bundle\FrameworkBundle\Controller\Controller;
        use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;

        class BlogController extends Controller
        {
            /**
             * @Route("/blog/{slug}", name="blog_show")
             */
            public function showAction($slug)
            {
                // ...
            }
        }

    .. code-block:: yaml
       :linenos:

        # app/config/routing.yml
        blog_show:
            path:      /blog/{slug}
            defaults:  { _controller: AppBundle:Blog:show }

    .. code-block:: xml
       :linenos:

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog_show" path="/blog/{slug}">
                <default key="_controller">AppBundle:Blog:show</default>
            </route>
        </routes>

    .. code-block:: php
       :linenos:

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('blog_show', new Route('/blog/{slug}', array(
            '_controller' => 'AppBundle:Blog:show',
        )));

        return $collection;

Ścieżka zdefiniowana w trasie ``blog_show`` działa jak ``/blog/*``, gdzie
znak wieloznaczny (``*``) otrzymuje nazwę ``slug``. Dla ścieżki URL ``/blog/moj-post``
zmienna ``slug`` przybierze wartość ``moj-post``, która jest dostępna z poziomu
akcji (wyjaśniono to dalej).

Jeśli nie chce sie stosować adnotacji, bo nie podoba się ten sposób lub ponieważ
nie chce się polegać na SensioFrameworkExtraBundle, można stosować trasowanie
w formacie Yaml, XML lub PHP. W formatach tych parametr ``_controller`` jest
specjalnym kluczem, który powiadamia Symfony o tym, która akcja powinna być
wykonana, gdy scieżka URL zostanie dopasowana do wzorca trasy. Wartością
``_controller`` jest ciąg znakowy określający :ref:`nazwę logiczną<controller-string-syntax>`.
Ma to zastosowanie do wzorców, które wskazują określoną klasę i metodę PHP.

Tak więc, utworzylismy trasę i połączyliśmy ją do akcji. Teraz, gdy odwiedzi
się ``/blog/my-post``, wykonana zostanie akcja ``showAction`` a zmienna
``$slug`` stanie się równoważnikiem ``my-post``.

To jest właśnie zadanie mechanizmu trasowania Symfony: odwzorować ścieżkę URL żądania
na akcję. W dalszej części artykułu podanych jest  wiele sztuczek,
które sprawiają, że odwzorowanie nawet najbardziej skomplikowanych adresów URL staje się łatwe.


.. index::
   single: trasowanie; mechanizm

Trasowanie - pod maską
----------------------

Kiedy do aplikacji wysłane jest żądanie, zawiera ono dokładny adres do
"zasobu", który żąda klient. Ten adres nazywany jest lokalizatorem URL
(lub identyfikatorem URI) i zawiera ścieżkę do zasobu, taką jak ``/kontakt``,
``/blog/informacje`` lub cokolwiek innego. Weźmy za
przykład poniższe żądanie HTTP:

.. code-block:: text

    GET /blog/moj-post

Zadaniem mechanizmu trasowania Symfony jest przetworzenie tej ścieżki URL
i określenie, która akcja powinnna zostać uruchomiona. Cały proces wygląda
mniej więcej tak:

#. Żądanie zostaje obsłużone przez kontroler wejścia Symfony (np. ``app.php``);

#. Rdzeń Symfony (czyli :term:`Kernel`) odpytuje mechaniz trasowania o treść żądania;

#. Mechanizm trasowania dopasowuje ścieżkę zawartą w przychodzącym adresie URL do
   konkretnej trasy i zwraca informacje o trasie, łącznie z nazwą akcji,
   która powinna zostać uruchomiona;

#. Rdzeń Symfony wykonuje kod akcji, która ostatecznie zwraca obiekt
   ``Response``.

.. figure:: /images/request-flow.png
   :align: center
   :alt: Przepływ żądania w Symfony

   Warstwa trasowania jest narzędziem, które tłumaczy przychodzący adres URL na
   oodpowiednią akcję jaka ma być wykonana.

.. index::
   single: trasowanie; tworzenie tras

.. _creating-routes:

Tworzenie tras
--------------

Symfony wczytuje wszystkie trasy dla aplikacji z pojedynczego pliku trasowania.
Ten plik to zazwyczaj ``app/config/routing.yml``, jednakże można
skonfigurować inny plik (nawet w formacie XML zy PHP) za pośrednictwem pliku
konfiguracyjnego aplikacji:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        framework:
            # ...
            router:        { resource: '%kernel.root_dir%/config/routing.yml' }

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <framework:config ...>
            <!-- ... -->
            <framework:router resource="%kernel.root_dir%/config/routing.xml" />
        </framework:config>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            // ...
            'router'        => array('resource' => '%kernel.root_dir%/config/routing.php'),
        ));

.. tip::

    Nawet, jeśli wszystkie trasy są wczytywane z pojedynczego pliku, dobrą praktyką
    jest dołączać dodatkowe zasoby trasowania z innych plików.
    Zobacz do rozdiału :ref:`routing-include-external-resources` w celu poznania
    szczegółów.

Podstawowa konfiguracja trasy
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Definiowanie tras jest proste, a typowa aplikacja będzie posiadała wiele tras.
Podstawowa trasa składa się z dwóch części: ``path`` (wzorca do dopasowania)
oraz z tablicy ``defaults`` przechowującej wartości domyślne:

.. configuration-block::

    .. code-block:: php-annotations
       :linenos:

        // src/AppBundle/Controller/MainController.php

        // ...
        class MainController extends Controller
        {
            /**
             * @Route("/")
             */
            public function homepageAction()
            {
                // ...
            }
        }

    .. code-block:: yaml
       :linenos:

        # app/config/routing.yml
        _welcome:
            path:      /
            defaults:  { _controller: AppBundle:Main:homepage }

    .. code-block:: xml
       :linenos:

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="_welcome" path="/">
                <default key="_controller">AppBundle:Main:homepage</default>
            </route>

        </routes>

    ..  code-block:: php
        :linenos:

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('_welcome', new Route('/', array(
            '_controller' => 'AppBundle:Main:homepage',
        )));

        return $collection;

Trasa ta dopasowuje stronę główną aplikacji (``/``) i odwzorowuje akcję
``AppBundle:Main:homepage``. Ciąg znakowy ``_controller`` jest zamieniany
na nazwę odpowiedniej funkcji PHP, która następnie zostaje wykonana.
Ten proces będzie wyjaśniony w sekcji :ref:`controller-string-syntax`.

.. index::
   single: trasowanie; parametry

Trasowanie z wieloznacznikami
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Mechanizm trasowania obsługuje również trasy z wieloznacznikami.
Do określenia wielu tras można wykorzystać jedno lub więcej
"wyrażeń wieloznacznych" zwanych wieloznacznikami (*ang. placeholders*):

.. configuration-block::

    .. code-block:: php-annotations
       :linenos:

        // src/AppBundle/Controller/BlogController.php

        // ...
        class BlogController extends Controller
        {
            /**
             * @Route("/blog/{slug}")
             */
            public function showAction($slug)
            {
                // ...
            }
        }

    .. code-block:: yaml
       :linenos:

        # app/config/routing.yml
        blog_show:
            path:      /blog/{slug}
            defaults:  { _controller: AppBundle:Blog:show }

    .. code-block:: xml
       :linenos:

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog_show" path="/blog/{slug}">
                <default key="_controller">AppBundle:Blog:show</default>
            </route>
        </routes>

    .. code-block:: php
       :linenos:

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('blog_show', new Route('/blog/{slug}', array(
            '_controller' => 'AppBundle:Blog:show',
        )));

        return $collection;

Wzorzec taki będzie pasował do wszystkiego, co wygląda jak ``/blog/*``. Co więcej,
wartość przypisana do parametru ``{slug}`` będzie dostępna wewnątrz akcji.
Innymi słowy, jeśli ścieżka URL wygląda tak: ``/blog/hello-world``,
to zmienna ``$slug`` z wartością ``hello-world`` będzie dostępna w akcji.
Może być to użyte np. do pobrania wpisu na blogu, którego adres pasuje do tego
ciągu znakowego.

Ten wzorzec jednakże nie będzie pasował do samego ``/blog``. Dzieje się tak,
ponieważ domyślnie wymagane jest określenie wszystkich wieloznaczników.
Może być to zmienione poprzez dodanie do tablicy ``defaults`` następnej wartości
wieloznacznika.


Wieloznaczniki obowiązkowe i opcjonalne
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Aby było ciekawiej, dodamy nową trasę wyświetlającą listę wszystkich dostępnych
wpisów na blogu wymyślonej aplikacji blogowej:

.. configuration-block::

    .. code-block:: php-annotations
       :linenos:

        // src/AppBundle/Controller/BlogController.php

        // ...
        class BlogController extends Controller
        {
            // ...

            /**
             * @Route("/blog")
             */
            public function indexAction()
            {
                // ...
            }
        }

    .. code-block:: yaml
       :linenos:

        # app/config/routing.yml
        blog:
            path:      /blog
            defaults:  { _controller: AppBundle:Blog:index }

    .. code-block:: xml
       :linenos:

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog" path="/blog">
                <default key="_controller">AppBundle:Blog:index</default>
            </route>
        </routes>

    .. code-block:: php
       :linenos:

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('blog', new Route('/blog', array(
            '_controller' => 'AppBundle:Blog:index',
        )));

        return $collection;

Jak dotąd, ta trasa jest tak prosta, jak to tylko możliwe - nie zawiera
żadnych wieloznaczników i pasuje tylko do jednej ścieżki URL ``/blog``. Ale co,
jeśli chce się, aby ta trasa obsługiwała stronicowanie, gdzie ``/blog/2``
wyświetlałby  drugą stronę wpisów blogu? Zmieńmy tą trasę, tak aby posiadała nowy
parameter ``{page}``:

.. configuration-block::

    .. code-block:: php-annotations
       :linenos:

        // src/AppBundle/Controller/BlogController.php

        // ...

        /**
         * @Route("/blog/{page}")
         */
        public function indexAction($page)
        {
            // ...
        }

    .. code-block:: yaml
       :linenos:

        # app/config/routing.yml
        blog:
            path:      /blog/{page}
            defaults:  { _controller: AppBundle:Blog:index }

    .. code-block:: xml
       :linenos:

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog" path="/blog/{page}">
                <default key="_controller">AppBundle:Blog:index</default>
            </route>
        </routes>

    .. code-block:: php
       :linenos:

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('blog', new Route('/blog/{page}', array(
            '_controller' => 'AppBundle:Blog:index',
        )));

        return $collection;

Podobnie jak poprzedni wieloznacznik ``{slug}``, wartość pasująca do ``{page}``
będzie też dostępna dla akcji. Ta wartość może być użyta do określenia,
którą część wpisu na blogu wyświetlić dla danej strony.

Chwileczkę! Ponieważ wieloznaczniki są domyślnie wymagane, ta trasa już nie będzie
pasować do adresu ``/blog``. Ponadto, aby zobaczyć stronę 1 blogu, trzeba użyć
ścieżki URL ``/blog/1``. Ponieważ nie jest to dobry sposób dla bardziej złożonej
aplikacji internetowej, to zmodyfikujemy trasę tak aby wileoznacznik ``{page}``
był opcjonalny. Można tego dokonać dołączając do tablicy ``defaults``, taki oto
zapis:

.. configuration-block::

    .. code-block:: php-annotations
       :linenos:

        // src/AppBundle/Controller/BlogController.php

        // ...

        /**
         * @Route("/blog/{page}", defaults={"page" = 1})
         */
        public function indexAction($page)
        {
            // ...
        }

    .. code-block:: yaml
       :linenos:

        # app/config/routing.yml
        blog:
            path:      /blog/{page}
            defaults:  { _controller: AppBundle:Blog:index, page: 1 }

    .. code-block:: xml
       :linenos:

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog" path="/blog/{page}">
                <default key="_controller">AppBundle:Blog:index</default>
                <default key="page">1</default>
            </route>
        </routes>

    .. code-block:: php
       :linenos:

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('blog', new Route('/blog/{page}', array(
            '_controller' => 'AppBundle:Blog:index',
            'page'        => 1,
        )));

        return $collection;

Po dodaniu ``page`` do tablicy ``defaults``, wieloznacznik ``{page}`` już nie jest
wymagany. Ścieżka URL ``/blog`` będzie teraz pasowała do tej trasy, a wartość wieloznacznika
``page`` zostanie ustawiona na ``1``. Ścieżka URL ``/blog/2`` również będzie pasować,
dając wieloznacznikowi ``page`` wartość ``2``.

===========  ========  ==================
URL          Trasa     Parametry
===========  ========  ==================
``/blog``    ``blog``  ``{page}`` = ``1``
``/blog/1``  ``blog``  ``{page}`` = ``1``
``/blog/2``  ``blog``  ``{page}`` = ``2``
===========  ========  ==================

.. caution::

    Oczywiście, można mieć więcej niż jeden opcjonalny wieloznacznik (np.
    ``/blog/{slug}/{page}``), ale wszystko po po opcjonalnym wieloznaczniku musi
    też być opcjonalne. Na przykład, ``/{page}/blog`` jest prawidłową ścieżką,
    ale ``page`` będzie zawsze wymagane (czyli proste ``/blog`` nie dopasuje tej
    trasy ).

.. tip::

    Trasy z opcjonalnymi parametrami na końcu nie będą pasować do żądań/ które
    mają końcowy ukośnik (np. ``/blog/`` nie będzie pasować, ale ``/blog`` tak).


.. index::
   single: trasowanie; wymagania

Dodawanie wymagań
~~~~~~~~~~~~~~~~~

Spójrzmy na utworzone przez nas wcześniej trasy:

.. configuration-block::

    .. code-block:: php-annotations
       :linenos:

        // src/AppBundle/Controller/BlogController.php

        // ...
        class BlogController extends Controller
        {
            /**
             * @Route("/blog/{page}", defaults={"page" = 1})
             */
            public function indexAction($page)
            {
                // ...
            }

            /**
             * @Route("/blog/{slug}")
             */
            public function showAction($slug)
            {
                // ...
            }
        }

    .. code-block:: yaml
       :linenos:

        # app/config/routing.yml
        blog:
            path:      /blog/{page}
            defaults:  { _controller: AppBundle:Blog:index, page: 1 }

        blog_show:
            path:      /blog/{slug}
            defaults:  { _controller: AppBundle:Blog:show }

    .. code-block:: xml
       :linenos:

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog" path="/blog/{page}">
                <default key="_controller">AppBundle:Blog:index</default>
                <default key="page">1</default>
            </route>

            <route id="blog_show" path="/blog/{slug}">
                <default key="_controller">AppBundle:Blog:show</default>
            </route>
        </routes>

    .. code-block:: php
       :linenos:

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('blog', new Route('/blog/{page}', array(
            '_controller' => 'AppBundle:Blog:index',
            'page'        => 1,
        )));

        $collection->add('blog_show', new Route('/blog/{show}', array(
            '_controller' => 'AppBundle:Blog:show',
        )));

        return $collection;

Czy nie występuje tu jakiś problem? Prosze zauważyć, że obie trasy mają wzorce,
do których pasują ścieżki URL takie jak ``/blog/*``. Mechanizm trasowania Symfony
zawsze będzie wybierał **pierwszą** trasę, którą znajdzie. Innymi słowy, trasa
``blog_show`` nigdy nie zostanie dopasowana. Ponadto ścieżka URL taka jak
``/blog/my-blog-post`` będzie pasowała do pierwszej trasy (``blog``) i zwracała
bezsensowną wartość ``my-blog-post`` dla wieloznacznika ``{page}``.

======================  ========  ===============================
URL                     Trasa     Parametry
======================  ========  ===============================
``/blog/2``             ``blog``  ``{page}`` = ``2``
``/blog/my-blog-post``  ``blog``  ``{page}`` = ``"my-blog-post"``
======================  ========  ===============================

Rozwiązaniem problemu jest dodanie *wymagań* trasy lub  *warunków* trasy
(zobacz :ref:`book-routing-conditions`). Trasy w tym przykładzie będą działać
perfekcyjnie, jeśli ścieżka ``/blog/{page}`` *tylko* będzie dopasowywać ścieżki
URL, w których  segment ``{page}`` jest liczbą. Na szczęście wyrażenie regularne
wymagań może łatwo zostać dodane dla każdego parametru. Na przykład:

.. configuration-block::

    .. code-block:: php-annotations
       :linenos:

        // src/AppBundle/Controller/BlogController.php

        // ...

        /**
         * @Route("/blog/{page}", defaults={"page": 1}, requirements={
         *     "page": "\d+"
         * })
         */
        public function indexAction($page)
        {
            // ...
        }

    .. code-block:: yaml
       :linenos:

        # app/config/routing.yml
        blog:
            path:      /blog/{page}
            defaults:  { _controller: AppBundle:Blog:index, page: 1 }
            requirements:
                page:  \d+

    .. code-block:: xml
       :linenos:

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog" path="/blog/{page}">
                <default key="_controller">AppBundle:Blog:index</default>
                <default key="page">1</default>
                <requirement key="page">\d+</requirement>
            </route>
        </routes>

    .. code-block:: php
       :linenos:

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('blog', new Route('/blog/{page}', array(
            '_controller' => 'AppBundle:Blog:index',
            'page'        => 1,
        ), array(
            'page' => '\d+',
        )));

        return $collection;

Wymaganie ``\d+`` jest wyrażeniem regularnym, które dopuszcza jako wartość wieloznacznika
``{page}`` wyłącznie cyfry. Trasa ``blog`` wciąż będzie pasować do ścieżki URL,
takiej jak ``/blog/2`` (ponieważ 2 jest liczbą), ale nie będzie już pasować do
ścieżki URL takiego jak ``/blog/my-blog-post`` (ponieważ ``my-blog-post`` nie jest liczbą).

W efekcie końcowym scieżka URL ``/blog/my-blog-post`` będzie odpowiednio pasować do
trasy ``blog_show``.

========================  =============  ===============================
URL                       Trasa          Parametry
========================  =============  ===============================
``/blog/2``               ``blog``       ``{page}`` = ``2``
``/blog/my-blog-post``    ``blog_show``  ``{slug}`` = ``my-blog-post``
``/blog/2-my-blog-post``  ``blog_show``  ``{slug}`` = ``2-my-blog-post``
========================  =============  ===============================

.. sidebar:: Wcześniejsze trasy zawsze wygrywają

    Znaczy to tyle, że kolejność tras jest bardzo istotna. Jeśli trasa
    ``blog_show`` jest umieszczona nad trasą ``blog``, ścieżka URL ``/blog/2`` będzie
    pasować do ``blog_show``, zamiast do ``blog``, ponieważ wieloznacznik ``{slug}``
    ścieżki ``blog_show`` nie ma żadnych wymagań. Stosując odpowiednią kolejność
    oraz sprytne wymagania, można osiągnąć niemal wszystko.

Ponieważ parametr ``requirements`` jest wyrażeniem regularnym, kompleksowość i
elastyczność każdego z wymagań zależy całkowicie od programisty. Załóżmy, że
strona główna aplikacji jest dostępna w dwóch różnych językach, zależnie od ścieżki URL:

.. configuration-block::

    .. code-block:: php-annotations
       :linenos:

        // src/AppBundle/Controller/MainController.php

        // ...
        class MainController extends Controller
        {
            /**
             * @Route("/{_locale}", defaults={"_locale": "en"}, requirements={
             *     "_locale": "en|fr"
             * })
             */
            public function homepageAction($_locale)
            {
            }
        }

    .. code-block:: yaml
       :linenos:

        # app/config/routing.yml
        homepage:
            path:      /{_locale}
            defaults:  { _controller: AppBundle:Main:homepage, _locale: en }
            requirements:
                _locale:  en|fr

    .. code-block:: xml
       :linenos:

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="homepage" path="/{_locale}">
                <default key="_controller">AppBundle:Main:homepage</default>
                <default key="_locale">en</default>
                <requirement key="_locale">en|fr</requirement>
            </route>
        </routes>

    .. code-block:: php
       :linenos:

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('homepage', new Route('/{_locale}', array(
            '_controller' => 'AppBundle:Main:homepage',
            '_locale'     => 'en',
        ), array(
            '_locale' => 'en|fr',
        )));

        return $collection;

Część ścieżki URL ``{culture}`` w przychodzącym żądaniu jest dopasowywana do wyrażenia
regularnego ``(en|fr)``.

=======  ========================
Ścieżka  Parametry
=======  ========================
``/``    ``{_locale}`` = ``"en"``
``/en``  ``{_locale}`` = ``"en"``
``/fr``  ``{_locale}`` = ``"fr"``
``/es``  *won't match this route*
=======  ========================

.. index::
   single: trasowanie; wymagania metody HTTP

Dodawanie wymagania dotyczącego metody HTTP
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Oprócz ścieżki URL, można również dopasować metodę przychodzącego żądania
(tj. GET, HEAD, POST, PUT, DELETE).
Załóżmy, że tworzymy API dla bloga i mamy dwie trasy: jedną do wyswietlania wpisu
(dla żądania GET lub HEAD) oraz drugą dla aktualizowania wpisu (w żądaniu PUT).
Można to osiągnąć poprzez następującą konfigurację trasowania:

.. configuration-block::

    .. code-block:: php-annotations
       :linenos:

        // src/AppBundle/Controller/MainController.php
        namespace AppBundle\Controller;

        use Sensio\Bundle\FrameworkExtraBundle\Configuration\Method;
        // ...

        class BlogApiController extends Controller
        {
            /**
             * @Route("/api/posts/{id}")
             * @Method({"GET","HEAD"})
             */
            public function showAction($id)
            {
                // ... return a JSON response with the post
            }

            /**
             * @Route("/api/posts/{id}")
             * @Method("PUT")
             */
            public function editAction($id)
            {
                // ... edit a post
            }
        }

    .. code-block:: yaml
       :linenos:

        # app/config/routing.yml
        api_post_show:
            path:     /api/posts/{id}
            defaults: { _controller: AppBundle:BlogApi:show }
            methods:  [GET, HEAD]

        api_post_edit:
            path:     /api/posts/{id}
            defaults: { _controller: AppBundle:BlogApi:edit }
            methods:  [PUT]

    .. code-block:: xml
       :linenos:

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="api_post_show" path="/api/posts/{id}" methods="GET|HEAD">
                <default key="_controller">AppBundle:BlogApi:show</default>
            </route>

            <route id="api_post_edit" path="/api/posts/{id}" methods="PUT">
                <default key="_controller">AppBundle:BlogApi:edit</default>
            </route>
        </routes>

    .. code-block:: php
       :linenos:

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('api_post_show', new Route('/api/posts/{id}', array(
            '_controller' => 'AppBundle:BlogApi:show',
        ), array(), array(), '', array(), array('GET', 'HEAD')));

        $collection->add('api_post_edit', new Route('/api/posts/{id}', array(
            '_controller' => 'AppBundle:BlogApi:edit',
        ), array(), array(), '', array(), array('PUT')));

        return $collection;

Pomimo faktu, iż te dwie trasy mają identyczne ścieżki (``/api/posts/{id}``),
pierwsza z nich będzie pasować tylko do żądań GET lub HEAD, a druga tylko do żądań
PUT. Oznacza to, że można wyświetlać i edytować wpis poprzez ten sam adres URL,
jednocześnie wykorzystując do tego oddzielne akcje dla tych dwóch różnych działań.

.. note::
    Jeśli nie zostanie podane wymaganie dla `methods``, trasa będzie pasować do
    wszystkich metod HTTP.


.. index::
   single: trasowanie; host
   
.. _adding-host:
   
Dodawanie wymagania dotyczacego hosta
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Można również dopasowywać nagłówek HTTP `Host`_ przychodzącego żądania. Więcej
informacji można uzyskać a artykule :doc:`/components/routing/hostname_pattern`
w dokumentacji komponentu Routing.

.. index::
   single: trasowanie; wyrażenia warunkowe

.. _book-routing-conditions:

Całkowicie przerobiona trasa wykorzystująca warunki trasowania
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Jak zobaczyliśmy, trasa może być wykonana dla dopasowywania tylko określonych wieloznaczników
trasowania (poprzez wyrażenie regularne), metod HTTP lub nazw hosta. Jednak system
trasowania może zostać rozszerzony, uzyskując prawie nieograniczoną elastyczność
przy zastosowaniu *wyrażeń warunkowych*:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        contact:
            path:     /contact
            defaults: { _controller: AcmeDemoBundle:Main:contact }
            condition: "context.getMethod() in ['GET', 'HEAD'] and request.headers.get('User-Agent') matches '/firefox/i'"

    .. code-block:: xml
       :linenos:

        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="contact" path="/contact">
                <default key="_controller">AcmeDemoBundle:Main:contact</default>
                <condition>context.getMethod() in ['GET', 'HEAD'] and request.headers.get('User-Agent') matches '/firefox/i'</condition>
            </route>
        </routes>

    .. code-block:: php
       :linenos:

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('contact', new Route(
            '/contact', array(
                '_controller' => 'AcmeDemoBundle:Main:contact',
            ),
            array(),
            array(),
            '',
            array(),
            array(),
            'context.getMethod() in ["GET", "HEAD"] and request.headers.get("User-Agent") matches "/firefox/i"'
        ));

        return $collection;

Wartość opcji ``condition`` jest *wyrażeniem warunkowym trasowania*, które w skrócie
będziemy nazywać *warunkiem trasowania*. Więcej o składni warunków trasowania
można przeczytać w dokumencie :doc:`/components/expression_language/syntax`.
W pwyższym przykładzie, trasa nie zostanie dopasowana, chyba że metoda HTTP, to
GET albo HEAD  i jeśli nagłówek
``User-Agent`` dopasowuje ``firefox``.

W wyrażeniu można wykonać bardziej złożoną logikę wprowadzając dwie zmienne, które
są przekazywane do wyrażenia:

* ``context``: instancja :class:`Symfony\\Component\\Routing\\RequestContext`,
  która przechowuje najbardziej podstawowe informacje o dopasowywanej trasie;
* ``request``: obiekt Symfony :class:`Symfony\\Component\\HttpFoundation\\Request`
(zobacz :ref:`component-http-foundation-request`).

.. caution::

    Warunki trasowania *nie są* brane pod uwagę podczas generowania lokalizatora URL.

.. sidebar:: Wyrażenia są kompilowane do PHP

    W tle, wyrażenia są kompilowane do surowego PHP. Nasz przykład będzie generował
    następujący kod PHP w katalogu cache::

        if (rtrim($pathinfo, '/contact') === '' && (
            in_array($context->getMethod(), array(0 => "GET", 1 => "HEAD"))
            && preg_match("/firefox/i", $request->headers->get("User-Agent"))
        )) {
            // ...
        }

    Dlatego używanie klucza ``condition`` nie powoduje żadnego dodatkowego narzutu
    czasowego.

.. index::
   single: Routing; zaawansowany przykład
   single: Routing; parametr _format

.. _advanced-routing-example:

Przykład zaawansowanego trasowania
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

W tym momencie mamy wszystko, co potrzebne jest do stworzenia pełno wartościowej
struktury trasowania w Symfony. Oto przykład tego, jak elastyczny może być system
trasowania:

.. configuration-block::

    .. code-block:: php-annotations
       :linenos:

        // src/AppBundle/Controller/ArticleController.php

        // ...
        class ArticleController extends Controller
        {
            /**
             * @Route(
             *     "/articles/{_locale}/{year}/{title}.{_format}",
             *     defaults={"_format": "html"},
             *     requirements={
             *         "_locale": "en|fr",
             *         "_format": "html|rss",
             *         "year": "\d+"
             *     }
             * )
             */
            public function showAction($_locale, $year, $title)
            {
            }
        }

    .. code-block:: yaml
       :linenos:

        # app/config/routing.yml
        article_show:
          path:     /articles/{_locale}/{year}/{title}.{_format}
          defaults: { _controller: AppBundle:Article:show, _format: html }
          requirements:
              _locale:  en|fr
              _format:  html|rss
              year:     \d+

    .. code-block:: xml
       :linenos:

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="article_show"
                path="/articles/{_locale}/{year}/{title}.{_format}">

                <default key="_controller">AppBundle:Article:show</default>
                <default key="_format">html</default>
                <requirement key="_locale">en|fr</requirement>
                <requirement key="_format">html|rss</requirement>
                <requirement key="year">\d+</requirement>

            </route>
        </routes>

    .. code-block:: php
       :linenos:

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add(
            'article_show',
            new Route('/articles/{_locale}/{year}/{title}.{_format}', array(
                '_controller' => 'AppBundle:Article:show',
                '_format'     => 'html',
            ), array(
                '_locale' => 'en|fr',
                '_format' => 'html|rss',
                'year'    => '\d+',
            ))
        );

        return $collection;

Jak widać, ta trasa będzie pasować tylko wtedy, kiedy wieloznacznik ``{culture}``
w ścieżce URL będzie równy ``en`` lub ``fr``, a ``{year}`` jest liczbą. Ponadto
ta trasa pokazuje, jak można wykorzystać kropkę pomiędzy wieloznacznikami zamiast
ukośnika. Ścieżki URL pasujące do tej trasy mogą wyglądać np. tak:

* ``/articles/en/2010/my-post``
* ``/articles/fr/2010/my-post.rss``
* ``/articles/en/2013/my-latest-post.html``

.. _book-routing-format-param:

.. sidebar:: Specjalny parametr ``_format``

    Powyższy przykład pokazuje również specjalny parametr trasowania ``_format``.
    Przy zastosowaniu tego parametru dopasowaną wartością może być "format żądania"
    obiektu Request.
    
    Ostatecznie format żądania służy do takich rzeczy jak ustawienie
    nagłówka ``Content-Type`` odpowiedzi (np. żądany format json tłumaczony jest na
    ``Content-Type application/json``). Do renderowania może być
    również stosowany w akcji inny szablon dla każdej wartości ``_format``.
    Parametr ``_format`` jest bardzo skutecznym sposobem na renderowanie tej samej
    treści w różnych formatach.
    
    W wersjach Symfony wcześniejszych niż 3.0, mozliwe jest zastępowanie formatu
    żądania przez dodanie parametru zapytania o nazwie ``_format`` (na przykład:
    ``/foo/bar?_format=json``). Stosowanie tego rozwiązania jest nie tylko złą
    praktyką, ale również utrudni aktualizacje kodu aplikacji do Symfony 3.

.. note::
   
   Czasami można chcieć, aby niektóre części tras były konfigurowalne globalnie.
   Symfony umożliwia zrobienie tego przez wykorzystanie parametrów poziomu
   kontenera usług. Więcej na ten temat można sie dowiedzieć w artykule
   ":doc:`Jak stosować parametry kontenera usług w trasowaniu</cookbook/routing/service_container_parameters>`".
   
Specjalne parametry trasowania
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Jak mogliśmy sie przekonać, każda wartość parametr trasowania jest dostępna jako
argument w metodzie kontrolera, zwanej akcją. Dodatkowo istnieją jeszcze trzy specjalne
parametry - każdy z nich dodaje unikatową cząstkę funkcjonalności do aplikacji:

* ``_controller``: określa która akcja ma zostać użyta gdy trasa zostanie dopasowana;

* ``_format``: służy do ustawienia formatu żądania (:ref:`czytaj więcej<book-routing-format-param>`);

* ``_locale``: służy do ustawienia języka sesji (:ref:`czytaj więcej<book-translation-locale-url>`).

.. tip::

    Jeśli używa się parametru ``_locale``, jego wartość będzie również przechowywana
    w sesji, dzięki czemu dla kolejnych żądań będą stosowane te same ustawinia
    regionalne.

.. index::
   single: trasowanie; akcje
   single: akcja; format łańcucha nazewniczego

.. _controller-string-syntax:

Wzorzec nazwy kontrolera
------------------------

Każda trasa musi posiadać parametr ``_controller``, który informuje Symfony,
która akcja powinna zostać uruchomiona, gdy trasa zostanie dopasowana.
Ten parametr używa prostego wzorca zwanego *logiczną nazwą kontrolera*, który
Symfony dopasowuje to do nazwy konkretnej metody i klasy PHP.
Wzorzec ten składa się z trzech części, każda z nich oddzielona jest dwukropkiem:

    **pakiet**:**kontroler**:**akcja**

Na przykład, wartość ``AcmeBlogBundle:Blog:show`` parametru ``_controller_`` oznacza:

=========  ==================  ==============
Pakiet     Klasa kontrolera    Nazwa metody
=========  ==================  ==============
AppBundle  ``BlogController``  ``showAction``
=========  ==================  ==============

Kontroler może wyglądać np. tak::
   
   // src/AppBundle/Controller/BlogController.php
    namespace AppBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\Controller;

    class BlogController extends Controller
    {
        public function showAction($slug)
        {
            // ...
        }
    }

Proszę zauważyć, że Symfony dodaje ciąg ``Controller`` do nazwy klasy (``Blog``
=> ``BlogController``) oraz ciąg ``Action`` do nazwy metody (``show`` => ``showAction``).

Można również odnieść się do akcji używając w pełni kwalifikowanej nazwy klasy
oraz metody:
``Acme\BlogBundle\Controller\BlogController::showAction``.
Jeśli przestrzega się kilku prostych konwencji, logiczna nazwa kontrolera jest
bardziej zwięzła i pozwala na większą elastyczność.

.. note::

   Oprócz używania logicznej nazwy oraz w pełni kwalifikowanej nazwy klasy,
   Symfony dostarcza trzeci sposób odwoływania się do akcji. Tą metoda używa
   tylko jednego dwukropka jako separatora (np. ``service_name:indexAction``)
   i odwołuje się do akcji jako usługi (patrz :doc:`/cookbook/controller/service`).

Parametry trasy a argumenty akcji
---------------------------------

Parametry trasy (np. ``{slug}``) są szczególnie ważne, ponieważ każdy z nich
jest dostępny jako argument metody kontrolera::
   
   public function showAction($slug)
    {
      // ...
    }

W rzeczywistości, cała kolekcja ``defaults`` jest scalana z wartościami parametrów
w jedną pojedynczą tablicę. Każdy klucz tej tablicy jest dostępny jako argument
akcji.

Innymi słowy, dla każdego argumentu akcji, Symfony szuka parametru
o nazwie takiej samej jak argument i przypisuje jego wartość do tego argumentu.
W powyżej rezentowanym zaawansowanym przykładzie, dowolna kombinacja (o dowolnej
kolejności) następujacych zmiennych może być użyta jako argumenty akcji
``showAction()``:

* ``$culture``
* ``$year``
* ``$title``
* ``$_format``
* ``$_controller``
* ``$_route``

Ponieważ wieloznaczniki i kolekcja ``defaults`` są łączone razem, nawet zmienna
``$_controller`` jest dostępna. Więcej szczegółów jest omówionych w rozdziale
:ref:`route-parameters-controller-arguments`.

.. tip::

    Możesz również używać specjalnej zmiennej ``$_route``, która przechowuje
    nazwę trasy, która została dopasowana.

.. index::
   single: trasowanie; dołącznie zewnętrznych zasobów

.. _routing-include-external-resources:

Dołączanie zewnętrznych zasobów trasowania
------------------------------------------

Wszystkie trasy są ładowane z pojedyńczego pliku konfiguracyjnego - zazwyczaj
``app/config/routing.yml`` (czytaj rozdział :ref:`creating-routes`).
Często jednak zachodzi potrzeba ładowania trasy z innych miejsc, takich jak plik
trasowania umieszczonego w pakiecie. Można tego dokonać poprzez "zaimportowanie"
tego pliku:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/routing.yml
        app:
            resource: '@AppBundle/Controller/'
            type:     annotation # required to enable the Annotation reader for this resource

    .. code-block:: xml
       :linenos:

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <!-- the type is required to enable the annotation reader for this resource -->
            <import resource="@AppBundle/Controller/" type="annotation"/>
        </routes>

    .. code-block:: php
       :linenos:

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;

        $collection = new RouteCollection();
        $collection->addCollection(
            // second argument is the type, which is required to enable
            // the annotation reader for this resource
            $loader->import("@AppBundle/Controller/", "annotation")
        );

        return $collection;

.. note::

   Podczas importowania zasobów YAML, klucz (np. ``acme_hello``) jest bez znaczenia.
   Wystarczy się upewnić, że jest on unikalny, przez co żadna inna linia nie nadpisze go.

Klucz ``resource`` wczytuje podany zasób trasowania. W tym przypadku zasobem jest
pełna ścieżka do pliku, gdzie skrót ``@AcmeHelloBundle`` przekształacany jest 
ścieżkę do pakietu. Gdy wskazuje na katalog, to sparsowane zostaną wszystkie pliki
w tym katalogu i wstawione do trasowania.

.. note::

    Mozna również dołaczyć inne plikikonfigurujace trasowanie, co jest często
    stosowane w pakietach zewnętrznych:

    .. configuration-block::

        .. code-block:: yaml
           :linenos:

            # app/config/routing.yml
            app:
                resource: '@AcmeOtherBundle/Resources/config/routing.yml'

        .. code-block:: xml
           :linenos:

            <!-- app/config/routing.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <routes xmlns="http://symfony.com/schema/routing"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xsi:schemaLocation="http://symfony.com/schema/routing
                    http://symfony.com/schema/routing/routing-1.0.xsd">

                <import resource="@AcmeOtherBundle/Resources/config/routing.xml" />
            </routes>

        .. code-block:: php
           :linenos:

            // app/config/routing.php
            use Symfony\Component\Routing\RouteCollection;

            $collection = new RouteCollection();
            $collection->addCollection(
                $loader->import("@AcmeOtherBundle/Resources/config/routing.php")
            );

            return $collection;

Przedrostki dla importowanych tras
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Można również zapewnić "przedrostek" dla importowanych tras.
Na przykład załóżmy, że chcemy popzedzić wszystkie trasy w AppBundle z ``/site``
(np. ``/site/blog/{slug}`` zamiast ``/blog/{slug}``):

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/routing.yml
        app:
            resource: '@AppBundle/Controller/'
            type:     annotation
            prefix:   /site

    .. code-block:: xml
       :linenos:

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <import
                resource="@AppBundle/Controller/"
                type="annotation"
                prefix="/site" />
        </routes>

    .. code-block:: php
       :linenos:

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;

        $app = $loader->import('@AppBundle/Controller/', 'annotation');
        $app->addPrefix('/site');

        $collection = new RouteCollection();
        $collection->addCollection($app);

        return $collection;

Ścieżka każdej ładowanej trasy z nowego trasowania będzie poprzedzona teraz
łańcuchem ``/site``.

Dodawanie wyrażeń regularnych hosta do importowanych tras
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Można ustawić wyrażenie regularne hosta na importowanych trasach. Więcej informacji
można znaleźć w rozdziale :ref:`component-routing-host-imported`.


.. index::
   single: trasowanie; debugowanie

Wizualizowanie i debugowanie tras
---------------------------------

Dodając i dostosowując ścieżki, pomocna może okazać się możliwość wizualizacji
oraz uzyskania szczegółowej informacji o trasach. Dobrym sposobem,
na zobaczenie wszystkich tras aplikacji jest użycie polecenia ``router:debug``.
Polecenie należy wykonać głównym katalogu projektu, tak jak poniżej:

.. code-block:: bash

    $ php app/console debug:router

Polecenie to wyświetli na ekranie listę wszystkich skonfigurowanych
tras aplikacji:

.. code-block:: text

    homepage              ANY       /
    contact               GET       /contact
    contact_process       POST      /contact
    article_show          ANY       /articles/{culture}/{year}/{title}.{_format}
    blog                  ANY       /blog/{page}
    blog_show             ANY       /blog/{slug}

Można również uzyskać szczegółowe informacje o pojedynczej trasie, dołączając
jej nazwę do powyższego polecenia:

.. code-block:: bash

    $ php app/console debug:router article_show
    
Można sprawdzić czy trasa pasuje do ścieżki posługując się poleceniem konsoli ``router:match``:

.. code-block:: bash
      
   $ php app/console router:match /blog/my-latest-post


Polecenie to wydrukuje dopasowana do ścieżki URL trasę.

.. code-block:: text

    Route "blog_show" matches

    
.. index::
   single: trasowanie; generowanie ścieżek URL

Generowanie ścieżek URL
-----------------------

System trasowania powinien również być używany do generowania ścieżek URL.
W rzeczywistości, trasowanie jest systemem dwukierunkowym: odwzorowuje ścieżkę URL
na akcję (i parametry), oraz z powrotem trasę (i parametry) na ścieżkę URL.
Ten dwukierunkowy system tworzony jest przez metody
:method:`Symfony\\Component\\Routing\\Router::match` oraz
:method:`Symfony\\Component\\Routing\\Router::generate`.
Przyjrzyjmy się poniższemu przykładowi wykorzystującemu wcześniejszą trasę
``blog_show``::

    $params = $this->get('router')->match('/blog/my-blog-post');
    // array(
    //     'slug'        => 'my-blog-post',
    //     '_controller' => 'AppBundle:Blog:show',
    // )

    $uri = $this->get('router')->generate('blog_show', array(
        'slug' => 'my-blog-post'
    ));
    // /blog/my-blog-post
    

Aby wygenerować ścieżkę URL, musi się określić nazwę trasy (np. ``blog_show``) oraz
wszystkie wieloznaczniki (np. ``slug = my-blog-post``) użyte we wzorcu tej trasy.
Z tej informacji można wygenerować łatwo każdą ścieżkę URL::

   class MainController extends Controller
    {
        public function showAction($slug)
        {
            // ...

            $url = $this->generateUrl(
                'blog_show',
                array('slug' => 'my-blog-post')
            );
        }
    }

.. note::

    Metoda ``generateUrl()`` zdefiniowana w bazowej klasie
    :class:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller`
    jest po prostu skrótem do kodu::
      
      $url = $this->container->get('router')->generate(
            'blog_show',
            array('slug' => 'my-blog-post')
        );
    
    
W kolejnym rozdziale poznasz jak generować ścieżki URL w szablonach.

.. tip::

    Jeśli fronton aplikacji wykorzystuje żądania AJAX, można generować ścieżki  URL
    w JavaScript na podstawie konfiguracji trasowania. Używając
    `FOSJsRoutingBundle`_, można to zrobić dokładnie tak:

    .. code-block:: javascript
       :linenos:

       var url = Routing.generate(
            'blog_show',
            {"slug": 'my-blog-post'}
        );
      
    Więcej informacji można znaleźć w dokumentacji tego pakietu.

.. index::
   single: trasowanie; generowanie ścieżek URL w łańcuchach zapytań

Generowanie scieżek URL w łańcuchach zapytań
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Metoda ``generate`` pobiera tablicę wartości wieloznacznych do generowania
identyfikatorów URI. Jeśli przekaże się dodatkowe wieloznacznki, to zostaną one
dodane do URI jako łańcuch zapytania::

    $this->get('router')->generate('blog', array(
        'page' => 2,
        'category' => 'Symfony'
    ));
    // /blog/2?category=Symfony

.. index::
   single: trasowanie; generowanie ścieżek URL w szablonach
   
Generowanie ścieżek URL w szablonach
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Najczęściej generowanię ścieżek URL ma miejsce wewnętrz szablonów
podczas łączenia stron w aplikacji. Dokonuje sie tego jak poprzednio, ale
stosując pomocnicze funkcje szablonów:

.. configuration-block::

    .. code-block:: html+jinja
       :linenos:

        <a href="{{ path('blog_show', {'slug': 'my-blog-post'}) }}">
          Read this blog post.
        </a>

    .. code-block:: html+php
       :linenos:

        <a href="<?php echo $view['router']->path('blog_show', array(
            'slug' => 'my-blog-post',
        )) ?>">
            Read this blog post.
        </a>
        
.. versionadded:: 2.8
    W Symfony 2.8 została wprowadzona szablonowa funkcja pomocnicza PHP ``path()``.
    Wcześniej trzeba było stosować metodę pomocniczą ``generate()``.



.. index::
   single: trasowanie; bezwględne ścieżki URL

Generowanie bezwzględnych ścieżek URL
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Domyślnie mechanizm trasowania generuje względne ścieżki URL (np. ``/blog``).
Dla wygenerowania bezwzględnych ścieżek URL, trzeba przekazać ``true`` jako trzeci
argument metody ``generate()``::
   
   use Symfony\Component\Routing\Generator\UrlGeneratorInterface;

    $this->generateUrl('blog_show', array('slug' => 'my-blog-post'), UrlGeneratorInterface::ABSOLUTE_URL);
    // http://www.example.com/blog/my-blog-post

Wewnątrz szablonu, w Twig, zamiast funkcji ``path()`` (generujacej wzgledną
ścieżkę URL) wystarczy użyć funkcję ``url()`` (generująca bezwzględny adres URL).
W PHP trzeba przekazać ``true`` do  ``generate()``:

.. configuration-block::

    .. code-block:: html+jinja
       :linenos:

        <a href="{{ url('blog_show', {'slug': 'my-blog-post'}) }}">
          Read this blog post.
        </a>

    .. code-block:: html+php
       :linenos:

        <a href="<?php echo $view['router']->url('blog_show', array(
            'slug' => 'my-blog-post',
        )) ?>">
            Read this blog post.
        </a>

.. versionadded:: 2.8
    W Symfony 2.8. wprowadzono szablonową funkcje pomocniczą PHP The ``url()``.
    Wczesniej trzeba było stosować w 
    ``Symfony\Component\Routing\Generator\UrlGeneratorInterface::ABSOLUTE_URL``
    metodę pomocniczą ``generate()``, przekazywaną jako trzeci argument.


.. note::

    Host używany podczas generowania bezwzględnego adresu URL jest automatycznie
    wykrywany przy uzyciu boeżacego obiektu ``Request``. Podczas generowania
    bezwzglednych adresów URL poza kontekstem web (na przykład w poleceniu kontekstowym)
    to nie bedzie działać. Zobacz :doc:`/cookbook/console/sending_emails` w celu
    zapoznania sie z rozwiązaniem tego problemu.

Podsumowanie
------------

Trasowanie to system odwzorowania adresu URL przychodzącego żądania na akcję w kontrolerze,
która ma być wywołany w celu przetworzenia żądania. Pozwala to zarówno na określanie
przyjaznych adresów URL, jak i oddzielenia funkcjonalności aplikacji od od struktury
adresów URL. Trasowanie jest dwukierunkowym mechanizmem, co oznacza, że może być
również wykorzystywany do generowania adresów URL.

Dowiedz się więcej w Receptariuszu
----------------------------------

* :doc:`/cookbook/routing/scheme`

.. _`FOSJsRoutingBundle`: https://github.com/FriendsOfSymfony/FOSJsRoutingBundle
.. _`Uniform Resource Locator`: http://pl.wikipedia.org/wiki/Uniform_Resource_Locator
.. _`Host`: http://pl.wikipedia.org/wiki/Lista_nag%C5%82%C3%B3wk%C3%B3w_HTTP
.. _`łańcuch zapytania`: http://pl.wikipedia.org/wiki/URI
.. highlight:: php
   :linenothreshold: 2

Kontroler
=========

Wciąż jesteś z nami po pierwszych dwóch rozdziałach? Zaczynasz być fanem Symfony!
Bez zbędnych ceregieli, zobacz co potrafią zrobić kontrolery.

Zwracanie surowych odpowiedzi
-----------------------------

Symfony można określić jako *framework żądanie-odpowiedź*. Gdy użytkownik wykonuje
żądanie do aplikacji, Symfony tworzy obiekt ``Request`` w celu hermetyzacji wszystkich
informacji odnoszących sie do żądania HTTP. Podobnie, wynikiem działania każdej
akcji dowolnego kontrolera jest utworzenie obiektu ``Response``, który Symfony
używa do wygenerowania treści HTML zwracanej użytkownikowi.

Jak dotąd, wszystkie akcje pokazane w tym poradniku uzywały skrótu ``$this->render()``
do zwracania jako wynik odpowiedzi. W przypadku, gdy zachodzi taka potrzeba, można
również utworzyć surowy obiekt ``Response`` w celu zwrócenia zawartości tekstowej::

    // src/AppBundle/Controller/DefaultController.php
    namespace AppBundle\Controller;

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;
    use Symfony\Component\HttpFoundation\Response;

    class DefaultController extends Controller
    {
        /**
         * @Route("/", name="homepage")
         */
        public function indexAction()
        {
            return new Response('Welcome to Symfony!');
        }
    }

Parametry trasy
---------------

W większości przypadków ścieżki tras zawierają część ze zmienną.
Jeśli tworzy się, na przykład, aplikację bloga, lokalizator URL do wyświetlanych
artykułów powinien zawierać ich tytuł lub jakiś inny unikalny identyfikator,
umożliwiający aplikacji dokładnie zlokalizować artykuł jaki ma być wyświetlony.

W aplikacjach Symfony część ścieżki trasy zawierająca zmienną jest zamykana w
nawiasach klamrowych (np. ``/blog/read/{article_title}/``). Każdej części ze
zmienną przypisywana jest unikalna nazwa, która może zostać później użyta w
akcji do pobrania właściwej wartości.

Stwórzmy nową akcję ze zmiennymi trasy, aby pokazać tą funkcjonalność w działaniu.
Otwórz plik ``src/AppBundle/Controller/DefaultController.php`` i dodaj
w nim nową metodę o nazwie ``helloAction`` z następującą zawartością::

    // src/AppBundle/Controller/DefaultController.php
    namespace AppBundle\Controller;

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;

    class DefaultController extends Controller
    {
        // ...

        /**
         * @Route("/hello/{name}", name="hello")
         */
        public function helloAction($name)
        {
            return $this->render('default/hello.html.twig', array(
                'name' => $name
            ));
        }
    }

Otwórz następnie przegladarkę i odwiedź adres ``http://localhost:8000/hello/fabien``,
aby zobaczyć wynik wykonania nowej akcji. Zamiast wyniku akcji, zobaczysz stronę
błędu. jak się przypuszczalnie domyślasz, przyczyną błędu jest to, że próbowaliśmy
renderować szablon (``default/hello.html.twig``), który jeszcze nie istnieje.

Utwórzmy nowy szablon ``app/Resources/views/default/hello.html.twig`` z następującą
zawartością:

.. code-block:: html+jinja
   :linenos:

    {# app/Resources/views/default/hello.html.twig #}
    {% extends 'base.html.twig' %}

    {% block body %}
        <h1>Hi {{ name }}! Welcome to Symfony!</h1>
    {% endblock %}

Odwiedź ponownie adres ``http://localhost:8000/hello/fabien`` a zobaczysz ten
nowy szablon zrenderowany z danymi przekazanymi przez kontroler.
Jeśli zmienisz ostatnia część ścieżki URL (np. ``http://localhost:8000/hello/thomas``)
i przeładujesz przegladarkę, zobaczysz stronę wyświetlajacą nowy komunikat.
Jeśli usuniesz ostatnią część z adresu URL (np. ``http://localhost:8000/hello``),
Symfony wyświetli strone błędu, ponieważ trasa oczekuje nazwy i trzeba ja dostarczyć
w adresie URL.

Używanie zmiennej ``_format``
-----------------------------

Obecnie, aplikacje internetowe powinna dostarczać coś więcej niż tylko
strony HTML. Od XML dla kanałów RSS lub usług internetowych, do JSON dla żądań Ajax,
istnieje wiele różnych formatów do wyboru. Obsługa tych formatów w Symfony jest prosta,
dzięki specjalnej zmiennej o nazwie ``_format``, która przechowuje żądany przez
użytkownika format wyjścia.

Poprawmy trasę ``hello``, dodając nowa zmienną ``_format`` z wartością ``html``
as its default value::

    // src/AppBundle/Controller/DefaultController.php
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Template;

    // ...

    /**
     * @Route("/hello/{name}.{_format}", defaults={"_format"="html"}, name="hello")
     */
    public function helloAction($name, $_format)
    {
        return $this->render('default/hello.'.$_format.'.twig', array(
            'name' => $name
        ));
    }

Oczywiście, gdy obsługuje się kilka żądanych formatów, trzeba dostarczyć szablon
dla każdego obsługiwanego formatu wyjścia. W naszym przypadku trzeba utworzyć
nowy szablon ``hello.xml.twig``:

.. code-block:: xml+php
   :linenos:

    <!-- app/Resources/views/default/hello.xml.twig -->
    <hello>
        <name>{{ name }}</name>
    </hello>

Teraz, gdy odwiedzisz adres ``http://localhost:8000/hello/fabien``, zobaczysz
zwykła stronę HTML, ponieważ ``html`` jest domyślnym formatem. Po odwiedzeniu
``http://localhost:8000/hello/fabien.html`` otrzymasz ponownie tą samą stronę
HTML, tym razem dlatego, że w adresie URL jawnie został uzyty format ``html``.
Wreszcie, jeśłi odwiedzisz adres ``http://localhost:8000/hello/fabien.xml``,
zobaczysz w przegladarce zrenderowany nowy szablon XML.

To wszystko co jest niezbędne. Dla standardowych formatów Symfony będzie również
automatycznie wybierać dla odpowiedzi najlepszy nagłówek ``Content-Type``.
W celu ograniczenia obsługiwanych formatów w danej akcji, trzeba użyć w adnotacji
``@Route()`` opcji ``requirements``::

    // src/AppBundle/Controller/DefaultController.php
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Template;

    // ...

    /**
     * @Route("/hello/{name}.{_format}",
     *     defaults = {"_format"="html"},
     *     requirements = { "_format" = "html|xml|json" },
     *     name = "hello"
     * )
     */
    public function helloAction($name, $_format)
    {
        return $this->render('default/hello.'.$_format.'.twig', array(
            'name' => $name
        ));
    }
    
Kontroler będzie teraz również dopasowywał ścieżkę URL taką jak ``/demo/hello/Fabien.xml``
lub ``/demo/hello/Fabien.json``, ale pokaże błąd 404, jeśli będzie sie próbowało
pobrać takie zasoby, jak ``/hello/fabien.js``, ponieważ wartość zmiennej ``_format``
nie zgadza się z wymaganiami ``requirements``.

.. _redirecting-and-forwarding:

Przekierowywanie
----------------

Jeśli chce się przekierować użytkownika do innej strony, to trzeba użyć metodę
``redirectToRoute()``::

    // src/AppBundle/Controller/DefaultController.php
    class DefaultController extends Controller
    {
        /**
         * @Route("/", name="homepage")
         */
        public function indexAction()
        {
            return $this->redirectToRoute('hello', array('name' => 'Fabien'));
        }
    }

Metoda ``redirectToRoute()`` pobiera jako argumenty nazwę trasy i opcjonalną tablicę
parametrów i przekierowuje uzytkownika do adresu URL generowanego przez te argumenty.

Wyświetlanie stron błędów
-------------------------

Podczas wykonywania każdej aplikacji internetowej nieuchronnie zdarzają się błędy.
W przypadku błędu ``404``, Symfony zawiera przydatny skrót, który można wykorzystać
w kontrolerach::

    // src/AppBundle/Controller/DefaultController.php
    // ...

    class DefaultController extends Controller
    {
        /**
         * @Route("/", name="homepage")
         */
        public function indexAction()
        {
            // ...
            throw $this->createNotFoundException();
        }
    }

Dla błędów ``500``, wystarczy zrzucić w kontrolerze zwykły wyjątek PHP a Symfony
przekształci go do odpowiedniej strony błędu ``500``::

    // src/AppBundle/Controller/DefaultController.php
    // ...

    class DefaultController extends Controller
    {
        /**
         * @Route("/", name="homepage")
         */
        public function indexAction()
        {
            // ...
            throw new \Exception('Something went horribly wrong!');
        }
    }

Pobieranie informacji z żądania
-------------------------------

Czasem kontroler potrzebuje mieć dostęp do inforamcji związanych z żądaniem
użytkownika, takich jak preferowany język, adres IP parametry zapytania URL.
W celu uzyskania dostęþu do tych informacji, trzeba dodać do akcji nowy argument
typu ``Request``. Nazwa tego nowego argumentu nie ma znaczenia, ale musi być
poprzedzona typem argumentu o wartości ``Request`` (nie
zapomnij dodać w kontrolerze nowe wyrażenie ``use``, aby zaimportować klasę
``Request``)::

    // src/AppBundle/Controller/DefaultController.php
    namespace AppBundle\Controller;

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;
    use Symfony\Component\HttpFoundation\Request;

    class DefaultController extends Controller
    {
        /**
         * @Route("/", name="homepage")
         */
        public function indexAction(Request $request)
        {
            // is it an Ajax request?
            $isAjax = $request->isXmlHttpRequest();

            // what's the preferred language of the user?
            $language = $request->getPreferredLanguage(array('en', 'fr'));

            // get the value of a $_GET parameter
            $pageName = $request->query->get('page');

            // get the value of a $_POST parameter
            $pageName = $request->request->get('page');
        }
    }

.. note::
   Wymuszaniu typów deklaracji w PHP jest opisane w `dokumentacji PHP`_   

W szablonie, można także uzyskać dostęp do obiektu ``Request`` poprzez
zmienną ``app.request``, automatycznie dostarczaną przez Symfony:

.. code-block:: html+jinja
   :linenos:

    {{ app.request.query.get('page') }}

    {{ app.request.request.get('page') }}


Utrzymywanie danych w sesji
---------------------------

Pomimo że protokół HTTP jest bezstanowy, Symfony dostarcza pomocny obiekt sesji,
który reprezentuje klienta (może to być realna osoba używająca przeglądarki, bot
lub usługa internetowa). Symfony przechowuje w pliku cookie atrybuty sesji pomiędzy
dwoma żądaniami, wykorzystując natywną sesję PHP.


Przechowywania i pobierania informacji z sesji można łatwo uzyskać w dowolnym kontrolerze::

    use Symfony\Component\HttpFoundation\Request;

    public function indexAction(Request $request)
    {
        $session = $request->getSession();

        // store an attribute for reuse during a later user request
        $session->set('foo', 'bar');

        // get the value of a session attribute
        $foo = $session->get('foo');

        // use a default value if the attribute doesn't exist
        $foo = $session->get('foo', 'default_value');
    }

Można również zapisać "wiadomości fleszowe", które będą automatycznie usuwane po
następnym żądaniu. Są one przydatne, gdy chce się ustawić komunikat o sukcesie,
przed przekierowaniem użytkownika na inną stronę::

    public function indexAction(Request $request)
    {
        // ...

        // store a message for the very next request
        $this->addFlash('notice', 'Congratulations, your action succeeded!');
    }

Następnie mozna wyświetlić ten komunikat w szablonie, tak jak tu:

.. code-block:: html+jinja
   :linenos:

    {% for flashMessage in app.session.flashbag.get('notice') %}
        <div class="flash-notice">
            {{ flashMessage }}
        </div>
    {% endfor %}
    
Podsumowanie
------------

To wszystko w tym temacie i nie jestem pewny, czy czytanie tego zajęło Ci pełne 10 minut.
W pierwszej części pokrótce zapoznaliśmy się z pakietami poznając, że wszystkie dotychczas
poznane funkcjonalności są składnikiem rdzenia frameworka i wiemy już też, że
dzięki pakietom wszystko w Symfony może zostać rozszerzone lub wymienione. To właśnie
jest tematem :doc:`następnej części przewodnika<the_architecture>`.


.. _`dokumentacji PHP`: http://php.net/manual/en/functions.arguments.php#functions.arguments.type-declaration
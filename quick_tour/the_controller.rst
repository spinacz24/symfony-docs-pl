.. highlight:: php
   :linenothreshold: 2

Kontroler
=========

Wciąż z nami po pierwszych dwóch częściach? Zaczynasz się uzależniać od Symfony2!
Bez zbędnych ceregieli, zobacz co potrafią zrobić kontrolery.

Używanie formatów
-----------------

Obecnie, aplikacje internetowe powinna dostarczać coś więcej niż tylko
strony HTML. Od XML dla kanałów RSS lub usług internetowych, do JSON dla żądań Ajax,
istnieje wiele różnych formatów do wyboru. Obsługa tych formatów w Symfony2 jest prosta.
Popraw wydajność definicji tras przez dodanie domyślnej wartości xml dla zmiennej
``_format``::

    // src/Acme/DemoBundle/Controller/DemoController.php
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Template;

    // ...

    /**
     * @Route("/hello/{name}", defaults={"_format"="xml"}, name="_demo_hello")
     * @Template()
     */
    public function helloAction($name)
    {
        return array('name' => $name);
    }

Dzięki użyciu stosowanego formatu żądania (zdefiniowanego w specjalnej zmiennej ``_format``),
Symfony2 automatycznie wybiera odpowiedni szablon, w tym przypadku ``hello.xml.twig``:

.. code-block:: xml+php

    <!-- src/Acme/DemoBundle/Resources/views/Demo/hello.xml.twig -->
    <hello>
        <name>{{ name }}</name>
    </hello>

To wszystko co jest niezbędne. Dla standardowych formatów Symfony2 będzie również
automatycznie wybierać najlepszy nagłówek ``Content-Type`` dla odpowiedzi. Jeżeli
chce się obsługiwać różne formaty dla pojedyńczej akcji, to trzeba zamiat tego
użyć w ścieżce trasy wieloznacznika {_format}::

    // src/Acme/DemoBundle/Controller/DemoController.php
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Template;

    // ...

    /**
     * @Route(
     *     "/hello/{name}.{_format}",
     *     defaults = { "_format" = "html" },
     *     requirements = { "_format" = "html|xml|json" },
     *     name = "_demo_hello"
     * @Template()
     */
    public function helloAction($name)
    {
        return array('name' => $name);
    }

Kontroler będzie teraz również dopasowaywał ścieżkę URL taka jak ``/demo/hello/Fabien.xml``
lub ``/demo/hello/Fabien.json``.

Wpis ``requirements`` określa wyrażenie regularne, jakie wieloznacznik musi dopasować.
W tym przykładzie, jeżeli zażąda się zasobu ``/demo/hello/Fabien.js``, to otrzyma się
komunikat błędu 404 HTTP, jako że podany łańcuch nie pasuje do wymagania ``_format``.

Przekierowanie i przekazanie
----------------------------

Jeśli chce się przekierować użytkownika do innej strony, to trzeba użyć metody
``redirect()``::

    return $this->redirect($this->generateUrl('_demo_hello', array('name' => 'Lucas')));

Metoda ``generateUrl()`` jest identyczna z funkcją ``path()`` którą użyliśmy w szablonach.
Jako argumenty, przyjmuje nazwę trasy oraz tablicę parametrów i zwraca związany
przyjazny adres URL.

Można także wewnętrznie przekazać akcję do innej akcji używając metodę ``forward()``::

    * @Route(
     *     "/hello/{name}.{_format}",
     *     defaults = { "_format" = "html" },
     *     requirements = { "_format" = "html|xml|json" },
     *     name = "_demo_hello"

Wyświetlanie stron błędów
-------------------------

Podczas wykonywania każdej aplikacji internetowej nieuchronnie zdarzają się błędy.
W przypadku błędu ``404``, Symfony zawiera przydatny skrót, który można wykorzystać
w kontrolerach::

    throw $this->createNotFoundException();

Dla błędów ``500``, wystarczy zrzucić w kontrolerze zwykły wyjątek PHP a Symfony
przekształci go do odpowiedniej strony błędu ``500``::

    throw new \Exception('Something went wrong!');

Pobieranie informacji z żądania
-------------------------------

Symfony automatycznie wstrzykuje obiekt ``Request`` gdy kontroler ma argument,
który jest typem odgadywanym w ``Symfony\Component\HttpFoundation\Request``::

    use Symfony\Component\HttpFoundation\Request;

    public function indexAction(Request $request)
    {
        $request->isXmlHttpRequest(); // is it an Ajax request?

        $request->getPreferredLanguage(array('en', 'fr'));

        $request->query->get('page');   // get a $_GET parameter

        $request->request->get('page'); // get a $_POST parameter
    }

W szablonie, możesz także uzyskać dostęp do obiektu ``Request`` poprzez
zmienną ``app.request``:

.. code-block:: html+jinja

    {{ app.request.query.get('page') }}

    {{ app.request.parameter('page') }}

Utrzymywanie danych w sesji
---------------------------

Pomimo że protokół HTTP jest bezstanowy, Symfony2 dostarcza pomocny obiekt sesji,
który reprezentuje klienta (może to być realna osoba używająca przeglądarki, bot
lub usługa internetowa). Symfony2 przechowuje w pliku cookie atrybuty sesji pomiędzy
dwoma żądaniami, wykorzystując natywną sesję PHP.


Przechowywania i pobierania informacji z sesji można łatwo uzyskać w dowolnym kontrolerze::

    $session = $this->getRequest()->getSession();

    // przechowanie atrybutu do ponownego użycia w późniejszym żądaniu użytkownika
    $session->set('foo', 'bar');

    // pobranie wartości atrybutu session
    $foo = $session->get('foo');

    // użycie domyślnej wartości, jeśli atrybut nie istnieje
    $foo = $session->get('foo', 'default_value');

Można również zapisać "wiadomości fleszowe", które będą automatycznie usuwane po
następnym żądaniu. Są one przydatne, gdy chce się ustawić komunikat o sukcesie,
przed przekierowaniem użytkownika na inną stronę (która będzie następnie pokazywać
ten komunikat)::

    // przechowanie (w kontrolerze) komunikatu dla następnych żądań
    $session->getFlashBag()->add('notice', 'Congratulations, your action succeeded!');

    // wyświetlenie z powrotem komunikatu w następnym żądaniu (w szablonie)

    {% for flashMessage in app.session.flashbag.get('notice') %}
        <div>{{ flashMessage }}</div>
    {% endfor %}

Jest to przydatne gdy chce się ustawić komunikat o powodzeniu przed przekierowaniem
użytkownika do innej strony (która wyświetli ten komunikat)::

    // store a message for the very next request (in a controller)
    $session->getFlashBag()->add('notice', 'Congratulations, your action succeeded!');

.. code-block:: html+jinja

    {# wyświetlenie komunikatu fleszowego w szablonie #}
    <div>{{ app.session.flashbag.get('notice') }}</div>

Buforowanie zasobów
-------------------

Gdy tylko witryna zacznie generować więcej ruchu, zachodzi potrzeba uniknnięcia
ciągłego generowania tych samych zasobów. Symfony2 używa nagłówków buforowania
HTTP do zarządzania zasobami pamięci podręcznej. Dla prostych strategi buforowania,
można użyć wygodnej adnotacji ``@Cache()``::

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Template;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Cache;

    /**
     * @Route("/hello/{name}", name="_demo_hello")
     * @Template()
     * @Cache(maxage="86400")
     */
    public function helloAction($name)
    {
        return array('name' => $name);
    }

W tym przykładzie zasoby będą buforowane przez jeden dzień(``86400`` sekund).
Buforowanie zasobów jest zarządzane przez rdzeń Symfony2. Ponieważ jednak buforowanie
jest zarządzane przy wykorzystaniu nagłówków HTTP buforowania, można zastosować
Varnish lubr Squid bez zmieniania nawet jednej linii kodu swojej aplikacji.

Podsumowanie
------------

To wszystko w tym temacie i nie jestem pewny, czy czytanie tego zajęło Ci pełne 10 minut.
W pierwszej części pokrótce zapoznaliśmy się z pakietami poznając, że wszystkie dotychczas
poznane funkcjonalności są składnikiem pakietu rdzenia frameworka i wiemy już też, że
dzięki pakietom wszystko w Symfony2 może zostać rozszerzone lub wymienione. To właśnie
jest tematem :doc:`następnej części przewodnika<the_architecture>`.

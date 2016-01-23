.. highlight:: php
   :linenothreshold: 2

.. index::
   single: strony

Utworzenie swojej pierwszej strony w Symfony
============================================

Tworzenia nowej strony w Symfony, niezależnie od tego, czy jest to strona HTML,
czy punkt końcowy JSON, jest prostą dwuetapową procedurą:

  * *Utworzenie trasy*: Trasa (*ang. route*) odpowiada części lokalizatora URL określającej
    ścieżkę dostępu do zasobu (np. ``/about``) i wyznacza też akcję, którą Symfony
    ma wykonać, gdy ścieżka dostępu przychodzącego żądania HTTP zostanie dopasowany do
    wzorca trasy;

  * *Utworzenie kontrolera i akcji*: Kontroler jest klasą, zawierającą metody,
    zwane akcjami, które są odpowiedzialne
    za wygenerowanie obiektu Symfony o nazwie *Response* i zwrócenie go klientowi
    w postaci odpowiedzi HTTP.

Tak jak w internecie każda interakcja jest inicjowana przez żądanie HTTP. Zadanie
programisty jest jasne i proste: przeanalizować to żądania i zwrócić odpowiedź HTTP.

.. index::
   single: tworzenie strony; przykład

Tworzenie strony: trasa i kontroler
-----------------------------------

.. tip::

    Radzimy, aby przed kontynuowaniem lektury tego rozdziału przeczytać rozdział
    :doc:`Instalacja </book/installation>` i upewnić się, że ma się dostęp w
    przegladarce do nowej aplikacji Symfony.

Załóżmy, że chcemy utworzyć stronę ``/lucky/number``, która generuje szczęśliwą
(no dobrze, losową) liczbę i ją drukuje. Dla wykonania tego, utworzymy klasę a
w niej metodę, która będzie wykonywana za każdym razem, gdy odwiedzi się adres
URL ze ścieżką ``/lucky/number``::

    // src/AppBundle/Controller/LuckyController.php
    namespace AppBundle\Controller;

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Symfony\Component\HttpFoundation\Response;

    class LuckyController
    {
        /**
         * @Route("/lucky/number")
         */
        public function numberAction()
        {
            $number = rand(0, 100);

            return new Response(
                '<html><body>Lucky number: '.$number.'</body></html>'
            );
        }
    }

Zanim do tego przejdziemy, wykonajmy test!

    http://localhost:8000/app_dev.php/lucky/number

.. tip::

    Jeśli skonfigurowało sie właściwie wirtualny host w 
    :doc:`Apache lub Nginx </cookbook/configuration/web_server_configuration>`,
    trzeba zamienić ``http://localhost:8000`` na nazwę swojego hosta, taką jak
    na przykład ``http://symfony.dev/app_dev.php/lucky/number``.

Jeśli widzisz na ekranie szczęśliwą liczbę, która została wydrukowana w odpowiedzi,
 to gratulacje! Jednak zanim skończysz grać na tej naszej loterii, sprawdzimy jak
 to działa.

``@Route`` powyżej ``numberAction()`` nazywa się *adnotacją* i definiuje wzorzec
ścieżki URL. Można również napisać trasy w YAML (lub innych formatach):
przeczytaj o tym w rozdziale :doc:`Trasowanie </book/routing>`. W tym podręczniku,
wiekszość przykładowego kodu dla trasowania jest podawana w kilku formatach.

Metoda poniżej adnotacji, ``numberAction``, jest nazywana *akcją*
i jest miejscem, w którym buduje się stronę. Jedyną zasadą jest to, że akcja
*musi* zwrócić obiekt :ref:`Response <component-http-foundation-response>`
(a Ty nauczysz się przypuszczalnie naginać tą zasadę).

.. sidebar:: Co to jest ``app_dev.php`` w adresie URL?

    Dobre pytanie! Dołaczając ``app_dev.php`` w adresie URL wykonuje sie kod
    Symfony poprzez plik ``web/app_dev.php``, który dokonuje rozuchu w :term:`środowisku <środowisko>`
    ``dev``. Środowisko to udostępnia doskonałe narzedzia debugowania i automatycznej
    przebudowy plików pamięci podręcznej. W środowisku produkcyjnym trzeba używać
    czystych adresów URL, takich jak ``http://localhost:8000/lucky/number``, co
    wykonuje inny plik, ``app.php``, który jest zoptymalizowany ze względu na prędkość.
    Wiecej na ten temat możesz sie dowiedzieć w :ref:`book-page-creation-prod-cache-clear`.

Tworzenie odpowiedzi JSON
~~~~~~~~~~~~~~~~~~~~~~~~~

Obiekt ``Response`` zwracany przez kontroler może zawierać kod HTML, JSON
oraz nawet plik binarny, taki jak obraz lub PDF. Można łatwo ustawić nagłówki
HTTP lub kod stanów.

Dla przykladu utwórzmy punkt końcowy JSON. który zwraca szczęśliwą liczbę.
Wystarczy dodać drugą metodę do ``LuckyController``::

    // src/AppBundle/Controller/LuckyController.php
    
    // ...

    class LuckyController
    {
        // ...

        /**
         * @Route("/api/lucky/number")
         */
        public function apiNumberAction()
        {
            $data = array(
                'lucky_number' => rand(0, 100),
            );

            return new Response(
                json_encode($data),
                200,
                array('Content-Type' => 'application/json')
            );
        }
    }

Spróbuj wyprowadzic to w przegladarce:

    http://localhost:8000/app_dev.php/api/lucky/number

Można to nawet skrócić przy użyciu poręcznej klasy :class:`Symfony\\Component\\HttpFoundation\\JsonResponse`::

    // src/AppBundle/Controller/LuckyController.php
    
    // ...

    // --> don't forget this new use statement
    use Symfony\Component\HttpFoundation\JsonResponse;

    class LuckyController
    {
        // ...

        /**
         * @Route("/api/lucky/number")
         */
        public function apiNumberAction()
        {
            $data = array(
                'lucky_number' => rand(0, 100),
            );

            // calls json_encode and sets the Content-Type header
            return new JsonResponse($data);
        }
    }

Dynamiczne wzorce ścieżek URL: /lucky/number/{count}
----------------------------------------------------

Trasowanie Symfony może robić jeszcze więcej. Przyjmijmy teraz, że potrzbna jest
strona ``/lucky/number/5`` do generowania na raz *pięciu* szczęśliwych liczb.
Poprawmy trasę tak, aby miała na końcu wieloznaczną część ``{wildcard}``:

.. configuration-block::

    .. code-block:: php-annotations
       :linenos:

        // src/AppBundle/Controller/LuckyController.php
        // ...

        class LuckyController
        {
            /**
             * @Route("/lucky/number/{count}")
             */
            public function numberAction()
            {
                // ...
            }

            // ...
        }        

    .. code-block:: yaml
       :linenos:

        # app/config/routing.yml
        lucky_number:
            path:     /lucky/number/{count}
            defaults: { _controller: AppBundle:Lucky:number }

    .. code-block:: xml
       :linenos:

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="lucky_number" path="/lucky/number/{count}">
                <default key="_controller">AppBundle:Lucky:number</default>
            </route>
        </routes>

    .. code-block:: php
       :linenos:

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('lucky_number', new Route('/lucky/number/{count}', array(
            '_controller' => 'AppBundle:Lucky:number',
        )));

        return $collection;

Z powodu "wieloznacznika" ``{count}``, ścieżka URL do strony jest *różna*:
teraz działa to dla ścieżek URL pasujacych do ``/lucky/number/*``, na przykład
``/lucky/number/5``.
Najlepsze jest to, że można uzyskać dostęp do tej wartości i stosować ten mechanizm
w kontrolerze::

    // src/AppBundle/Controller/LuckyController.php
    
    // ...

    class LuckyController
    {

        /**
         * @Route("/lucky/number/{count}")
         */
        public function numberAction($count)
        {
            $numbers = array();
            for ($i = 0; $i < $count; $i++) {
                $numbers[] = rand(0, 100);
            }
            $numbersList = implode(', ', $numbers);

            return new Response(
                '<html><body>Lucky numbers: '.$numbersList.'</body></html>'
            );
        }

        // ...
    }

Wypróbuj to przechodząc w przeglądarce do ``/lucky/number/XX``, zamieniając XX
na *dowolną* liczbę:

    http://localhost:8000/app_dev.php/lucky/number/7

Na nowej stronie powinno pojawić się *7* szczęśliwych liczb. Można uzyskać wartość
z dowolnie podawanego elementu ``{placeholder}`` dodając do akcji argument
``$placeholder`. Wystarczy upewnić się, że element wieloznaczny w trasie adnotacji
i zmienna w akcji są takie same.

System trasowania może dużo więcej, jak obsługa wielu wieloznaczników
(np. ``/blog/{category}/{page})``), czynienie wieloznaczników opcjonalnymi
i wymuszanie, aby wieloznacznik dopasowywał wyrażenie regularne (np. aby ``{count}``
 *musiało być* liczbą).

Wszystkie informacje o tym można znaleźć w rozdziale :doc:`Trasowanie </book/routing>`.

Renderowanie szablonu (w kontenerze usług)
------------------------------------------

Jeśli akcja zwraca kod HTML, to najlepiej jest zrenderować go w szablonie.
Symfony dostarczane jest z Twig: językiem szablonowania, który jest łatwy, wydajny
i nawet zabany.

Jak dotąd, klasa ``LuckyController`` nie rozszerzała żadnej klasy bazowej.
Najprostszym sposobem wykorzystania systemu Twig (lub wielu innych narzędzi Symfony)
jest rozszerzenie bazowej klasy
:class:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller` Symfony::
    
    // src/AppBundle/Controller/LuckyController.php
    
    // ...

    // --> add this new use statement
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;

    class LuckyController extends Controller
    {
        // ...
    }

Używanie usługi ``templating``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To niczego nie zmieniło, ale uzyskaliśmy dostęp do
:doc:`kontenera usług </book/service_container>` Symfony: obiektu podobnego do
tablicy, który daje dostęp do *każdego* użytecznego obiektu w systemie. Obiekty
te są nazywane *usługami* i Symfony dostarczany jest z obiektem usługi, który
może renderować szablony Twig, jak też z obiektem usługi mogącym rejestrować
komunikaty w dzienniku zdarzeń i innymi.

W celu zrenderowania szablonu Twig trzeba użyć usługi o nazwie ``templating``::

    // src/AppBundle/Controller/LuckyController.php
    
    // ...

    class LuckyController extends Controller
    {
        /**
         * @Route("/lucky/number/{count}")
         */
        public function numberAction($count)
        {
            // ...
            $numbersList = implode(', ', $numbers);

            $html = $this->container->get('templating')->render(
                'lucky/number.html.twig',
                array('luckyNumberList' => $numbersList)
            );

            return new Response($html);
        }

        // ...
    }

Dowiesz się wiecej o tym ważnym "kontenerze usług" w dalszej lekturze. Na razie,
po prosty trzeba wiedzieć, że mieści on wiele obiektów i można pobierać każdy obiekt
wykorzystując metodę ``get()`` z odpowiednią nazwą usługi, taką jak ``templating``
lub ``logger``. Usługa ``templating`` jest instancją klasy :class:`Symfony\\Bundle\\TwigBundle\\TwigEngine`
i ma metodę ``render()``.

Usługę tą mozemy pobierać prościej. Wystarczy rozszerzyć klasę ``Controller``
i juz sie ma dostęþ do kilku skrótowych metod, takich jak ``render()``::

    // src/AppBundle/Controller/LuckyController.php
    
    // ...

    /**
     * @Route("/lucky/number/{count}")
     */
    public function numberAction($count)
    {
        // ...

        /*
        $html = $this->container->get('templating')->render(
            'lucky/number.html.twig',
            array('luckyNumberList' => $numbersList)
        );

        return new Response($html);
        */

        // render: a shortcut that does the same as above
        return $this->render(
            'lucky/number.html.twig',
            array('luckyNumberList' => $numbersList)
        );
    }

Wiecej na temat metod skrótowych i o tym jak one działają, mozna przeczytać
w rozdziale :doc:`Kontroler </book/controller>`.

.. tip::

    Dla bardziej zaawansowanych użytkowników: mozna również
    :doc:`zarejestrować swój kontroler jako usługę </cookbook/controller/service>`.

Tworzenie szablonu
~~~~~~~~~~~~~~~~~~

Jeśli teraz odświeżysz przeglądarke, to otrzymasz błąd:

    ``Unable to find template "lucky/number.html.twig"``

Mozna to naprawić tworząc nowy katalog ``app/Resources/views/lucky`` i wstawiając
tam plik ``number.html.twig`` z zawartością:

.. configuration-block::

    .. code-block:: twig
       :linenos:

        {# app/Resources/views/lucky/number.html.twig #}
        {% extends 'base.html.twig' %}

        {% block body %}
            <h1>Lucky Numbers: {{ luckyNumberList }}</h1>
        {% endblock %}

    .. code-block:: html+php
       :linenos:

        <!-- app/Resources/views/lucky/number.html.php -->
        <?php $view->extend('base.html.php') ?>

        <?php $view['slots']->start('body') ?>
            <h1>Lucky Numbers: <?php echo $view->escape($luckyNumberList) ?>
        <?php $view['slots']->stop() ?>

"Welcome to Twig!". Ten prosty plik pokazuje podstawowe rzeczy, takie jak to, że
składnia zmiennej ``{{ nazwaZmiennej }}`` jest używana do wydrukowania czegoś w
miejscu umieszczenia tej zmiennej. Zmienna ``luckyNumberList`` jest przekazywana
do szablonu z wywołania ``render`` w akcji.

Wyrażenie ``{% extends 'base.html.twig' %}`` wskazuje na plik układu strony, który
umieszczony jest w `app/Resources/views/base.html.twig`_ i tworzony jest automatycznie
w ramach nowego projektu.
Jest to podstawowy układ (niestylizowana struktura HTML) i trzeba to dostosować
do swoich potrzeb.
Część ``{% block body %}`` uzywa :ref:`systemu dziedziczenia <twig-inheritance>`
Twig do wstawienia treści do układu ``base.html.twig``.

Po odświeżeniu przeglądarki zobaczysz swój szablon w działaniu.

    http://localhost:8000/app_dev.php/lucky/number/9

Jeśli teraz obejrzysz w przegladarce kod źródłowy strony, to zobaczysz podstawową
strukturę HTML uzyskana dzięki ``base.html.twig``.

To jest tylko podstawowy obraz możliwosci systemu Twig. Gdy Czytelniku bedziesz
gotowy do opanowania jego składni, zapętlenia po tablicach, rebderowania innych
szablonów i jeszcze więcej fajnych rzeczy, przeczytaj rozdział
:doc:`Templating </book/templating>`.

Struktura projektu
------------------

Poznaliśmy już tworzenie elastycznych ścieżek URL, renderowanie szablonu, który
wykorzystuje dziedziczenie i utworzyliśmy punkt końcowy JSON. Doskonale!

Teraz przyszedł czas, aby zbadać i wyjaśnić pliki zawarte w projekcie. Już
pracowaliśmy wewnątrz dwóch najważniejszych katalogów:

``app/``
    Zawiera rzeczy takie jak konfiguracja i szablony. Zasadniczo nie ma tu
    żadnego kodu PHP.

``src/``
    Tutaj umieszczony jest Twój kod PHP.

99% swojego czasu poświęcisz na prace w katalogu ``src/`` (pliki PHP) lub ``app/``
(wszystko inne). Jak już będziesz, drogi Czytelniku, bardziej zaawansowany, to
dowiesz się, co można zrobić w każdym z tych katalogów.

W katalogu ``app/`` jest przechowywanych również kilka innych rzeczy, takich jak
plik ``app/AppKernel.php``, który można użyć, aby udostępnić nowe pakiety.

Katalog ``src/`` ma tylko jeden podkatalog , ``src/AppBundle``, wraz z zawartością
tego pakietu.
Pakiet jest podobny do "wtyczki". Możesz `znaleźć otwarto-źródłowe pakiety`_
i je zainstalować w swoim projekcie. Nawet *Twój* kod jest umieszczany w pakiecie,
chociażby w ``AppBundle`` (ale jest to tylko pakiet demonstracyjny). Więcej na
temat pakietów można przecztać w rozdziale :doc:`Pakiety </book/bundles>`.

Co z innymi katalogami w projekcie?

``web/``
    Jest to główny katalog dokumentów HTML projektu, zawierajacy wszystkie publicznie
    dostępne pliki, takie jak CSS, obrazy i :term:`kontrolery wejścia <kontroler wejścia>`,
    które wykonyje aplikacja (``app_dev.php`` i ``app.php``).

``tests/``
    Przechowywane są tu automatyczne testy (np. testy jednostkowe).

``bin/``
    Znajdują się tutaj pliki "binarne". Jednym z najważniejszych jest plik ``console``,
    który jest używany do wykonywania poleceń Symfony z poziomu konsoli.

``var/``
    Jest to miejsce, w którym są tworzone automatycznie i przechowywane pliki
    pamieci podręcznej (``var/cache/``) i pliki dziennika zdarzeń (``var/logs/``).


``vendor/``
    Tutaj umieszczane są biblioteki "dostawców" (czyli firm zewnętrznych) pobierane
    przy użyciu menadżera pakietów `Composer`_.


.. seealso::

    Symfony jest eleastyczny. Jeśli potrzeba, to można łatwo zamienić domyślną
    strukturę katalogową. Zobacz :doc:`/cookbook/configuration/override_dir_structure`.

Konfiguracja aplikacji
----------------------

Symfony dostarczany jest z kilkoma wbudowanymi pakietami (zobacz plik
``app/AppKernel.php``) i przypuszczalnie zainstalujesz ich więcej. Głównym plikiem
konfiguracyjnym pakietów jest ``app/config/config.yml``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        
        # ...

        framework:
            secret: '%secret%'
            router:
                resource: '%kernel.root_dir%/config/routing.yml'
            # ...

        twig:
            debug:            '%kernel.debug%'
            strict_variables: '%kernel.debug%'

        # ...

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd
                http://symfony.com/schema/dic/twig
                http://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <!-- ... -->

            <framework:config secret="%secret%">
                <framework:router resource="%kernel.root_dir%/config/routing.xml" />
                <!-- ... -->
            </framework:config>

            <!-- Twig Configuration -->
            <twig:config debug="%kernel.debug%" strict-variables="%kernel.debug%" />

            <!-- ... -->
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        // ...

        $container->loadFromExtension('framework', array(
            'secret' => '%secret%',
            'router' => array(
                'resource' => '%kernel.root_dir%/config/routing.php',
            ),
            // ...
        ));

        // Twig Configuration
        $container->loadFromExtension('twig', array(
            'debug'            => '%kernel.debug%',
            'strict_variables' => '%kernel.debug%',
        ));

        // ...

Klucz ``framework`` konfiguruje FrameworkBundle, klucz ``twig`` konfiguruje
TwigBundle i tak dalej. Prawie całe zachowanie Symfony może być kontrolowane
po prostu przez zmiane opcji w tym pliku konfiguracyjnym. wiecej na ten temat
znajdziesz w rozdziale :doc:`Informator konfiguracji </reference/index>`.

Pobranie większego zrzutu wszystkich ważniejszych opcji jest mozliwe przy użyciu
polecenia ``bin/console``:

.. code-block:: bash

    $ bin/console config:dump-reference framework

Jest dużo wiecej rzeczy do omówienia w ramach konfiguracji Symfony, takich jak
środowiska, importowanie i parametry. Mozesz dowiedzieć się o tym w czasie lektury
rozdziału :doc:`Konfiguracja </book/configuration>`.

Co dalej?
---------

Gratulacje! Rozpocząłeś już Czytelniku opanowywać Symfony
i poznawać całkiem nowy sposób budowania pięknych, funkcjonalnych, szybkich
i łatwych w utrzymaniu aplikacji.

Czas do końca opanować podstawy czytając rozdziały:

* :doc:`/book/controller`
* :doc:`/book/routing`
* :doc:`/book/templating`

Następnie nauczysz sie o :doc:`kontenerze usług </book/service_container>`
Symfony, :doc:`systemie formularzy </book/forms>` i używaniu :doc:`Doctrine </book/doctrine>`,
(co pozwoli Ci na używanie zapytań do bazy danych) i jeszcze więcej, studiując
dalej :doc:`Podręcznik Symfony </book/index>`.

Istnieje rówież :doc:`Receptariusz  </cookbook/index>` zawierający bardziej
zaawansowane artykuły "jak to zrobić", pozwalające rozwiązać wiele problemów.

Miłej lektury!

.. _`app/Resources/views/base.html.twig`: https://github.com/symfony/symfony-standard/blob/2.7/app/Resources/views/base.html.twig
.. _`Composer`: https://getcomposer.org
.. _`znaleźć otwarto-źródłowe pakiety`: http://knpbundles.com


.. highlight:: php
   :linenothreshold: 2

.. index::
   single: testy

Testowanie
==========

Ilekroć pisze się nową linie kodu, to również potencjalnie dodaje się nowe błędy.
Aby tworzyć lepsze i bardziej niezawodne aplikacje, należy używać testów, zarówno
funkcjonalnych jak i jednostkowych.

Framework testowania PHPUnit
----------------------------

Symfony integruje się z niezależną biblioteką o nazwie PHPUnit w celu udostępnienia
zaawansowanego frameworka testowania. Rozdział niniejszy nie opisuje samej PHPUnit,
ale biblioteka ta ma świetną `dokumentację`_.


.. note::
    Zaleca sie używanie najnowszej stabilnej wersji PHPUnit (do testowania kodu
    rdzenia Symfony konieczna jest wersja 4.2 lub wyższa).
    
Każdy test, czy to jednostkowy czy funkcjonalny, jest klasą PHP, która powinna się
znajdować w katalogu ``tests/`` swojej aplikacji. Jeśli zastosuje się tą zasadę,
to będzie można uruchomić wszystkie testy swojej aplikacji, po wydaniu tego polecenia:


.. code-block:: bash

    $ phpunit

PHPunit jest skonfigurowane w pliku ``phpunit.xml.dist`` w katalogu głównym
aplikacji Symfony.

.. tip::

    Raport z pokrycia kodu może zostać wygenerowany przez użycie argumentu ``--coverage-html``.
    Proszę zapoznać sie z komunikatmi pomocy wyświetlanymi po zastosowaniu
    argumentu ``--help``.

.. index::
   single: testy; testy jednostkowe


Testy jednostkowe
-----------------

Test jednostkowy dotyczy zazwyczaj określonej klasy PHP, zwanej w nomenklaturze
PHPUnit *jednostką*. Zagadnienia związane z testowaniem działania całej aplikacji
są omówione w rozdziale :ref:`Testy funkcjonalne<functional_tests>`.

Pisanie testów jednostkowych dla Symfony nie różni się niczym od pisania standardowych
testów jednostkowych PHPUnit. Załóżmy, że mamy bardzo prostą klasę o nazwie ``Calculator``
w katalogu ``Util/`` swojego pakietu::
   
    // src/AppBundle/Util/Calculator.php
    namespace AppBundle\Util;

    class Calculator
    {
        public function add($a, $b)
        {
            return $a + $b;
        }
    }
    
Aby przetestowac powyższy kod, trzeba utworzyć plik ``CalculatorTest`` w katalogu
``tests/AppBundle/Util`` swojego pakietu::


    // tests/AppBundle/Util/CalculatorTest.php
    namespace Tests\AppBundle\Util;
    
    use AppBundle\Util\Calculator;

    class CalculatorTest extends \PHPUnit_Framework_TestCase
    {
        public function testAdd()
        {
            $calc = new Calculator();
            $result = $calc->add(30, 12);

            // przyjęcie, że kalkulator doda poprawnie liczby!
            $this->assertEquals(42, $result);
        }
    }


.. note::

    Zgodnie z konwencją, podkatalog ``Tests/AppBundle`` powinien replikować strukturę
    katalogu pakietu. Więc, jeśli testowana jest klasa w katalogu ``AppBundle/Util/``
    pakietu, to test powinien znajdować się w katalogu ``tests/AppBundle/Util/``.


Podobnie jak w prawdziwej aplikacji, automatycznie jest włączane autoładowanie
poprzez plik ``autoload.php`` (jak skonfigurowano to domyślnie w pliku
``phpunit.xml.dist``).

Uruchomienie testów dla określonego pliku lub katalogu jest również bardzo proste:


.. code-block:: bash

    # uruchomienie wszystkich testów aplikacji
    $ phpunit

    # uruchomienie wszystkich testów w katalogu Util
    $ phpunit tests/AppBundle/Util

    # uruchomienie testów dla klasy Calculator
    $ phpunit tests/AppBundle/Util/CalculatorTest.php

    # uruchomienie wszystkich testów dla całego pakietu
    $ phpunit tests/AppBundle/

.. index::
   single: testy; testy funkcjonalne

.. _functional_tests:

Testy funkcjonalne
------------------

Testy funkcjonalne sprawdzają integralność różnych warstw aplikacji (od trasowania
po widoki). Nie różnią się od testów jednostkowych, o ile chodzi o PHPUnit, ale
mają bardzo specyficzną procedurę:

* Wykonanie żądania;
* Przetestowanie odpowiedzi;
* Kliknięcie łącza lub przesłanie formularza;
* Przetestowanie odpowiedzi;
* Wyczyszczenie i powtórzenie testu.

Przykład testu funkcjonalnego
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Testy funkcjonalne, to proste pliki PHP, które zazwyczaj umieszcza się w katalogu
``tests/AppBundle/Controller`` pakietu. Jeśli chce się przetestować strony obsługiwane przez
klasę ``PostController``, należy rozpocząć od utworzenia nowego pliku
``PostControllerTest.php``, który rozszerza klasę ``WebTestCase``.

Dla przykładu, test może wyglądać tak::

    // tests/AppBundle/Controller/PostControllerTest.php
    namespace Tests\AppBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Test\WebTestCase;

    class PostControllerTest extends WebTestCase
    {
        public function testShowPost()
        {
            $client = static::createClient();

            $crawler = $client->request('GET', '/post/hello-world');

            $this->assertGreaterThan(
                0,
                $crawler->filter('html:contains("Hello World")')->count()
            );
        }
    }

.. tip::

    Aby uruchomić testy funkcjonalne, klasa ``WebTestCase`` inicjuje kernel aplikacji.
    W większości przypadków odbywa się to automatycznie. Jednak, gdy kernel jest
    zlokalizowany w niestandardowym katalogu, to zachodzi konieczność zmiany pliku
    ``phpunit.xml.dist`` przez ustawienie zmiennej środowiskowej ``KERNEL_DIR``
    na katalog kernela aplikacji:

.. code-block:: xml

        <?xml version="1.0" charset="utf-8" ?>
        <phpunit>
            <php>
                <server name="KERNEL_DIR" value="/path/to/your/app/" />
            </php>
            <!-- ... -->
        </phpunit>

Metoda ``createClient()`` zwraca klienta, podobnego do przeglądarki, który będzie
używany do analizy witryny::

    $crawler = $client->request('GET', '/post/hello-world');

Metoda ``request()`` (przeczytaj :ref:`więcej o metodzie request<book-testing-request-method-sidebar>`)
zwraca obiekt ``Crawler``, który może zostać użyty do wyboru elementów z odpowiedzi
oraz symulowania kliknięcia łącza i wysłania formularza.

.. tip::

    ``Crawler`` działa tylko gdy odpowiedź jest dokumentem XML lub HTML.
    Aby pobrać surową zawartość odpowiedzi, trzeba wywołać
    ``$client->getResponse()->getContent()``.

W pierwszej kolejności wybiera się z obiektu ``Crawler`` kliknięcie łącza, stosując
albo wyrażenie Xpath albo wykorzystując selektor CSS, a następnie stosuje się klienta
do kliknięcia łącza. Na przykład:::

    $link = $crawler
        ->filter('a:contains("Greet")') // find all links with the text "Greet"
        ->eq(1) // select the second link in the list
        ->link()
    ;
    
    // and click it
    $crawler = $client->click($link);

Przesłanie formularza jest podobnie proste. Poniższy kod wybierze przycisk formularza,
ewentualnie zastąpi niektóre wartości formularza i przesłoni rzeczywisty formularz::


    $form = $crawler->selectButton('submit')->form();

    // set some values
    $form['name'] = 'Lucas';
    $form['form_name[subject]'] = 'Hey there!';

    // submit the form
    $crawler = $client->submit($form);


.. tip::

    Formularz może również obsługiwać ładowanie i zawierać metody wypełniania różnych
    typów pól formularza (np. ``select()`` i ``tick()``). Więcej informacji na ten
    temat można znaleźć w rozdziale :doc:`Formularze</book/forms>` w dalszej części
    dokumentacji.

Teraz można łatwo poruszać się po strukturze aplikacji, używając metod asercji
do testowania tego, czy elementy rzeczywiście wykonują to, co się od nich oczekuje.
Oto zastosowanie Crawlera do wykonania asercji na elementach DOM::


    // Twierdzenie, że odpowiedź dopasowuje określony selektor CSS.
    $this->assertGreaterThan(0, $crawler->filter('h1')->count());


albo do przetestowania bezpośrednio zawartość odpowiedzi, sprawdzając czy treść
ta zawiera jakiś tekst lub czy odpowiedź nie jest dokumentem XML lub HTML:: 

    $this->assertContains(
        'Hello World',
        $client->getResponse()->getContent()
    );



.. index::
   single: testy; asercje

.. sidebar:: Użyteczne asercje

    Oto lista najczęściej stosowanych i użytecznych metod asercji::
        
        use Symfony\Component\HttpFoundation\Response;

        // ...

        // Przyjecie, że istnieje co najmniej jeden znacznik h2
        // z klasa "subtitle"
        $this->assertGreaterThan(
            0,
            $crawler->filter('h2.subtitle')->count()
        );

        // Przyjęcie, że na stronie istnieją dokładnie 4 znaczniki h2
        $this->assertCount(4, $crawler->filter('h2'));

        // Przyjecie, że nagłówek "Content-Type", to "application/json"
        $this->assertTrue(
            $client->getResponse()->headers->contains(
                'Content-Type',
                'application/json'
            )
        );

        // Przyjęcie, że treść odpowiedzi zawiera łańcuch tekstowy
        $this->assertContains('foo', $client->getResponse()->getContent());
        // ...or matches a regex
        $this->assertRegExp('/foo(bar)?/', $client->getResponse()->getContent());

        // Przyjecie, że kod stanu odpowiedzi, to 2xx
        $this->assertTrue($client->getResponse()->isSuccessful());
        // Przyjecie, że kod stanu odpowiedzi, to 404
        $this->assertTrue($client->getResponse()->isNotFound());
        // Przejęcie, ze stan kodu wynosi 200
        $this->assertEquals(
            200, // or Symfony\Component\HttpFoundation\Response::HTTP_OK
            $client->getResponse()->getStatusCode()
        );

        // Przyjęcie, że odpowiedź jest przekierowywana do /demo/contact
        $this->assertTrue(
            $client->getResponse()->isRedirect('/demo/contact')
        );
        // ...lub po prostu sparwdzenie, ze odpowiedź jest przekierowywana na jakiś adres URL
        $this->assertTrue($client->getResponse()->isRedirect());

    .. versionadded:: 2.4
        Obsługa stałych kodu stanu HTTP została dodana w Symfony 2.4.
        
.. index::
   single: testy; klient

.. _book_testing_client:

Praca z klientem testowania
---------------------------

Klient testowania symuluje klienta HTTP, takiego jak przeglądarka i wykonuje żądania
do aplikacji Symfony::

    $crawler = $client->request('GET', '/post/hello-world');
    
Metoda ``request()`` pobiera jako argumenty metodę HTTP i adres URL a zwraca instancję
``Crawler``.

.. tip::

    Sztywne podawanie adresów URL jest najlepszą praktyką dla testów funkcjonalnych.
    Jeśli test generuje ścieżki URL używając routera Symfony, to nie wykrywa żadnych
    zmian na ścieżkach URL applikacji, które mogą wpłynąć na końcowych użytkowników.

.. _book-testing-request-method-sidebar:

.. sidebar:: Więcej o metodzie ``request()``:

    Pełna sygnatura metody ``request()`` jest następująca::

        request(
            $method,
            $uri,
            array $parameters = array(),
            array $files = array(),
            array $server = array(),
            $content = null,
            $changeHistory = true
        )

    Tablica ``server`` jest surową wartością, jaką można oczekiwać przy zwykłym
    odczytywaniu wartości z superglobalnej zmiennej PHP `$_SERVER`_. Na przykład,
    aby ustawić nagłówki HTTP ``Content-Type``, ``Referer`` i ``X-Requested-With``,
    trzeba przekazać co nastęþuje (pamietając o przedrostku ``HTTP_`` dla nie
    standardowych nagłówków)::

        $client->request(
            'GET',
            '/post/hello-world',
            array(),
            array(),
            array(
                'CONTENT_TYPE'          => 'application/json',
                'HTTP_REFERER'          => '/foo/bar',
                'HTTP_X-Requested-With' => 'XMLHttpRequest',
            )
        );


Użyjmy Crawler do odnalezienie w odpowiedzi elementów DOM. Elementy te mogą
być następnie użyte do klikania łączy i składania formularzy::


    $link = $crawler->selectLink('Go elsewhere...')->link();
    $crawler = $client->click($link);

    $form = $crawler->selectButton('validate')->form();
    $crawler = $client->submit($form, array('name' => 'Fabien'));


Obie metody ``click()`` i ``submit()`` zwracają obiekt ``Crawler``. Metody te są
najlepszym sposobem do przeglądania swojej aplikacji, jako że zapewniają wiele
pożytecznych rzeczy, jak wykrywanie metody HTTP w formularzu i udostępniając
dobre API dla ładowania plików.

.. tip::

    Można dowiedzieć się więcej o obiektach ``Link`` i ``Form`` w rozdziale
    :ref:`book-testing-crawler`.

Metoda ``request`` może również zostać użyta do bezpośredniego symulowania składania
formularza lub wykonania bardziej złożonych żądań. Oto przydatny przykład::


    // Bezpośrednie przesłanie formularza (ale przy użyciu Crawler jest to łatwiejsze)
    $client->request('POST', '/submit', array('name' => 'Fabien'));
    
    // Przesłanie surowego łańcucha JSON w ciele żądania
    $client->request(
        'POST',
        '/submit',
        array(),
        array(),
        array('CONTENT_TYPE' => 'application/json'),
        '{"name":"Fabien"}'
    );
      
    // Przesłanie formularza z załadowaniem pliku
    use Symfony\Component\HttpFoundation\File\UploadedFile;

    $photo = new UploadedFile(
        '/path/to/photo.jpg',
        'photo.jpg',
        'image/jpeg',
        123
    );
    
    $client->request(
        'POST',
        '/submit',
        array('name' => 'Fabien'),
        array('photo' => $photo)
    );

    // Wykonanie żądania DELETE i przekazanie nagłówków HTTP
    $client->request(
        'DELETE',
        '/post/12',
        array(),
        array(),
        array('PHP_AUTH_USER' => 'username', 'PHP_AUTH_PW' => 'pa$$word')
    );


Niemniej można wymusić aby każde żądanie było wykonywane we własnym procesie PHP,
aby uniknąć skutków ubocznych w trakcie pracy z różnymi klientami w tym samym skrypcie::


    $client->insulate();


Przeglądanie
~~~~~~~~~~~~

Klient obsługuje wiele operacji, które mogą być wykonywane w rzeczywistych
przeglądarkach::


    $client->back();
    $client->forward();
    $client->reload();

    // Wyczyszczenie wszystkich ciasteczek i historii
    $client->restart();


Dostęp do wewnętrznych obiektów
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. versionadded:: 2.3
    W Symfony 2.3 dodano metody
    :method:`Symfony\\Component\\BrowserKit\\Client::getInternalRequest` i
    :method:`Symfony\\Component\\BrowserKit\\Client::getInternalResponse`

Jeśli używa się klienta do testowania aplikacji, to można uzyskać dostęp do obiektów
wewnętrznych klienta::


    $history = $client->getHistory();
    $cookieJar = $client->getCookieJar();


Można również pobrać obiekty związane z ostatnim żądaniem::
   
    // instancja żądania HttpKernel
    $request = $client->getRequest();

    // instancja żądania BrowserKit
    $request = $client->getInternalRequest();

    // instancja odpowiedzi HttpKernel
    $response = $client->getResponse();

    // instancja odpowiedzi BrowserKit
    $response = $client->getInternalResponse();

    $crawler = $client->getCrawler();

Jeśli żądania nie są izolowane, to można uzyskać również dostęp do kontenera i kernela::

    $container = $client->getContainer();
    $kernel = $client->getKernel();


Dostęp do kontenera
~~~~~~~~~~~~~~~~~~~

Jest wysoce zalecane testowanie testami jednostkowymi tylko odpowiedzi.
Lecz w niektórych wyjątkowych sytuacjach można wykorzystać możliwość uzyskania
dostępu do niektórych obiektów wewnętrznych pisząc metody asercji. W takim przypadku
można uzyskać dostęp do kontenera wstrzykiwania zależności::


    $container = $client->getContainer();

Trzeba pamiętać, że to nie działa, jeśli izoluje się klienta lub jeśli używa się
warstwy HTTP. W celu uzyskania listy dostępnych w aplikacji usług, użyj zadania
konsoli ``debug:container``.

.. tip::

    Jeśli potrzebna Ci informacja jest dostępna z poziomu profilera, to go użyj
    zamiast powyższego polecenia.

Dostęp do danych profilera
~~~~~~~~~~~~~~~~~~~~~~~~~~

Przy każdym żądaniu profiler Symfony gromadzi i przechowuje wiele danych o wewnętrznie
przetwarzanym żądaniu. Na przykład, profiler może zostać wykorzystany do sprawdzenia,
czy dana strona przy ładowaniu wykonuje mniej niż jakąś liczba zapytań.

Aby uzyskać obiekt klasy Profiler z danymi ostatniego żądania, trzeba zastosować
następujące wyrażenie::


    // włączenie profilera dla kolejnego żądania
    $client->enableProfiler();

    $crawler = $client->request('GET', '/profiler');

    // pobranie profilera
    $profile = $client->getProfile();


Szczegółowe informacje o używaniu profilera wewnątrz testów znaleźć można w artykule
:doc:`Jak używać profilera w testście funkcjonalnym</cookbook/testing/profiling>`.

Przekierowania
~~~~~~~~~~~~~~

Gdy żądanie zwraca odpowiedź przekierowania, klient nie stosuje tego automatycznie.
Można zbadać odpowiedź i wymusić potem przekierowanie stosując metodę ``followRedirect()``::


    $crawler = $client->followRedirect();


Jeśli chce się aby klient automatycznie wykonywał wszystkie przekierowania, należy
wymusić to metodą ``followRedirects()``::


    $client->followRedirects();

Gdy do metody ``followRedirects()`` przekaże się ``false``, przekierowania nie
będą już dokonywane::

    $client->followRedirects(false);


.. index::
   single: testy; Crawler

.. _book-testing-crawler:

Crawler
-------

Instancja Crawlera zwracana jest po każdym wykonaniu żądania w kliencie.
Umożliwia to przechodzenie po dokumencie HTML, wybór węzłów, odnajdowanie łączy
i formularzy.

Przechodzenie
~~~~~~~~~~~~~

Podobnie do jQuery, Crawler posiada metody do przechodzenia po strukturze DOM
dokumentów HTML/XML. Poniższy przykład odnajduje wszystkie elementy ``input[type=submit]``,
wybiera ostatni z nich i następnie wybiera jego bezpośredni element rodzicielski::


    $newCrawler = $crawler->filter('input[type=submit]')
        ->last()
        ->parents()
        ->first()
    ;

Dostępnych jest też wiele innych metod:

``filter('h1.title')``
   
   Zwraca węzły, które pasują do określonego selektora.
   
``filterXpath('h1')`
   
   Zwraca węzły, które pasują do określonego wyrażenia XPath.
   
``eq(1)``
   
   Zwraca węzeł o określonym indeksie.
   
``first()``
   
   Zwraca pierwszy węzeł.
   
``last()``
   
   Zwraca ostatni węzeł.
   
``siblings()``
   
   Zwraca rodzeństwo
   
``nextAll()``
   
   Zwraca wszystkie następne węzły rodzeństwa.

``previousAll()``
   
   Zwraca wszysystkie poprzedzające węzły rodzeństwa.
   
``parents()``
   
   Zwraca węzły nadrzędne (rodziców).
   
``children()``
   
   Zwraca węzły podrzędne (dzieci).
   
``reduce($lambda)``
   
   Węzły, dla których wywoływanie nie zwróci false.

Ponieważ każda z tych metod zwraca nową instancję ``Crawler``, więc można zawęzić
wybór węzła przez łańcuchowe wywołanie tych metod::


    $crawler
        ->filter('h1')
        ->reduce(function ($node, $i)
        {
            if (!$node->getAttribute('class')) {
                return false;
            }
        })
        ->first();

.. tip::

    Aby uzyskać liczbę węzłów przechowywanych w Crawler, trzeba użyć funkcję
    ``count($crawler)``.

Pozyskiwanie informacji
~~~~~~~~~~~~~~~~~~~~~~~

Crawler może pozyskiwać informację z węzłów::


    // Zwrócenie wartości atrybutu dla pierwszego węzła
    $crawler->attr('class');

    // Zwrócenie wartości węzła dla pierwszego węzła
    $crawler->text();

    // Wyodrębnienie tablicy atrybutów dla wszystkich węzłów
    // (_text zwraca wartość węzła)
    // Zwrócenie tablicy dla każdego elementu w crawler,
    // każdy z wartością i href
    $info = $crawler->extract(array('_text', 'href'));

    // Wykonanie domlnięcie dla każdego węzła i zwrócenie tablicy wyników
    $data = $crawler->each(function ($node, $i)
    {
        return $node->attr('href');
    });


Odnośniki
~~~~~~~~~

Do wybrania odnośników, można użyć metody przechodzenia lub wygodny skrót ``selectLink()`::

    $crawler->selectLink('Click here');

Wyrażenie to wybiera wszystkie łącza, które zawierają określony tekst lub klikalne
obrazy, dla których atrybut ``alt`` zawiera dany tekst. Podobnie jak w przypadku
innych metod filtrujących kod ten zwraca inny obiekt klasy ``Crawler``.

Po wybraniu łącza, uzyskuje się dostęp do specjalnego obiektu ``Link``, który posiada
przydatne, pomocne metody dla połączeń (takie jak ``getMethod()`` i ``getUri()``).
Aby kliknąć łącze, trzeba użyć metodę ``click()`` klienta i przekazać to jako obiekt
``Link``::

    $link = $crawler->selectLink('Click here')->link();

    $client->click($link);

Formularze
~~~~~~~~~~

Wybór formularzy dokonuje się przy użyciu metody ``selectButton()``, podobnie jak
w przypadku odnośników::

    $buttonCrawlerNode = $crawler->selectButton('submit');


.. note::

    Proszę zwrócić uwagę, że wybiera się przyciski formularza a nie formularze,
    które mają różne przyciski. Jeżeli użyje się API przechodzenia, to trzeba
    pamiętać, że musi się szukać przycisków.

Metoda ``selectButton()`` może wybierać znacznik ``button`` i wysyłać znaczniki
``input``. Wykorzystuje to kilka części przycisków, aby odnaleźć:

* wartość atrybutu ``value``;

* wartość atrybutu ``id`` lub ``alt`` dla obrazów;

* wartość atrybutu ``id`` lub ``name`` dla znaczników ``button``.

Gdy już ma się Crawler reprezentujący przycisk, trzeba wywołać metodę ``form()``,
aby pobrać instancję ``Form`` opakowującą węzeł przycisku::


    $form = $buttonCrawlerNode->form();


Podczas wywołania metody ``form()`` można również przekazać tablicę wartości pól,
które przesłaniają wartości domyślne::


    $form = $buttonCrawlerNode->form(array(
        'name'              => 'Fabien',
        'my_form[subject]'  => 'Symfony rocks!',
    ));


Jeśli chce się symulować określoną metodę HTTP dla formularza, trzeba przekazać
ją jako drugi argument::


    $form = $buttonCrawlerNode->form(array(), 'DELETE');


Klient może wysłać instancję ``Form``::


    $client->submit($form);


Również można przekazywać wartości pól jako drugi argument metody ``submit()``::


    $client->submit($form, array(
        'name'              => 'Fabien',
        'my_form[subject]'  => 'Symfony rocks!',
    ));


W sytuacjach bardziej bardziej skomplikowanych, aby ustawić wartość każdego pola
indywidualnie, trzeba użyć instancji ``Form`` jako tablicy::

    // Change the value of a field
    $form['name'] = 'Fabien';
    $form['my_form[subject]'] = 'Symfony rocks!';


Istnieje również dobre API umożliwiające manipulowanie wartościami pól, w zależności
od jego typu::


    // Wybór opcji lub radio
    $form['country']->select('France');

    // Zaznaczenie pola wyboru
    $form['like_symfony']->tick();

    // Załadowanie pliku
    $form['photo']->upload('/path/to/lucas.jpg');

.. tip::

    Jeśli świadomie chcesz wybrać "nieprawidłowe" wartości pól select/radio,
    zapoznaj się z :ref:`components-dom-crawler-invalid`.

.. tip::

    Można pobrać wartości, które będą przekazywane przez wywołanie metody
    ``getValues()`` obiektu klasy ``Form``. Załadowane pliki są dostępne w oddzielnej
    tablicy zwracanej przez ``getFiles()``. Metody ``getPhpValues()`` i ``getPhpFiles()``
    zwracają przesłane wartości, ale w formacie PHP format (konwertuje to klucze
    w notacji kwadratowych nawiasów, np. ``my_form[subject]``, do tablic PHP).
    

.. index::
   pair: testy; konfiguracja

Konfiguracja testowania
-----------------------

Stosowany w testach funkcjonalnych klient tworzy kernel, który uruchamia specyficzne
środowisko testowe. Ponieważ Symfony ładuje ``app/config/config_test.yml`` w środowisku
testowym, to można zmienić jakiekolwiek z ustawień aplikacji specjalnie dla testowania.

Przykładowo, swiftmailer jest domyślnie skonfigurowany, aby w środowisku ``test``
w rzeczywistości nie wysyłać wiadomości e-mail. Można to zobaczyć w opcji konfiguracji
swiftmailer:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config_test.yml

        # ...
        swiftmailer:
            disable_delivery: true

    .. code-block:: xml

        <!-- app/config/config_test.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/swiftmailer
                http://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd">

            <!-- ... -->
            <swiftmailer:config disable-delivery="true" />
        </container>

    .. code-block:: php

        // app/config/config_test.php

        // ...
        $container->loadFromExtension('swiftmailer', array(
            'disable_delivery' => true,
        ));

Można również użyć w całości innego środowiska lub zastąpić domyślny tryb debugowania
(``true``) przekazując każde ustawienie jako opcje metody ``createClient()``::


    $client = static::createClient(array(
        'environment' => 'my_test_env',
        'debug'       => false,
    ));


Jeśli aplikacja wykorzystuje jakieś nagłówki HTTP, to trzeba je przekazać jako
drugi argument metody ``createClient()``::


    $client = static::createClient(array(), array(
        'HTTP_HOST'       => 'en.example.com',
        'HTTP_USER_AGENT' => 'MySuperBrowser/1.0',
    ));


Można również zastąpić nagłówki HTTP odnoszące się do jednego żądania::


    $client->request('GET', '/', array(), array(), array(
        'HTTP_HOST'       => 'en.example.com',
        'HTTP_USER_AGENT' => 'MySuperBrowser/1.0',
    ));


.. tip::

    Klient testowy jest dostępny jako usługa w środowisku ``test`` w kontenerze
    (lub gdziekolwiek, gdzie dostępna jest opcja
    :ref:`framework.test<reference-framework-test>`). Oznacza to, że można zastąpić
    całkowicie tą usługę, jeśli jest to potrzebne.

.. index::
   pair: PHPUnit; konfiguracja

Konfiguracja PHPUnit
~~~~~~~~~~~~~~~~~~~~

Każda aplikacja ma własną konfigurację PHPUnit, zapisaną w pliku ``phpunit.xml.dist``.
Można edytować ten plik, zmieniając wartości domyślne lub utworzyć plik ``phpunit.xml``,
aby zmienić konfigurację na swoim komputerze lokalnym.

.. tip::

    Przechowuj plik ``phpunit.xml.dist`` w repozytorium kodu i ignoruj plik ``phpunit.xml``.

Domyślnie, polecenie ``phpunit`` uruchamia tylko testy przechowywane w katalogu
``/tests``, tak jak skonfigurowano to w pliku ``phpunit.xml.dist``:

.. code-block:: xml

    <!-- phpunit.xml.dist -->
    <phpunit>
        <!-- ... -->
        <testsuites>
            <testsuite name="Project Test Suite">
                <directory>tests</directory>
            </testsuite>
        </testsuites>
        <!-- ... -->
    </phpunit>

Można łatwo dodać więcej katalogów. Na przykład, następująca konfiguracja dodaje
testy z własnego katalogu ``lib/tests``:

.. code-block:: xml

    <!-- phpunit.xml.dist -->
    <phpunit>
        <!-- ... -->
        <testsuites>
            <testsuite name="Project Test Suite">
                <!-- ... --->
                <directory>../lib/tests</directory>
            </testsuite>
        </testsuites>
        <!-- ... --->
    </phpunit>

W celu dołaczenia innych katalogów do pokrycia kodu (*ang. code coverage*),
trzeba także edytować sekcje ``<filter>``:

.. code-block:: xml

    <!-- phpunit.xml.dist -->
    <phpunit>
        <!-- ... -->
        <filter>
            <whitelist>
                <!-- ... -->
                <directory>../lib</directory>
                <exclude>
                    <!-- ... -->
                    <directory>../lib/tests</directory>
                </exclude>
            </whitelist>
        </filter>
        <!-- ... --->
    </phpunit>


Dalsza lektura
--------------

* :doc:`rozdział o testach w "Najlepszych praktykach frameworka Symfony </best_practices/tests>`
* :doc:`/components/dom_crawler`
* :doc:`/components/css_selector`
* :doc:`/cookbook/testing/http_authentication`
* :doc:`/cookbook/testing/insulating_clients`
* :doc:`/cookbook/testing/profiling`
* :doc:`/cookbook/testing/bootstrap`

.. _`DemoControllerTest`: https://github.com/symfony/symfony-standard/blob/master/src/Acme/DemoBundle/Tests/Controller/DemoControllerTest.php
.. _`$_SERVER`: http://php.net/manual/en/reserved.variables.server.php
.. _`dokumentację`: https://www.phpunit.de/manual/3.8/en/

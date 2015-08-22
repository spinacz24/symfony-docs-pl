.. index::
   pair: konfiguracja; Framework

Konfiguracja FrameworkBundle
============================

``FrameworkBundle`` posiada większość z "podstawowych" funkcjonalności frameworka
i może być konfigurowany w konfiguracji aplikacji poprzez klucz ``framework``.
Podczas stosowania XML musi sie używać przestrzeni nazewniczej
``http://symfony.com/schema/dic/symfony``.

Obejmuje to ustawienia sesji, tłumaczeń, formularzy, walidacji, routingu i wiele więcej.


.. tip::

   Schemat XSD jest dostępny pod adresem
   ``http://symfony.com/schema/dic/symfony/symfony-1.0.xsd``.


Konfiguracja
------------

* `secret`_
* `http_method_override`_
* `trusted_proxies`_
* `ide`_
* `test`_
* `default_locale`_
* `trusted_hosts`_
* :ref:`form <reference-framework-form>`
    * :ref:`enabled <reference-form-enabled>`
* `csrf_protection`_
    * :ref:`enabled <reference-csrf_protection-enabled>`
    * `field_name`_ (deprecated as of 2.4)
* `esi`_
    * :ref:`enabled <reference-esi-enabled>`
* `fragments`_
    * :ref:`enabled <reference-fragments-enabled>`
    * :ref:`path <reference-fragments-path>`
* `profiler`_
    * :ref:`enabled <reference-profiler-enabled>`
    * `collect`_
    * `only_exceptions`_
    * `only_master_requests`_
    * `dsn`_
    * `username`_
    * `password`_
    * `lifetime`_
    * `matcher`_
        * `ip`_
        * :ref:`path <reference-profiler-matcher-path>`
        * `service`_
* `router`_
    * `resource`_
    * `type`_
    * `http_port`_
    * `https_port`_
    * `strict_requirements`_
* `session`_
    * `storage_id`_
    * `handler_id`_
    * `name`_
    * `cookie_lifetime`_
    * `cookie_path`_
    * `cookie_domain`_
    * `cookie_secure`_
    * `cookie_httponly`_
    * `gc_divisor`_
    * `gc_probability`_
    * `gc_maxlifetime`_
    * `save_path`_
* `templating`_
    * `assets_version`_
    * `assets_version_format`_
    * `hinclude_default_template`_
    * :ref:`form <reference-templating-form>`
        * `resources`_
    * `assets_base_urls`_
        * http
        * ssl
    * :ref:`cache <reference-templating-cache>`
    * `engines`_
    * `loaders`_
    * `packages`_
* `translator`_
    * :ref:`enabled <reference-translator-enabled>`
    * `fallbacks`_
    * `logging`_
* `property_accessor`_
    * `magic_call`_
    * `throw_exception_on_invalid_index`_
* `validation`_
    * :ref:`enabled <reference-validation-enabled>`
    * :ref:`cache <reference-validation-cache>`
    * :ref:`enable_annotations <reference-validation-enable_annotations>`
    * `translation_domain`_
    * `strict_email`_
    * `api`_
* `annotations`_
    * :ref:`cache <reference-annotations-cache>`
    * `file_cache_dir`_
    * `debug`_
* `serializer`_
    * :ref:`enabled <reference-serializer-enabled>`
    * :ref:`cache <reference-serializer-cache>`
    * :ref:`enable_annotations <reference-serializer-enable_annotations>`

secret
~~~~~~

**typ**: ``string`` **wymagane**

Jest to łańcuch tekstowy, który powinien być unikalny w skali aplikacji i powszechnie
jest wykorzystywany do zwiększenia entropii w operacjach związanych z bezpieczeństwem.
Jego wartoscią powinien być ciąg znaków, liczb i symboli wybranych losowo. Zalecana
długość, to około 32 znaków.

W praktyce Symfony uzywa tej wartości do generowania :ref:`tokenów CSRF<forms-csrf>`,
dla szyfrowania plików cookie stosowanych w
:doc:`funkcjonalności remember me </cookbook/security/remember_me>` i do tworzenia
podpisanych cyfrowo adresach URI podczas używania :ref:`ESI (Edge Side Includes) <edge-side-includes>`.

Opcja ta staje się parametrem konteneru usługi o nazwie ``kernel.secret``,
która może zostać użyta gdy aplikacja wymaga niezmiennego losowego ciagu znaków
do zwiększenia entropii.

Podobnie jak w przypadku innych parametrów związanych z bezpieczeństwem, dobrą
praktyka jest zmienianie tej wartości od czasu do czaso. Trzeba jednak pamietać,
że zmiana tej wartości skutkuje unieważnieniem wszystkich podpisanych adresów
URI i ciasteczek Remember Me. Dlatego, po zmianie tej wartości trzeba zregenerować
pamięć podręczną i wylogować wszystkich użytkowników aplikacji. 

.. _configuration-framework-http_method_override:

http_method_override
~~~~~~~~~~~~~~~~~~~~

.. versionadded:: 2.3
   Opcja ``http_method_override`` wprowadzona została W Symfony 2.3.

**typ**: ``Boolean`` **domyślnie**: ``true``

Określa czy parametr żądania ``_method`` jest używany jako zamierzona metoda HTTP
dla żądań POST. Jeśli jest włączona, to metoda
:method:`Request::enableHttpMethodParameterOverride <Symfony\\Component\\HttpFoundation\\Request::enableHttpMethodParameterOverride>`
jest wywoływana automatycznie. Jest to parametr kontenera usług
o nazwie ``kernel.http_method_override``.

.. seealso::
    Więcej informacji można znaleźć w :doc:`/cookbook/routing/method_parameters`.
    
.. caution::

    Jeśli z tą opcją używa się :ref:`AppCache Reverse Proxy <symfony2-reverse-proxy>`,
    kernel bedzie ignorował parametr ``_method``, co moze prowadzić do błędów.

    Dla rozwiązania tego problemu trzeba wywołać metodę ``enableHttpMethodParameterOverride()``
    zanim utworzy się obiekt ``Request``::

        // web/app.php

        // ...
        $kernel = new AppCache($kernel);

        Request::enableHttpMethodParameterOverride(); // <-- add this line
        $request = Request::createFromGlobals();
        // ...    

.. _reference-framework-trusted-proxies:

trusted_proxies
~~~~~~~~~~~~~~~

**typ**: ``array``

Konfiguruje adresy IP, którymi powinny być zaufane odwrotne serwery pośredniczące.
Szczegóły można znaleźć w :doc:`/cookbook/request/load_balancer_reverse_proxy`.

.. versionadded:: 2.3
    CIDR notation support was introduced in Symfony 2.3, so you can whitelist
    whole subnets (e.g. ``10.0.0.0/8``, ``fc00::/7``).

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            trusted_proxies:  [192.0.0.1, 10.0.0.0/8]

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config trusted-proxies="192.0.0.1, 10.0.0.0/8" />
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            'trusted_proxies' => array('192.0.0.1', '10.0.0.0/8'),
        ));

ide
~~~

**typ**: ``string`` **domyślnie**: ``null``

Jeśli używa się jakieś środowisko IDE, takie jak TextMate lub Mac Vim, to Symfony
może włączyć w komunikacie wyjątku wszystkie ścieżki do pliku, który otworzy ten
plik w IDE.

Symfony zawiera wstępnie skonfigurowane scieżki URL dla niektórych popularnych
środowisk IDE, które można ustawić, używając następujących kluczy:

* ``textmate``
* ``macvim``
* ``emacs``
* ``sublime``

.. versionadded:: 2.3.14
    Edytory ``emacs`` i ``sublime`` zostały dodane w Symfony 2.3.14.

Można też określić własny łańcuch URL. Jeśli sie to zrobi, to trzeba wszyskie
znaki procentu (``%``) zabezpieczyć znakiem ucieczki, czyli podwajając je. Na przykład,
jeśli uzywa się edytora PHPstorm na platformie Mac OS, trzeba zrobić coś takiego:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            ide: "phpstorm://open?file=%%f&line=%%l"

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config ide="phpstorm://open?file=%%f&line=%%l" />
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            'ide' => 'phpstorm://open?file=%%f&line=%%l',
        ));

.. tip::

    Jeśli używa się Windows PC, można zainstalować `PhpStormProtocol`_ w celu
    używania tego edytora.

Oczywiście, ponieważ kazdy programista używa innego IDE, lepiej jest ustawić
to na poziomie systemu. Można to zrobić ustawiając ``xdebug.file_link_format``
w konfiguracji ``php.ini`` na właściwy ciąg URL. Jeśli ustawi się tą wartość
konfiguracji, to opcja ``ide`` będzie ignorowana.


.. _reference-framework-test:

test
~~~~

**typ**: ``Boolean``

Jeśli ten parametr konfiguracyjny znajduje się w konfiguracji (i nie ma wartości
``false``), to będą ładowane usługi związane  z testowaniem aplikacji (np.
``test.client``). Ustawienie to powinno znajdować się w środowisku ``test``
(zazwyczaj poprzez umieszczenie go w ``app/config/config_test.yml``).

.. seealso::
   Więcej informacji można znaleźć w :doc:`/book/testing`.


default_locale
~~~~~~~~~~~~~~

**typ**: ``string`` **domyślnie**: ``en``

Domyślne ustawienie regionalne jest stosowane, jeśli nie został ustawiony parametr
trasowania ``_locale``. Jest dostępne w metodzie
:method:`Request::getDefaultLocale <Symfony\\Component\\HttpFoundation\\Request::getDefaultLocale>`.

.. seealso::

    Więcej informacji o ustawieniach regionalnych można znaleźć w
    :ref:`book-translation-default-locale`.

trusted_hosts
~~~~~~~~~~~~~

**typ**: ``array`` | ``string`` **domyślnie**: ``array()``

Wykryto dużo ataków opierajacych sie na niespójności w obsłudze nagłówka ``Host``
przez różne oprogramowanie (serwery internetowe, odwrotne serwery pośredniczące,
frameworki internetowe itd.). W zasadzie, za każdym razem gdy framework generuje
bezwzględny adres URL (podczas wysyłania wiadomości email w celu zresetowania
hasła dla instacji), host może być zmanipulowany przez atakującego.

.. seealso::

    Proszę przeczytać artykuł "`HTTP Host header attacks`_" w celu uzyskania
    więcej informacji o rodzajach ataków.

Metoda  :method:`Request::getHost() <Symfony\\Component\\HttpFoundation\\Request:getHost>`
może być podatna na pewne ataki, ponieważ jest ona uzalezniona od konfiguracji
serwera internetowego. Jednym z prostszych rozwiązań zabezpieczenia sie przed tymi
atakami jest zastosowanie białej listy hostów, do których aplikacja Symfony może
odpowiadać. W tym celu stworzona jest opcja ``trusted_hosts``. Jeśli host przychodzącego
żądania nie będzie pasować do tej listy, aplikacja nie zareaguje a użytkownik otrzyma
odpowiedź 500.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            trusted_hosts:  ['example.com', 'example.org']

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <trusted-host>example.com</trusted-host>
                <trusted-host>example.org</trusted-host>
                <!-- ... -->
            </framework>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            'trusted_hosts' => array('example.com', 'example.org'),
        ));

Hosty można również skonfigurować używając wyrażeń regularnych (np.  ``.*\.?example.com$``),
które umożliwiaja wykonanie wzorca odpowiedzi dla poddomen.

Dodatkowo, zaufane hosty można ustawić w kontrolerze wejścia (*ang. front controller*)
wykorzystując metodę ``Request::setTrustedHosts()``::

    // web/app.php
    Request::setTrustedHosts(array('.*\.?example.com$', '.*\.?example.org$'));

Domyślną wartością dla tej opcji jest pusta tablica, co oznacza, że aplikacja
może odpowiadać każdemu hostowi.

.. seealso::

    Więcej na ten temat można przeczytać we `wpisie na blogu Security Advisory`_.

.. _reference-framework-form:

form
~~~~

.. _reference-form-enabled:

enabled
.......

**typ**: ``boolean`` **domyślnie**: ``false``

Decyduje, czy ma być włączona usługa formularza w kontenerze usług, czy też nie.
Jeśli nie używa sie formularzy, ustawienie tej opcji na ``false`` moze zwiększyć
wydajność aplikacji, ponieważ do kontenera zostanie załadowane mniej usług.

Opcja ta zostanie automatycznie ustawiona na ``true``, gdy zostana skonfigurowane
ustawienia potomne.

.. note::

    Opcja ta automatycznie włącza `validation`_.

.. seealso::

    Więcej informacji można znaleźć w :doc:`/book/forms`.

csrf_protection
~~~~~~~~~~~~~~~

.. seealso::

    Więcej informacji o ochronie CSRF w formularzach znajduje się w rozdziale
    :ref:`forms-csrf`.

.. _reference-csrf_protection-enabled:

enabled
.......

**typ**: ``boolean`` **domyślnie**: ``true`` jeśli obsługiwany jest formularz,
inaczej  ``false``

Opcja ta może zostać użyta do wyłączenia ochrony CSRF dla *wszystkich* formularzy.
Lecz można również :ref:`wyłączyć ochronę CSRF dla poszczególnych formularzy <form-disable-csrf>`.

Jeśli używa sie formularzy, ale chce sie unikać rozpoczynania sesji (np. wykorzystując
formularze na stronie API-only), trzeba ustawić ``csrf_protection`` na ``false``.

field_name
..........

.. caution::

    Ustawienienie ``framework.csrf_protection.field_name`` jest przestarzałe od
    Symfony 2.4, zamiast tego trzeba uzywać opcji ``framework.form.csrf_protection.field_name`.

**typ**: ``string`` **domyślnie**: ``"_token"``

Nazwa ukrytego pola, wykorzystywana do renderowania :ref:`tokenu CSRF <forms-csrf>`.

esi
~~~

.. seealso::

    Więcej na temat Edge Side Includes (ESI) można przeczytać w :ref:`edge-side-includes`.

.. _reference-esi-enabled:

enabled
.......

**typ**: ``boolean`` **domyślnie**: ``false``

Ustala, czy we frameworku ma zostać włączona obsługa ESI (Edge Side Includes).

Ustawiajac tą opcje na ``true`` włącza sie obsługe ESI:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            esi: true

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <esi />
            </framework:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            'esi' => true,
        ));

fragments
~~~~~~~~~

.. seealso::

    Więcej na temat fragmentowania można przeczytać w
    :ref:`artykule HTTP Cache <book-http_cache-fragments>`.

.. _reference-fragments-enabled:

enabled
.......

**typ**: ``boolean`` **domyślnie**: ``false``

Decyduje, czy włączyć nasłuch fragmentów, czy też nie. Nasłuch fragmentów jest
używany do renderowania fragmentów ESI niezaleznie od reszty strony.

Opcja ta jest automatycznie ustawiana na ``true`` gdy zostanie skonfigurowana
jakakolwiek opcja potomna.

.. _reference-fragments-path:

path
....

**typ**: ``string`` **domyślnie**: ``'/_fragment'``

Przedrostek ścieżki dla fragmentów. Nasłuch fragmentów bedzie wykonywany tylko gdy
żądanie rozpoczyna się od tej ścieżki.

profiler
~~~~~~~~

.. _reference-profiler-enabled:

enabled
.......

.. versionadded:: 2.2
    Opcja ``enabled`` została wprowadzona w Symfony 2.2. Wcześniej profiler można
    było wyłączyć pomijając całkowicie konfiguracje ``framework.profiler``.

**typ**: ``boolean`` **domyślnie**: ``false``

Jeśli ``true``, to profiler jest włączony. Podczas używania Symfony Standard Edition,
profiler jest włączony w środowiskach ``dev`` i ``test``.

.. note::

    The profiler works independently from the Web Developer Toolbar, see
    the :doc:`WebProfilerBundle configuration </reference/configuration/web_profiler>`
    on how to disable/enable the toolbar.

collect
.......

.. versionadded:: 2.3
    Opcja ``collect`` została wprowadzona w Symfony 2.3. Poprzednio, gdy opcja
    ``profiler.enabled`` miała wartość ``false``, profiler *był* w rzeczywistości
    włączony, ale wyłączone były kolektory. Teraz, profiler i kolektory mogą być
    sterowane niezależnie.

**typ**: ``boolean`` **domyślnie**: ``true``

Opcja ta konfiguruje sposób w jaki zachowuje się profiler, gdy jest włączony.
Jeśli ustawiona jest na ``true``, profiler gromadzi dane dla wszystkich żądań
(chyba, że skonfigurowalo się to inaczej, jak opcję  `matcher`_). Jeśli chce się
tylko gromadzić informacje "na żądanie", można ustawić flagę ``collect`` na ``false``
i aktywować kolektory ręcznie::

    $profiler->enable();

only_exceptions
...............

**typ**: ``boolean`` **domyślnie**: ``false``

Gdy opcja jest ustawiona na ``true``, profiler będzie włączony jeśli podczas obsługi
żądania pojawi się wyjątek.

only_master_requests
....................

**typ**: ``boolean`` **domyślnie**: ``false``

Gdy opcja jest ustawiona na ``true``, profiler będzie dostępny na głównych żądaniach
(a nie na podżądaniach).

dsn
...

**typ**: ``string`` **domyślnie**: ``'file:%kernel.cache_dir%/profiler'``

DSN, w którym przechowuje się informacje profilowania.

.. seealso::

    Proszę przeczytać :doc:`/cookbook/profiler/storage` w celu uzyskania więcej
    informacji o pamięci profilera.

username
........

**typ**: ``string`` **domyślnie**: ``''``

Jeśli potrzebne, nazwa użytkownika dla pamięci profilera.

password
........

**typ**: ``string`` **domyślnie**: ``''``

Jeśli potrzebne, hasło dla pamięci profilera.

lifetime
........

**typ**: ``integer`` **domyślnie**: ``86400``

Czas przechowywania danych profilowania w sekundach. Po tym czasie dane w pamięci
profilera zostaną usunięte.

matcher
.......

Opcje ``matcher`` są konfigurowane w celu dynamicznego włączania profilera.
Na przykład, w oparciu o `ip`_ lub :ref:`path <reference-profiler-matcher-path>`.

.. seealso::

    Proszę przeczytać :doc:`/cookbook/profiler/matchers` w celu uzyskania więcej
    informacji o używaniu "matcherów" do włączania i wyłączania profilera.

ip
""

**typ**: ``string``

Jeśli ustawione, profiler bedzie włączany tylko wtedy, gdy dopasowany zostanie
adres IP.

.. _reference-profiler-matcher-path:

path
""""

**typ**: ``string``

Jeśli ustawione, profiler bedzie włączany tylko wtedy, gdy dopasowana zostanie
bieżąca ścieżka.

service
"""""""

**typ**: ``string``

Ustawienie to zawiera identyfikator usługi indywidualnego matchera.

router
~~~~~~

resource
........

**typ**: ``string`` **required**

Ścieżka głównego źródła trasowania (np. pliku YAML), które zawiera trasy i importy,
które router powinien ładować.

type
....

**typ**: ``string``

Typ źródła do informowania loaderów o formatach. Nie jest to potrzebne, jeśli
używa się domyślnych routerów z oczekiwanymi rozszerzeniami pliku
(``.xml``, ``.yml`` / ``.yaml``, ``.php``).

http_port
.........

**typ**: ``integer`` **domyślnie**: ``80``

Port dla zwykłych żądań http (jest to wykorzystywane podczas dopasowywania schematu).

https_port
..........

**typ**: ``integer`` **domyślnie**: ``443``

Port dla żądań https (jest to wykorzystywane podczas dopasowywania schematu).

strict_requirements
...................

**typ**: ``mixed`` **domyślnie**: ``true``

Określa zachowanie generatora trasowania. Podczas generowania trasy. która ma
określone :ref:`wymagania <book-routing-requirements>`, generator może zachowywać
się różnie w przypadku, gdy zastosowane parametry nie spełnią tych wymagań.

Wartościami tej opcji może być:

``true``
    Zrzucenie wyjątku, gdy wymagania nie są spełnione;
``false``
    Wyłącznie zrzutu wyjatków, gdy nie są spełnione wymagania i zwrócenie zamiast
    tego awrtości ``null``;
``null``
    Wyłączenie sprawdzania wymagań (w ten sposób, trasa zostaje dopasowania nawet
    gdy nie są spełnione wymagania).

Wartość ``true`` jest zalecana w środowisku programistycznym, natomiast ``false``
lub ``null`` w środowisku produkcyjnym.

session
~~~~~~~

storage_id
..........

**typ**: ``string`` **domyślnie**: ``'session.storage.native'``

Identyfikator usługi używanej do przechowywania sesji. Na ten identyfikator
zostanie ustawiony alias usługi ``session.storage``. Klasa ta musi implementować
:class:`Symfony\\Component\\HttpFoundation\\Session\\Storage\\SessionStorageInterface`.

handler_id
..........

**typ**: ``string`` **domyślnie**: ``'session.handler.native_file'``

Identyfikator usługi używanej do przechowywania sesji. Na ten identyfikatora
zostanie ustawiony alias usługi ``session.handler``.

Można również ustawić to na ``null``, aby domyślnie wskazaywać handler swojej
instalacji PHP.

.. seealso::

    Proszę zobaczyc przykład w
    :doc:`/cookbook/configuration/pdo_session_storage`.

name
....

**typ**: ``string`` **domyślnie**: ``null``

Określa nazwę pliku cookie sesji. Domyślnie stosowana jest nazwa ciasteczka określona
w ``php.ini`` w dyrektywie ``session.name``.

cookie_lifetime
...............

**typ**: ``integer`` **domyślnie**: ``null``

Określa czas życia sesji w sekundach. Domyślną wartością jest ``null``, co oznacza
że, zostanie użyta wartość ``sesssion.cookie_lifetime`` z ``php.ini``. Ustawinie
tej wartości na ``0``, oznacza że plik cookie jest ważny przez długość sesji
przeglądarki.

cookie_path
...........

**typ**: ``string`` **domyślnie**: ``/``

Określa ścieżkę, jaka ma być ustawiona w ciasteczku sesji. Domyślnie będzie to
``/``.

cookie_domain
.............

**typ**: ``string`` **domyślnie**: ``''``

Określa domenę do ustawienia w ciasteczku sesji. Domyślnie opcja ta jest pusta,
co oznacza nazwę hosta, który wygenerował ciasteczko zgodnie ze specyfikacją cookie.

cookie_secure
.............

**typ**: ``boolean`` **domyślnie**: ``false``

Określa czy ciasteczka należy przesyłać wyłącznie przez bezpieczne połączenie.

cookie_httponly
...............

**typ**: ``boolean`` **domyślnie**: ``false``

Okreśła czy ciasteczka powinny być dostępne wyłącznie poprzez protokół HTTP.
Oznacza to, że ciasteczka nie będą dostępne przez języki skryptowe, takie jak
JavaScript. Ustawienie to może skutecznie przyczynić sie do zmniejszenia zagrożenia
kradzieżą tożsamości poprzez ataki XSS.

gc_divisor
..........

**typ**: ``integer`` **domyślnie**: ``100``

Zobacz `gc_probability`_.

gc_probability
..............

**typ**: ``integer`` **domyślnie**: ``1``

Określa prawdobieństwo tego, że proces garbage collector (GC) rozpoczynany jest
przy każdej inicjacji sesji. Prawdopodobieństwo jest kalkulowane przy zastosowaniu
``gc_probability`` / ``gc_divisor``, czyli 1/100 oznacza, że jest 1% szansa na to,
aby proces GC rozpoczynał sie przy każdym żądaniu.

gc_maxlifetime
..............

**typ**: ``integer`` **domyślnie**: ``1440``

Określa liczbę sekund, po upływie których dane zostana uznane za "śmieci"
i ewentualnie wyczyszczone. Czyszczenie danych może wystąpić podczas rozpoczynania
sesji, co zależy od ustawienia `gc_divisor`_ i `gc_probability`_.

save_path
.........

**typ**: ``string`` **domyślnie**: ``%kernel.cache.dir%/sessions``

Określa argument, który ma być przekazany do handlera zapisu. Jeśli wybierze się
domyślny handler pliku, jest to ścieżka do miejsca, w którym tworzone są pliki
sesji.
Więcej informacji można znaleźć w artykule :doc:`/cookbook/session/sessions_directory`.

Ustawiajac tu ``null`` powoduje sie, że wykorzystywana będzie opcja ``save_path``
z pliku ``php.ini``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            session:
                save_path: ~

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:session save-path="null" />
            </framework:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            'session' => array(
                'save_path' => null,
            ),
        ));

templating
~~~~~~~~~~

.. _reference-framework-assets-version:
.. _ref-framework-assets-version:

assets_version
..............

**typ**: ``string``

This option is used to *bust* the cache on assets by globally adding a query
parameter to all rendered asset paths (e.g. ``/images/logo.png?v2``). This
applies only to assets rendered via the Twig ``asset`` function (or PHP
equivalent) as well as assets rendered with Assetic.

For example, suppose you have the following:

.. configuration-block::

    .. code-block:: html+jinja

        <img src="{{ asset('images/logo.png') }}" alt="Symfony!" />

    .. code-block:: php

        <img src="<?php echo $view['assets']->getUrl('images/logo.png') ?>" alt="Symfony!" />

By default, this will render a path to your image such as ``/images/logo.png``.
Now, activate the ``assets_version`` option:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            # ...
            templating: { engines: ['twig'], assets_version: v2 }

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:templating assets-version="v2">
                <!-- ... -->
                <framework:engine>twig</framework:engine>
            </framework:templating>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            // ...
            'templating'      => array(
                'engines'        => array('twig'),
                'assets_version' => 'v2',
            ),
        ));

Now, the same asset will be rendered as ``/images/logo.png?v2`` If you use
this feature, you **must** manually increment the ``assets_version`` value
before each deployment so that the query parameters change.

It's also possible to set the version value on an asset-by-asset basis (instead
of using the global version - e.g. ``v2`` - set here). See
:ref:`Versioning by Asset <book-templating-version-by-asset>` for details.

You can also control how the query string works via the `assets_version_format`_
option.

.. tip::

    As with all settings, you can use a parameter as value for the
    ``assets_version``. This makes it easier to increment the cache on each
    deployment.

.. _reference-templating-version-format:

assets_version_format
.....................

**typ**: ``string`` **domyślnie**: ``%%s?%%s``

This specifies a :phpfunction:`sprintf` pattern that will be used with the
`assets_version`_ option to construct an asset's path. By default, the pattern
adds the asset's version as a query string. For example, if
``assets_version_format`` is set to ``%%s?version=%%s`` and ``assets_version``
is set to ``5``, the asset's path would be ``/images/logo.png?version=5``.

.. note::

    All percentage signs (``%``) in the format string must be doubled to
    escape the character. Without escaping, values might inadvertently be
    interpreted as :ref:`book-service-container-parameters`.

.. tip::

    Some CDN's do not support cache-busting via query strings, so injecting
    the version into the actual file path is necessary. Thankfully,
    ``assets_version_format`` is not limited to producing versioned query
    strings.

    The pattern receives the asset's original path and version as its first
    and second parameters, respectively. Since the asset's path is one
    parameter, you cannot modify it in-place (e.g. ``/images/logo-v5.png``);
    however, you can prefix the asset's path using a pattern of
    ``version-%%2$s/%%1$s``, which would result in the path
    ``version-5/images/logo.png``.

    URL rewrite rules could then be used to disregard the version prefix
    before serving the asset. Alternatively, you could copy assets to the
    appropriate version path as part of your deployment process and forgot
    any URL rewriting. The latter option is useful if you would like older
    asset versions to remain accessible at their original URL.

hinclude_default_template
.........................

**typ**: ``string`` **domyślnie**: ``null``

Sets the content shown during the loading of the fragment or when JavaScript
is disabled. This can be either a template name or the content itself.

.. seealso::

    See :ref:`book-templating-hinclude` for more information about hinclude.

.. _reference-templating-form:

form
....

resources
"""""""""

**typ**: ``string[]`` **domyślnie**: ``['FrameworkBundle:Form']``

A list of all resources for form theming in PHP. This setting is not required
if you're using the Twig format for your templates, in that case refer to
:ref:`the form book chapter <book-forms-theming-twig>`.

Assume you have custom global form themes in
``src/WebsiteBundle/Resources/views/Form``, you can configure this like:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            templating:
                form:
                    resources:
                        - 'WebsiteBundle:Form'

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>

                <framework:templating>

                    <framework:form>

                        <framework:resource>WebsiteBundle:Form</framework:resource>

                    </framework:form>

                </framework:templating>

            </framework:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            'templating' => array(
                'form' => array(
                    'resources' => array(
                        'WebsiteBundle:Form'
                    ),
                ),
            ),
        ));

.. note::

    The default form templates from ``FrameworkBundle:Form`` will always
    be included in the form resources.

.. seealso::

    See :ref:`book-forms-theming-global` for more information.

.. _reference-templating-base-urls:

assets_base_urls
................

**domyślnie**: ``{ http: [], ssl: [] }``

This option allows you to define base URLs to be used for assets referenced
from ``http`` and ``ssl`` (``https``) pages. If multiple base URLs are
provided, Symfony will select one from the collection each time it generates
an asset's path:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            # ...
            templating:
                assets_base_urls:
                    http:
                        - "http://cdn.example.com/"
                # you can also pass just a string:
                # assets_base_urls:
                #     http: "//cdn.example.com/"

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:framework="http://symfony.com/schema/dic/symfony">

            <framework:config>
                <!-- ... -->

                <framework:templating>
                    <framework:assets-base-url>
                        <framework:http>http://cdn.example.com/</framework:http>
                    </framework:assets-base-url>
                </framework:templating>
            </framework:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            // ...
            'templating' => array(
                'assets_base_urls' => array(
                    'http' => array(
                        'http://cdn.example.com/',
                    ),
                ),
                // you can also pass just a string:
                // 'assets_base_urls' => array(
                //     'http' => '//cdn.example.com/',
                // ),
            ),
        ));

For your convenience, you can pass a string or array of strings to
``assets_base_urls`` directly. This will automatically be organized into
the ``http`` and ``ssl`` base urls (``https://`` and `protocol-relative`_
URLs will be added to both collections and ``http://`` only to the ``http``
collection):

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            # ...
            templating:
                assets_base_urls:
                    - "//cdn.example.com/"
                # you can also pass just a string:
                # assets_base_urls: "//cdn.example.com/"

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:framework="http://symfony.com/schema/dic/symfony">

            <framework:config>
                <!-- ... -->

                <framework:templating>
                    <framework:assets-base-url>//cdn.example.com/</framework:assets-base-url>
                </framework:templating>
            </framework:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            // ...
            'templating' => array(
                'assets_base_urls' => array(
                    '//cdn.example.com/',
                ),
                // you can also pass just a string:
                // 'assets_base_urls' => '//cdn.example.com/',
            ),
        ));

.. _reference-templating-cache:

cache
.....

**typ**: ``string``

The path to the cache directory for templates. When this is not set, caching
is disabled.

.. note::

    When using Twig templating, the caching is already handled by the
    TwigBundle and doesn't need to be enabled for the FrameworkBundle.

engines
.......

**typ**: ``string[]`` / ``string`` **required**

The Templating Engine to use. This can either be a string (when only one
engine is configured) or an array of engines.

At least one engine is required.

loaders
.......

**typ**: ``string[]``

An array (or a string when configuring just one loader) of service ids for
templating loaders. Templating loaders are used to find and load templates
from a resource (e.g. a filesystem or database). Templating loaders must
implement :class:`Symfony\\Component\\Templating\\Loader\\LoaderInterface`.

packages
........

You can group assets into packages, to specify different base URLs for them:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            # ...
            templating:
                packages:
                    avatars:
                        base_urls: 'http://static_cdn.example.com/avatars'

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>

                <framework:templating>

                    <framework:package
                        name="avatars"
                        base-url="http://static_cdn.example.com/avatars">

                </framework:templating>

            </framework:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            // ...
            'templating' => array(
                'packages' => array(
                    'avatars' => array(
                        'base_urls' => 'http://static_cdn.example.com/avatars',
                    ),
                ),
            ),
        ));

Now you can use the ``avatars`` package in your templates:

.. configuration-block:: php

    .. code-block:: html+jinja

        <img src="{{ asset('...', 'avatars') }}">

    .. code-block:: html+php

        <img src="<?php echo $view['assets']->getUrl('...', 'avatars') ?>">

Each package can configure the following options:

* :ref:`base_urls <reference-templating-base-urls>`
* :ref:`version <reference-framework-assets-version>`
* :ref:`version_format <reference-templating-version-format>`

translator
~~~~~~~~~~

.. _reference-translator-enabled:

enabled
.......

**typ**: ``boolean`` **domyślnie**: ``false``

Whether or not to enable the ``translator`` service in the service container.

.. _fallback:

fallbacks
.........

**typ**: ``string|array`` **domyślnie**: ``array('en')``

.. versionadded:: 2.3.25
    The ``fallbacks`` option was introduced in Symfony 2.3.25. Prior
    to Symfony 2.3.25, it was called ``fallback`` and only allowed one fallback
    language defined as a string. Please note that you can still use the
    old ``fallback`` option if you want define only one fallback.

This option is used when the translation key for the current locale wasn't
found.

.. seealso::

    For more details, see :doc:`/book/translation`.

.. _reference-framework-translator-logging:

logging
.......

.. versionadded:: 2.6
    The ``logging`` option was introduced in Symfony 2.6.

**domyślnie**: ``true`` when the debug mode is enabled, ``false`` otherwise.

When ``true``, a log entry is made whenever the translator cannot find a translation
for a given key. The logs are made to the ``translation`` channel and at the
``debug`` for level for keys where there is a translation in the fallback
locale and the ``warning`` level if there is no translation to use at all.

property_accessor
~~~~~~~~~~~~~~~~~

magic_call
..........

**typ**: ``boolean`` **domyślnie**: ``false``

When enabled, the ``property_accessor`` service uses PHP's
:ref:`magic __call() method <components-property-access-magic-call>` when
its ``getValue()`` method is called.

throw_exception_on_invalid_index
................................

**typ**: ``boolean`` **domyślnie**: ``false``

When enabled, the ``property_accessor`` service throws an exception when you
try to access an invalid index of an array.

validation
~~~~~~~~~~

.. _reference-validation-enabled:

enabled
.......

**typ**: ``boolean`` **domyślnie**: ``true`` if :ref:`form support is enabled <reference-form-enabled>`,
``false`` otherwise

Whether or not to enable validation support.

This option will automatically be set to ``true`` when one of the child
settings is configured.

.. _reference-validation-cache:

cache
.....

**typ**: ``string``

The service that is used to persist class metadata in a cache. The service
has to implement the :class:`Symfony\\Component\\Validator\\Mapping\\Cache\\CacheInterface`.

.. _reference-validation-enable_annotations:

enable_annotations
..................

**typ**: ``boolean`` **domyślnie**: ``false``

If this option is enabled, validation constraints can be defined using annotations.

translation_domain
..................

**typ**: ``string`` **domyślnie**: ``validators``

The translation domain that is used when translating validation constraint
error messages.

strict_email
............

.. versionadded:: 2.5
    The ``strict_email`` option was introduced in Symfony 2.5.

**typ**: ``Boolean`` **domyślnie**: ``false``

If this option is enabled, the `egulias/email-validator`_ library will be
used by the :doc:`/reference/constraints/Email` constraint validator. Otherwise,
the validator uses a simple regular expression to validate email addresses.

api
...

.. versionadded:: 2.5
    The ``api`` option was introduced in Symfony 2.5.

**typ**: ``string``

Starting with Symfony 2.5, the Validator component introduced a new validation
API. The ``api`` option is used to switch between the different implementations:

``2.5``
    Use the validation API introduced in Symfony 2.5.

``2.5-bc`` or ``auto``
    If you omit a value or set the ``api`` option to ``2.5-bc`` or ``auto``,
    Symfony will use an API implementation that is compatible with both the
    legacy ``2.4`` implementation and the ``2.5`` implementation.

.. note::

    The support for the native 2.4 API has been dropped since Symfony 2.7.

To capture these logs in the ``prod`` environment, configure a
:doc:`channel handler </cookbook/logging/channels_handlers>` in ``config_prod.yml`` for
the ``translation`` channel and set its ``level`` to ``debug``.

annotations
~~~~~~~~~~~

.. _reference-annotations-cache:

cache
.....

**typ**: ``string`` **domyślnie**: ``'file'``

This option can be one of the following values:

file
    Use the filesystem to cache annotations
none
    Disable the caching of annotations
a service id
    A service id referencing a `Doctrine Cache`_ implementation

file_cache_dir
..............

**typ**: ``string`` **domyślnie**: ``'%kernel.cache_dir%/annotations'``

The directory to store cache files for annotations, in case
``annotations.cache`` is set to ``'file'``.

debug
.....

**typ**: ``boolean`` **domyślnie**: ``%kernel.debug%``

Whether to enable debug mode for caching. If enabled, the cache will
automatically update when the original file is changed (both with code and
annotation changes). For performance reasons, it is recommended to disable
debug mode in production, which will happen automatically if you use the
default value.

.. _configuration-framework-serializer:

serializer
~~~~~~~~~~

.. _reference-serializer-enabled:

enabled
.......

**typ**: ``boolean`` **domyślnie**: ``false``

Whether to enable the ``serializer`` service or not in the service container.

.. _reference-serializer-cache:

cache
.....

**typ**: ``string``

The service that is used to persist class metadata in a cache. The service
has to implement the :class:`Doctrine\\Common\\Cache\\Cache` interface.

.. seealso::

    For more information, see :ref:`cookbook-serializer-enabling-metadata-cache`.

.. _reference-serializer-enable_annotations:

enable_annotations
..................

**typ**: ``boolean`` **domyślnie**: ``false``

If this option is enabled, serialization groups can be defined using annotations.

.. seealso::

    For more information, see :ref:`cookbook-serializer-using-serialization-groups-annotations`.

Pełna domyślna konfiguracja
---------------------------

.. configuration-block::

    .. code-block:: yaml

        framework:
            secret:               ~
            http_method_override: true
            trusted_proxies:      []
            ide:                  ~
            test:                 ~
            default_locale:       en

            csrf_protection:
                enabled:              false
                field_name:           _token # Deprecated since 2.4, to be removed in 3.0. Use form.csrf_protection.field_name instead

            # form configuration
            form:
                enabled:              false
                csrf_protection:
                    enabled:          true
                    field_name:       ~

            # esi configuration
            esi:
                enabled:              false

            # fragments configuration
            fragments:
                enabled:              false
                path:                 /_fragment

            # profiler configuration
            profiler:
                enabled:              false
                collect:              true
                only_exceptions:      false
                only_master_requests: false
                dsn:                  file:%kernel.cache_dir%/profiler
                username:
                password:
                lifetime:             86400
                matcher:
                    ip:                   ~

                    # use the urldecoded format
                    path:                 ~ # Example: ^/path to resource/
                    service:              ~

            # router configuration
            router:
                resource:             ~ # Required
                type:                 ~
                http_port:            80
                https_port:           443

                # * set to true to throw an exception when a parameter does not
                #   match the requirements
                # * set to false to disable exceptions when a parameter does not
                #   match the requirements (and return null instead)
                # * set to null to disable parameter checks against requirements
                #
                # 'true' is the preferred configuration in development mode, while
                # 'false' or 'null' might be preferred in production
                strict_requirements:  true

            # session configuration
            session:
                storage_id:           session.storage.native
                handler_id:           session.handler.native_file
                name:                 ~
                cookie_lifetime:      ~
                cookie_path:          ~
                cookie_domain:        ~
                cookie_secure:        ~
                cookie_httponly:      ~
                gc_divisor:           ~
                gc_probability:       ~
                gc_maxlifetime:       ~
                save_path:            "%kernel.cache_dir%/sessions"

            # serializer configuration
            serializer:
               enabled: false

            # templating configuration
            templating:
                assets_version:       ~
                assets_version_format:  "%%s?%%s"
                hinclude_default_template:  ~
                form:
                    resources:

                        # Default:
                        - FrameworkBundle:Form
                assets_base_urls:
                    http:                 []
                    ssl:                  []
                cache:                ~
                engines:              # Required

                    # Example:
                    - twig
                loaders:              []
                packages:

                    # Prototype
                    name:
                        version:              ~
                        version_format:       "%%s?%%s"
                        base_urls:
                            http:                 []
                            ssl:                  []

            # translator configuration
            translator:
                enabled:              false
                fallbacks:            [en]
                logging:              "%kernel.debug%"

            # validation configuration
            validation:
                enabled:              false
                cache:                ~
                enable_annotations:   false
                translation_domain:   validators

            # annotation configuration
            annotations:
                cache:                file
                file_cache_dir:       "%kernel.cache_dir%/annotations"
                debug:                "%kernel.debug%"


.. _`protocol-relative`: http://tools.ietf.org/html/rfc3986#section-4.2
.. _`HTTP Host header attacks`: http://www.skeletonscribe.net/2013/05/practical-http-host-header-attacks.html
.. _`wpisie na blogu Security Advisory`: https://symfony.com/blog/security-releases-symfony-2-0-24-2-1-12-2-2-5-and-2-3-3-released#cve-2013-4752-request-gethost-poisoning
.. _`Doctrine Cache`: http://docs.doctrine-project.org/projects/doctrine-common/en/latest/reference/caching.html
.. _`egulias/email-validator`: https://github.com/egulias/EmailValidator
.. _`PhpStormProtocol`: https://github.com/aik099/PhpStormProtocol 
.. highlight:: php
   :linenothreshold: 2
   
.. index::
   single: konfiguracja

Konfigurowanie Symfony (i środowisk)
====================================

Aplikacja zawiera kolekcję pakietów reprezentujacych wszystkie funkcjonalności
i możliwości aplikacji. Każdy pakiet można dostosować poprzez plik konfiguracyjny
napisany w formacie YAML, XML lub PHP. Główny plik konfiguracyjny
znajduje się domyślnie w katalogu ``app/config/`` i nosi nazwę ``config.yml``,
``config.xml`` lub ``config.php`` w zależności od stosowanego formatu:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        imports:
            - { resource: parameters.yml }
            - { resource: security.yml }

        framework:
            secret:          '%secret%'
            router:          { resource: '%kernel.root_dir%/config/routing.yml' }
            # ...

        # Twig Configuration
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

            <imports>
                <import resource="parameters.yml" />
                <import resource="security.yml" />
            </imports>

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
        $this->import('parameters.yml');
        $this->import('security.yml');

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

.. note::
   
   W rozdziale :ref:`environments-summary`_ dowiesz się jak załadować każdy plik o określonym
   formacie.

Każdy wpis najwyższego poziomu, taki jak ``framework`` czy ``twig`` definiuje
konfigurację dla określonego pakietu. Na przykład, klucz ``framework`` definiuje
konfiguracje pakietu FrameworkBundle rdzenia Symfony, łącznie z konfiguracją dla
trasowania, szablonowania i innych elementów rdzenia.

Na razie nie musisz się martwić o poszczególne opcje konfiguracyjne w każdej
sekcji. Pliki konfiguracyjne są dostarczane już z rozsądnym domyślnym ustawieniem.
Podczas czytania tego podręcznika i odkrywania każdej części Symfony, nauczysz
się znaczenia poszczególnych opcji konfiguracyjnych dla każdej funkcjonalności.

.. sidebar:: Formaty konfiguracyjne

    W wszystkich rozdziałach wszystkie przykłady kobfiguracji sa pokazane
    we wszystkich dostęþnych formatach(YAML, XML i PHP). Każdy z tych formatów
    ma swoje zalety i wady. Wybór któregoś z nich zalezy tylko od Ciebie:

    * *YAML*: Prosty, czysty i czytelny (więcej o YAML w
      ":doc:`/components/yaml/yaml_format`");

    * *XML*: W tym momencie bardziej zaawansowany niż YAML i obsługujący autuzupełnianie
      w IDE;

    * *PHP*: Bardzo zaawansowany, ale mniej czytelny niż standardowe formaty
      konfiguracyjne.

Dmomyślny zrzut konfiguracji
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Można zrzucić na konsoli domyślną konfiguracje dla jakiegoś pakietu w formacie
YAML używając polecenia ``config:dump-reference``. Oto przykład zrzutu domyślnej
konfiguracji dla FrameworkBundle:

.. code-block:: bash

    $ php bin/console config:dump-reference FrameworkBundle

Możne również wykorzystać alias rozszerzenia (klucz konfiguracyjny):

.. code-block:: bash

    $ php bin/console config:dump-reference framework

.. note::

    Proszę przeczytać artykuł :doc:`/cookbook/bundles/extension` w celu uzyskania
    informacji o dodawaniu konfiguracji dla własnego pakietu.

.. index::
   single: środowiska

.. _environments-summary:
.. _page-creation-environments:
.. _book-page-creation-prod-cache-clear:

Środowiska
----------

Aplikacja może być uruchamiana w różnych środowiskach. Różne środowska współdzielą
ten sam kod PHP (z wyjątkiem :term:`kontrolera wejścia <kontroler wejścia>`),
ale używają inną konfigurację. Na przykład, w środowisku ``dev`` będą rejestrowane
ostrzeżenia i błędy, natomiast w środowisku ``prod`` tylko błędy. W środowisku
``dev`` przebudowywane są niektóre pliki przy każdym żądaniu (dla wygody programisty),
ale w środowisku ``prod`` są one buforowane. Wszystkie środowiska są umieszczone
na tym samym komputerze i wykonują tą samą aplikację.

Projekt Symfony rozpoczyna się na ogół z trzema środowiskami (``dev``, ``test``
i ``prod``), ale tworzenie nowych środowiski jest łatwe. Mozna przegladać aplikację
w róznych śodowiskach, po prostu zmieniając w adresie URL część dotyczącą kontrolera
wejścia. W celu zobacznie aplikacji w środowisku ``dev``, trzeba wywołać aplikację
z programistycznym kontrolerem wejścia:

.. code-block:: text

    http://localhost/app_dev.php/random/10

Jeśli chcesz zobaczyć, jak będzie się zachowywała aplikacja w środowisku produkcyjnym,
zamiast tego wywołaj kontroler wejścia ``prod``:

.. code-block:: text

    http://localhost/app.php/random/10

Ponieważ środowisko ``prod`` jest zoptymalizowane pod kątem szybkości; konfiguracja,
trasowanie i szablony Twig są kompilowane do płaskich klas PHP i buforowane.
Podczas przegladania zmian w środowisku ``prod`` zachodzi potrzeba wyczyszczenia
plików pamięci podręcznej i ponownego ich odbudowania:

.. code-block:: bash

    $ php bin/console cache:clear --env=prod --no-debug

.. note::

   Jeśli otworzysz plik ``web/app.php`` file, to znajdziesz tam jawną deklarację
   użycia konfiguracji dla środowiska``prod``::

       $kernel = new AppKernel('prod', false);

   Można utworzyć mowy kontroler wejścia dla nowego środowiska, kopiując plik
   ``app.php`` i zmieniając w nim ``prod`` na jakąś inna wartość.

.. note::

    Środowisko ``test`` jest uzywane podczas uruchamiania automatycznych testów
    i nie moze być dostęþne bezpośrednio w przegladarce. Proszę przeczytać
    :doc:`testing chapter </book/testing>` w celu poznania szczegółów.

.. index::
   single: środowiska; konfiguracja

Konfiguracja środowiska
~~~~~~~~~~~~~~~~~~~~~~~

Klasa ``AppKernel`` jest odpowiedzialna za faktyczne ładowanie pliku konfiguracyjnego,
jaki się wybierze::

    // app/AppKernel.php
    public function registerContainerConfiguration(LoaderInterface $loader)
    {
        $loader->load(
            __DIR__.'/config/config_'.$this->getEnvironment().'.yml'
        );
    }

Wiesz juz, że rozszerzenie ``.yml`` moze zistać zmienione na ``.xml`` lub ``.php``,
jeśli preferuje się konfiguracje w formacie odpowiednio XML albo PHP.
Proszę zauważyć, że kazde środowisko ładuje swój własny plik konfiguracyjny.
Przyjrzyjmy się plikowi konfiguracyjnemu dla środowiska ``dev``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config_dev.yml
        imports:
            - { resource: config.yml }

        framework:
            router:   { resource: '%kernel.root_dir%/config/routing_dev.yml' }
            profiler: { only_exceptions: false }

        # ...

    .. code-block:: xml
       :linenos:

        <!-- app/config/config_dev.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <imports>
                <import resource="config.xml" />
            </imports>

            <framework:config>
                <framework:router resource="%kernel.root_dir%/config/routing_dev.xml" />
                <framework:profiler only-exceptions="false" />
            </framework:config>

            <!-- ... -->
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config_dev.php
        $loader->import('config.php');

        $container->loadFromExtension('framework', array(
            'router' => array(
                'resource' => '%kernel.root_dir%/config/routing_dev.php',
            ),
            'profiler' => array('only-exceptions' => false),
        ));

        // ...

Klucz ``imports`` jest podobny do wyrażenie ``include`` w PHP i gwarantuje, że
jako pierwsza zostanie załadowana główny plik konfiguracyjny (``config.yml``).
Pozostała część pliku zmienia konfigurację w zakresie rejestrowania zdarzeń
i innych ustawień indywidualizowanych w środowisku programistycznym.

Zarówno środowisko ``prod`` jak i ``test`` spełniają ten sam model: każde środowisko
impotuje podstawowy plik konfiguracyjny i następnie zmienia swoje wartości konfiguracyjne,
tak jak to jest potrzebne w danym środowisku. Jest to tylko konwencja, ale pozwala
na istotne zmniejszenie kodu w poszczególnych plikach, przez wyodrębnienie kodu
wspólnego.

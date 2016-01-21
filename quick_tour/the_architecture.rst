.. highlight:: php
   :linenothreshold: 2

Architektura
============

Jestem pełen uznania dla Ciebie! Miałem obawy, że nie znajdziesz sie tutaj po
lekturze trzech pierwszych części przewodnika. Twoje starania zostaną niedługo
nagrodzone. Pierwsze trzy części nie zagłebiały się w architekturę frameworka.
Ona to sprawia, że Symfony wyróżnia się z tłumu innych frameworków.
Zajmijmy się więc teraz architekturą Symfony.

Struktura katalogów
-------------------

Struktura katalogów Symfony jest dość elastyczna, ale zalecana struktura katalogów

jest następująca:

``app/``
   Konfiguracja aplikacji, szablony i tłumaczenia.

``bin/``
    Pliki wykonywalne (np. ``bin/console``).   

``src/``
   Kod PHP projektu
   
``tests/``
    Testy automatyczne (np. testy jednostkowe).
    
``var/``
    Wygenerowane pliki (pamięć podręczna, dzienniki itd.).   

``vendor/``
   Biblioteki zewnętrzne;

``web/``
   Katalog główny serwera.

Katalog ``web/``
~~~~~~~~~~~~~~~~

Katalog ``web`` jest miejscem wszystkich publicznych oraz statycznych
plików takich jak obrazy, arkusze stylów oraz pliki JavaScript. Tam także
umiejscowiony jest :term:`kontroler wejścia <kontroler wejścia>`, taki jak
kontroler wejścia środowiska produkcyjnego, z takim kodem::

    // web/app.php
    require_once __DIR__.'/../var/bootstrap.php.cache';
    require_once __DIR__.'/../app/AppKernel.php';

    use Symfony\Component\HttpFoundation\Request;

    $kernel = new AppKernel('prod', false);
    $kernel->loadClassCache();
    $request = Request::createFromGlobals();
    $response = $kernel->handle($request);
    $response->send();

Kontroler ten najpierw ładuje aplikacje wykorzystując klasę kernela (``AppKernel``).
Następnie, tworzy obiekt ``Request`` używając globalnych zmiennych
PHP i przekazuje ten obiekt do kernela. Następnym krokiem jest przesłanie zawartości
odpowiedzi zwracanej przez kernel z powrotem do użytkownika.

.. _the-app-dir:

Katalog ``app/``
~~~~~~~~~~~~~~~~

Klasa ``AppKernel`` jest głównym punktem wejścia do konfiguracji aplikacji
i jako taka, jest przechowywana w folderze ``app/``.

Ta klasa musi implementować dwie metody:

``registerBundles()``
   Zwraca tablicę wszystkich pakietów potrzebnych do uruchomienia aplikacji, tak
   jak wyjaśniono to w następnym rozdziale;

``registerContainerConfiguration()``
    Wczytuje konfigurację aplikacji (więcej na ten temat później).

Automatyczne ładowanie jest obsługiwane poprzez `Composer`_, co oznacza, że można
wykorzystać dowolną klasę PHP, nie robiąc nic w ogóle! Wszystkie zależności są
przechowywane w katalogu ``vendor/``, ale to jest tylko konwencja.
Można przechowywać je tam gdzie się chce, globalnie na serwerze lub lokalnie w projektach.


Wyjaśnienie systemu pakietów
----------------------------

Rozdział ten jest wprowadzeniem do jedenej z największych i najpotężniejszych cech Symfony -
systemu :term:`pakietów <pakiet>`.

Pakiet jest czymś w rodzaju wtyczki w innych programach. Więc dlaczego został nazwany
pakietem (*ang. bundle*) a nie wtyczką (*ang. plugin*)? To dlatego, że wszystko w Symfony
należy do jakiegoś pakietu, od funkcji rdzenia frameworka po kod napisany dla aplikacji.

Cały kod, jaki się pisze dla aplikacji jest zorganizowany w pakiety. W Symfony
mówi się, że pakiet jest ustrukturyzowanym zestawem plików (pliki PHP, arkusze stylów,
pliki JavaScripts, obrazy itd.), które implementuja pojedyncze funkcjonalności
(blog, forum  itd.) i które mogą być łatwo współdzielone przez innych programistów.

Pakiety są obywatelem numer jeden w Symfony. Zapewnia to elastyczność w używaniu
wbudowanych pakietów funkcyjnych rozpowszechnianych przez osoby trzecie lub w dystrybucji
własnych pakietów. Stwarza to możliwość łatwego doboru i wyboru odpowiednich
dla swojej aplikacji funkcjonalności i umożliwia łatwą optymalizację całości.
Tak na koniec - w Symfony Twój kod jest tak samo ważny jak kod frameworka.

.. note::

   W rzeczywistości pojęcie pakietu (*ang. bundle*), nie jest pojęciem wyłącznym
   dla Symfony. Pakiety są ściśle związane z przestrzenią nazw i w niektórych
   językach (jak np. w Java) są sformalizowane od początku. W PHP pojęcia te nie
   istniały, co było powodem wielu problemów. Zmuszało to programistów do stosowania
   własnych konwencji nazewniczych. Pakiety i przestrzenie nazw zostały formalnie
   wprowadzone w PHP 5.3 i tym samym pojawiły się w Symfony.

Rejestrowanie pakietów
~~~~~~~~~~~~~~~~~~~~~~

Aplikacja składa się z pakietów określonych przez metodę ``registerBundles()``
klasy ``AppKernel``. Każdy pakiet jest katalogiem zawierającym pojedyńczą klasę
``Bundle`` opisującą ten pakiet::

    // app/AppKernel.php
    public function registerBundles()
    {
        $bundles = array(
            new Symfony\Bundle\FrameworkBundle\FrameworkBundle(),
            new Symfony\Bundle\SecurityBundle\SecurityBundle(),
            new Symfony\Bundle\TwigBundle\TwigBundle(),
            new Symfony\Bundle\MonologBundle\MonologBundle(),
            new Symfony\Bundle\SwiftmailerBundle\SwiftmailerBundle(),
            new Symfony\Bundle\DoctrineBundle\DoctrineBundle(),
            new Symfony\Bundle\AsseticBundle\AsseticBundle(),
            new Sensio\Bundle\FrameworkExtraBundle\SensioFrameworkExtraBundle(),
            new AppBundle\AppBundle(),
        );

        if (in_array($this->getEnvironment(), array('dev', 'test'))) {
            $bundles[] = new Symfony\Bundle\WebProfilerBundle\WebProfilerBundle();
            $bundles[] = new Sensio\Bundle\DistributionBundle\SensioDistributionBundle();
            $bundles[] = new Sensio\Bundle\GeneratorBundle\SensioGeneratorBundle();
        }

        return $bundles;
    }
    
Proszę zauważyć, że oprócz pakietu ``AppBundle``, który już był omawiany, kernel
udostępnia również inne pakiety, takie jak ``FrameworkBundle``, ``DoctrineBundle``,
``SwiftmailerBundle`` czy ``AsseticBundle``.

Konfiguracja pakietu
~~~~~~~~~~~~~~~~~~~~

Każdy pakiet może być dostosowywany poprzez pliki konfiguracyjne w języku YAML,
XML, czy też PHP. Wystarczy popatrzeć na domyślną konfigurację Symfony:

.. code-block:: yaml
   :linenos:

    # app/config/config.yml
    imports:
        - { resource: parameters.yml }
        - { resource: security.yml }
        - { resource: services.yml }

    framework:
        #esi:             ~
        #translator:      { fallbacks: ["%locale%"] }
        secret:          '%secret%'
        router:
            resource: '%kernel.root_dir%/config/routing.yml'
            strict_requirements: '%kernel.debug%'
        form:            true
        csrf_protection: true
        validation:      { enable_annotations: true }
        templating:      { engines: ['twig'] }
        default_locale:  '%locale%'
        trusted_proxies: ~
        session:         ~

    # Twig Configuration
    twig:
        debug:            '%kernel.debug%'
        strict_variables: '%kernel.debug%'

    # Swift Mailer Configuration
    swiftmailer:
        transport: '%mailer_transport%'
        host:      '%mailer_host%'
        username:  '%mailer_user%'
        password:  '%mailer_password%'
        spool:     { type: memory }

    # ...

Każdy wpis pierwszego poziomu, jak np. ``framework``, ``twig`` lub ``doctrine``,
 definiuje konfigurację dla określonego pakietu. Dla przykładu, ``framework``
 konfiguruje pakiet FrameworkBundle a ``swiftmailer`` konfiguruje SwiftmailerBundle.

Każde :term:`środowisko` może nadpisać domyślną konfigurację poprzez dostarczenie
odpowiedniego pliku konfiguracyjnego. Dla przykładu, środowisko ``dev`` wczytuje plik
``config_dev.yml``, który to wczytuje główną konfigurację (np. ``config.yml``) oraz
modyfikuje go w celu dodania narzędzi do debugowania:

.. code-block:: yaml
   :linenos:

    # app/config/config_dev.yml
    imports:
        - { resource: config.yml }

    framework:
        router:   { resource: '%kernel.root_dir%/config/routing_dev.yml' }
        profiler: { only_exceptions: false }

    web_profiler:
        toolbar: true
        intercept_redirects: false

    # ...

Rozszerzanie pakietu
~~~~~~~~~~~~~~~~~~~~

Oprócz tego że pakiety są dobrym sposobem na zorganizowanie i skonfigurowanie kodu,
pakiet może rozszerzać inny pakiet. Dziedziczenie pakietu umożliwia zamienienie
istniejącego pakietu w celu dostosowania jego kontrolerów, szablonów lub każdego
z jego plików. Tu właśnie przydadzą się logiczne nazwy
(np. ``@AcmeDemoBundle/Controller/SecuredController.php``) - są abstraktem,
niezależnym od rzeczywistego miejsca przechowywania zasobu.

Logiczne nazwy plików
.....................

Kiedy chce się odwołać do pliku pakietu, trzeba użyj notacji:
``@BUNDLE_NAME/path/to/file``. Symfony zamieni ``@BUNDLE_NAME`` na
realną ścieżkę do pakietu. Na przykład, logiczna ścieżka
``@AppBundle/Controller/DemoController.php`` zostanie przekształcona
do ``src/AppBundle/Controller/DemoController.php`` ponieważ Symfony
zna lokalizację ``AcmeDemoBundle``.

Logiczne nazwy kontrolerów
..........................

W przypadku kontrolerów trzeba odwołać się do akcji stosując notację
``BUNDLE_NAME:CONTROLLER_NAME:ACTION_NAME``. Dla przykładu,
``AppBundle:Default:index`` wskazuje na metodę ``indexAction``
z klasy ``AppBundle\Controller\DefaultController``.


Rozszerzenie pakietów
.....................

Stosując tą konwencję, można następnie wykorzystać
:doc:`dziedziczenia pakietów </cookbook/bundles/inheritance>` do "napisania" plików,
kontrolerów lub szablonów. Na przykład, można utworzyć pakiet ``NewBundle``
i  określić, że zastępuje on pakiet AppBundle. Gdy Symfony ładuje kontroler
``AppBundle:Default:index``, to najpierw będzie wyszukiwał klasy ``DefaultController``
w pakiecie NewBundle i jeśli jej nie znajdzie, to rozpocznie przeszukiwanie
pakietu AppBundle. Oznacza to, że pakiet może zastąpić prawie każdą część
innego pakietu.

Rozumiesz teraz dlaczego Symfony jest tak elastyczny? Współdziel swoje pakiety
pomiędzy aplikacjami, przechowuj je lokalnie lub globalnie, to zależy od tylko
Ciebie.

.. _using-vendors:

Korzystanie ze żródeł dostawców
-------------------------------

Jest bardzo prawdopodobne, że Twoja aplikacja będzie zależeć od bibliotek i pakietów
osób trzecich. Powinny być one przechowywane w katalogu ``vendor/``.
Nie powinno się niczego dotykać w tym katalogu, ponieważ jest on wyłacznie zarządzany
przez Composer.
Katalog ten już zawiera biblioteki Symfony, biblioteki ``SwiftMailer``, ``Doctrine ORM``,
system szablonów Twig i trochę innych bibliotek i pakietów osób trzecich, zwanych
też **dostawcami**.

Wyjaśnienie pamięci podręcznej i dzienników zdarzeń
---------------------------------------------------

Symfony jest prawdopodobnie jednym z najszybszych pełnych frameworków PHP.
Ale jak może tak szybko działać, skoro parsuje oraz interpretuje kilkadziesiąt
plików YAML oraz XML dla każdego zapytania. Prędkość jest po części związana
z systemem buforowania. Konfiguracja aplikacji jest parsowana tylko dla pierwszego
żądania i przetwarzana do kodu PHP przechowywanego w katalogu ``var/cache/``.

W środowisku programistycznym, Symfony jest wystarczająco inteligentny aby czyścić
pamięć podręczną po zmianie pliku. Natomiast w środowisku produkcyjnym, to do 
do zadań programisty należy czyszczenie pamięci podręcznej po zmianie kodu lub
konfiguracji. W celu wyczyszczenia pamięci podręcznej w srodowisku ``prod`` można
użyć tego poleenia:

.. code-block:: bash

    $ php bin/console cache:clear --env=prod

Podczas tworzenia aplikacji, dużo rzeczy może pójść źle. Pliki dzienników zdarzeń,
znajdujące się w katalogu ``var/logs/``, informują o wszystkich żądaniach
i mogą pomóc w naprawieniu napotkanych problemów.

Stosowanie interfejsu linii poleceń
-----------------------------------

Każda aplikacja posiada narzędzie interfejsu linii poleceń (``bin/console``)
który pomaga w utrzymywaniu aplikacji. Udostępnia on polecenia które zwiększają
wydajność prac programistycznych i administracyjnych poprzez automatyzację żmudnych
i powtarzających się zadań.

Uruchom go bez żadnych argumentów aby dowiedzieć się więcej o jego możliwościach:

.. code-block:: bash

    php bin/console

Opcja ``--help`` pomaga w poznaniu dostępnych poleceń:

.. code-block:: bash

    php bin/console debug:router --help

Podsumowanie
------------

Sądzę, że po przeczytaniu tego przewodnika uważny czytelnik powinieneś czuć
się komfortowo w pracy z Symfony. Wszystko w Symfony jest zaprojektowane tak by
sprostać oczekiwaniom programisty. Zatem, zmieniaj nazwy, przenoś katalogi zgodnie
z swoimi potrzebami.

To wszystko jeśli chodzi o szybkie wprowadzenie w tematyke Symfony. Musisz się
jeszcze dużo nauczyć o Symfony by stać się mistrzem, od testowania do wysyłania
poczty e-mail. Chcesz zapoznać sie z tymi tematami? Nie musisz specjalnie
szukać - przejdź do lektury podręcznika i wybierz tam dowolny temat.

.. _`Composer`:                http://getcomposer.org
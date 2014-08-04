.. index::
    single: trasowanie; DynamicRouter

Dynamic Router
==============

Implementacja routera jest skonfigurowana do ładowania tras z interfejsu
``RouteProviderInterface``. Interfejs ten może być łatwo zaimplementowany,
na przykład w Doctrine. Zapoznaj się z :ref:`odpowiednim rozdziałem dokumentacji
<bundle-routing-document>` w celu poznania szczegółów domyślnego dostawcy
`PHPCR-ODM`_ oraz z :ref:`further below <bundle-routing-entity>`, gdzie opisano
implementację opartą na Doctrine ORM. Jeśli to nie pasuje do Twoich potrzeb,
możesz :ref:`zbudować własnego dostawcę tras <bundle-routing-custom_provider>`.

Można skonfigurować ulepszacze tras, które będą decydować, który kontroler jest
używany do obsługiwania żądań, tak aby uniknąć sztywnego kodowania nazw kontrolerów
w dokumentach tras.

Konfiguracja
------------

Minimalna konfiguracja wymagana do załadowania dynamicznego routera jest taka,
aby określała zaplecze dostawcy tras dynamicznego routera w łańcuchu routerów.

.. note::

    Jeżeli w projekcie używa się też pakiet :doc:`CoreBundle <../core/introduction>`,
    to wystarczy skonfigurować utrzymywanie dynamicznego routera w ``cmf_core``
    i nie potrzeba powtarzać konfiguracji odrębnie dla dynamicznego routera.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        cmf_routing:
            chain:
                routers_by_id:
                    router.default: 200
                    cmf_routing.dynamic_router: 100
            dynamic:
                persistence:
                    phpcr:
                        enabled: true

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">
            <config xmlns="http://cmf.symfony.com/schema/dic/routing">
                <chain>
                    <router-by-id id="router.default">200</router-by-id>
                    <router-by-id id="cmf_routing.dynamic_router">100</router-by-id>
                </chain>
                <dynamic>
                    <persistence>
                        <phpcr enabled="true" />
                    </persistence>
                </dynamic>
            </config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('cmf_routing', array(
            'chain' => array(
                'routers_by_id' => array(
                    'router.default' => 200,
                    'cmf_routing.dynamic_router' => 100,
                ),
            ),
            'dynamic' => array(
                'persistence' => array(
                    'phpcr' => array(
                        'enabled' => true,
                    ),
                ),
            ),
        ));

Gdy nie ma konfiguracji lub opcja ``cmf_routing.dynamic.enabled`` jest ustawiona
na ``false``, usługi dynamicznego routera nie będą w ogóle ładowane, pozwalając
używać ``ChainRouter`` z własnymi routerami.

.. _bundle-routing-dynamic-match:

Proces dopasowywania
--------------------

Większość procesów dopasowywania jest opisanych w dokumentacji `komponentu CMF Routing`_.
Jedyną różnicą jest to, że pakiet umieści ``contentDocument`` w atrybutach żądania
zamiast w domyślnej trasie, aby uniknąć problemów podczas generowania ścieżki URL
dla bieżącego żądania.

Kontrolery mogą (a nawet powinny) deklarować parametr ``$contentDocument`` w swoich
metodach ``Action`, jeśli mają pracować z treścią przywoływaną w trasach.  Należy
pamiętać, że :doc:`../content/introduction` dostarcza domyślny kontroler renderujący
treść w okreśłonym szablonie do czego nie jest potrzebna jakakolwiek logika.

Własna akcja kontrolera może wyglądać następująco::

    namespace Acme\DemoBundle\Controller;

    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;

    /**
     * A custom controller to handle a content specified by a route.
     */
    class ContentController extends Controller
    {
        /**
         * @param object $contentDocument the name of this parameter is defined
         *      by the RoutingBundle. You can also expect any route parameters
         *      or $contentTemplate if you configured templates_by_class (see below).
         *
         * @return Response
         */
        public function demoAction($contentDocument)
        {
            // ... do things with $contentDocument and gather other information
            $customValue = 42;

            return $this->render('AcmeDemoBundle:Content:demo.html.twig', array(
                'cmfMainContent' => $contentDocument,
                'custom_parameter' => $customValue,
            ));
        }
    }

.. note::

    ``DynamicRouter`` odpala zdarzenie na początku procesu dopasowywania. Przeczytaj
    więcej na ten temat w :ref:`dokumentacji komponentu <components-routing-events>`.

.. _bundles-routing-dynamic_router-enhancer:

Konfigurowanie kontrolera dla trasy
-----------------------------------

Aby skonfigurować, który kontroler jest używany dla trasy, można skonfigurować
*ulepszcze tras*. Wiele z nich działa na trasach implementujących interfejs
``RouteObjectInterface``. Interfejs ten powiadamia, że trasa jest poinformowana
o swojej treści i zwraca ta treść w metodzie ``getRouteContent()``.
Proszę zapoznać się z `komponentem CMF Routing`_ , jeśli chce się uzyskać więcej
wiedzy o tym interfejsie.

Występujace ulepszacze, jeśli są skonfigurowane, to (w kolejności
pierwszeństwa):

#. (kontroler jawny): Jeśli ustawiona jest opcja ``_controller`` w
   ``getRouteDefaults()``, to żaden polepszacz nie zostanie zastąpiony
   w kontrolerze. Opcja ``_template`` będzie dalej wstawiona, jeśli jej
   wartość nie jest już skonfigurowana;
#. ``controllers_by_type``: wymaga dokumentu trasy, aby zwrócić wartość 'type'
   w ``getRouteDefaults()``. **priority: 60**;
#. ``controllers_by_class``: wymagają dokumentu klasy, aby zostać instancją
   ``RouteObjectInterface`` i zwracają obiekt dla ``getRouteContent()``.
   Dokument treści wykonuje ``instanceof`` sprawdzając nazwy klas w odwzorowaniu
   i czy pasują do używanego kontrolera. Instanceof jest używane zamiast
   bezpośredniego porównywania, czy działa z klasami proxy i innymi rozszerzonymi
   klasami. **priority: 50**;
#. ``templates_by_class``: wymaga dokumentu trasy by zostać instancją
   ``RouteObjectInterface`` i zwraca obiekt dla ``getRouteContent()``.
   W dokumencie treści wykonywane jest ``instanceof`` sprawdzając nazwy klas w
   odwzorowaniu i czy pasują do szablonu, który będzie ustawiony jako
   ``'_template'``.  **priority: 40** dla szablonu, ogólny kontroler jest
   ustawiony na **priority: 30**;
#. Jeśli w ``$defaults`` znajduje się ``_template``, ale nie jest jeszcze określony
   żaden kontroler (nie jest ustawiony w trasie, ani dopasowany w kontrolerze przez
   typ lub klasę), wybierany jest ogólny kontroler. **priority: 10**;
#. Wybierany jest domyślny kontroler. Kontroler ten może wykorzystywać domyślny
   szablon do renderowania treści, który może dodatkowo obsługiwać tą treść.
   Zapoznaj się również z :ref:`dokumentacją pakietu treści
   <bundles-content-introduction_default-template>`. **priority: -100**.

Zobacz do :ref:`informatora konfiguracji <reference-config-routing-dynamic>`, aby
poznać jak skonfigurować te ulepszacze.

Jeśli w aplikacji występuje ContentBundle, to kontrolerami ogólnym i domyślnym
jest ``ContentController`` dostarczany przez ten pakiet.

.. tip::

    Aby poznać więcej przykładów, proszę zapoznać się z `piaskownicą CMF`_ i specjalne
    konfiguratory testowe trasowania.

.. tip::

    Można zdefiniować własną klasę ``RouteEnhancer`` dla szczególnych przypadków
    użycia. Zobacz :ref:`bundle-routing-customize`. Wykorzystuj priorytet do
    wstawiania ulepszaczy w odpowiedniej kolejności.

.. _bundle-routing-document:

Integracja z Doctrine PHPCR-ODM
-------------------------------

Pakiet RoutingBundle dostarczany jest z dostawcą tras implementującym `PHPCR-ODM`_.
PHPCR jest dobrze dostosowany do charakteru drzewa danych. Jeśli stosuje się
`PHPCR-ODM`_ z dokumentem trasy, takim jak przewidziano, można po prostu pozostawić
domyślnie usługę dostawcy.

Domyślny dostawca ładuje trasę na ścieżce w żądaniu i wszystkie nadrzędne ścieżki
umożliwiając, aby niektóre segmenty ścieżki były parametrami. Jeśli potrzeba innego
sposobu ładownia tras lub na przykład nie chce się używać parametrów, można napisać
implementację własnego dostawcy, w celu optymalizacji, poprzez implementowanie
``RouteProviderInterface`` z własną usługą i określenie tej usługi jako
``cmf_routing.dynamic.route_provider_service_id``.

.. index:: PHPCR, ODM

Dokument trasy PHPCR-ODM
~~~~~~~~~~~~~~~~~~~~~~~~

Wszystkie klasy trasy muszą rozszerzać rdzenną klasę ``Route`` Symfony.
Domyślny dokument trasy PHPCR-ODM również implementuje ``RouteObjectInterface``
w celu odniesienia tras do treści. Odwzorowuje on wszystkie funkcjonalności
rdzennej trasy w celu przechowywania, tak więc można użyć ``setDefault``,
``setRequirement``, ``setOption`` i ``setHostnamePattern``. Dodatkowo podczas
tworzenia trasy można określić, czy ``.{_format}`` powinien być dołączony do
wzorca oraz skonfigurować wymagany ``_format`` w wymaganiach. Drugi argument
konstruktora pozwala kontrolować, czy do trasy powinien być dodany  poprzedzający
ukośnik, ponieważ może on nie znajdować się z nazwie PHPCR. Domyślnie poprzedzający
ukośnik nie jest dołączany. Obie opcje można również zmienić później w metodach setter.

Wszystkie trasy są zlokalizowane na ścieżce głównej konfiguracji, na przykład
``/cms/routes``. Nowa trasa może być utworzona w kodzie PHP w następujący sposób::

    use Symfony\Cmf\Bundle\RoutingBundle\Doctrine\Phpcr\Route;

    $route = new Route();
    $route->setParentDocument($dm->find(null, '/cms/routes'));
    $route->setName('projects');

    // set explicit controller (both service and Bundle:Name:action syntax work)
    $route->setDefault('_controller', 'sandbox_main.controller:specialAction');

Powyższy przykład powinien prawdopodobnie być wykonany jako konfiguracja trasy
w pliku konfiguracyjnym Symfony, chyba że końcowy użytkownik powinien zmieniać
ścieżkę URL lub kontroler.

Aby połączyć treść do tej trasy, trzeba po prostu ustawić to w dokumencie::

    use Symfony\Cmf\Bundle\ContentBundle\Doctrine\Phpcr\Content;

    // ...
    $content = new Content('my content'); // Content must be a mapped class
    $route->setRouteContent($content);

Spowoduje to, że trasowanie umieści dokument w parametrach żądania i jeśli kontroler
określa parametr o nazwie ``$contentDocument``, to będzie przekazany do dokumentu.

Można również użyć wzorców zmiennych dla ścieżek URL oraz zdefiniować wymagania
i wartości domyślne::

    // do not forget leading slash if you want /projects/{id} and not /projects{id}
    $route->setVariablePattern('/{id}');
    $route->setRequirement('id', '\d+');
    $route->setDefault('id', 1);

Określa to trasę pasująca do ścieżki URL ``/projects/<number>``, ale również do
``/projects``, bo jest ona wartością domyślną dla parametru ``id``. Pasuje to też
do ``/projects/7`` jak również do ``/projects`` ale nie do ``/projects/x-4``.
Dokument jest nadal przechowywany w ``/routes/projects``. To będzie działać,
ponieważ jak wspomniano powyżej, dostawca trasy będzie wyszukiwał dokumentów trasy
na wszystkich możliwych ścieżkach i wybierze pierwsza dopasowaną trasę. W naszym
przykładzie, jeśli istnieje pasujący dokument trasy na ścieżce ``/routes/projects/7``
(beż żadnych dodatkowych parametrów), to zostanie on wybrany. Jeśli nie, to trasowanie
sprawdzi czy ``/routes/projects`` ma pasujący wzorzec. Jeśli nie, to w celu dopasowania
wzorca sprawdzany jest szczytowy dokument na ścieżce ``/routes``.

Oczywiście można stosować wiele parametrów, jak w zwykłych trasach Symfony. Semantyka,
zasady wzorców, wartości domyślne i wymagania są dokładnie te same jak w rdzennych
trasach.

Kontroler może oczekiwać parametru ``$id`` jak też ``$contentDocument``, w zależności
od ustawiono treść w trasie. Treść może zostać wykorzystana do określenia sekcji
wstępu. Jeśli nie potrzebuje się treści, można po prostu nie ustawiać dokumentu trasy.

.. _component-route-generator-and-locales:

.. sidebar:: Ustawienia regionalne

    W Route można wykorzystać wartość domyślną ``_locale`` do stworzenia odrębnej
    trasy dla każdego języka, wszystko odwołujące się do tej samej instancji treści
    wielojęzycznej. ``ContentAwareGenerator`` respektuje ustawienia ``_locale``
    podczas generowania tras dla instancji treści. Podczas przetwarzania
    trasy, wartość ``_locale`` jest kojarzona z żądaniem i jest pobierana przez
    system ustawień regionalnych Symfony2.

    Upewnij się, że w konfiguracji ustawiona została prawidłowa wartość, tak aby
    pakiet mógł optymalnie obsługiwać języki. W :ref:`configuration reference
    <reference-config-routing-locales>` zestawiono kilka opcji umożliwiających
    dostosowanie zachowanie i wydajność.

.. note::

    W PHPCR-ODM, trasy nie powinny być dokumentami tłumaczonymi, ponieważ dokument
    Route reprezentuje jeden pojedynczy URL i obsługuje kilka tłumaczeń pod tym
    samym adresem URL, co nie jest zalecane.

    Jeśli potrzeba przetłumaczyć lokalizatory URL, trzeba wykonać część ``locale``
    nazwy trasy i utworzyć jedną trasę na język dla tej samej treści. Generator
    trasy będzie wybierać prawidłową trasę, jeśli jest dostępna.

Klasy Doctrine PHPCR-ODM Admin
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Jeśli pakiet SonataDoctrinePHPCRAdminBundle_ jest załadowany w kernelu aplikacji,
dokumenty trasy i przekierowania trasy mogą być administrowane w administratorze
Sonata. W celu poznania szczegółów, proszę przeczytać instrukcję
`konfiguracji administratora Sonata`_.

Domyślnie, opcja ``use_sonata_admin`` jest automatycznie ustawiana na podstawie
tego, czy dostępny jest pakiet SonataDoctrinePHPCRAdminBundle, ale można jawnie
to wyłączyć, nawet jeśli Sonata jest włączona lub jawnie włączyć, co spowoduje
błąd, gdy Sonata staje się niedostępna.

Administrator Sonata używa ``content_basepath`` drzewa treści do wyboru docelowej
trasy.

Główna ścieżka dodaje domyślne trasy do pierwszego wpisu w ``route_basepaths``,
ale można zastąpić to przez ``admin_basepath``, jeśli potrzebuje się innej ścieżki
bazowej.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        cmf_routing:
            dynamic:
                persistence:
                    phpcr:
                        # use true/false to force using / not using sonata admin
                        use_sonata_admin: auto

                        # used with Sonata Admin to manage content; defaults to %cmf_core.basepath%/content
                        content_basepath: ~

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://cmf.symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">

            <config xmlns="http://cmf.symfony.com/schema/dic/routing">
                <dynamic>
                    <persistence>
                        <!-- use-sonata-admin: use true/false to force using / not using sonata admin -->
                        <!-- content-basepath: used with Sonata Admin to manage content;
                                               defaults to %cmf_core.basepath%/content -->
                        <phpcr
                            use-sonata-admin="auto"
                            content-basepath="null"
                        />
                    </persistence>
                </dynamic>
            </config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('cmf_routing', array(
            'dynamic' => array(
                'persistence' => array(
                    'phpcr' => array(
                        // use true/false to force using / not using sonata admin
                        'use_sonata_admin' => 'auto',

                        // used with Sonata Admin to manage content; defaults to %cmf_core.basepath%/content
                        'content_basepath' => null,
                    ),
                ),
            ),
        ));

.. _bundle-routing-entity:

Integracja Doctrine ORM
-----------------------

Alternatywnie można użyć dostawcę `Doctrine ORM`_ przez określenie części
``persistence.orm`` konfiguracji. Działa to podobnie, ale jak nazwa wskazuje,
ładuje to encję ``Route`` dla bazy danych ORM.

.. caution::

    Trzeba zainstalować pakiet CoreBundle aby skorzystać z tej funkcjonalności,
    jeśli w aplikacji nie jest zainstalowany pakiet DoctrineBundle w wersji
    co najmniej 1.3.0.

.. _bundles-routing-dynamic-generator:

Generowanie ścieżek URL z DynamicRouter
---------------------------------------

Oprócz dopasowywania przychodzących żadań w celu ustawienia parametrów, router
jest również odpowiedzialny za generowanie adresów URL z trasy i jej parametrów.
``DynamicRouter`` powiększa `możliwości generowania ścieżek URL Symfony2`_.

.. tip::

    Niżej podane przykłady są przedstawione z funkcją ``path``, która generuje
    URL bez domeny, ale będą również działać z funkcja ``url``.

    Można też określić parametry dla generatora, które będą użyte, jeśli trasa
    zawiera dynamiczny wzorzec lub w inny sposób będzie dodawana jako łańcuch
    zapytania, podobnie jak w standardowym trasowaniu.

Można zastosować obiekt ``Route`` jako parametr nazwy metody generującej.
Będzie to wyglądać tak:

.. configuration-block::

    .. code-block:: html+jinja

        {# myRoute is an object of class Symfony\Component\Routing\Route #}
        <a href="{{ path(myRoute) }}>Read on</a>

    .. code-block:: html+php

        <!-- $myRoute is an object of class Symfony\Component\Routing\Route -->
        <a href="<?php echo $view['router']->generate($myRoute) ?>">
            Read on
        </a>

Podczas używania warstwy utrwalania PHPCR-ODM, ścieżka repozytorium dokumentu
trasy jest traktowana jako nazwa trasy. Ścieżkę repozytorium do generowania trasy
można określić w taki sposób:

.. configuration-block::

    .. code-block:: html+jinja

        {# Create a link to / on this server #}
        <a href="{{ path('/cms/routes') }}>Home</a>

    .. code-block:: html+php

        <!-- Create a link to / on this server -->
        <a href="<?php echo $view['router']->generate('/cms/routes') ?>">
            Home
        </a>

.. caution::

    Niebezpieczne jest sztywne kodowanie ścieżek do dokumentów PHPCR-ODM
    w szablonach. Użytkownik admin może edytować lub kasować je tak, że
    aplikacja zostanie uszkodzona. Jeśli trasa musi na pewno istnieć, to
    prawdopodobnie powinna być trasą skonfigurowana statycznie, ale nazwy
    tras mogą pochodzić na przykład z kodu.

``DynamicRouter`` wykorzystuje generator URL, który działa na ``RouteReferrersInterface``.
Oznacza to, że można również wygenerować trasę z dowolnego obiektu, który implementuje
interfejs i zapewnia dla niego trasę:

.. configuration-block::

    .. code-block:: html+jinja

        {# myContent implements RouteReferrersInterface #}
        <a href="{{ path(myContent) }}>Read on</a>

    .. code-block:: html+php

        <!-- $myContent implements RouteReferrersInterface -->
        <a href="<?php echo $view['router']->generate($myContent) ?>">
            Home
        </a>

.. tip::

    Jeśli istnieje kilka tras dla tej samej treści, to preferowane jest jedno z
    dopasowań językowych bieżącego żądania.

Dodatkowo generator rozumie również parametr ``content_id`` z pustą nazwą trasy
i próbuje znaleźć treść implementująca ``RouteReferrersInterface`` ze skonfigurowanego
repozytorium treści:

.. configuration-block::

    .. code-block:: html+jinja

        <a href="{{ path(null, {'content_id': '/cms/content/my-content'}) }}>
            Read on
        </a>

    .. code-block:: html+php

        <!-- $myContent implements RouteReferrersInterface -->
        <a href="<?php echo $view['router']->generate(null, array(
            'content_id' => '/cms/content/my-content',
        )) ?>">
            Home
        </a>

.. note::

    Dla uściślenia, to wystarczy aby treść implementowała ``RouteReferrersReadInterface``,
    jeśli pisanie tras nie jest pożądane. Dla uzyskania więcej informacji o nazewnictwie,
    proszę przeczytać :ref:`contributing-bundles-interface_naming`.

Dla poznania szczegółów implementacyjnych, proszę zapoznać się z rozdziałem
:ref:`component-routing-generator` w dokumentacji komponentu trasowania.

.. sidebar:: Zrzut tras

    ``RouterInterface`` definiuje metodę ``getRouteCollection`` uzyskującą wszystkie
    dostępne trasy w routerze. ``DynamicRouter`` jest w stanie dostarczyć taka
    kolekcję , jednak ta funkcjonalność jest domyślnie wyłączona, aby uniknąć
    zrzucania dużej ilości tras. Można ustawić ``cmf_routing.dynamic.route_collection_limit``
    na wartość większa niż 0, aby mieć router zwracający trasy do określonego limitu
    albo ``false``, aby wyłączyć ograniczenie i zwracać wszystko.

    Przy aktywowaniu tej opcji, narzędzia takie jak polecenie ``router:debug``
    lub `FOSJsRoutingBundle`_ będą dalej pokazywać trasy pochodzące z bazy danych.

    W przypadki `FOSJsRoutingBundle`_, jeśli użyje się nadchodząca wersje 2 tego
    pakietu, to można skonfigurować ``fos_js_routing.router`` na ``router.default``,
    aby uniknąć dołączania dynamicznych tras.

Obsługiwanie RedirectRoutes
---------------------------

Pakiet ten zawiera również kontroler obsługujacy dokumenty ``RedirectionRouteInterface``.
Trzeba skonfigurować ulepszacz trasy dla tego interfejsu:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        cmf_routing:
            dynamic:
                controllers_by_class:
                    Symfony\Cmf\Component\Routing\RedirectRouteInterface: cmf_routing.redirect_controller:redirectAction

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">
            <config xmlns="http://cmf.symfony.com/schema/dic/routing">
                <dynamic>
                    <controller-by-class class="Symfony\Cmf\Component\Routing\RedirectRouteInterface">
                        cmf_routing.redirect_controller:redirectAction
                    </controller-by-class>
                </dynamic>
            </config>
        </container>

    .. code-block:: php

        $container->loadFromExtension('cmf_routing', array(
            'dynamic' => array(
                'controllers_by_class' => array(
                    'Symfony\Cmf\Bundle\Routing\RedirectRouteInterface' => 'cmf_routing.redirect_controller:redirectAction',
                ),
            ),
        ));

Rozszerzenie RouteReferrersInterface Sonata Admin
-------------------------------------------------

Pakiet ten zawiera rozszerzenie dla edycji tras odnoszących się do treści, które
implementują ``RouteReferrersInterface``.

W celu włączenia tego rozszerzenia w klasach administratora, wystarczy zdefiniować
konfiguracje rozszerzenia w sekcji ``sonata_admin`` konfiguracji projektu:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        sonata_admin:
            # ...
            extensions:
                cmf_routing.admin_extension.route_referrers:
                    implements:
                        - Symfony\Cmf\Component\Routing\RouteReferrersInterface

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <config xmlns="http://sonata-project.org/schema/dic/admin">
            <!-- ... -->
            <extension id="cmf_routing.admin_extension.route_referrers">
                <implement>Symfony\Cmf\Component\Routing\RouteReferrersInterface</implement>
            </extension>
        </config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('sonata_admin', array(
            'extensions' => array(
                'cmf_routing.admin_extension.route_referrers' => array(
                    'implements' => array(
                        'Symfony\Cmf\Component\Routing\RouteReferrersInterface',
                    ),
                ),
            ),
        ));

W celu uzyskania więcej informacji, przeczytaj `dokumentację rozszerzenia Sonata Admin`_.

Dostosowywanie DynamicRouter
----------------------------

Ptzeczuyaj rozdział :doc:`Dostosowywanie dynamicznego routera <dynamic_customize>`.

.. _`piaskownicą CMF`: https://github.com/symfony-cmf/cmf-sandbox
.. _`komponentu CMF Routing`: https://github.com/symfony-cmf/Routing
.. _`Doctrine ORM`: http://www.doctrine-project.org/projects/orm.html
.. _`PHPCR-ODM`: http://www.doctrine-project.org/projects/phpcr-odm.html
.. _`dokumentację rozszerzenia Sonata Admin`: http://sonata-project.org/bundles/admin/master/doc/reference/extensions.html
.. _`możliwości generowania ścieżek URL Symfony2`: http://symfony.com/doc/current/book/routing.html#generating-urls
.. _`SonataDoctrinePHPCRAdminBundle`: http://sonata-project.org/bundles/doctrine-phpcr-admin/master/doc/index.html
.. _`konfiguracji zaplecza administracyjnego Sonata`: http://sonata-project.org/bundles/doctrine-phpcr-admin/master/doc/reference/configuration.html
.. _`FOSJsRoutingBundle`: https://github.com/FriendsOfSymfony/FOSJsRoutingBundle

.. highlight:: php
   :linenothreshold: 2

.. index::
    single: trasowanie; pierwsze kroki
    single: CmfRoutingBundle

Trasowanie
==========

Jest to wprowadzenie do koncepcji trasowania CMF. W celu poznania szczegółów
dokumentacyjnych prosimy zapoznać się z :doc:`Komponentem trasowania
<../components/routing/introduction>` oraz z :doc:`RoutingBundle
<../bundles/routing/introduction>`.

Koncepcja
---------

Dlaczego nowy mechanizm trasowania?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

CMS jest witryna wysoce dynamiczną, w której większość treści jest zarządzana
przez administratora a nie przez programistę. Ilość dostępnych stron może łatwo
osiągnąć liczbę kilku tysięcy, która zwykle trzeba jeszcze pomnożyć przez liczbę
tłumaczeń. Najlepsze praktyki dostępności i SEO, jak również preferencje użytkownika
decydują o tym, że ścieżki URL powinny być definiowane przez menadżerów treści.

Domyślny mechanizm trasowania Symfony2, z jego oparciem na pliku konfiguracyjnym,
nie jest najlepszym rozwiązaniem problemu. Nie zapewnia on dynamicznej obsługi,
definiowania tras przez użytkownika, ani skalowalności do dużej ilości tras.

Rozwiązanie
~~~~~~~~~~~

W celu rozwiązania tych problemów nowy system trasowania powinien być zaprojektowany
tak, aby uwzględniał typowe wymagania trasowania CMS:

* lokalizatory URL definiowane przez uzytkownika;
* wielowitrynowość;
* wielojęzyczność;
* drzewiasta struktura w celu łatwiejszego zarządzania;
* rozdzielone treść, menu i  trasa w celu zapewnienia elastyczności.

Komponent Symfony CMF Routing został stworzony zgodnie z tymi wymaganiami.

``ChainRouter``
---------------

W rdzeniu komponentu Routing Symfony CMF umieszczony jest ``ChainRouter``.
Używany jest on w miejsce domyślnego systemu trasowania Symfony2 i podobnie jak
router Symfony2, jest odpowiedzialny za określanie, który kontroler ma
obsługiwać poszczególne żądania.

``ChainRouter`` działa zgodnie z zestawem priorytetowych  strategii trasowania –
implementacji klas :class:`Symfony\\Component\\Routing\\RouterInterface` -
powszechnie nazywanych "routerami". Routery są odpowiedzialne za dopasowanie do
konkretnego kontrolera przychodzących żądań, co jest wykonywane w ten sposób, że
``ChainRouter`` iteruje po skonfigurowanych routerach według ich priorytetów:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        cmf_routing:
            chain:
                routers_by_id:
                    # włączenie DynamicRouter z niskim priorytetem
                    # w ten sposób nie dynamiczne trasy mają pierwszeństwo
                    # aby zapobiec niepotrzebnemu sprawdzaniu bazy danych
                    cmf_routing.dynamic_router: 20

                    # włączenie domyślnego routera symfony z wysokim priorytetem
                    router.default: 100

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>

        <container xmlns="http://cmf.symfony.com/schema/dic/services">

            <config xmlns="http://cmf.symfony.com/schema/dic/routing">
                <chain>
                    <!-- włączenie DynamicRouter z niskim priorytetem
                         w ten sposób nie dynamiczne trasy mają pierwszeństwo
                         aby zapobiec niepotrzebnemu sprawdzaniu bazy danych
                    -->
                    <routers-by-id
                        id="cmf_routing.dynamic_router">
                        20
                    </routers-by-id>

                    <!-- włączenie domyślnego routera symfony z wysokim priorytetem -->
                    <routers-by-id
                        id="router.default">
                        100
                    </routers-by-id>
                </chain>
            </config>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('cmf_routing', array(
            'chain' => array(
                'routers_by_id' => array(
                    // włączenie DynamicRouter z niskim priorytetem
                    // w ten sposób nie dynamiczne trasy mają pierwszeństwo
                    // aby zapobiec niepotrzebnemu sprawdzaniu bazy danych
                    'cmf_routing.dynamic_router' => 20,

                    // włączenie domyślnego routera symfony z wysokim priorytetem
                    'router.default' => 100,
                ),
            ),
        ));

Można również załadować routery wykorzystując tagowaną usługę, przez użycie znacznika
``router`` i ewentualnie opcji ``priority``. Im wyższy priorytet, to tym wcześniej
router zostanie poproszony o dopasowanie trasy. Jeśli nie określi się priorytetu,
to ten router będzie ostatni. Jeśli jest kilka routerów o tym samym priorytecie,
ich kolejność jest nieokreślona.  Tagowana usługa wygląda podobnie do tego:

.. configuration-block::

    .. code-block:: yaml
       "linenos:

        services:
            my_namespace.my_router:
                class: "%my_namespace.my_router_class%"
                tags:
                    - { name: router, priority: 300 }

    .. code-block:: xml
       :linenos:

        <service id="my_namespace.my_router" class="%my_namespace.my_router_class%">
            <tag name="router" priority="300" />
        </service>

    .. code-block:: php
       :linenos:

        $container
            ->register('my_namespace.my_router', '%my_namespace.my_router_class%')
            ->addTag('router', array('priority' => 300))
        ;

System trasowania Symfony CMF dodaje nowy ``DynamicRouter``, który uzupełnia
domyślny ``Router`` Symfony2.

Domyślny router Symfony2
------------------------

Pomimo że domyślny mechanizm trasowania Symfony2 został całkowicie
zastąpiony, trasowanie Symfony CMF umożliwia wykorzystanie trasowania Symfony2.
W rzeczywistości, standardowy system trasowania Symfony2 jest domyślnie włączony,
więc można z niego korzystać używając tras określanych w pliku konfiguracyjnym lub
określanych przez inne pakiety.

.. _start-routing-dynamic-router:

DynamicRouter
-------------

Router ten może dynamicznie ładować instancje ``Route`` z dynamicznego źródła
poprzez tak zwanego *dostawcę*. W rzeczywistości tylko ładuje on trasy kandydujące.
Rzeczywisty proces dopasowania jest dokładnie taki sam jak w standardowym mechanizmie
trasowania Symfony2. Jednak ``DynamicRouter`` dodatkowo jest w stanie określić który
kontroler i szablon ma być użyty na podstawie dopasowanego obiektu ``Route``.

``DynamicRouter`` domyślnie jest wyłączony. Do aktywowania, wystarczy dodać
następujący zapis do pliku konfiguracyjnego:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        cmf_routing:
            dynamic:
                enabled: true

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>

        <container xmlns="http://cmf.symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">

            <config xmlns="http://cmf.symfony.com/schema/dic/routing">
                <dynamic enabled="true" />
            </config>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('cmf_routing', array(
            'dynamic' => array(
                'enabled' => true,
            ),
        ));

Jest to minimalna konfiguracja wymagana dla załadowania ``DynamicRouter`` jako usługi,
dzięki czemu jest on w stanie wykonywać trasowanie. Właściwie, gdy przegląda się
domyślne strony, które dostarczone zostały Symfony CMF SE, to dzieje się to z
wykorzystaniem ``DynamicRouter``, który dopasowuje żądania do kontrolerów i szablonów.

.. _start-routing-getting-route-object:

Pobieranie obiektu Route
~~~~~~~~~~~~~~~~~~~~~~~~

Używany dostawca może być skonfigurowany tak, aby najlepiej spełniać każdą potrzebę
implementacji. W ramach tego pakietu dostarczana jest implementacja dla
`Doctrine ORM`_ i `PHPCR-ODM`_. Ponadto można łatwo stworzyć własną, implementując
``RouteProviderInterface``. Dostawcy są odpowiedzialni za pobieranie uporządkowanych
podzbiorów kandydujących tras, które mogą pasować do żądania. Na przykład, domyślny
dostawca `PHPCR-ODM`_ ładuje obiekty ``Route`` ze ścieżką z żądania i ze wszystkimi
ścieżkami nadrzędnymi aby dopuścić niektóre segmenty ścieżki będące parametrami.

W celu uzyskania bardziej szczegółowych informacji na temat tej implementacji
i jak można to dostosować lub rozszerzyć, proszę zapoznać się z
:doc:`../bundles/routing/introduction`.

``DynamicRouter`` jest w stanie dopasować przychodzące żądanie do obiektu Route
od podstawowego dostawcy. Szczegółowa informację o tym jak ten proces dopasowania
jest realizowany można znaleźć w
:doc:`dokumentacji komponentu <../components/routing/dynamic>`.

.. note::

    W celu posiadania dostawcy znajdującego trasy, potrzeba również dostarczyć dane z magazynu
    danych. W PHPCR-ODM jest to robione albo przez interfejs administracyjny albo
    przez konfiguratory treści (ang. fixtures).

    Jednak zanim będzie można wyjaśnić jak to zrobić, trzeba zrozumieć jak działa
    ``DynamicRouter``. Przykład podany będzie :ref:`dalej w tym artykule
    <start-routing-document>`.

.. _start-routing-getting-controller-template:

Pobieranie kontrolera i szablonu
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Trasa musi określać, który kontroler ma obsługiwać konkretne żądanie.
Do określenia tego ``DynamicRouter`` używa jednej z kilku możliwych metod
(w kolejności występowania):

* jawnie: dokument ``Route`` może sam jawnie zadeklarować docelowy kontroler,
  jeśli z ``getDefault('_controller')`` jest zwracany jeden kontroler;
* przez typ: dokument ``Route`` zwraca wartość z ``getDefault('type')``,
  która jest następnie dopasowywana do dostarczonej konfiguracji z config.yml;
* przez klasę: wymaga dokumentu ``Route`` do zaimplementowania ``RouteObjectInterface``
  i zwraca obiekt dla ``getContent()``. Zwracany ty klasy jest następnie ponownie
  porównywany z dostarczona konfiguracja z config.yml;
* domyślnie: jeśli skonfigurowano, zastosowany zostanie domyślny kontroler.

Oprócz tego ``DynamicRouter`` jest również zdolny do dynamicznego określania, który
szablon ma zostać użyty, w podobny sposób do stosowanego przy określaniu kontrolera
(w kolejności występowania):

* jawnie: przechowywany dokument ``Route`` może sam jawnie określić docelowy szablon
  przez zwrócenie nazwy szablonu w ``getDefault('_template')``;
* przez klasę: wymaga instancji ``Route`` do zaimplementowania ``RouteObjectInterface``
  i zwraca obiekt dla ``getContent()``. Zwrócony typ klasy jest następnie ponownie
  porównywany z dostarczona konfiguracja z config.yml.

Oto przykład skonfigurowania powyżej omówionych opcji:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        cmf_routing:
            dynamic:
                generic_controller: cmf_content.controller:indexAction
                controllers_by_type:
                    editable_static: sandbox_main.controller:indexAction
                controllers_by_class:
                    Symfony\Cmf\Bundle\ContentBundle\Document\StaticContent: cmf_content.controller::indexAction
                templates_by_class:
                    Symfony\Cmf\Bundle\ContentBundle\Document\StaticContent: CmfContentBundle:StaticContent:index.html.twig

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>

        <container xmlns="http://cmf.symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">

            <config xmlns="http://cmf.symfony.com/schema/dic/routing">
                <dynamic generic-controller="cmf_content.controller:indexAction">
                    <controllers-by-type type="editable_static">
                        sandbox_main.controller:indexAction
                    </controllers-by-type>

                    <controllers-by-class
                        class="Symfony\Cmf\Bundle\ContentBundle\Document\StaticContent"
                    >
                        cmf_content.controller::indexAction
                    </controllers-by-class>

                    <templates-by-class class="Symfony\Cmf\Bundle\ContentBundle\Document\StaticContent"
                    >
                        CmfContentBundle:StaticContent:index.html.twig
                    </templates-by-class>
                </dynamic>
            </config>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('cmf_routing', array(
            'dynamic' => array(
                'generic_controller' => 'cmf_content.controller:indexAction',
                'controllers_by_type' => array(
                    'editable_static' => 'sandbox_main.controller:indexAction',
                ),
                'controllers_by_class' => array(
                    'Symfony\Cmf\Bundle\ContentBundle\Document\StaticContent' => 'cmf_content.controller::indexAction',
                ),
                'templates_by_class' => array(
                    'Symfony\Cmf\Bundle\ContentBundle\Document\StaticContent' => 'CmfContentBundle:StaticContent:index.html.twig',
                ),
            ),
        ));

Proszę zauważyć, że już nie występuje ``enabled: true``. Jest to wymagane tylko
wtedy, gdy nie są dostarczone jakiekolwiek parametry konfiguracji. Router zostaje
włączony automatycznie zaraz po dodaniu jakiejś konfiguracji do wpisu ``dynamic``.

.. note::

    Wewnętrznie, komponent trasowania odwzorowuje te opcje konfiguracyjne na kilka
    instancji ``RouteEnhancerInterface``. Rzeczywisty zakres tych ulepszeń jest
    znacznie szerszy a informacje o tym można znaleźć w rozdziale dokumentacji
    :ref:`routing enhancers <component-routing-enhancers>`.

.. _start-routing-linking-a-route-with-a-model-instance:

Linkowanie trasy z instancją modelu
-----------------------------------

W zależności od logiki aplikacji żądany adres URL może mieć związek z instancją
modelu z bazy danych. Trasy te mogą implementować ``RouteObjectInterface``
i opcjonalnie zwracać instancje modelu, która będzie automatycznie przekazywana
do kontrolera jako parametr metody ``contentDocument``.

Proszę mieć na uwadze, że obiekt Route może implementować wyżej omawiany interfejs,
ale nadal nie będzie zwracał jakiejkolwiek instancji modelu. W takim przypadku nie
zostanie dostarczony żaden powiązany obiekt.

Ponadto, trasy implementujące ten interfejs mogą mieć własną nazwę trasy,
zamiast domyślnej nazwy zgodnej z rdzeniem Symfony i mogą zawierać dowolne znaki.
Pozwala to, przykładowo, ustawić ścieżkę jako nazwę trasy.

Przekierowania
--------------

Można budować przekierowania poprzez implementację ``RedirectRouteInterface``.
Jeśli wykorzystuje się domyślnego dostawcę tras ``PHPCR-ODM``, gotowa do użycia
implementacja jest dostarczona w dokumencie ``RedirectRoute``. Może ona przekierowywać
albo do bezwzględnego adresu URI, do nazwanej trasy, która może być wygenerowana
przez dowolny router w łańcuchu albo inny obiekt Route znany dostawcy tras.
Rzeczywiste przekierowanie jest obsługiwane przez określony kontroler, który może
być skonfigurowany tak:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        cmf_routing:
            dynamic:
                controllers_by_class:
                    Symfony\Cmf\Component\Routing\RedirectRouteInterface: cmf_routing.redirect_controller:redirectAction

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>

        <container xmlns="http://cmf.symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">

            <config xmlns="http://cmf.symfony.com/schema/dic/routing">
                <dynamic>
                    <controllers-by-class
                        class="Symfony\Cmf\Component\Routing\RedirectRouteInterface">
                        cmf_routing.redirect_controller:redirectAction
                    </controllers-by-class>
                </dynamic>
            </config>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('cmf_routing', array(
            'dynamic' => array(
                'controllers_by_class' => array(
                    'Symfony\Cmf\Component\Routing\RedirectRouteInterface' => 'cmf_routing.redirect_controller:redirectAction',
                ),
            ),
        ));

.. note::
   
   Rzeczywista konfiguracja dla tego związku istnieje jako usługa, a nie jako
   część pliku ``config.yml``. Jak powiedziano to poprzedni, można stosować każde
   z tych rozwiązań.

Generowanie ścieżek URL
-----------------------

Komponent trasowania Symfony CMF korzysta z domyślnych komponentów Symfony2 do
obsługi generowania tras. Tak więc można używać domyślnych metod do generowania
ścieżek URL z kilkoma dodatkowymi możliwościami:

* przekazując implementację ``RouteObjectInterface`` albo ``RouteReferrersInterface``
  jako parametr ``name``;
* alternatywnie, dostarczając implementację ``ContentRepositoryInterface``
  i identyfikator instancji modelu jako parametr ``content_id``.

Zobacz :ref:`bundles-routing-dynamic-generator` w celu zapoznania się z przykładami
obydwu przypadków.

Generowanie tras obsługuje również ustawienia regionalne, zobacz
":ref:`ContentAwareGenerator and Locales <component-route-generator-and-locales>`".

.. _start-routing-document:

Dokument trasy PHPCR-ODM
------------------------

Jak już wspomniano, można użyć dowolnego dostawcy tras. Przykład w tym rozdziale
ma zastosowanie, jeśli używa się domyślnego dostawcy tras PHPCR-ODM
(``Symfony\Cmf\Bundle\RoutingBundle\Doctrine\Phpcr\RouteProvider``).

Wszystkie trasy są umieszczone na ścieżce skonfigurowanej w konfiguracji aplikacji
``cmf_routing.persistence.phpcr.route_basepath``. Domyślnie ścieżka ta, to ``/cms/routes``.
Każda nowa trasa może być utworzona w kodzie PHP w następujący sposób::

    // src/Acme/MainBundle/DataFixtures/PHPCR/LoadRoutingData.php
    namespace Acme\DemoBundle\DataFixtures\PHPCR;

    use Doctrine\ODM\PHPCR\DocumentManager;
    use Symfony\Cmf\Bundle\RoutingBundle\Doctrine\Phpcr\Route;
    use Symfony\Cmf\Bundle\ContentBundle\Doctrine\Phpcr\StaticContent;

    class LoadRoutingData implements FixtureInterface
    {
        /**
         * @param DocumentManager $dm
         */
        public function load(ObjectManager $dm)
        {
            $route = new Route();
            $route->setParentDocument($dm->find(null, '/cms/routes'));
            $route->setName('projects');

            // link a content to the route
            $content = new StaticContent();
            $content->setParentDocument($dm->find(null, '/cms/content'));
            $content->setName('my-content');
            $dm->persist($content);
            $route->setRouteContent($content);

            // now define an id parameter; do not forget the leading slash if you
            // want /projects/{id} and not /projects{id}
            $route->setVariablePattern('/{id}');
            $route->setRequirement('id', '\d+');
            $route->setDefault('id', 1);

            $dm->persist($route),
            $dm->flush();
        }
    }

Daje to dokument pasujący do ścieżki URL ``/projects/<number>``, ale również do
``/projects``, jako że jest to wartość domyślna parametru parameteru id.

Ponieważ zdefiniowaliśmy parametr trasy ``{id}``, kontroler może oczekiwać parametru
``$id``. Dodatkowo, ponieważ wywołaliśmy w trasie setRouteContent, kontroler może
oczekiwać parametru ``$contentDocument``.
Treść może być użyta do określenia sekcji wstępu (intro), która jest taka sama
w każdym projekcie lub innych wspólnych danych. Jeśli nie potrzeba treści, można
po prostu nie ustawiać tego dokumentu.

Więcej szczgółów mozna znaleźć w 
:ref:`w dokumentacji RoutingBundle <bundle-routing-document>`.

Uwagi końcowe
-------------

W celu uzyskania więcej informacji o komponencie Routing Symfony CMF, proszę przeczytać:

* :doc:`../components/routing/introduction` dla poznania więcej rzeczywistych implementacji funkcjonalnych,
* :doc:`../bundles/routing/introduction` dla zapoznania się z integracją pakietów Symfony2 z pakietem Routing;
* stronę komponentu `Routing`_ Symfony2;
* :doc:`../book/handling_multilang` w celu poznania kilku uwag o trasowaniu wielojęzycznym;

.. _`Doctrine ORM`: http://www.doctrine-project.org/projects/orm.html
.. _`PHPCR-ODM`: http://www.doctrine-project.org/projects/phpcr-odm.html
.. _`Routing`: http://symfony.com/doc/current/components/routing/introduction.html

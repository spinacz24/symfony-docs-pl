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

    Aby poznać więcej przykładów, proszę zobaczyć `CMF sandbox`_ i specjalne
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

Oczywiście można mieć wiele parametrów, jak w zwykłych trasach Symfony. Semantyka,
zasady wzorców, wartości domyślne i wymagania są dokładnie te same jak w rdzennych
trasach.

Your controller can expect the ``$id`` parameter as well as the ``$contentDocument``
as you set a content on the route. The content could be used to define an intro
section that is the same for each project or other shared data. If you don't
need content, you can just not set it in the route document.

.. _component-route-generator-and-locales:

.. sidebar:: Locales

    You can use the ``_locale`` default value in a Route to create one Route
    per locale, all referencing the same multilingual content instance. The
    ``ContentAwareGenerator`` respects the ``_locale`` when generating routes
    from content instances. When resolving the route, the ``_locale`` gets
    into the request and is picked up by the Symfony2 locale system.

    Make sure you configure the valid locales in the configuration so that the
    bundle can optimally handle locales. The
    :ref:`configuration reference <reference-config-routing-locales>` lists
    some options to tweak behaviour and performance.

.. note::

    Under PHPCR-ODM, Routes should not be translatable documents, as one
    Route document represents one single url, and serving several translations
    under the same url is not recommended.

    If you need translated URLs, make the ``locale`` part of the route name and
    create one route per language for the same content. The route generator will
    pick the correct route if available.

Sonata Doctrine PHPCR-ODM Admin classes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If the SonataDoctrinePHPCRAdminBundle_ is loaded in the application kernel,
route and redirect route documents can be administrated in sonata admin. For
instructions on how to configure Sonata, see `configuring sonata admin`_.

By default, ``use_sonata_admin`` is automatically set based on whether
SonataDoctrinePHPCRAdminBundle is available, but you can explicitly
disable it to not have it even if sonata is enabled, or explicitly enable to
get an error if Sonata becomes unavailable.

Sonata admin is using the ``content_basepath`` to show the tree of content to
select the route target.

The root path to add Routes defaults to the first entry in ``route_basepaths``,
but you can overwrite this with the ``admin_basepath`` if you need a different
base path.

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

Doctrine ORM integration
------------------------

Alternatively, you can use the `Doctrine ORM`_ provider by specifying the
``persistence.orm`` part of the configuration. It does a similar job but, as
the name indicates, loads ``Route`` entities from an ORM database.

.. caution::

    You must install the CoreBundle to use this feature if your application
    does not have at least DoctrineBundle 1.3.0.

.. _bundles-routing-dynamic-generator:

URL generation with the DynamicRouter
-------------------------------------

Apart from matching an incoming request to a set of parameters, a Router is
also responsible for generating an URL from a route and its parameters. The
``DynamicRouter`` adds more power to the
`URL generating capabilities of Symfony2`_.

.. tip::

    All Twig examples below are given with the ``path`` function that generates
    the URL without domain, but will work with the ``url`` function as well.

    Also, you can specify parameters to the generator, which will be used if
    the route contains a dynamic pattern or otherwise will be appended as
    query string, just like with the standard routing.

You can use a ``Route`` object as the name parameter of the generating method.
This will look as follows:

.. configuration-block::

    .. code-block:: html+jinja

        {# myRoute is an object of class Symfony\Component\Routing\Route #}
        <a href="{{ path(myRoute) }}>Read on</a>

    .. code-block:: html+php

        <!-- $myRoute is an object of class Symfony\Component\Routing\Route -->
        <a href="<?php echo $view['router']->generate($myRoute) ?>">
            Read on
        </a>

When using the PHPCR-ODM persistence layer, the repository path of the route
document is considered the route name. Thus you can specify a repository path
to generate a route:

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

    It is dangerous to hardcode paths to PHPCR-ODM documents into your
    templates. An admin user could edit or delete them in a way that your
    application breaks. If the route must exist for sure, it probably
    should be a statically configured route. But route names could come from
    code for example.

The ``DynamicRouter`` uses a URL generator that operates on the
``RouteReferrersInterface``. This means you can also generate a route from any
object that implements this interface and provides a route for it:

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

    If there are several routes for the same content, the one with the locale
    matching the current request locale is preferred

Additionally, the generator also understands the ``content_id`` parameter with
an empty route name and tries to find a content implementing the
``RouteReferrersInterface`` from the configured content repository:

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

    To be precise, it is enough for the content to implement the
    ``RouteReferrersReadInterface`` if writing the routes is not desired. See
    :ref:`contributing-bundles-interface_naming` for more on the naming scheme.)

For the implementation details, please refer to the
:ref:`component-routing-generator` section in the the cmf routing component
documentation.

.. sidebar:: Dumping Routes

    The ``RouterInterface`` defines the method ``getRouteCollection`` to get
    all routes available in a router. The ``DynamicRouter`` is able to provide
    such a collection, however this feature is disabled by default to avoid
    dumping large numbers of routes. You can set
    ``cmf_routing.dynamic.route_collection_limit`` to a value bigger than 0
    to have the router return routes up to the limit or ``false`` to disable
    limits and return all routes.

    With this option activated, tools like the ``router:debug`` command or the
    `FOSJsRoutingBundle`_ will also show the routes coming from the database.

    For the case of `FOSJsRoutingBundle`_, if you use the upcoming version 2 of
    the bundle, you can configure ``fos_js_routing.router`` to
    ``router.default`` to avoid the dynamic routes being included.

Handling RedirectRoutes
-----------------------

This bundle also provides a controller to handle ``RedirectionRouteInterface``
documents. You need to configure the route enhancer for this interface:

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

RouteReferrersInterface Sonata Admin Extension
----------------------------------------------

This bundle provides an extension to edit referring routes for content that
implements the ``RouteReferrersInterface``.

To enable the extensions in your admin classes, simply define the extension
configuration in the ``sonata_admin`` section of your project configuration:

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

See the `Sonata Admin extension documentation`_ for more information.

Customize the DynamicRouter
---------------------------

Read on in the chapter :doc:`customizing the dynamic router <dynamic_customize>`.

.. _`CMF sandbox`: https://github.com/symfony-cmf/cmf-sandbox
.. _`CMF Routing component`: https://github.com/symfony-cmf/Routing
.. _`Doctrine ORM`: http://www.doctrine-project.org/projects/orm.html
.. _`PHPCR-ODM`: http://www.doctrine-project.org/projects/phpcr-odm.html
.. _`Sonata Admin extension documentation`: http://sonata-project.org/bundles/admin/master/doc/reference/extensions.html
.. _`URL generating capabilities of Symfony2`: http://symfony.com/doc/current/book/routing.html#generating-urls
.. _SonataDoctrinePHPCRAdminBundle: http://sonata-project.org/bundles/doctrine-phpcr-admin/master/doc/index.html
.. _`configuring sonata admin`: http://sonata-project.org/bundles/doctrine-phpcr-admin/master/doc/reference/configuration.html
.. _`FOSJsRoutingBundle`: https://github.com/FriendsOfSymfony/FOSJsRoutingBundle

.. highlight:: php
   :linenothreshold: 2

.. index::
    single: router; wprowadzenie

Router
======

Witamy w trzeciej części „Krótkiego kursu”. Ta część traktuje o routerze -
podstawie Symfony CMF.

Podstawa CMF
------------

Jak już to zostało powiedziane, router jest podstawą. Dla zrozumienia tego trzeba
mieć pojęcie o tym co CMS próbuje robić. W zwykłej aplikacji Symfony, trasa
odnosi się do kontrolera, który obsługuje określone encje. Inna trasa odnosi
się do innego kontrolera, który obsługuje inną encje. W ten sposób trasa jest
powiązana z kontrolerem. W rzeczywistości, wykorzystując rdzeń Symfony programista
jest ograniczony przez ten mechanizm.

Lecz jeśli spojrzy się na istotę CMS, to system taki musi obsługiwać tylko jeden
typ encji - treść. Więc większość tras nie musi być już związana kontrolerem,
jako że potrzebny jest tylko jeden kontroler. Obiekt Route jest związany z określonym
obiektem Content, który z kolei odwołuje się do określonego szablonu i kontrolera.

Inne części CMF są również powiązane z routerem. Oto dwa przykłady: menu jest
tworzone poprzez wygenerowanie określonych tras przy użyciu routera a bloki są
wyświetlane dla określonych tras (ponieważ są związane z szablonem).

Ładowanie tras z drzewa PHPCR
-----------------------------

W pierwszym rozdziale już dowiedzieliśmy się, że trasy są ładowane z bazy danych
przy użyciu specjalnego obiektu ``DynamicRouter``. W ten sposób nie wszystkie trasy
muszą zostać załadowane dla poszczególnego żądania.

Dopasowanie tras z PHPCR jest naprawdę proste. Jeśli pamiętasz poprzedni rozdział,
wiesz, że można pobrać stronę ``quick_tour`` z PHPCR stosując ścieżkę
``/cms/simple/quick_tour``. Adresem URL do pobrania strony jest ``quick_tour``.
Kilka innych przykładów:

.. code-block:: text
   :linenos:

    /cms
        /simple
            /about       # /about Route
            /contact     # /contact Route
                /team    # /contact/team Route
                /docs    # /docs Route

Jedyna rzeczą, którą musi zrobić router, to poprzedzić trasy określonym przedrostkiem
ścieżki i załadować dokument. W przypadku SimpleCmsBundle, wszystkie ścieżki są
poprzedzane przedrostkiem ``/cms/simple``.

Ja widać, trasa taka jak ``/contact/team``, która składa się z 2 "jednostek adresowych",
ma 2 dokumenty w drzewie PHPCR tree: ``contact`` i ``team``.

Łączenie wielu tras
-------------------

Może być potrzebnych kilka przedrostków lub kilka tras. Na przykład, można chcieć
używać zarówno ``DynamicRouter`` dla tras stron, ale także statycznego trasowania
z Symfony dla niestandardowej logiki. W celu umożliwienia takiego rozwiazania CMF
dostarcza ``ChainRouter``.
Router ten łączy trasy z wielu routerów i zatrzymuje się gdy trasa zostanie dopasowana.

Domyślnie``ChainRouter`` zastępuje router Symfony i tylko ma rdzenny router w swoim
łańcuchu. Można dodać do łańcucha więcej routerów w konfiguracji lub przez oznaczenie
usług routera. Na przykład, router używany przez SimpleCmsBundle jest usługą
zarejestrowaną przez pakiet i oznaczoną jako ``cmf_routing.router``.

Tworzenie nowej trasy
---------------------

Teraz już znasz podstawy trasowania, więc możesz samodzielnie dodać nową trasę do
drzewa. Skonfigurujmy nowy łańcuch tras w pliku konfiguracyjnym, tak aby można było
umieścić nowe trasy w ``/cms/routes``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml

        # ...
        cmf_routing:
            chain:
                routers_by_id:
                    # standardowy DynamicRouter
                    cmf_routing.dynamic_router: 200

                    # rdzenny router Symfony
                    router.default: 100
            dynamic:
                persistence:
                    phpcr:
                        route_basepath: /cms/routes
                templates_by_class:
                     Symfony\Cmf\Bundle\SimpleCmsBundle\Doctrine\Phpcr\Page: AcmeDemoBundle:Page:index.html.twig

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">
            <!-- ... -->

            <config xmlns="http://cmf.symfony.com/schema/dic/routing">
                <chain>
                    <!-- standardowy DynamicRouter -->
                    <router-by-id id="cmf_routing.dynamic_router">200</router-by-id>

                    <!-- rdzennycrouter Symfony -->
                    <router-by-id id="router.default">100</router-by-id>
                </chain>

                <dynamic>
                    <persistence>
                        <phpcr route-basepath="/cms/routes" />
                    </persistence>
                    <tempaltes_by_class>
                        <Symfony\Cmf\Bundle\SimpleCmsBundle\Doctrine\Phpcr\Page = "AcmeDemoBundle:Page:index.html.twig"  />
                    </tempaltes_by_class>
                </dynamic>
            </config>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('cmf_routing', array(
            'chain' => array(
                'routers_by_id' => array(
                    // standardowy DynamicRouter
                    'cmf_routing.dynamic_router' => 200,

                    // rdzenny router Symfony
                    'router.default' => 100,
                ),
            ),
            'dynamic' => array(
                'persistence' => array(
                    'phpcr' => array(
                        'route_basepath' => '/cms/routes',
                    ),
                ),
                'tempaltes_by_class' => array(
                     'Symfony\Cmf\Bundle\SimpleCmsBundle\Doctrine\Phpcr\Page' => `AcmeDemoBundle:Page:index.html.twig`,
                ),
            ),
        ));

Dodajmy nowy obiekt ``Route`` do drzewa używając Doctrine::

    // src/Acme/DemoBundle/DataFixtures/PHPCR/LoadRoutingData.php
    namespace Acme\DemoBundle\DataFixtures\PHPCR;

    use Doctrine\Common\Persistence\ObjectManager;
    use Doctrine\Common\DataFixtures\FixtureInterface;

    use Symfony\Cmf\Bundle\RoutingBundle\Doctrine\Phpcr\Route;

    class LoadRoutingData implements FixtureInterface
    {
        public function load(ObjectManager $documentManager)
        {
            $routesRoot = $documentManager->find(null, '/cms/routes');

            $route = new Route();
            // ustawienie $routesBase jako rodzica a 'new-route' jako nazwy węzła,
            // jest to równoważne:
            // $route->setName('new-route');
            // $route->setParentDocument($routesRoot);
            $route->setPosition($routesRoot, 'new-route');

            $page = $documentManager->find(null, '/cms/routes/quick_tour');
            $route->setContent($page);

            $documentManager->persist($route); // wstawienie $route do kolejki
            $documentManager->flush(); // zapisanie tego
        }
    }

Utworzy to nowy węzeł o nazwie ``/cms/routes/new-route``, który będzie wyswietlał
naszą stronę ``quick_tour``, gdy przejdzie się do ``/new-route``.

.. tip::

    Gdy będzie się to robić w prawdziwej aplikacji, to zamiast tego można użyć RedirectRoute.

.. TODO napisz coś o templates_by_class, itd.

Wnioski końcowe
---------------

Można powiedzieć, że po dotarciu do końca tego artykuły czytelnik jest zaznajomiony
z podstawami Symfony CMF. Po pierwsze, poznaliśmy przepływ żądania i krótko omówiliśmy
każdy krok w tym procesie. Następnie przedstawiona została domyślna warstwa magazynowania
i system trasowania.

System trasowania jest tworzony z udziałem niektórych programistów Drupal8.
W rzeczywistości, Drupal 8 wykorzystuje komponent Routing Symfony CMF. Symfony CMF
również stosuje kilka zewnętrznych pakietów i integruje je z PHPCR.
W :doc:`następnym rozdziale <the_third_party_bundles>` dowiesz się więcej o tych
pakietach i innych projektach wspomagających Symfony CMF.

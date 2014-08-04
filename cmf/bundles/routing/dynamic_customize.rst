.. _bundle-routing-customize:

Dostosowywanie dynamicznego routera
===================================

``DynamicRouter`` został zbudowany po to, aby był dostosowywany. Wstrzykiwane mogą
być zarówno dopasowania tras jak i usługi generowania URL. Dostarczony dopasowywacz
tras (*ang. route matcher*) oraz klasy generatora URL są budowane tak, by mogły być
dostosowywane w przyszłości.

Rozdział ten opisuje najczęściej wykonywane dostosowania. Jeśli chcesz pojść dalej,
to przeczytaj :doc:`dokumentacje komponentu <../../components/routing/introduction>`
i przeglądnij kod źródłowy.

Jeśli ``DynamicRouter`` zupełnie nie pasuje do Twoich potrzeb, to masz możliwość
napisania własnych routerów podłączających się do ``ChainRouter``.

.. index:: ulepszacz tras

Pisanie własnych ulepszaczy tras
--------------------------------

Można dodać własne implementacje :ref:`RouteEnhancerInterface
<bundles-routing-dynamic_router-enhancer>`, jeśli ma się przypadek nie obsługiwany
przez :ref:`dostarczone ulepszacze <component-routing-enhancers>`. Wystarczy
zdefiniować usługi dla swoich ulepszaczy i oznaczyć je z ``dynamic_router_route_enhancer``,
aby mieć je dodane do trasowania. Można w tagu określić opcjonalny parametr ``priority``,
aby sterować kolejnością wykonywania ulepszczy. Wyższy priorytet powoduje wcześniejsze
wykonanie ulepszczacza.

.. index:: Route Provider
.. _bundle-routing-custom_provider:

Stosowanie własnego dostawcy tras
---------------------------------

Dynamiczny router umożliwia dostosowanie dostawcy tras (tj. klasy odpowiedzialnej
za pobieranie tras z bazy danych) i co za tym idzie obiektów tras.

Tworzenie dostawcy tras
~~~~~~~~~~~~~~~~~~~~~~~

Dostawca trasy musi implementować ``RouteProviderInterface``. Poniższa klasa
dostarcza proste rozwiązanie przy wykorzystaniu repozytorium ODM.

.. code-block:: php

    // src/Acme/DemoBundle/Repository/RouteProvider.php
    namespace Acme\DemoBundle\Repository;

    use Doctrine\ODM\PHPCR\DocumentRepository;
    use Symfony\Cmf\Component\Routing\RouteProviderInterface;
    use Symfony\Component\Routing\RouteCollection;
    use Symfony\Component\Routing\Route as SymfonyRoute;

    class RouteProvider extends DocumentRepository implements RouteProviderInterface
    {
        /**
         * This method is used to find routes matching the given URL.
         */
        public function findManyByUrl($url)
        {
            // for simplicity we retrieve one route
            $document = $this->findOneBy(array(
                'url' => $url,
            ));

            $pattern = $document->getUrl(); // e.g. "/this/is/a/url"

            $collection = new RouteCollection();

            // create a new Route and set our document as
            // a default (so that we can retrieve it from the request)
            $route = new SymfonyRoute($pattern, array(
                'document' => $document,
            ));

            // add the route to the RouteCollection using
            // a unique ID as the key.
            $collection->add('my_route_'.uniqid(), $route);

            return $collection;
        }

        /**
         * This method is used to generate URLs, e.g. {{ path('foobar') }}.
         */
        public function getRouteByName($name, $params = array())
        {
            $document = $this->findOneBy(array(
                'name' => $name,
            ));

            if ($route) {
                $route = new SymfonyRoute($route->getPattern(), array(
                    'document' => $document,
                ));
            }

            return $route;
        }
    }

.. tip::

    Jak można zauważyć, zwrócony został obiekt ``RouteCollection`` - dlaczego nie
    pojedynczy obiekt ``Route``? Dynamiczny router pozwala zwrócić wiele tras
    *kandydujących*. Innymi słowami, trasy *mogą* dopasowywać przychodzące adresy
    URL. Jest ważne, aby włączyć możliwość dopasowania *dynamicznych* tras, na
    przykład, ``/page/{page_id}/edit``. W naszym przykładzie dopasowujemy dokładnie
    określony adres URL i nigdy nie zwraca pojedynczego obiektu ``Route``.

Zastępowanie domyślnego dostawcy CMF
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Aby zastąpić domyślny ``RouteProvider`` należy zmodyfikować konfiguracje
w następujący sposób:

.. configuration-block::

   .. code-block:: yaml

       # app/config/config.yml
       cmf_routing:
           dynamic:
               enabled: true
               route_provider_service_id: acme_demo.provider.endpoint

   .. code-block:: xml

       <!-- app/config/config.xml -->
       <?xml version="1.0" encoding="UTF-8" ?>
       <container xmlns="http://symfony.com/schema/dic/services">
           <config xmlns="http://cmf.symfony.com/schema/dic/routing">
               <dynamic
                   enabled="true"
                   route-provider-service-id="acme_demo.provider.endpoint"
               />
           </config>
       </container>

   .. code-block:: php

       // app/config/config.php
       $container->loadFromExtension('cmf_routing', array(
           'dynamic' => array(
              'enabled'                   => true,
              'route_provider_service_id' => 'acme_demo.provider.endpoint',
           ),
       ));

Gdzie ``acme_demo.provider.endpoint`` jest identyfikatorem usługi dostawcy tras.
W celu uzyskania informacji o tworzeniu własnych usług proszę przeczytać artykuł
`Tworzenie i konfigurowanie usług w kontenerze`_ .

.. _`Tworzenie i konfigurowanie usług w kontenerze`: http://symfony.com/doc/current/book/service_container.html#creating-configuring-services-in-the-container/
.. _`PHPCR-ODM`: http://www.doctrine-project.org/projects/phpcr-odm.html

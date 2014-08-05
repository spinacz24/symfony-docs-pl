.. index::
    single: RoutingAutoBundle; dostosowywanie

Dostosowywanie
--------------

.. _routingauto_customization_pathproviders:

Dodawanie dostawców ścieżek
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Celem klasy ``PathProvider`` jest dodawanie jednej lub kilku elementów ścieżki
do stosu ścieżki. Na przykład, następujący dostawca będzie dodawał ścieżkę
``foo/bar`` do stosu trasy::

    // src/Acme/CmsBundle/RoutingAuto/PathProvider/FoobarProvider.php
    namespace Acme\CmsBundle\RoutingAuto\PathProvider;

    use Symfony\Cmf\Bundle\RoutingAutoBundle\AutoRoute\PathProviderInterface;
    use Symfony\Cmf\Bundle\RoutingAutoBundle\AutoRoute\RouteStack;

    class FoobarProvider implements PathProviderInterface
    {
        public function providePath(RouteStack $routeStack)
        {
            $routeStack->addPathElements(array('foo', 'bar'));
        }
    }

Aby zastosować dostawcę ścieżek musimy go zarejestrować w kontenerze i dodać tag
``cmf_routing_auto.provider`` oraz ustawić odpowiednio **alias**:

.. configuration-block::

    .. code-block:: yaml

        services:
            acme_cms.path_provider.foobar:
                class: Acme\CmsBundle\RoutingAuto\PathProvider\FoobarProvider
                scope: prototype
                tags:
                    - { name: cmf_routing_auto.provider, alias: "foobar"}

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">
            <service
                id="acme_cms.path_provider.foobar"
                class="Acme\CmsBundle\RoutingAuto\PathProvider\FoobarProvider"
                scope="prototype"
            >
                <tag name="cmf_routing_auto.provider" alias="foobar"/>
            </service>
        </container>

    .. code-block:: php

        use Symfony\Component\DependencyInjection\Definition;

        $definition = new Definition('Acme\CmsBundle\RoutingAuto\PathProvider\FoobarProvider');
        $definition->addTag('cmf_routing_auto.provider', array('alias' => 'foobar'));
        $definition->setScope('prototype');

        $container->setDefinition('acme_cms.path_provider.foobar', $definition);

Dostawca ``FoobarProvider`` jest teraz dostępny jako **foobar** w konfiguracji
automatycznego trasowania.

.. caution::

    Zarówno dostawcy ścieżek jak i akcje ścieżek muszą być definiowane w zakresie
    "prototype". Gwarantuje to, że za każdym razem system automatycznego trasowania
    żąda klasy, która jest nowa i nie ma żadnych problemów ze stanem.

Dodawanie akcji ścieżek
~~~~~~~~~~~~~~~~~~~~~~~

W systemie automatycznego trasowania "akcja ścieżki" jest akcją podejmowaną,
jeśli ścieżka dostarczona przez "dostawcę ścieżki" istnieje lub nie istnieje.

Można dodać akcje ścieżki rozszerzając ``PathActionInterface`` i rejestrując
odpowiednio nową klasę w konfiguracji DI (wstrzykiwania zależności).

Jest to bardzo prosta implementacja w pakiecie – służy do zrzucenia wyjątku w
przypadku istnienia scieżki::

    namespace Symfony\Cmf\Bundle\RoutingAutoBundle\RoutingAuto\PathNotExists;

    use Symfony\Cmf\Bundle\RoutingAutoBundle\AutoRoute\PathActionInterface;
    use Symfony\Cmf\Bundle\RoutingAutoBundle\AutoRoute\Exception\CouldNotFindRouteException;
    use Symfony\Cmf\Bundle\RoutingAutoBundle\AutoRoute\RouteStack;

    class ThrowException implements PathActionInterface
    {
        public function init(array $options)
        {
        }

        public function execute(RouteStack $routeStack)
        {
            throw new CouldNotFindRouteException('/'.$routeStack->getFullPath());
        }
    }

Metoda ``init()`` konfiguruje dostawcę (zrzucającego błędy, gdy wymagane opcje
nie istnieją) a metoda ``execute()`` wykonuje akcję.

Zapisy rejestrujące w konfiguracji DI wyglądają tak:

.. configuration-block::

    .. code-block:: yaml

        services:
            cmf_routing_auto.not_exists_action.throw_exception:
                class: Symfony\Cmf\Bundle\RoutingAutoBundle\RoutingAuto\PathNotExists\ThrowException
                scope: prototype
                tags:
                    - { name: cmf_routing_auto.not_exists_action, alias: "throw_exception"}

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">
            <service
                id="cmf_routing_auto.not_exists_action.throw_exception"
                class="Symfony\Cmf\Bundle\RoutingAutoBundle\RoutingAuto\PathNotExists\ThrowException"
                scope="prototype"
                >
                <tag name="cmf_routing_auto.not_exists_action" alias="throw_exception"/>
            </service>
        </container>

    .. code-block:: php

        use Symfony\Component\DependencyInjection\Definition;

        $definition = new Definition('Symfony\Cmf\Bundle\RoutingAutoBundle\RoutingAuto\PathNotExists\ThrowException');
        $definition->addTag('cmf_routing_auto.provider', array('alias' => 'throw_exception'));
        $definition->setScope('prototype');

        $container->setDefinition('cmf_routing_auto.not_exists_action.throw_exception', $definition);

Trzeba mieć na uwadze, co następuje:

* **Scope**: musi być *zawsze* ustawione na *prototype*;
* **Tag**: Tag rejestruje usługę w systemie automatycznego trasowania, może być
  jednym z następujących:

    * ``cmf_routing_auto.exists.action`` - jeśli akcja ma być stosowana, gdy
      ścieżka istnieje;
    * ``cmf_routing_auto.not_exists.action`` - jeśli akcja ma być stosowana, gdy
      ścieżka nie istnieje;

* **Alias**: alias tagu jest nazwą przez którą można odwoływać się do akcji
  w konfiguracji automatycznego trasowania.

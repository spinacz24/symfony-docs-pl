.. highlight:: php
   :linenothreshold: 2

.. index::
    single: wytwórnia menu; MenuBundle

Wytwórnia menu
==============

Dokumenty menu używane są tylko do utrwalania danych menu i nie są w rzeczywistości
wykorzystywane podczas renderowania menu. W procesie renderowania potrzebne są klasy
``MenuItem`` z komponentu KnpMenu. Wytwórnia menu wykorzystuje dane dostarczane
przez klasę implementującą ``NodeInterface`` i tworzy drzewo ``MenuItem``.

.. _bundles_menu_menu_factory_url_generation:

Generowanie URL
---------------

Element menu powinien mieć skojarzony z sobą URL. CMF dostarcza
``ContentAwareFactory`` rozszerzający ``RouterAwareFactory`` i ``MenuFactory``
KnpMenu.

* ``MenuFactory`` obsługuje tylko stosowanie opcji ``uri`` do określenia odwołań
  elementów menu.
* ``RouterAwareFactory`` dodaje obsługę generowania URL z opcji
  ``route`` i ``routeParameters``.
* CMF dodaje ``ContentAwareFactory``, który obsługuje generowania URL z opcji
  ``content`` i ``routeParameters`` podczas używania
  :ref:`dynamicznego  routera <bundles-routing-dynamic-generator>`.

Opcja ``content``, jeśli jest określona, musi zawierać klasę implementującą
``RouteReferrersInterface``. W celu uzyskania więcej informacji proszę zobaczyć
dokumentację :ref:`dynamicznego routera <bundles-routing-dynamic-generator>`.

Generowany URL jest bezwzględne lub względne, w zależności od logicznej
wartości opcji ``routeAbsolute``.

.. _bundles_menu_menu_factory_link_type:

Typ odnośnika
-------------

Opcja ``linkType`` jest dodatkiem CMF pozwalającym określić do użycia jedną
z trzech technik generowania lokalizatora URL.

Wartościami tej opcji moga być:

* ``null``: Jeśli wartość wynosi ``null`` (lub gdy opcja jest nieustawiona) to
  typ odnośnika zostaje określony automatycznie przez sprawdzenie każdej opcji
  ``uri``, ``route`` i ``content`` i zastosowanie pierwszej z nich, która nie
  jest pusta.
* **uri**: Używa identyfikatora URI dostarczonego przez opcję ``uri``.
* **route**: Generuje URL wykorzystując trasę z nazwą określoną w opcji ``route``
  i parametry określone w opcji ``routeParameters``.
* **content**: Generuje URL przez przekazanie instancji ``RouteReferrersInterface``
  określonej w opcji ``content`` do routera, z wykorzystaniem parametrów określonych
  w opcji ``routeParameters``.

Proces publikowania
-------------------

Wytwórnia menu CMF ustala także, przy wykorzystaniu
:doc:`procedury kontroli procesu publikowania <../core/publish_workflow>`,
czy węzły menu są publikowane, a zatem widoczne.

.. versionadded:: 1.1
    W CmfMenuBundle 1.1 dodano ``MenuContentVoter``.

``MenuContentVoter`` decyduje, że określony węzeł menu nie jest publikowany,
jeśli jego treść wskazuje na niepublikowanie.

Dostosowywanie menu przy użyciu zdarzeń
---------------------------------------

Wytwórnia menu CMF uwalnia zdarzenie ``cmf_menu.create_menu_item_from_node``
podczas procesu tworzenia ``MenuItem`` z klasy implementującej ``NodeInterface``.
Można wykorzystać to zdarzenie do kontroli utworzonego ``MenuItem``.
``CreateMenuItemFromNodeEvent`` zapewnia dostęp do ``NodeInterface`` i
``ContentAwareFactory``, które mogą być wykorzystywane do tworzenia indywidualnych
obiektów ``MenuItem`` lub do powiadamiania wytwórni, aby ignorowała bieżący węzeł
lub jego węzły potomne. Na przykład, to zdarzenie jest używane przez
:doc:`procedurę kontroli procesu publikowania <../core/publish_workflow>` do
pomijania generowania ``MenuItem`` dla niepublikowanych węzłów.

Uwalniane zdarzenie ``CreateMenuItemFromNodeEvent`` zawiera następujące metody,
które mogą zostać użyte do dostosowania tworzenia ``MenuItem`` dla ``NodeInterface``.

* ``CreateMenuItemFromNodeEvent::setSkipNode(true|false)``: Ustawienie skipNode
  na true zapobiegnie tworzeniu elementu z węzła i spowoduje pominięcie wszystkich
  węzłów potomnych. **Uwaga:** Jeśli setSkipNode(true) jest wywoływana dla ``Menu``,
  to ``ContentAwareFactory`` nadal będzie tworzyć pusty element dla menu.
  Zabezpiecza to kod KnpMenuBundle przed zrzucaniem wyjątku z powodu przekazywania
  ``null`` do funkcji renderującej menu;
* ``CreateMenuItemFromNodeEvent::setItem(ItemInterface $item|null)``: Detektor
  zdarzeń może wywoływać setItem w celu dostarczenia niestandardowego
  elementu menu do wykorzystania dla danego węzła. Jeśli element menu jest ustawiony,
  to zamiast tworzenia elemntu dla węzła, zostanie użyty ``ContentAwareFactory``.
  Węzły potomne będą nadal przetwarzane przez ``ContentAwareFactory``
  a detektory zdarzeń będą mogły następnie zastępować elementy menu tych węzłów
  wykorzystując tą metodę;
* ``CreateMenuItemFromNodeEvent::setSkipChildren(true|false)``: Detektory zdarzeń
  mogą ustawiać to na true i w konsekwencji ``ContentAwareFactory`` będzie opuszczał
  przetwarzanie elementów potomnych bieżącego węzła.

Przykład detektora zdarzeń menu
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Detoktor zdarzeń obsługujący węzły menu, które wskazują na różne menu
implementujące ``MenuReferrerInterface``::

    namespace Acme\DemoBundle;

    interface MenuReferrerInterface
    {
        public function getMenuName();
        public function getMenuOptions();
    }

    namespace Acme\DemoBundle\EventListener;

    use Symfony\Cmf\Bundle\MenuBundle\Event\CreateMenuItemFromNodeEvent;
    use Acme\DemoBundle\MenuReferrerInterface;
    use Knp\Menu\Provider\MenuProviderInterface;

    class CreateMenuItemFromNodeListener
    {
        protected $provider;

        public function __construct(MenuProviderInterface $provider)
        {
            $this->provider = $provider;
        }

        public function onCreateMenuItemFromNode(CreateMenuItemFromNodeEvent $event)
        {
            $node = $event->getNode();

            if ($node implements MenuReferrerInterface) {
                $menuName = $node->getMenuName();
                $menuOptions = $node->getMenuOptions();

                if (!$this->provider->has($menuName)) {
                    return;
                }

                $menu = $this->provider->get($menuName, $menuOptions);
                $event->setItem($menu);

                // as this does not call $event->setSkipChildren(true),
                // children of $node will be rendered as children items of $menu.
            }
        }

    }

Usługa musi być oflagowana jako detektor zdarzeń:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        services:
            acme_demo.listener.menu_referrer_listener:
                class: Acme\DemoBundle\EventListener\CreateMenuItemFromNodeListener
                arguments:
                    - "@knp_menu.menu_provider"
                tags:
                    -
                        name: kernel.event_listener
                        event: cmf_menu.create_menu_item_from_node
                        method: onCreateMenuItemFromNode

    .. code-block:: xml
       :linenos:

        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">
            <service id="acme_demo.listener.menu_referrer_listener" class="Acme\DemoBundle\EventListener\CreateMenuItemFromNodeListener">
                <argument type="service" id="knp_menu.menu_provider" />
                <tag name="kernel.event_listener"
                    event="cmf_menu.create_menu_item_from_node"
                    method="onCreateMenuItemFromNode"
                />
            </service>
        </container>

    .. code-block:: php
       :linenos:

        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;

        $definition = new Definition('Acme\DemoBundle\EventListener\CreateMenuItemFromNodeListener', array(
            new Reference('knp_menu.menu_provider'),
        ));
        $definition->addTag('kernel.event_listener', array(
            'event' => 'cmf_menu.create_menu_item_from_node',
            'method' => 'onCreateMenuItemFromNode',
        ));

        $container->setDefinition('acme_demo.listener.menu_referrer_listener', $definition);

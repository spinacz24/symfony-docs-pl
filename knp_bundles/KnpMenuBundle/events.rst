.. highlight:: php
   :linenothreshold: 2

Wykorzystywanie zdarzeń dla umożliwienia rozszerzania menu
==========================================================

Jeśli chce się podłączać różne części systemu do budowy menu, dobrym sposobem jest
wykorzystanie podejścia opartego na komponencie EventDispatcher  Symfony2.

Utworzenie budowniczego menu
----------------------------

Budowniczy menu (*ang. menu builder*) tworzy podstawowy element menu i następnie
wysyła zdarzenie, aby umożliwić innym częściom aplikacji dodać do niego więcej rzeczy.

.. code-block:: php
   :linenos:
   
   <?php
   // src/Acme/DemoBundle/Menu/MainBuilder.php
   
   namespace Acme\DemoBundle\Menu;
   
   use Acme\DemoBundle\MenuEvents;
   use Acme\DemoBundle\Event\ConfigureMenuEvent;
   use Knp\Menu\FactoryInterface;
   use Symfony\Component\DependencyInjection\ContainerAware;
   
   class MainBuilder extends ContainerAware
   {
      public function build(FactoryInterface $factory)
      {
         $menu = $factory->createItem('root');
         
         $menu->setCurrentUri($this->container->get('request')->getRequestUri());
         $menu->addChild('Dashboard', array('route' => '_acp_dashboard'));
         
         $this->container->get('event_dispatcher')->dispatch(ConfigureMenuEvent::CONFIGURE, new ConfigureMenuEvent($factory, $menu));
         
         return $menu;
      }
   }

.. note::
   
   Implementacja ta zakłada, że używa się BuilderAliasProvider (pobierającego menu
   jako ``AcmeDemoBundle:MainBuilder:build``) ale można również zdefiniować ją jako
   usługę i wstrzyknąć ``event_dispatcher`` jako zależność.

Utworzenie obiektu zdarzenia
----------------------------

Obiekt zdarzenia pozwala przekazać trochę danych do obiektu nasłuchu. W tym przypadku
będzie posiadać utworzone menu i wytwórnię.

.. code-block:: php
   :linenos:
   
   <?php
   // src/Acme/DemoBundle/Event/ConfigureMenuEvent.php
   
   namespace Acme\DemoBundle\Event;
   
   use Knp\Menu\FactoryInterface;
   use Knp\Menu\ItemInterface;
   use Symfony\Component\EventDispatcher\Event;
   
   class ConfigureMenuEvent extends Event
   {
      const CONFIGURE = 'acme_demo.menu_configure';
      
      private $factory;
      private $menu;
      
      /**
      * @param \Knp\Menu\FactoryInterface $factory
      * @param \Knp\Menu\ItemInterface $menu
      */
      public function __construct(FactoryInterface $factory, ItemInterface $menu)
      {
         $this->factory = $factory;
         $this->menu = $menu;
      }
      
      /**
      * @return \Knp\Menu\FactoryInterface
      */
      public function getFactory()
      {
         return $this->factory;
      }
      
      /**
      * @return \Knp\Menu\ItemInterface
      */
      public function getMenu()
      {
         return $this->menu;
      }
   }

.. note::
   
   Zgodnie z najlepszymi praktykami Symfony2, pierwszy segment nazwy zdarzenia
   jest aliasem pakietu, co pozwala uniknąć konfliktu nazewniczego.

Teraz nasz budowniczy dostarcza hak. Przyjrzyjmy się jak można go użyć.

Utworzenie obiektu nasłuchu
---------------------------


Można zarejestrować dla zdarzenia tyle nasłuchów, ile się chce. Dodajmy jeden.

.. code-block:: php
   :linenos:
   
   <?php
   // src/Acme/OtherBundle/EventListener/ConfigureMenuListener.php
   
   namespace Acme\OtherBundle\EventListener;
   
   use Acme\DemoBundle\Event\ConfigureMenuEvent;
   
   class ConfigureMenuListener
   {
      /**
      * @param \Acme\DemoBundle\Event\ConfigureMenuEvent $event
      */
      public function onMenuConfigure(ConfigureMenuEvent $event)
      {
         $menu = $event->getMenu();
         
         $menu->addChild('Matches', array('route' => 'versus_rankedmatch_acp_matches_index'));
         $menu->addChild('Participants', array('route' => 'versus_rankedmatch_acp_participants_index'));
      }
   }

Teraz możemy zarejestrować nasłuch.

.. code-block:: yaml
   :linenos:
   
   services:
      acme_other.configure_menu_listener:
         class: Acme\OtherBundle\EventListener\ConfigureMenuListener
         tags:
             - { name: kernel.event_listener, event: acme_demo.menu_configure, method: onMenuConfigure }

.. note::
   
   Podczas używania Symfony 2.1 można również utworzyć własny nasłuch jako
   subskrybenta i użyć znacznik ``kernel.event_subscriber`` (który nie posiada
   żadnych dodatkowych atrybutów).

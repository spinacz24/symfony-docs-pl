.. highlight:: php
   :linenothreshold: 2

Tworzenie menu jako usług
=========================

Pakiet ten oferuje naprawdę wygodny sposób tworzenia menu z przestrzeganiem
następującej konwencji i wstrzykiwania całego kontenera, jeśli potrzeba.

Jednak, gdy ma to miejsce, można zamiast tego wybrać utworzenie usługi
dla obiektu menu. Zaletą tego sposobu jest możliwość wstrzykiwania dokładnych
zależności potrzebnych dla menu, zamiast wstrzykiwania całego kontenera usług.
Prowadzi to do kodu, który jest wygodniejszy w testowaniu i potencjalnie możliwym
do wielokrotnego zastosowania. Wadą jest trochę więcej pracy w stworzeniu konfiguracji.

Zacznijmy od utworzenia budowniczego (*ang. builder*) dla menu. Można utworzyć tyle
 menu, ile potrzeba, więc można mieć w aplikacji tylko jedną lub tylko kilka klas
 budowniczego::

   <?php
   // src/Acme/MainBundle/Menu/MenuBuilder.php
   
   namespace Acme\MainBundle\Menu;
   
   use Knp\Menu\FactoryInterface;
   use Symfony\Component\HttpFoundation\Request;
   
   class MenuBuilder
   {
      private $factory;
      
         /**
         * @param FactoryInterface $factory
         */
         public function __construct(FactoryInterface $factory)
         {
            $this->factory = $factory;
         }
      
      public function createMainMenu(Request $request)
      {
         $menu = $this->factory->createItem('root');
         
         $menu->addChild('Home', array('route' => 'homepage'));
         // ... add more children
         
         return $menu;
      }
   }

Następnie zarejestrujemy dwie usługi: jedną dla budowniczego menu i drugą dla obiektu
menu utworzonego przez metodę ``createMainMenu``:

.. code-block:: yaml
   :linenos:
   
   # src/Acme/MainBundle/Resources/config/services.yml
   services:
      acme_main.menu_builder:
         class: Acme\MainBundle\Menu\MenuBuilder
         arguments: ["@knp_menu.factory"]
      
      acme_main.menu.main:
         class: Knp\Menu\MenuItem # the service definition requires setting the class
         factory_service: acme_main.menu_builder
         factory_method: createMainMenu
         arguments: ["@request"]
         scope: request # needed as we have the request as a dependency here
         tags:
            - { name: knp_menu.menu, alias: main } # The alias is what is used to retrieve the menu

.. note::
   
   Usługa menu musi być publiczna, jako że będzie pobierana w czasie wykonania,
   aby realizować leniwe ładowanie.

Teraz można wyrenderować menu bezpośrednio w szablonie za pomocą nazwy podanej
powyżej w kluczu ``alias``:

.. code-block:: jinja
   
   {{ knp_menu_render('main') }}

Przypuśćmy, że potrzebujemy utworzyć drugie menu dla paska bocznego. Zaczniemy od
dodania nowej metody do budowniczego::
  
   <?php
   // src/Acme/MainBundle/Menu/MenuBuilder.php
   
   // ...
   
   class MenuBuilder
   {
      // ...
      
      public function createSidebarMenu(Request $request)
      {
         $menu = $this->factory->createItem('sidebar');
         
         $menu->addChild('Home', array('route' => 'homepage'));
         // ... add more children
         
         return $menu;
      }
   }

Utworzymy teraz usługę *tylko* dla nowego menu, nadając mu nazwę, powiedzmy ``sidebar``:

.. code-block:: yaml
   :linenos:
   
   # src/Acme/MainBundle/Resources/config/services.yml
   services:
      acme_main.menu.sidebar:
         class: Knp\Menu\MenuItem
         factory_service: acme_hello.menu_builder
         factory_method: createSidebarMenu
         arguments: ["@request"]
         scope: request
         tags:
            - { name: knp_menu.menu, alias: sidebar } # Named "sidebar" this time

Na koniec zrenderujemy to menu:

.. code-block:: jinja
   
   {{ knp_menu_render('sidebar') }}

Wyłączenie dostawców rdzennego menu
-----------------------------------

W celu wspólnego używania różnych dostawców menu (na przykład, jeden oparty na kontenerze,
drugi oparty na budowniczym) wykorzystywany jest dostawca łańcuchowy.
Jednak nie jest on używany gdy dostępny jest tylko jeden dostawca ze względu na
spadek wydajności związany z obsługą opakowaniem. Jeśli nie chce się używać wbudowanych
dostawców, można je wyłączyć w konfiguracji:

.. code-block:: yaml
   :linenos:
   
   #app/config/config.yml
   knp_menu:
      providers:
         builder_alias: false    # disable the builder-based provider
         container_aware: true   # keep this one enabled. Can be omitted as it is the default

.. note::
   
   Obydwa dostawcy są domyśłnie włączone.

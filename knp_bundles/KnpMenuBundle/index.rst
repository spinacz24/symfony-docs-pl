.. index::
   KnpMenuBundle

KnpMenuBundle
=============

Witamy w KnpMenuBundle - tworzenie menu jest teraz naprawdę proste.

Instalacja
----------
   
Krok 1 - Pobranie pakietu i biblioteki
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Najpierw pobieramy bibliotekę KnpMenu i KnpMenuBundle. Są trzy różne sposoby
wykonania tego:

Metoda a) użycie programu Composer (wzorzec symfony 2.1)
........................................................

Dodaj do composer.json (zobacz http://getcomposer.org/)::

    "require" :  {
        // ...
        "knplabs/knp-menu": "2.0.*@dev",
        "knplabs/knp-menu-bundle": "2.0.*@dev"
    }

Metoda b) użycie pliku `deps` (wzorzec symfony 2.0)
...................................................

Dodaj następujące linie do pliku `deps` file i następnie uruchom ``php bin/vendors install``::

   [KnpMenu]
    git=https://github.com/KnpLabs/KnpMenu.git
   
   [KnpMenuBundle]
    git=https://github.com/KnpLabs/KnpMenuBundle.git
    target=bundles/Knp/Bundle/MenuBundle


Metoda c) użycie podmodułów
...........................

Uruchom następujące polecenia, aby uzyskać niezbędne biblioteki jako podmoduły. 

.. code-block:: bash
   
   $ git submodule add https://github.com/KnpLabs/KnpMenuBundle.git vendor/bundles/Knp/Bundle/MenuBundle
   $ git submodule add https://github.com/KnpLabs/KnpMenu.git vendor/KnpMenu

Krok 2 - rejestracja przestrzeni nazw
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Jeśli zainstalowano pakiet przez Composer, to należy zastosować utworzony plik
autoload.php  (przejdź do kroku 3).
Dodaj następujące dwa wpisy przestrzeni nazw do wywołania `registerNamespaces` 
w autoloaderze:

.. code-block:: php
   
   <?php
   // app/autoload.php
   $loader->registerNamespaces(array(
      // ...
      'Knp\Bundle' => __DIR__.'/../vendor/bundles',
      'Knp\Menu'   => __DIR__.'/../vendor/KnpMenu/src',
      // ...
   ));
      
Krok 3 - rejestracja pakietu
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Dla rozpocząć stosowania pakietu, zarejestruj go w jądrze:

.. code-block:: php
   
   <?php
   // app/AppKernel.php
    public function registerBundles()
    {
      $bundles = array(
      // ...
      new Knp\Bundle\MenuBundle\KnpMenuBundle(),
   );
   // ...

Krok 4 - (opcjonalnie) konfiguracja pakietu
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Pakiet dostarczany jest z rozsądną konfiguracją domyślną, która jest wykazana poniżej.
Jeśli pominie się ten krok, użyta będzie konfiguracja domyślna.

.. code-block:: yaml
   
   # app/config/config.yml
   knp_menu:
      twig:  # use "twig: false" to disable the Twig extension and the TwigRenderer
         template: knp_menu.html.twig
      templating: false # if true, enables the helper for PHP templates
      default_renderer: twig # The renderer to use, list is also available by default

.. note::
   
   Zadbaj o zmianę domyślnego rendera, jeśli wyłączysz obsługę Twig.
      
      
.. first-menu_

Utworzenie pierwszego menu
--------------------------

Są dwa sposoby utworzenia menu: the sposób "łatwy"  i bardziej elastyczna metoda
utworzenia menu jako usługi.

Metoda a) sposób prosty
~~~~~~~~~~~~~~~~~~~~~~~

W celu utworzenia menu, najpierw trzeba stworzyć nowa klasę w katalogu `Menu` swoich
pakietów. Klasa ta, zwana `Builder` w naszym przykładzie, będzie mieć jedną metodę
dla każdego menu, które trzeba zbudować.

Przykładowo klasa buildera może wyglądać tak:

.. code-block::
   
   <?php
   // src/Acme/DemoBundle/Menu/Builder.php
   namespace Acme\DemoBundle\Menu;
   
   use Knp\Menu\FactoryInterface;
   use Symfony\Component\DependencyInjection\ContainerAware;
   
   class Builder extends ContainerAware
   {
      public function mainMenu(FactoryInterface $factory, array $options)
      {
         $menu = $factory->createItem('root');
         
         $menu->addChild('Home', array('route' => 'homepage'));
         $menu->addChild('About Me', array(
            'route' => 'page_show',
            'routeParameters' => array('id' => 42)
        ));
        // ... add more children

        return $menu;
      }
   }



.. note::
   
   Jeśli potrzebuje się kontenera usługi dostępnej poprzez ``$this->container``
   można tylko rozszerzyć ``ContainerAware``. Można również zaimplementować
   ``ContainerAwareInterface`` zamiast rozszerzać tą klasę.

.. note::
   
   Builder może zostać zastąpiony przy wykorzystaniu dziedziczenia pakietów.

Dla faktycznego wyrenderowania menu, wystarczy w szablonie Twig umieścić następujący
kod (w miejscu, gdzie chce się mieć menu):

.. code-block:: jinja
   
   {{ knp_menu_render('AcmeDemoBundle:Builder:mainMenu') }}


Sposób ten odwołuje się do menu uzywając trzycześciowegi ciągu::
   
   **bundle**:**class**:**method**.

Jeśli potrzeba utworzyć drugie menu, wystarczy dodać inną metodę do klasy ``Builder``
(np. ``sidebarMenu``), budująca i zwracającą nowe menu, renderowane poprzez
``AcmeDemoBundle:Builder:sidebarMenu``.

Menu jest *wysoce* konfigurowalne. Więcej informacji można znaleźć w `dokumentacji
pakietu KnpMenu <https://github.com/KnpLabs/KnpMenu/blob/master/doc/01-Basic-Menus.markdown>`_.

Metoda b) menu jako usługa
~~~~~~~~~~~~~~~~~~~~~~~~~~

Więcej informacji na temat zarejestrowania usługi i otagowania menu mozna znaleźć 
w artkule :doc:`menu_service`.

.. rendering-menus_

Renderowanie menu
-----------------

Po skonfigurowaniu menu, wyrenderowanie go jest proste. Jeśli stosuje się "łatwy"
sposób konfiguracji, trzeba w szablonie Twig umieścić następujący znacznik:

.. code-block:: jinja
   
   {{ knp_menu_render('AcmeDemoBundle:Builder:mainMenu') }}

Dodatkowo mozna przekazać kilka opcji do rendera:

.. code-block:: jinja
   
   {{ knp_menu_render('AcmeDemoBundle:Builder:mainMenu', {'depth': 2, 'currentAsLink': false}) }}

W celu poznania pełnej listy opcji prosimy zapoznać się z dokumentacją
`KnpMenu <https://github.com/KnpLabs/KnpMenu/blob/master/doc/01-Basic-Menus.markdown>`_.

Można również "pobrać" menu, które można użyć później:

.. code-block:: jinja
   
   {% set menuItem = knp_menu_get('AcmeDemoBundle:Builder:mainMenu') %}
   
   {{ knp_menu_render(menuItem) }}

Jeśli chce się pobrać tylko określona gałąź menu, można napisać następujący kod,
w którym 'Contact' jest jednym z głównych elementów menu i ma elementy podrzędne.

.. code-block:: jinja
   
   {% set menuItem = knp_menu_get('AcmeDemoBundle:Builder:mainMenu', ['Contact']) %}
   
   {{ knp_menu_render(['AcmeDemoBundle:Builder:mainMenu', 'Contact']) }}


Jeśli chce się przekazać jakieś opcje do buildera, można użyć trzeci parametr
funkcji ``knp_menu_get``:

.. code-block:: jinja
   
   {% set menuItem = knp_menu_get('AcmeDemoBundle:Builder:mainMenu', [], {'some_option': 'my_value'}) %}
   
   {{ knp_menu_render(menuItem) }}


.. php-templates

Stosowanie szablonów PHP
------------------------

Jeśli woli się stosować szablony PHP, do renderowania i pobrania menu z szablonu
można użyć helpera szablonowania, podobnie jak w Twig.

.. code-block:: php
   
   // Retrieves an item by its path in the main menu
   $item = $view['knp_menu']->get('AcmeDemoBundle:Builder:main', array('child'));
   
   // Render an item
   echo $view['knp_menu']->render($item, array(), 'list');
   
Zagadnienia bardziej zaawansowane
---------------------------------

.. toctree::
   :maxdepth: 1
   
   menu_service
   custom_renderer
   custom_provider
   i18n
   events
   
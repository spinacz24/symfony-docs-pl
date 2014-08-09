Rejestrowanie własnego dostawcy
===============================

Rejestracja własnego dostawcy menu umożliwia zasilać menu własnymi danymi,
udostępnianymi przez kod. Mogą one na przykład pochodzić z repozytorium PHPCR
i tworzyć odpowiednie elementy menu.

Utwórzmy pierwszą klasę dostawcy w katalogu Provider pakietu:


.. code-block:: php
   
   <?php
   
   namespace Acme\DemoBundle\Provider;
   
   use Knp\Menu\FactoryInterface;
   use Knp\Menu\Provider\MenuProviderInterface;
   
   class CustomMenuProvider implements MenuProviderInterface
   {
      /**
      * @var FactoryInterface
      */
      protected $factory = null;
      
      /**
      * @param FactoryInterface $factory the menu factory used to create the menu item
      */
      public function __construct(FactoryInterface $factory)
      {
         $this->factory = $factory;
      }
      
      /**
      * Retrieves a menu by its name
      *
      * @param string $name
      * @param array $options
      * @return \Knp\Menu\ItemInterface
      * @throws \InvalidArgumentException if the menu does not exists
      */
      public function get($name, array $options = array())
      {
         if ('demo' == $name) {
            
            //several menu could call this provider
            $menu = /* construct / get a \Knp\Menu\NodeInterface */;
            
            if ($menu === null) {
               throw new \InvalidArgumentException(sprintf('The menu "%s" is not defined.', $name));
            }

            /*
            * Populate your menu here
            */

            $menuItem = $this->factory->createFromNode($menu);

            return $menuItem;
        }
      }
      
      /**
      * Checks whether a menu exists in this provider
      *
      * @param string $name
      * @param array $options
      * @return bool
      */
      public function has($name, array $options = array())
      {
         $menu = /* find the menu called $name */;
         
         return $menu !== null;
      }
   }

Następnie konfigurujemy serwis likując go do nowego dostawcy.

.. code-block:: yaml
   
   services:
      acme_demo_menu.provider:
         class: Acme\DemoBundle\Provider\CustomMenuProvider
         arguments:
            - @knp_menu.factory
         tags:
            - { name: knp_menu.provider }


Na koniec generujemy menu, na przykład wewnatrz szablonu Twig:

.. code-block:: jinja
   
   {{ knp_menu_render('demo') }}

W dokumentacji :doc:`Symfony CMF MenuBundle <../../cmf/bundles/menu/index>`_ podano pełny przykład.

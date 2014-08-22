.. highlight:: php
   :linenothreshold: 2

Rejestrowanie własnego renderera
================================

Rejestracja własnego renderera w dostawcy rendererów jest prostą sprawa, sprowadzającą
się do utworzenia otagowanej usługi z nazwą ``knp_menu.renderer``:

.. code-block:: yaml
   :linenos:
   
   # src/Acme/MainBundle/Resources/config/services.yml
   services:
      acme_hello.menu_renderer:
         # The class implements Knp\Menu\Renderer\RendererInterface
         class: Acme\MainBundle\Menu\CustomRenderer
         arguments: [%kernel.charset%] # set your own dependencies here
         tags:
            # The alias is what is used to retrieve the menu
            - { name: knp_menu.renderer, alias: custom }

Jeśli renderer rozszerza ListRenderer, potrzeba dostarczyć instancję Matcher.
Konfiguracja jest następująca:

.. code-block:: yaml
   :linenos:
   
   # src/Acme/MainBundle/Resources/config/services.yml
   services:
      acme_hello.menu_renderer:
         # The class implements Knp\Menu\Renderer\RendererInterface
         class: Acme\MainBundle\Menu\CustomRenderer
         arguments:
            - @knp_menu.matcher
            - %knp_menu.renderer.list.options%
            - %kernel.charset%
            # add your own dependencies here
         tags:
            # The alias is what is used to retrieve the menu
            - { name: knp_menu.renderer, alias: custom }

.. note::
   
   Usługa renderująca musi być publiczna, jako że będzie pobierana w czasie
   wykonania ab realizować leniwe ładowanie.

Teraz można użyć swojego renderera do wyrenderowania menu:

.. code-block:: jinja
   
   {{ knp_menu_render('main', {}, 'custom') }}

.. note::
   
   Ponieważ renderer jest odpowiedzialny za renderowanie kodu HTML, funkcja
   ``knp_menu_render`` jest oznaczona jako bezpieczna. Zadbaj o zabezpieczenie
   danych w rendererze znakami ucieczki aby uniknąć ataków XSS, jeśli używa się
   jakichś pól wejściowych dla menu.
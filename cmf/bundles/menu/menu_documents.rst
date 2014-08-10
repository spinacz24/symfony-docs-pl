.. index::
    single: dokumenty menu; MenuBundle

Dokumenty menu
==============

Zgodnie ze :ref:`standardami pakietów CMF <contrib_bundles_baseandstandardimplementations>`
można korzystać z dwóch implementacji węzła menu: dokumentu bazowego i dokumentu
standardowego. Oba istnieją w przestrzeni nazewniczej ``Model`` jako klasy
obojętnego przechowywania a w ``Doctrine\Phpcr`` ze specyficznymi funkcjonalnościami
dla PHPCR-ODM.

Węzeł bazowego menu
-------------------

Dokument ``MenuNodeBase`` implementuje tylko te funkcjonalności, które są dostępne
w oryginalnym węźle KnpMenu. Proszę zapoznać się z rozdziałem
"`Tworzenie menu: podstawy`_" dokumentacji KnpMenu.

.. code-block:: php

    use Symfony\Cmf\Bundle\MenuBundle\Doctrine\Phpcr\MenuNodeBase;

    // find the menu tree root
    $mainMenu = $dm->find('Symfony\Cmf\Bundle\MenuBundle\Doctrine\Phpcr\Menu', '/cms/menu/main');

    $node = new MenuNodeBase();
    $mainMenu->addChild($node);
    $node->setName('home');

    // Attributes are the HTML attributes of the DOM element representing the
    // menu node (e.g. <li/>)
    $node->setAttribute('attr_name', 'attr_value');
    $node->setAttributes(array('attr_name', 'attr_value'));
    $node->setChildrenAttributes(array('attr_name', 'attr_value'));

    // Display the node or not
    $node->setDisplay(true);
    $node->setDisplayChildren(true);

    // Any extra attributes you wish to associate with the menu node
    $node->setExtras(array('extra_param_1' => 'value'));

    // The label and the HTML attributes of the label
    $node->setLabel('Menu Node');
    $node->setLabelAttributes(array('style' => 'color: red;'));

    // The HTML attributes of the link (i.e. <a href=.../>
    $node->setLinkAttributes(array('style' => 'color: yellow;'));

    // Specify a route name to use and the parameters to use with it
    $node->setRoute('my_hard_coded_route_name');
    $node->setRouteParameters(array());

    // Specify if the route should be rendered absolute (otherwise relative)
    $node->setRouteAbsolute(true);

    // Specify a URI
    $node->setUri('http://www.example.com');

Węzeł standardowego menu
------------------------

Węzeł standardowego menu obsługuje od początku następujące specyficzne
funkcjonalności CMF:

* metody do ustawiania i pobierania dokumentu rodzicielskiego;
* kojarzenie treści;
* specyfikowanie typu odnośnika (URI, trasa lub treść);
* standardową integrację :doc:`procesu publikowania <../core/publish_workflow>`;
* tłumaczenia.

Kojarzenie treści
~~~~~~~~~~~~~~~~~

Oprócz pozyskiwania adresów URL dla odnośników elementów menu z wyraźnym
identyfikatorem URI lub obiektem trasy, węzeł standardowego menu dostarcza metodę
do kojarzenia dokumentu treści z każdym adresem URL, który może być wygenerowany::

    $menuItem = ...;
    $menuItem->setContent($content);

Dokumentem treści może być każdy dokument implementujący ``RouteReferrersInterface``.
Zobacz :ref:`bundles-routing-dynamic-generator`.

Ten dokument treści może zostać następnie przekazany do ``ContentAwareFactory``.
W celu poznania szczegółów proszę zapoznać się z rozdziałem
 :ref:`Generowanie URL <bundles_menu_menu_factory_url_generation>`.

Specyfikowanie typu odnośnika
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Węzeł standardowego menu obsługuje określanie typu odnośnika elementów menu
z wykorzystaniem metody ``setLinkType``::

    $menuItem = ...;
    $menuItem->setLinkType('content');

W celu poznania szczegółów proszę zapoznać się z
:ref:`dokumentacja wytwórni menu <bundles_menu_menu_factory_link_type>`.

Tłumaczenie
~~~~~~~~~~~

Węzeł standardowego menu obsługuje tłumaczenie, gdy jest to włączone, co umożliwia
ustawienie języka poprzez metodę ``setLocale``::

    $menuItem = ...;
    $menuItem->setLocale('pl');

W celu poznania szczegółów proszę zapoznać się z dokumentem
:ref:`Utrwalanie dokumentów wielojezycznych <bundles-core-multilang-persisting_multilang_documents>`.

Proces publikowania
~~~~~~~~~~~~~~~~~~~

Węzeł standardowego menu implementuje ``PublishTimePeriodInterface`` i
``PublishableInterface``. Proszę zapoznać się z 
:doc:`dokumentacją procesu publikowania <../core/publish_workflow>`.

.. versionadded:: 1.1
    W CmfMenuBundle 1.1 dodano ``MenuContentVoter``.

``MenuContentVoter`` decyduje, że węzeł menu nie jest publikowany, jeśli treść
jest wskazana jako niepublikowana.

.. _`Tworzenie menu: podstawy`: https://github.com/KnpLabs/KnpMenu/blob/1.1.x/doc/01-Basic-Menus.markdown

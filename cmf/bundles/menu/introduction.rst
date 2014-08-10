.. index::
    single: menu; pakiety
    single: MenuBundle

MenuBundle
==========

CmfMenuBundle rozszerza `KnpMenuBundle`_ i dodaje kilka funkcjonalności:

* Renderowanie menu przechowywane w warstwie utrwalania;
* Generowanie lokalizatorów URL węzłów menu ze zlinkowanej treści lub trasy.

Trzeba pamiętać, że w wydaniu 1.0 obsługiwana jest tylko warstwa utrwalania
Doctrine PHPCR-ODM.

.. caution::

    Gdy chcesz dostosować CmfMenuBundle, to musisz rozumieć jak działa
    :doc:`KnpMenuBundle </knp_bundles/KnpMenuBundle/index>`. Pakiet menu CMF
    dodaje tylko swoją funkcjonalność na szczycie pakietu menu Knp.
    Ta dokumentacja koncentruje się tylko na tej dodatkowej funkcjonalności.

Instalacja
----------

Pakiet ten można zainstalować `poprzez Composer`_, wykorzytując `symfony-cmf/menu-bundle`_.

Tworzenie prostego utrwalonego menu
-----------------------------------

Menu tworzone przy użyciu KnpMenuBundle składa się z hierarchii instancji klas
implementujących ``NodeInterface``. Odnosi się to też do menu tworzonego przy
użyciu dokumentów MenuBundle.

Zaleca się, aby dokument główny drzewa menu był dokumentem ``Menu``. Wszystkie
dokumenty potomne powinny być instancja ``MenuNode``.

Dokument główny powinien być dzieckiem dokumentu określonego w konfiguracji
przez parametr ``persistence.phpcr.menu_basepath``, którego wartość domyślna
to ``/cms/menu``. Trzeba pamiętać, że jeśli dokument ten nie istnieje, to musi
zostać utworzony.

Poniższy przykład tworzy nowe menu z dwoma elementami, "Home" i "Contact",
z których każdy określa identyfikator URI::

    use Symfony\Cmf\Bundle\MenuBundle\Doctrine\Phpcr\MenuNode;
    use Symfony\Cmf\Bundle\MenuBundle\Doctrine\Phpcr\Menu;
    use PHPCR\Util\NodeHelper;

    // this node should be created automatically, see note below this example
    $menuParent = $manager->find(null, '/cms/menu');

    $menu = new Menu();
    $menu->setName('main-menu');
    $menu->setLabel('Main Menu');
    $menu->setParentDocument($menuParent);

    $manager->persist($menu);

    $home = new MenuNode();
    $home->setName('home');
    $home->setLabel('Home');
    $home->setParentDocument($menu);
    $home->setUri('http://www.example.com/home');

    $manager->persist($home);

    $contact = new MenuNode();
    $contact->setName('contact');
    $contact->setLabel('Contact');
    $contact->setParentDocument($menu);
    $contact->setUri('http://www.example.com/contact');

    $manager->persist($contact);

    $manager->flush();

.. note::

    Gdy pakiet jest zarejestrowany, to podczas wykonywania
    ``doctrine:phpcr:repository:init`` tworzy ścieżkę ``/cms/menu``.
    Więcej informacji można znaleźć w artykule
    :ref:`Inicjatory repozytorium <phpcr-odm-repository-initializers>`

Renderowanie menu
-----------------

Menu renderuje się w ten sam sposób jak w :doc:`KnpMenuBundle </knp_bundles/KnpMenuBundle/index>`.
Nazwa menu będzie odpowiadać nazwie dokumentu głównego w drzewie menu:

.. configuration-block::

    .. code-block:: jinja

        {{ knp_menu_render('main-menu') }}

    .. code-block:: php

        echo $view['knp_menu']->render('main-menu');

Oto dokument ``main-menu`` określony w poprzednim przykładzie.
Będzie to renderować nieuporządkowaną listę w następujący sposób:

.. code-block:: html

    <ul>
        <li class="first">
          <a href="http://www.example.com/home">Home</a>
        </li>
        <li class="last">
          <a href="http://www.example.com/contact">Contact</a>
        </li>
    </ul>

.. tip::

    Czasami menu nie zostanie umieszczone w obrębie ``persistence.phpcr.menu_basepath``.
    W takim przypadku, aby renderować menu, można użyć ścieżkę bezwzględną
    (rozpoczynającą się od ukośnika):

    .. configuration-block::

        .. code-block:: jinja

            {{ knp_menu_render('/cms/some/path/my-menu') }}

        .. code-block:: php

            echo $view['knp_menu']->render('/cms/some/path/my-menu');

.. tip::

    Gdy używa się :doc:`BlockBundle <../block/introduction>`, można również
    wykorzystać ``MenuBlock``. Proszę przeczytać o tym w
    :ref:`dokumentacji BlockBundle <bundles-block-menu>`

.. note::

     Jest to klasa ``PhpcrMenuProvider``, co pozwala nam określić dokument
     PHPCR-ODM jako menu. Więcej informacji można znaleźć w :doc:`dokumentacji
     dostawcy menu <menu_provider>`.

.. caution::

    Jeśli chcesz renderować menu w Twig, to upewnij się, że nie masz wyłączonego
    Twig w sekcji konfiguracji ``knp_menu``.

Więcej informacji mozna znaleźć w seckocji :ref:`renderowanie menu <rendering-menus>`
dokumentacji KnpMenuBundle.

.. _`KnpMenu`: https://github.com/knplabs/KnpMenu
.. _`KnpMenuBundle`: https://github.com/knplabs/KnpMenuBundle
.. _`poprzez Composer`: http://getcomposer.org
.. _`symfony-cmf/menu-bundle`: https://packagist.org/packages/symfony-cmf/menu-bundle

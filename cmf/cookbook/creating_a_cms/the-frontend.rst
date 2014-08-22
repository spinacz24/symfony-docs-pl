.. highlight:: php
   :linenothreshold: 2

Tworzenie menu
--------------

W tym rozdziale zmodyfikujemy aplikację, tak aby dokument ``Page`` działał jako
węzły menu. Główny dokument strony może być następnie renderowany przy wykorzystaniu
helpera Twig pakietu `KnpMenuBundle`_.

Instalacja
..........

Upenij sie, że zainstalowany jest następujący pakiet:

.. code-block:: javascript
   :linenos:

    {
        ...
        require: {
            ...
            "symfony-cmf/menu-bundle": "1.1.*"
        },
        ...
    }

Dodaj do kernela pakiet CMF `MenuBundle`_ i jego zależności, `CoreBundle`_::

    // app/AppKernel.php
    class AppKernel extends Kernel
    {
        public function registerBundles()
        {
            $bundles = array(
                // ...
                new Knp\Bundle\MenuBundle\KnpMenuBundle(),
                new Symfony\Cmf\Bundle\CoreBundle\CmfCoreBundle(),
                new Symfony\Cmf\Bundle\MenuBundle\CmfMenuBundle(),
            );

            // ...
        }
    }

Modyfikacja dokumentu strony
............................

Dokument menu musi implementować ``Knp\Menu\NodeInterface``
dostarczany przez KnpMenuBundle::

    // src/Acme/BasicCmsBundle/Document/Page.php
    namespace Acme\BasicCmsBundle\Document;

    // ...
    use Knp\Menu\NodeInterface;

    use Doctrine\ODM\PHPCR\Mapping\Annotations as PHPCR;

    class Page implements RouteReferrersReadInterface, NodeInterface
    {
        // ...

        /**
         * @PHPCR\Children()
         */
        protected $children;

        public function getName()
        {
            return $this->title;
        }

        public function getChildren()
        {
            return $this->children;
        }

        public function getOptions()
        {
            return array(
                'label' => $this->title,
                'content' => $this,

                'attributes'         => array(),
                'childrenAttributes' => array(),
                'displayChildren'    => true,
                'linkAttributes'     => array(),
                'labelAttributes'    => array(),
            );
        }
    }

.. caution::

    W typowym zastosowaniu aplikacji CMF istnieją da interfejsy ``NodeInterface``,
    które nie mają z sobą nic wspólnego. Wykorzystywany tutaj przez nas interfejs
    pochodzi z KnpMenuBundle i opisuje węzły drzewa menu. Inne interfejsy pochodzą
    z repozytorium treści PHP i opisują węzły drzewa repozytorium.

Wszystkie menu są hierarchiczne, hierarchiczne jest również PHPCR-ODM, tak więc
świetnie nadają się do użycia w naszym przypadku.

Tutaj można dodać dodatkowe odwzorowanie, ``@Children``, które spowoduje, że PHPCR-ODM
będzie wypełniał adnotowaną instancje właściwości ``$children`` dokumentami potomnymi
tego dokumentu.

Opcje są opcjami używanymi przez system KnpMenu podczas renderowania menu.
Adres URL menu zostaje wywnioskowany z opcji ``content`` (zauważ, że dodaliśmy
wcześniej ``RouteReferrersReadInterface`` do ``Page``).

Atrybuty odnoszą się do elementów HTML. Zobacz dokumentację `KnpMenu`_ w celu
uzyskania więcej informacji.

Modyfikowanie danych testowych
..............................

System menu oczekuje, że będzie w stanie odnaleźć element główny, który zawiera
pierwszy poziom elementów potomnych. Zmodyfikujemy dane testowe, tak aby deklaracja
elementu głównego została dodana do istniejącej strony ``Home`` i dodatkowo do
strony ``About``::

    // src/Acme/BasicCmsBundle/DataFixtures/Phpcr/LoadPageData.php

    // ...
    class LoadPageData implements FixtureInterface
    {
        public function load(ObjectManager $dm)
        {
            // ...
            $rootPage = new Page();
            $rootPage->setTitle('main');
            $rootPage->setParentDocument($parent);
            $dm->persist($rootPage);

            $page = new Page();
            $page->setTitle('Home');
            $page->setParentDocument($rootPage);
            $page->setContent(<<<HERE
    Welcome to the homepage of this really basic CMS.
    HERE
            );
            $dm->persist($page);

            $page = new Page();
            $page->setTitle('About');
            $page->setParentDocument($rootPage);
            $page->setContent(<<<HERE
    This page explains what its all about.
    HERE
            );
            $dm->persist($page);

            $dm->flush();
        }
    }

Załaduj ponownie dane testowe:

.. code-block:: bash

    $ php app/console doctrine:phpcr:fixtures:load

Rejestracja dostawcy menu
.........................

Teraz można zarejestrować ``PhpcrMenuProvider`` z pakietu menu w konfiguracji
kontenera usługi:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # src/Acme/BasicCmsBundle/Resources/config/config.yml
        services:
            acme.basic_cms.menu_provider:
                class: Symfony\Cmf\Bundle\MenuBundle\Provider\PhpcrMenuProvider
                arguments:
                    - '@cmf_menu.factory'
                    - '@doctrine_phpcr'
                    - /cms/pages
                calls:
                    - [setRequest, ["@?request="]]
                tags:
                    - { name: knp_menu.provider }

    .. code-block:: xml
       :linenos:

        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:acme_demo="http://www.example.com/symfony/schema/"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <!-- ... -->
            <services>
                <!-- ... -->
                <service
                    id="acme.basic_cms.menu_provider"
                    class="Symfony\Cmf\Bundle\MenuBundle\Provider\PhpcrMenuProvider">
                    <argument type="service" id="cmf_menu.factory"/>
                    <argument type="service" id="doctrine_phpcr"/>
                    <argument>/cms/pages</argument>
                    <call method="setRequest">
                        <argument
                            type="service"
                            id="request"
                            on-invalid="null"
                            strict="false"
                        />
                    </call>
                    <tag name="knp_menu.provider" />
                </service>
            </services>
        </container>

    .. code-block:: php
       :linenos:

        // src/Acme/BasicCmsBundle/Resources/config/config.php
        use Symfony\Component\DependencyInjection\Reference;
        // ...

        $container
            ->register(
                'acme.basic_cms.menu_provider',
                'Symfony\Cmf\Bundle\MenuBundle\Provider\PhpcrMenuProvider'
            )
            ->addArgument(new Reference('cmf_menu.factory'))
            ->addArgument(new Reference('doctrine_phpcr'))
            ->addArgument('/cms/pages')
            ->addMethodCall('setRequest', array(
                new Reference(
                    'request',
                    ContainerInterface::NULL_ON_INVALID_REFERENCE,
                    false
                )
            ))
            ->addTag('knp_menu.provider')
        ;

i włączyć funkcjonalność renderowania Twig pakietu KnpMenu:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        knp_menu:
            twig: true

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.yml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">
            <config xmlns="http://example.org/schema/dic/knp_menu">
                <twig>true</twig>
            </config>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('knp_menu', array(
            'twig' => true,
        ));

i w końcu można renderować menu:

.. configuration-block::

    .. code-block:: jinja
       :linenos:

        {# src/Acme/BasicCmsBundle/Resources/views/Default/page.html.twig #}

        {# ... #}
        {{ knp_menu_render('main') }}

    .. code-block:: html+php
       :linenos:

        <!-- src/Acme/BasicCmsBundle/Resources/views/Default/page.html.php -->

        <!-- ... -->
        <?php echo $view['knp_menu']->render('main') ?>

Należy pamiętać, że ``main`` odnosi się do nazwy strony głównej dodanej w danych
testowych.

.. _`knpmenubundle`: https://github.com/KnpLabs/KnpMenuBundle
.. _`knpmenu`: https://github.com/KnpLabs/KnpMenu
.. _`MenuBundle`: https://github.com/symfony-cmf/MenuBundle
.. _`CoreBundle`: https://github.com/symfony-cmf/CoreBundle

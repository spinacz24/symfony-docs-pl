.. highlight:: php
   :linenothreshold: 2

Zaplecze administracyjne - Sonata Admin
---------------------------------------

W tym rozdziale zbudujemy interfejs administracyjny za pomocą SonataDoctrinePHPCRAdminBundle_.

Instalacja
~~~~~~~~~~

Upewnij się, że masz zainstalowany następujący pakiet:

.. code-block:: javascript
   :linenos:

    {
        ...
        require: {
            ...
            "sonata-project/doctrine-phpcr-admin-bundle": "1.1.*",
        },
        ...
    }

Włącz w kernelu związane pakiety Sonata::

    // app/AppKernel.php
    class AppKernel extends Kernel
    {
        public function registerBundles()
        {
            $bundles = array(
                // ...
                new Knp\Bundle\MenuBundle\KnpMenuBundle(),
                new Sonata\CoreBundle\SonataCoreBundle(),
                new Sonata\jQueryBundle\SonatajQueryBundle(),
                new Sonata\BlockBundle\SonataBlockBundle(),
                new Sonata\AdminBundle\SonataAdminBundle(),
                new Sonata\DoctrinePHPCRAdminBundle\SonataDoctrinePHPCRAdminBundle(),
            );

            // ...
        }
    }

Sonata wymaga pakietu ``sonata_block`` w celu umożliwienia konfigurowania jej
w konfiguracji głównej:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml

        # ...
        sonata_block:
            default_contexts: [cms]
            blocks:
                # Enable the SonataAdminBundle block
                sonata.admin.block.admin_list:
                    contexts: [admin]

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="htp://symfony.com/schema/dic/services">
            <config xmlns="http://sonata-project.org/schema/dic/block">
                <default-context>cms</default-context>

                <block id="sonata.admin.block.admin_list">
                    <context>admin</context>
                </block>
            </config>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('sonata_block', array(
            'default_contexts' => array('cms'),
            'blocks' => array(
                'sonata.admin.block.admin_list' => array(
                    'contexts' => array('admin'),
                ),
            ),
        ));

i wymaga następujących wpisów w pliku trasowania:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/routing.yml

        admin:
            resource: '@SonataAdminBundle/Resources/config/routing/sonata_admin.xml'
            prefix: /admin

        _sonata_admin:
            resource: .
            type: sonata_admin
            prefix: /admin

    .. code-block:: xml
       :linenos:

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <import
                resource="@SonataAdminBundle/Resources/config/sonata_admin.xml"
                prefix="/admin"
            />

            <import
                resource="."
                type="sonata_admin"
                prefix="/admin"
            />

        </routes>

    .. code-block:: php
       :linenos:

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;

        $collection = new RouteCollection();
        $routing = $loader->import(
            "@SonataAdminBundle/Resources/config/sonata_admin.xml"
        );
        $routing->setPrefix('/admin');
        $collection->addCollection($routing);

        $_sonataAdmin = $loader->import('.', 'sonata_admin');
        $_sonataAdmin->addPrefix('/admin');
        $collection->addCollection($_sonataAdmin);

        return $collection;

i opublikowania swoich zasobów (usuń ``--symlink`` jeśli używasz Windows):

.. code-block:: bash

    $ php app/console assets:install --symlink web/

Świetnie, teraz zajrzyj pod adres http://localhost:8000/admin/dashboard

Brak tłumaczeń? Odkomentuj translator w pliku konfiguracyjnym:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml

        # ...
        framework:
            # ...
            translator:      { fallback: "%locale%" }

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <config xmlns="http://symfony.com/schema/dic/symfony">
                <!-- ... -->
                <translator fallback="%locale%" />
            </config>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            // ...
            'translator' => array(
                'fallback' => '%locale%',
            ),
        ));

.. tip::

    Przeczytaj :ref:`book_handling-multilang_sonata-admin` w celu uzyskania więcej
    informacji o Sonata Admin i wielojęzyczności.

Patrząc na kokpit administracyjny, zauważysz, że istnieje wpis do tras administracyjnych.
Klasa admin pakietu RoutingBundle jest rejestrowana automatycznie. Jednak
nie ma potrzeby z tego korzystać w swojej aplikacji, gdyż trasy są zarządzane przez
RoutingAutoBundle a nie przez administratora. Można wyłączyć administratora RoutingBundle:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        cmf_routing:
            # ...
            dynamic:
                # ...
                persistence:
                    phpcr:
                        # ...
                        use_sonata_admin: false

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">
            <config xmlns="http://cmf.symfony.com/schema/dic/routing">
                <dynamic>
                    <!-- ... -->
                    <persistence>
                        <phpcr use-sonata-admin="false"/>
                    </persistence>
                </dynamic>
            </config>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('cmf_routing', array(
            // ...
            'dynamic' => array(
                'persistence' => array(
                    'phpcr' => array(
                        // ...
                        'use_sonata_admin' => false,
                    ),
                ),
            ),
        ));

.. tip::

    Wszystkie panele Sonata Admin są powiadamiane o pakietach CMF mających
    opcję konfiguracyjną i chronią zarejestrowaną klasę (lub klasy) admin.

Utworzenie klas admin
~~~~~~~~~~~~~~~~~~~~~

Utwórzmy następującą klasę admin, najpierw dla dokumentu ``Page``::

    // src/Acme/BasicCmsBundle/Admin/PageAdmin.php
    namespace Acme\BasicCmsBundle\Admin;

    use Sonata\DoctrinePHPCRAdminBundle\Admin\Admin;
    use Sonata\AdminBundle\Datagrid\DatagridMapper;
    use Sonata\AdminBundle\Datagrid\ListMapper;
    use Sonata\AdminBundle\Form\FormMapper;

    class PageAdmin extends Admin
    {
        protected function configureListFields(ListMapper $listMapper)
        {
            $listMapper
                ->addIdentifier('title', 'text')
            ;
        }

        protected function configureFormFields(FormMapper $formMapper)
        {
            $formMapper
                ->with('form.group_general')
                ->add('title', 'text')
                ->add('content', 'textarea')
            ->end();
        }

        public function prePersist($document)
        {
            $parent = $this->getModelManager()->find(null, '/cms/pages');
            $document->setParentDocument($parent);
        }

        protected function configureDatagridFilters(DatagridMapper $datagridMapper)
        {
            $datagridMapper->add('title', 'doctrine_phpcr_string');
        }

        public function getExportFormats()
        {
            return array();
        }
    }

a następnie dla dokumentu ``Post``. Jak widzimy, jest on niemal identyczny z
dokumentem ``Page``, tak więc rozszerzymy klasę ``PageAdmin``, aby uniknąć
duplikowanie kodu::

    // src/Acme/BasicCmsBundle/Admin/PostAdmin.php
    namespace Acme\BasicCmsBundle\Admin;

    use Sonata\DoctrinePHPCRAdminBundle\Admin\Admin;
    use Sonata\AdminBundle\Datagrid\DatagridMapper;
    use Sonata\AdminBundle\Datagrid\ListMapper;
    use Sonata\AdminBundle\Form\FormMapper;

    class PostAdmin extends PageAdmin
    {
        protected function configureFormFields(FormMapper $formMapper)
        {
            parent::configureFormFields($formMapper);

            $formMapper
                ->with('form.group_general')
                ->add('date', 'date')
            ->end();
        }
    }

.. note::

    W metodzie ``prePersist`` klasy ``PageAdmin`` można trwale zakodować ścieżkę
    elementu nadrzędnego. Można zmienić to zachowanie, aby umożliwić strukturyzację
    stron (na przykład w celu stworzenia zagnieżdżonego menu).

Teraz wystarczy zarejestrować te klasy w konfiguracji kontenera wstrzykiwania zależności:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

            # src/Acme/BasicCmsBundle/Resources/config/config.yml
            services:
                acme.basic_cms.admin.page:
                    class: Acme\BasicCmsBundle\Admin\PageAdmin
                    arguments:
                        - ''
                        - Acme\BasicCmsBundle\Document\Page
                        - 'SonataAdminBundle:CRUD'
                    tags:
                        - { name: sonata.admin, manager_type: doctrine_phpcr, group: 'Basic CMS', label: Page }
                    calls:
                        - [setRouteBuilder, ['@sonata.admin.route.path_info_slashes']]
                acme.basic_cms.admin.post:
                    class: Acme\BasicCmsBundle\Admin\PostAdmin
                    arguments:
                        - ''
                        - Acme\BasicCmsBundle\Document\Post
                        - 'SonataAdminBundle:CRUD'
                    tags:
                        - { name: sonata.admin, manager_type: doctrine_phpcr, group: 'Basic CMS', label: 'Blog Posts' }
                    calls:
                        - [setRouteBuilder, ['@sonata.admin.route.path_info_slashes']]

    .. code-block:: xml
       :linenos:

        <!-- src/Acme/BasicCmsBundle/Resources/config/config.yml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <!-- ... -->
            <services>
                <!-- ... -->
                <service id="acme.basic_cms.admin.page"
                    class="Acme\BasicCmsBundle\Admin\PageAdmin">

                    <call method="setRouteBuilder">
                        <argument type="service" id="sonata.admin.route.path_info_slashes" />
                    </call>

                    <tag
                        name="sonata.admin"
                        manager_type="doctrine_phpcr"
                        group="Basic CMS"
                        label="Page"
                    />
                    <argument/>
                    <argument>Acme\BasicCmsBundle\Document\Page</argument>
                    <argument>SonataAdminBundle:CRUD</argument>
                </service>

                <service id="acme.basic_cms.admin.post"
                    class="Acme\BasicCmsBundle\Admin\PostAdmin">

                    <call method="setRouteBuilder">
                        <argument type="service" id="sonata.admin.route.path_info_slashes" />
                    </call>

                    <tag
                        name="sonata.admin"
                        manager_type="doctrine_phpcr"
                        group="Basic CMS"
                        label="Blog Posts"
                    />
                    <argument/>
                    <argument>Acme\BasicCmsBundle\Document\Post</argument>
                    <argument>SonataAdminBundle:CRUD</argument>
                </service>
            </services>
        </container>

    .. code-block:: php
       :linenos:

            // src/Acme/BasicCmsBundle/Resources/config/config.php
            use Symfony\Component\DependencyInjection\Reference;
            // ...

            $container->register('acme.basic_cms.admin.page', 'Acme\BasicCmsBundle\Admin\PageAdmin')
              ->addArgument('')
              ->addArgument('Acme\BasicCmsBundle\Document\Page')
              ->addArgument('SonataAdminBundle:CRUD')
              ->addTag('sonata.admin', array(
                  'manager_type' => 'doctrine_phpcr',
                  'group' => 'Basic CMS',
                  'label' => 'Page'
              )
              ->addMethodCall('setRouteBuilder', array(
                  new Reference('sonata.admin.route.path_info_slashes'),
              ))
            ;
            $container->register('acme.basic_cms.admin.post', 'Acme\BasicCmsBundle\Admin\PostAdmin')
              ->addArgument('')
              ->addArgument('Acme\BasicCmsBundle\Document\Post')
              ->addArgument('SonataAdminBundle:CRUD')
              ->addTag('sonata.admin', array(
                   'manager_type' => 'doctrine_phpcr',
                   'group' => 'Basic CMS',
                   'label' => 'Blog Posts'
              )
              ->addMethodCall('setRouteBuilder', array(
                  new Reference('sonata.admin.route.path_info_slashes'),
              ))
            ;

.. note::

    W wersji XML powyższej konfiguracji określiliśmy ``manager_type``
    (ze znakiem podkreślenia). Powinno to być napisane jako ``manager-type``
    (z myślnikiem) i zostało poprawione w wersji Symfony 2.4.

Sprawdź efekt naszej pracy po adresem http://localhost:8000/admin/dashboard

.. image:: ../../_images/cookbook/basic-cms-sonata-admin.png

Konfiguracja drzewa administracyjnego na pulpicie
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sonata Admin dostarcza użytecznego widoku drzewa całej treści. Można kliknąć
na element drzewa aby go edytować. Kliknięcie prawym przyciskiem myszy spowoduje
usuniecie elementu lub dodanie jego elementu potomnego. Uchwycenie i upuszczenie
elementu spowoduje reorganizacje treści.

Włącz w kernelu pakiety CmfTreeBundle i FOSJsRoutingBundle::

    // app/AppKernel.php
    class AppKernel extends Kernel
    {
        // ...

        public function registerBundles()
        {
            $bundles = array(
                // ...
                new FOS\JsRoutingBundle\FOSJsRoutingBundle(),
                new Symfony\Cmf\Bundle\TreeBrowserBundle\CmfTreeBrowserBundle(),
            );

            // ...
        }
    }

Trasy wykorzystywane przez drzewo we frontowej części aplikacji są obsługiwane
przez pakiet FOSJsRoutingBundle.
Odpowiednie trasy są oflagowane flagą ``expose`` - są one dostępne automatycznie.
Jednak trzeba załadować trasy pakietów TreeBundle i FOSJsRoutingBundle:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/routing.yml
        cmf_tree:
            resource: .
            type: 'cmf_tree'

        fos_js_routing:
            resource: "@FOSJsRoutingBundle/Resources/config/routing/routing.xml"

    .. code-block:: xml
       :linenos:

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <import resource="." type="cmf_tree" />

            <import resource="@FOSJsRoutingBundle/Resources/config/routing/routing.xml" />

        </routes>

    .. code-block:: php
       :linenos:

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;

        $collection = new RouteCollection();

        $collection->addCollection($loader->import('.', 'cmf_tree'));

        $collection->addCollection($loader->import(
            "@FOSJsRoutingBundle/Resources/config/routing/routing.xml"
        ));

        return $collection;

Dodajmy blok drzewa do konfiguracji ``sonata_block`` i powiadommy 
Sonata Admin, aby wyświetlił blok:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml

        # ...
        sonata_block:
            blocks:
                # ...
                sonata_admin_doctrine_phpcr.tree_block:
                    settings:
                        id: '/cms'
                    contexts: [admin]

        sonata_admin:
            dashboard:
                blocks:
                    - { position: left, type: sonata_admin_doctrine_phpcr.tree_block }
                    - { position: right, type: sonata.admin.block.admin_list }

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="htp://symfony.com/schema/dic/services">

            <config xmlns="http://sonata-project.org/schema/dic/block">
                <! ... -->
                <block id="sonata_admin_doctrine_phpcr.tree_block">
                    <setting id="id">/cms</setting>
                    <context>admin</context>
                </block>
            </config>

            <config xmlns="http://sonata-project.org/schema/dic/admin">
                <dashboard>
                    <block position="left" type="sonata_admin_doctrine_phpcr.tree_block"/>
                    <block position="right" type="sonata.admin.block.admin_list"/>
                </dashboard>
            </config>

        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('sonata_block', array(
            'blocks' => array(
                // ...
                'sonata_admin_doctrine_phpcr.tree_block' => array(
                    'settings' => array(
                        'id' => '/cms',
                    ),
                    'contexts' => array('admin'),
                ),
            ),
        ));

        $container->loadFromExtension('sonata_admin', array(
            'dashboard' => array(
                'blocks' => array(
                    array('position' => 'left', 'type' => 'sonata_admin_doctrine_phpcr.tree_block'),
                    array('position' => 'right', 'type' => 'sonata.admin.block.admin_list'),
                ),
            ),
        ));

Dla wyświetlenia dokumentów w drzewie pulpitu administracyjnego, należy poinformować
o tym Sonata Admin:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        sonata_doctrine_phpcr_admin:
            document_tree_defaults: [locale]
            document_tree:
                Doctrine\ODM\PHPCR\Document\Generic:
                    valid_children:
                        - all
                Acme\BasicCmsBundle\Document\Page:
                    valid_children:
                        - Acme\BasicCmsBundle\Document\Post
                Acme\BasicCmsBundle\Document\Post:
                    valid_children: []
                # ...

    .. code-block:: xml
       :linenos:

        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">

            <config xmlns="http://sonata-project.org/schema/dic/doctrine_phpcr_admin" />

                <document-tree-default>locale</document-tree-default>

                <document-tree class="Doctrine\ODM\PHPCR\Document\Generic">
                    <valid-child>all</valid-child>
                </document-tree>

                <document-tree class="Acme\BasicCmsBundle\Document\Post">
                    <valid-child>Acme\BasicCmsBundle\Document\Post</valid-child>
                </document-tree>

                <document-tree class="Acme\BasicCmsBundle\Document\Post" />

                <!-- ... -->
            </config>
        </container>

    .. code-block:: php
       :linenos:

        $container->loadFromExtension('sonata_doctrine_phpcr_admin', array(
            'document_tree_defaults' => array('locale'),
            'document_tree' => array(
                'Doctrine\ODM\PHPCR\Document\Generic' => array(
                    'valid_children' => array(
                        'all',
                    ),
                ),
                'Acme\BasicCmsBundle\Document\Post' => array(
                    'valid_children' => array(
                        'Acme\BasicCmsBundle\Document\Post',
                    ),
                ),
                'Acme\BasicCmsBundle\Document\Post' => array(
                    'valid_children' => array(),
                ),
                // ...
        ));

.. tip::

    Dokument wyświetlany w drzewie wymaga swojego własnego wpisu.
    Można zezwolić, aby wszystkie typy dokumentów poniżej niego miały wszystkie
    (``all``) dokumenty potomne. Ale jeśli jawnie wykaże się dozwolone elementy potomne,
    to kliknięcie prawym przyciskiem myszy wyświetli menu kontekstowe tylko z tymi
    dokumentami. Ułatwia to użytkownikom nie popełnianie błędów.
    
.. _SonataDoctrinePHPCRAdminBundle: http://sonata-project.org/bundles/doctrine-phpcr-admin/master/doc/index.html

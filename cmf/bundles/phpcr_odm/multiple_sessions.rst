.. highlight:: php
   :linenothreshold: 2

.. index::
    single: wielosesyjność; DoctrinePHPCRBundle

Konfigurowanie wielu sesji w PHPCR-ODM
======================================

Jeśli zachodzi potrzeba stosowania więcej niż jednego zaplecza PHPCR, można
zdefiniować ``sessions`` jako informację podrzędna ``session``. Każda sesja ma
nazwę i konfigurację powielającą ten sam schemat co w ``session``. Można również
zamienić atrybut ``default_session``.

.. _bundle-phpcr-odm-multiple-phpcr-sessions:

Wiele sesji PHPCR
-----------------

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        doctrine_phpcr:
            session:
                default_session: ~
                sessions:
                    <name>:
                        workspace: ... # Required
                        username:  ~
                        password:  ~
                        backend:
                            # ...
                        options:
                            # ...

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">
            <config xmlns="http://doctrine-project.org/schema/symfony-dic/odm/phpcr">
                <session default-session="null">
                    <!-- workspace: Required -->
                    <session name="<name>"
                        workspace="..."
                        username="null"
                        password="null"
                    >
                        <backend>
                            <!-- ... -->
                        </backend>
                        <option>
                            <!-- ... -->
                        </option>
                    </session>
                </session>
            </config>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('doctrine_phpcr', array(
            'session' => array(
                'default_session' => null,
                'sessions' => array(
                    '<name>' => array(
                        'workspace' => '...', // Required
                        'username'  => null,
                        'password'  => null,
                        'backend'   => array(
                            // ...
                        ),
                        'options'   => array(
                            // ...
                        ),
                    ),
                ),
            ),
        ));

Wiele menadżerów dokumentów
---------------------------

Jeśli używa się ODM, można również skonfigurować wiele menadżerów dokumentów.

Wewnątrz sekcji odm w opcji ``document_managers`` można dodać nazwane wpisy.
Użycie innej sesji niż domyślna wymaga określenia atrybutu sesji.

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        odm:
            default_document_manager: ~
            document_managers:
                <name>:
                    session: <sessionname>
                    # ... configuration as above

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">
            <config xmlns="http://doctrine-project.org/schema/symfony-dic/odm/phpcr">
                <odm default-document-manager="null">
                    <document-manager
                        name="<name>"
                        session="<sessionname>"
                    >
                        <!-- ... configuration as above -->
                    </document-manager>
                </odm>
            </config>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('doctrine_phpcr', array(
            'odm' => array(
                'default_document_manager' => null,
                'document_managers' => array(
                    '<name>' => array(
                        'session' => '<sessionname>',
                        // ... configuration as above
                    ),
                ),
            ),
        ));

Zebranie tego wszystkiego razem
-------------------------------

Pełny przykład wygląda następujaco:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        doctrine_phpcr:
            # configure the PHPCR sessions
            session:
                sessions:
                    default:
                        backend: "%phpcr_backend%"
                        workspace: "%phpcr_workspace%"
                        username: "%phpcr_user%"
                        password: "%phpcr_pass%"

                    website:
                        backend:
                            type: jackrabbit
                            url: "%magnolia_url%"
                        workspace: website
                        username: "%magnolia_user%"
                        password: "%magnolia_pass%"

                    dms:
                        backend:
                            type: jackrabbit
                            url: "%magnolia_url%"
                        workspace: dms
                        username: "%magnolia_user%"
                        password: "%magnolia_pass%"

            # enable the ODM layer
            odm:
                auto_generate_proxy_classes: "%kernel.debug%"
                document_managers:
                    default:
                        session: default
                        mappings:
                            SandboxMainBundle: ~
                            CmfContentBundle: ~
                            CmfMenuBundle: ~
                            CmfRoutingBundle: ~

                    website:
                        session: website
                        configuration_id: sandbox_magnolia.odm_configuration
                        mappings:
                            SandboxMagnoliaBundle: ~

                    dms:
                        session: dms
                        configuration_id: sandbox_magnolia.odm_configuration
                        mappings:
                            SandboxMagnoliaBundle: ~

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">
            <config xmlns="http://doctrine-project.org/schema/symfony-dic/odm/phpcr">
                <session>
                    <session name="default"
                        backend="%phpcr_backend%"
                        workspace="%phpcr_workspace%"
                        username="%phpcr_user%"
                        password="%phpcr_pass%"
                    />
                    <session name="website"
                        workspace="website"
                        username="%magnolia_user%"
                        password="%magnolia_pass%"
                    >
                        <backend type="jackrabbit" url="%magnolia_url%"/>
                    </session>
                    <session name="dms"
                        workspace="dms"
                        username="%magnolia_user%"
                        password="%magnolia_pass%"
                    >
                        <backend type="jackrabbit" url="%magnolia_url%"/>
                    </session>
                </session>

                <!-- enable the ODM layer -->
                <odm auto-generate-proxy-classes="%kernel.debug%">
                    <document-manager
                        name="default"
                        session="default"
                    >
                        <mapping name="SandboxMainBundle" />
                        <mapping name="CmfContentBundle" />
                        <mapping name="CmfMenuBundle" />
                        <mapping name="CmfRoutingBundle" />
                    </document-manager>

                    <document-manager
                        name="website"
                        session="website"
                        configuration-id="sandbox_magnolia.odm_configuration"
                    >
                        <mapping name="SandboxMagnoliaBundle" />
                    </document-manager>

                    <document-manager
                        name="dms"
                        session="dms"
                        configuration-id="sandbox_magnolia.odm_configuration"
                    >
                        <mapping name="SandboxMagnoliaBundle" />
                    </document-manager>

                </odm>
            </config>
        </container>

    .. code-block:: php
       ;linenos:

        // app/config/config.php
        $container->loadFromExtension('doctrine_phpcr', array(
            'session' => array(
                'sessions' => array(
                    'default' => array(
                        'backend'   => '%phpcr_backend%',
                        'workspace' => '%phpcr_workspace%',
                        'username'  => '%phpcr_user%',
                        'password'  => '%phpcr_pass%',
                    ),
                    'website' => array(
                        'backend' => array(
                            'type' => 'jackrabbit',
                            'url'  => '%magnolia_url%',
                        ),
                        'workspace' => 'website',
                        'username'  => '%magnolia_user%',
                        'password'  => '%magnolia_pass%',
                    ),
                    'dms' => array(
                        'backend' => array(
                            'type' => 'jackrabbit',
                            'url'  => '%magnolia_url%',
                        ),
                        'workspace' => 'dms',
                        'username'  => '%magnolia_user%',
                        'password'  => '%magnolia_pass%',
                    ),
                ),
            ),

            // enable the ODM layer
            'odm' => array(
                'auto_generate_proxy_classes' => '%kernel.debug%',
                'document_managers' => array(
                    'default' => array(
                        'session'  => 'default',
                        'mappings' => array(
                            'SandboxMainBundle' => null,
                            'CmfContentBundle'  => null,
                            'CmfMenuBundle'     => null,
                            'CmfRoutingBundle'  => null,
                        ),
                    ),
                    'website' => array(
                        'session'          => 'website',
                        'configuration_id' => 'sandbox_magnolia.odm_configuration',
                        'mappings'         => array(
                            'SandboxMagnoliaBundle' => null,
                        ),
                    ),
                    'dms' => array(
                        'session'          => 'dms',
                        'configuration_id' => 'sandbox_magnolia.odm_configuration',
                        'mappings'         => array(
                            'SandboxMagnoliaBundle' => null,
                        ),
                    ),
                ),
            ),
        ));


Można uzyskać dostęp do menadżera poprzez rejestr menadżera w ``doctrine_phpcr``::

    /** @var $container \Symfony\Component\DependencyInjection\ContainerInterface */

    // get the named manager from the registry
    $dm = $container->get('doctrine_phpcr')->getManager('website');

    // get the manager for a specific document class
    $dm = $container->get('doctrine_phpcr')->getManagerForClass('CmfContentBundle:StaticContent');

Dodatkowo, każdy menadżer jest dostępny jako usługa w kontenerze DI.
Nazwą usługi jest ``doctrine_phpcr.odm.<name>_document_manager``, tak więc na
przykład, menadżer witryny internetowej ma nazwę
``doctrine_phpcr.odm.website_document_manager``.

.. index::
   pair: Doctrine; konfiguracja


Konfiguracja Doctrine
=====================

Pełna domyślna konfiguracja
---------------------------

.. configuration-block::

    .. code-block:: yaml

        doctrine:
            dbal:
                default_connection:   default
                types:
                    # A collection of custom types
                    # Example
                    some_custom_type:
                        class:                Acme\HelloBundle\MyCustomType
                        commented:            true
                # If enabled all tables not prefixed with sf2_ will be ignored by the schema
                # tool. This is for custom tables which should not be altered automatically.
                #schema_filter:        ^sf2_

                connections:
                    # A collection of different named connections (e.g. default, conn2, etc)
                    default:
                        dbname:               ~
                        host:                 localhost
                        port:                 ~
                        user:                 root
                        password:             ~
                        charset:              ~
                        path:                 ~
                        memory:               ~

                        # The unix socket to use for MySQL
                        unix_socket:          ~

                        # True to use as persistent connection for the ibm_db2 driver
                        persistent:           ~

                        # The protocol to use for the ibm_db2 driver (default to TCPIP if omitted)
                        protocol:             ~

                        # True to use dbname as service name instead of SID for Oracle
                        service:              ~

                        # The session mode to use for the oci8 driver
                        sessionMode:          ~

                        # True to use a pooled server with the oci8 driver
                        pooled:               ~

                        # Configuring MultipleActiveResultSets for the pdo_sqlsrv driver
                        MultipleActiveResultSets:  ~
                        driver:               pdo_mysql
                        platform_service:     ~

                        # the version of your database engine
                        server_version:       ~

                        # when true, queries are logged to a "doctrine" monolog channel
                        logging:              "%kernel.debug%"
                        profiling:            "%kernel.debug%"
                        driver_class:         ~
                        wrapper_class:        ~
                        options:
                            # an array of options
                            key:                  []
                        mapping_types:
                            # an array of mapping types
                            name:                 []
                        slaves:

                            # a collection of named slave connections (e.g. slave1, slave2)
                            slave1:
                                dbname:               ~
                                host:                 localhost
                                port:                 ~
                                user:                 root
                                password:             ~
                                charset:              ~
                                path:                 ~
                                memory:               ~

                                # The unix socket to use for MySQL
                                unix_socket:          ~

                                # True to use as persistent connection for the ibm_db2 driver
                                persistent:           ~

                                # The protocol to use for the ibm_db2 driver (default to TCPIP if omitted)
                                protocol:             ~

                                # True to use dbname as service name instead of SID for Oracle
                                service:              ~

                                # The session mode to use for the oci8 driver
                                sessionMode:          ~

                                # True to use a pooled server with the oci8 driver
                                pooled:               ~

                                # the version of your database engine
                                server_version:       ~

                                # Configuring MultipleActiveResultSets for the pdo_sqlsrv driver
                                MultipleActiveResultSets:  ~

            orm:
                default_entity_manager:  ~
                auto_generate_proxy_classes:  false
                proxy_dir:            "%kernel.cache_dir%/doctrine/orm/Proxies"
                proxy_namespace:      Proxies
                # search for the "ResolveTargetEntityListener" class for a cookbook about this
                resolve_target_entities: []
                entity_managers:
                    # A collection of different named entity managers (e.g. some_em, another_em)
                    some_em:
                        query_cache_driver:
                            type:                 array # Required
                            host:                 ~
                            port:                 ~
                            instance_class:       ~
                            class:                ~
                        metadata_cache_driver:
                            type:                 array # Required
                            host:                 ~
                            port:                 ~
                            instance_class:       ~
                            class:                ~
                        result_cache_driver:
                            type:                 array # Required
                            host:                 ~
                            port:                 ~
                            instance_class:       ~
                            class:                ~
                        connection:           ~
                        class_metadata_factory_name:  Doctrine\ORM\Mapping\ClassMetadataFactory
                        default_repository_class:  Doctrine\ORM\EntityRepository
                        auto_mapping:         false
                        hydrators:

                            # An array of hydrator names
                            hydrator_name:                 []
                        mappings:
                            # An array of mappings, which may be a bundle name or something else
                            mapping_name:
                                mapping:              true
                                type:                 ~
                                dir:                  ~
                                alias:                ~
                                prefix:               ~
                                is_bundle:            ~
                        dql:
                            # a collection of string functions
                            string_functions:
                                # example
                                # test_string: Acme\HelloBundle\DQL\StringFunction

                            # a collection of numeric functions
                            numeric_functions:
                                # example
                                # test_numeric: Acme\HelloBundle\DQL\NumericFunction

                            # a collection of datetime functions
                            datetime_functions:
                                # example
                                # test_datetime: Acme\HelloBundle\DQL\DatetimeFunction

                        # Register SQL Filters in the entity manager
                        filters:
                            # An array of filters
                            some_filter:
                                class:                ~ # Required
                                enabled:              false

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:doctrine="http://symfony.com/schema/dic/doctrine"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/doctrine
                http://symfony.com/schema/dic/doctrine/doctrine-1.0.xsd">

            <doctrine:config>
                <doctrine:dbal default-connection="default">
                    <doctrine:connection
                        name="default"
                        dbname="database"
                        host="localhost"
                        port="1234"
                        user="user"
                        password="secret"
                        driver="pdo_mysql"
                        driver-class="MyNamespace\MyDriverImpl"
                        path="%kernel.data_dir%/data.sqlite"
                        memory="true"
                        unix-socket="/tmp/mysql.sock"
                        wrapper-class="MyDoctrineDbalConnectionWrapper"
                        charset="UTF8"
                        logging="%kernel.debug%"
                        platform-service="MyOwnDatabasePlatformService"
                        server-version="5.6"
                    >
                        <doctrine:option key="foo">bar</doctrine:option>
                        <doctrine:mapping-type name="enum">string</doctrine:mapping-type>
                    </doctrine:connection>
                    <doctrine:connection name="conn1" />
                    <doctrine:type name="custom">Acme\HelloBundle\MyCustomType</doctrine:type>
                </doctrine:dbal>

                <doctrine:orm
                    default-entity-manager="default"
                    auto-generate-proxy-classes="false"
                    proxy-namespace="Proxies"
                    proxy-dir="%kernel.cache_dir%/doctrine/orm/Proxies"
                >
                    <doctrine:entity-manager
                        name="default"
                        query-cache-driver="array"
                        result-cache-driver="array"
                        connection="conn1"
                        class-metadata-factory-name="Doctrine\ORM\Mapping\ClassMetadataFactory"
                    >
                        <doctrine:metadata-cache-driver
                            type="memcache"
                            host="localhost"
                            port="11211"
                            instance-class="Memcache"
                            class="Doctrine\Common\Cache\MemcacheCache"
                        />

                        <doctrine:mapping name="AcmeHelloBundle" />

                        <doctrine:dql>
                            <doctrine:string-function name="test_string">
                                Acme\HelloBundle\DQL\StringFunction
                            </doctrine:string-function>

                            <doctrine:numeric-function name="test_numeric">
                                Acme\HelloBundle\DQL\NumericFunction
                            </doctrine:numeric-function>

                            <doctrine:datetime-function name="test_datetime">
                                Acme\HelloBundle\DQL\DatetimeFunction
                            </doctrine:datetime-function>
                        </doctrine:dql>
                    </doctrine:entity-manager>

                    <doctrine:entity-manager name="em2" connection="conn2" metadata-cache-driver="apc">
                        <doctrine:mapping
                            name="DoctrineExtensions"
                            type="xml"
                            dir="%kernel.root_dir%/../vendor/gedmo/doctrine-extensions/lib/DoctrineExtensions/Entity"
                            prefix="DoctrineExtensions\Entity"
                            alias="DExt"
                        />
                    </doctrine:entity-manager>
                </doctrine:orm>
            </doctrine:config>
        </container>

Przegląd Konfiguracji
---------------------

Poniższy przykład konfiguracji pokazuje wszystkie domyślne ustawienia konfiguracji
rozpoznawane przez ORM:

.. code-block:: yaml
   
    doctrine:
        orm:
            auto_mapping: true
            # the standard distribution overrides this to be true in debug, false otherwise
            auto_generate_proxy_classes: false
            proxy_namespace: Proxies
            proxy_dir: "%kernel.cache_dir%/doctrine/orm/Proxies"
            default_entity_manager: default
            metadata_cache_driver: array
            query_cache_driver: array
            result_cache_driver: array

Istnieje jeszcze wiele innych opcji konfiguracyjnych które można użyć do
zastąpienia niektórych klas, ale jest to już zastosowanie bardzo zaawansowane.

Sterowniki buforowania
~~~~~~~~~~~~~~~~~~~~~~

Dla sterowników buforowania można ustawić następujące wartości ``array``, ``apc``,
``memcache``, ``memcached``, ``redis``, ``wincache``, ``zenddata``, ``xcache``
lub ``service``.

Poniższy przykład pokazuje ogólny zarys konfiguracji buforowania:

.. code-block:: yaml
      
   doctrine:
        orm:
            auto_mapping: true
            metadata_cache_driver: apc
            query_cache_driver:
                type: service
                id: my_doctrine_common_cache_service
            result_cache_driver:
                type: memcache
                host: localhost
                port: 11211
                instance_class: Memcache
    

Konfiguracja mapowania
~~~~~~~~~~~~~~~~~~~~~~

Niezbędną konfiguracją dla ORM jest tylko jawne zdefiniowanie wszystkich
odwzorowywanych dokumentów i ma ona kilka opcji konfiguracyjnych, które
można kontrolować. Dla odwzorowań istnieją następujące opcje konfiguracyjne:

type
....

Przyjmuje wartości ``annotation``, ``xml``, ``yml``, ``php`` lub ``staticphp``.
Opcja określa typ metadanych stosowany w mapowaniu.

dir
...

Ścieżka do plików odwzorowań lub encji (w zależności od sterownika).
Jeśli jest to ścieżka względna, to odnosi się ona do katalogu pakietu. Działa
to tylko wtedy, gdy nazwa odwzorowań jest taka sama jak nazwa pakietu. Jeżeli
chce się użyć tej opcji do określenia ścieżki bezwzględnej, to należy podać
przedrostek ścieżki z parametrami *kernel*, które istnieją w DIC (na przykład
``%kernel.root_dir%``).

prefix
......

Wspólny przedrostek przestrzeni nazw dla wszystkich encji z tego
udziału odwzorowań. Przedrostek ten nie powinien kolidować z przedrostkami innych
definicji odwzorowań, gdyż w takim przypadku encje nie będą mogły być odnalezione
przez Doctrine. Opcja ta domyślnie przyjmuje wartość nazwy pakietu + ``Entity``.
Przykładowo, dla pakietu aplikacji o nazwie ``AcmeHelloBundle`` przedrostkiem będzie
``Acme\HelloBundle\Entity``.

alias
.....

W celu uproszczenia, Doctrine oferuje możliwość aliasowanie nazw
przestrzeni nazw encji przez używanie w zapytaniach DQL lub przy dostępie do
repozytorium krótkich nazw. W przypadku używania pakietu, domyślną wartością
aliasu jest nazwa pakietu.

is_bundle
.........

Wartość tej opcji jest pochodną wartością opcji ``dir`` i domyślnie
jest to *true*, jeśli wartość ``dir`` jest adresem względnym dla którego funkcja
``file_exists()`` zwraca *false*. Gdy sprawdzenie istnienia pliku zwraca *true*,
to jest wartość *false*. W takim przypadku zostaje określona ścieżka bezwzględna
a pliki metadanych prawdopodobnie znajdują się poza pakietem.


.. index::
    single: konfiguracja; Doctrine DBAL
    single: Doctrine; konfiguracja DBAL

.. _`reference-dbal-configuration`:


Konfiguracja Doctrine DBAL
--------------------------

DoctrineBundle obsługuje wszystkie parametry które są akceptowane przez sterowniki
Doctrine, przekonwertowane na standardy nazewnicze XML lub YAML egzekwowane przez
Symfony. Proszę przeczytać dokumentację Doctrine `DBAL documentation`_ w celu
uzyskania większej ilości informacji. Poniższy przykład pokazuje wszystkie możliwe
opcje konfiguracyjne:

.. configuration-block::

    .. code-block:: yaml

        doctrine:
            dbal:
                dbname:               database
                host:                 localhost
                port:                 1234
                user:                 user
                password:             secret
                driver:               pdo_mysql
                # the DBAL driverClass option
                driver_class:         MyNamespace\MyDriverImpl
                # the DBAL driverOptions option
                options:
                    foo: bar
                path:                 "%kernel.data_dir%/data.sqlite"
                memory:               true
                unix_socket:          /tmp/mysql.sock
                # the DBAL wrapperClass option
                wrapper_class:        MyDoctrineDbalConnectionWrapper
                charset:              UTF8
                logging:              "%kernel.debug%"
                platform_service:     MyOwnDatabasePlatformService
                server_version:       5.6
                mapping_types:
                    enum: string
                types:
                    custom: Acme\HelloBundle\MyCustomType
                # the DBAL keepSlave option
                keep_slave:           false

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:doctrine="http://symfony.com/schema/dic/doctrine"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/doctrine
                http://symfony.com/schema/dic/doctrine/doctrine-1.0.xsd"
        >

            <doctrine:config>
                <doctrine:dbal
                    name="default"
                    dbname="database"
                    host="localhost"
                    port="1234"
                    user="user"
                    password="secret"
                    driver="pdo_mysql"
                    driver-class="MyNamespace\MyDriverImpl"
                    path="%kernel.data_dir%/data.sqlite"
                    memory="true"
                    unix-socket="/tmp/mysql.sock"
                    wrapper-class="MyDoctrineDbalConnectionWrapper"
                    charset="UTF8"
                    logging="%kernel.debug%"
                    platform-service="MyOwnDatabasePlatformService"
                    server-version="5.6">

                    <doctrine:option key="foo">bar</doctrine:option>
                    <doctrine:mapping-type name="enum">string</doctrine:mapping-type>
                    <doctrine:type name="custom">Acme\HelloBundle\MyCustomType</doctrine:type>
                </doctrine:dbal>
            </doctrine:config>
        </container>

.. note::

    Opcja ``server_version`` została dodana w Doctrine DBAL 2.5, która jest
    używana przez DoctrineBundle 1.3. Wartość tej opcji powinna być zgodna z
    wersją serwera bazy danych (uzyj polecenie ``postgres -V`` lub ``psql -V``,
    aby odczytać wersję PostgreSQL a ``mysql -V`` do odczytania wersji MySQL).

    Jeśłi nie zdefinuije sie tej opcji i nie ma się jeszcze utworzonej bazy danych,
    to może pojawić się błąd ``PDOException``, ponieważ Doctrine będzie próbowało
    automatycznie zgadywać wersję serwera bazy danych a on nie będzie dostępn.

Jeżeli w pliku YAML chce się skonfigurować wiele połączeń, należy je umieścić w
kluczu ``connections`` i nadać im unikalna nazwę:

.. code-block:: yaml

    doctrine:
        dbal:
            default_connection:       default
            connections:
                default:
                    dbname:           Symfony
                    user:             root
                    password:         null
                    host:             localhost
                    server_version:   5.6
                customer:
                    dbname:           customer
                    user:             root
                    password:         null
                    host:             localhost
                    server_version:   5.7

Usługa ``database_connection`` zawsze odnosi się do połączenia *default*,
które jest skonfigurowane pierwsze lub połączenia skonfigurowanego w parametrze
``default_connection``.

Każde z połączeń jest także dostępne poprzez usługę ``doctrine.dbal.[name]_connection``
gdzie ``[name]`` jest nazwą połączenia.

Shortened Configuration Syntax
------------------------------

When you are only using one entity manager, all config options available
can be placed directly under ``doctrine.orm`` config level.

.. code-block:: yaml

    doctrine:
        orm:
            # ...
            query_cache_driver:
               # ...
            metadata_cache_driver:
                # ...
            result_cache_driver:
                # ...
            connection: ~
            class_metadata_factory_name:  Doctrine\ORM\Mapping\ClassMetadataFactory
            default_repository_class:  Doctrine\ORM\EntityRepository
            auto_mapping: false
            hydrators:
                # ...
            mappings:
                # ...
            dql:
                # ...
            filters:
                # ...

This shortened version is commonly used in other documentation sections.
Keep in mind that you can't use both syntaxes at the same time.

Custom Mapping Entities in a Bundle
-----------------------------------

Doctrine's ``auto_mapping`` feature loads annotation configuration from
the ``Entity/`` directory of each bundle *and* looks for other formats (e.g.
YAML, XML) in the ``Resources/config/doctrine`` directory.

If you store metadata somewhere else in your bundle, you can define your
own mappings, where you tell Doctrine exactly *where* to look, along with
some other configurations.

If you're using the ``auto_mapping`` configuration, you just need to overwrite
the configurations you want. In this case it's important that the key of
the mapping configurations corresponds to the name of the bundle.

For example, suppose you decide to store your ``XML`` configuration for
``AppBundle`` entities in the ``@AppBundle/SomeResources/config/doctrine``
directory instead:

.. configuration-block::

    .. code-block:: yaml

        doctrine:
            # ...
            orm:
                # ...
                auto_mapping: true
                mappings:
                    # ...
                    AppBundle:
                        type: xml
                        dir: SomeResources/config/doctrine

    .. code-block:: xml

        <?xml version="1.0" charset="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:doctrine="http://symfony.com/schema/dic/doctrine">

            <doctrine:config>
                <doctrine:orm auto-mapping="true">
                    <mapping name="AppBundle" dir="SomeResources/config/doctrine" type="xml" />
                </doctrine:orm>
            </doctrine:config>
        </container>

    .. code-block:: php

        $container->loadFromExtension('doctrine', array(
            'orm' => array(
                'auto_mapping' => true,
                'mappings' => array(
                    'AppBundle' => array('dir' => 'SomeResources/config/doctrine', 'type' => 'xml'),
                ),
            ),
        ));

Mapping Entities Outside of a Bundle
------------------------------------

You can also create new mappings, for example outside of the Symfony folder.

For example, the following looks for entity classes in the ``App\Entity``
namespace in the ``src/Entity`` directory and gives them an ``App`` alias
(so you can say things like ``App:Post``):

.. configuration-block::

    .. code-block:: yaml

        doctrine:
                # ...
                orm:
                    # ...
                    mappings:
                        # ...
                        SomeEntityNamespace:
                            type: annotation
                            dir: "%kernel.root_dir%/../src/Entity"
                            is_bundle: false
                            prefix: App\Entity
                            alias: App

    .. code-block:: xml

        <?xml version="1.0" charset="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:doctrine="http://symfony.com/schema/dic/doctrine">

            <doctrine:config>
                <doctrine:orm>
                    <mapping name="SomeEntityNamespace"
                        type="annotation"
                        dir="%kernel.root_dir%/../src/Entity"
                        is-bundle="false"
                        prefix="App\Entity"
                        alias="App"
                    />
                </doctrine:orm>
            </doctrine:config>
        </container>

    .. code-block:: php

        $container->loadFromExtension('doctrine', array(
            'orm' => array(
                'auto_mapping' => true,
                'mappings' => array(
                    'SomeEntityNamespace' => array(
                        'type'      => 'annotation',
                        'dir'       => '%kernel.root_dir%/../src/Entity',
                        'is_bundle' => false,
                        'prefix'    => 'App\Entity',
                        'alias'     => 'App',
                    ),
                ),
            ),
        ));

Detecting a Mapping Configuration Format
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If the ``type`` on the bundle configuration isn't set, the DoctrineBundle
will try to detect the correct mapping configuration format for the bundle.

DoctrineBundle will look for files matching ``*.orm.[FORMAT]`` (e.g.
``Post.orm.yml``) in the configured ``dir`` of your mapping (if you're mapping
a bundle, then ``dir`` is relative to the bundle's directory).

The bundle looks for (in this order) XML, YAML and PHP files.
Using the ``auto_mapping`` feature, every bundle can have only one
configuration format. The bundle will stop as soon as it locates one.

If it wasn't possible to determine a configuration format for a bundle,
the DoctrineBundle will check if there is an ``Entity`` folder in the bundle's
root directory. If the folder exist, Doctrine will fall back to using an
annotation driver.

Default Value of Dir
~~~~~~~~~~~~~~~~~~~~

If ``dir`` is not specified, then its default value depends on which configuration
driver is being used. For drivers that rely on the PHP files (annotation,
staticphp) it will be ``[Bundle]/Entity``. For drivers that are using
configuration files (XML, YAML, ...) it will be
``[Bundle]/Resources/config/doctrine``.

If the ``dir`` configuration is set and the ``is_bundle`` configuration
is ``true``, the DoctrineBundle will prefix the ``dir`` configuration with
the path of the bundle.


.. _DBAL documentation: http://www.doctrine-project.org/docs/dbal/2.0/en
.. _`DQL User Defined Functions`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/cookbook/dql-user-defined-functions.html

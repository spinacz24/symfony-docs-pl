.. index::
   single: Doctrine; konfiguracja ORM - informator
   single: konfiguracja; Doctrine ORM

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

Omówienie konfiguracji
----------------------

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

.. _DBAL documentation: http://docs.doctrine-project.org/projects/doctrine-dbal/en/latest/reference/configuration.html

Składnia skróconej konfiguracji
-------------------------------

Gdy używa się tylko jednego menadżera encji, wszystkie dostępne opcje konfiguracyjne
mozna umieścić bezpośrednio na poziomie ``doctrine.orm`` konfiguracji.

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

Ta skrócona wersja jest powszechnie stosowana w innych rozdziałach dokumentacji.
Trzeba pamiętać, że nie można stosować w tym samym czasie obu składni konfiguracyjnych.

Indywidualne mapowanie encji w pakiecie
---------------------------------------

Funkcjonalność ``auto_mapping`` Doctrine ładuje konfigurację adnotacji z katalogu
``Entity/`` pakietu i wyszukuje inne formaty (np. YAML, XML) w katalogu
``Resources/config/doctrine``.

W przypadku przechowywania metadanych gdzieś w pakiecie, można zdefiniować własne
mapowania, dokładnie powiadamiając Doctrine gdzie ma wyszukiwać konfigurację tego
mapowania wraz z innymi konfiguracjami.

Jeśli używa się konfiguracji ``auto_mapping``, wystarczy przesłonić te konfiguracje,
tak jak się to potrzebuje. W takim przypadku ważne jest, aby klucz konfiguracji
mapowania odpowiadał nazwie pakietu.

Dla przykłady przyjmijmy, że zdecydowaliśmy się przechowywać konfigurację ``XML``
dla encji pakietu ``AppBundle`` w katalogu ``@AppBundle/SomeResources/config/doctrine``:

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

Mapowanie encji poza pakietem
-----------------------------

Mozna również utworzyć nowe mapowania poza folderem Symfony.

Na przyklad, poniższa konfiguracja wyszukuje klas encji w przestrzeni nazewniczej
``App\Entity`` w katalogu ``src/Entity`` i nadaje alias ``App`` (tak, aby można
było odwoływać sie do takich rzeczy jak ``App:Post``):

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

Wykrywanie formatu konfiguracji mapowania
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Gdy opcja ``type`` konfiguracji pakietu nie jest ustawiona, DoctrineBundle
będzie próbował wykryć właściwy fomat konfiguracji mapowania dla danego pakietu.

DoctrineBundle będzie wyszukiwał pliki z nazwą pasującą do ``*.orm.[FORMAT]``
(np. ``Post.orm.yml``) w katalogu skonfigurowanym w opcji ``dir`` mapowania
(jeśli mapowany jest pakiet, to ``dir`` jest odnoszony do katalogu pakietu).

Pakiet DoctrineBundle wyszukuje pliki XML, YAML i PHP (w tej właśnie kolejności).
Korzystając z funkcjonalności ``auto_mapping`` każdy pakiet może mieć tylko jeden
format konfiguracyji. Pakiet zostanie zatrzymany, gdy tylko znajdzie plik konfiguracyjny.

Jeśli nie będzie możliwe określenie formatu konfiguracji dla pakietu,
DoctrineBundle sprawdza czy istnieje folder ``Entity`` w katalogu głównym pakietu.
Gdy taki folder istnieje, Doctrine awaryjnie uzyje sterownika adnotacji.

Domyślna wartość ``dir``
~~~~~~~~~~~~~~~~~~~~~~~~

W przypadku nie określenia wartości ``dir``, domyślna wartość zależy od tego, czy
skonfigurowany został do zastosowania sterownik. Dla sterowników, które opierają
się na plikach PHP (adnotacje, 'staticphp') bedzi to wartość ``[Bundle]/Entity``.
Dla sterowników, które wykorzystują pliki konfiguracyjne (XML, YAML, ...) będzie
to wartość ``[Bundle]/Resources/config/doctrine``.

Gdy jest ustawiona opcja ``dir`` a opcja ``is_bundle`` ma wartość ``true``,
DoctrineBundle będzie dodawał do wartości ``dir`` przedrostek ze ścieżką pakietu.

.. _`DQL User Defined Functions`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/cookbook/dql-user-defined-functions.html

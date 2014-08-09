.. index::
    single: inicjatory; DoctrinePHPCRBundle
    single: konfiguratory testowania; DoctrinePHPCRBundle
    single: migracje; DoctrinePHPCRBundle

Utrzymywanie danych w repozytorium
==================================

PHPCR-ODM dostarcza *inicjatory*, które zapewniają, ze repozytorium jest gotowe
do zastosowania produkcyjnego, *migratory* do programowego ładowania danych
i *konfiguratory testowania* dla obsługi testowania i demonstracji danych testowych.

.. note:: *Przypis tłumacza*.
   Dla przypomnienia, angielskie słowo *fixtures* jest tu tłumczone, w zależności
   od kontekstu, albo jako *konfigurator testowania* albo jako *dane testowe*.

.. _phpcr-odm-repository-initializers:

Inicjatory repozytorium
-----------------------

Inicjator jest równoważnikiem narzędzi schematu ORM PHPCR. Jest używany do umożliwienia
pakietom rejestracji typów węzłów PHPCR i utworzenia wymaganych ścieżek bazowych
w repozytorium.

.. note::

    Potrzebna jest koncepcja ścieżek, ponieważ nie ma żadnych oddzielnych "tabel"
    jak w relacyjnych bazach danych. Dodawanie dokumentu wymaga wcześniejszego
    podania w repozytorium ścieżki rodzica 

Inicjatory implementują ``Doctrine\Bundle\PHPCRBundle\Initializer\InitializerInterface``.
Jeśli nie jest potrzebna specjalna logika i chce się utworzyć zwykłe węzły PHPCR
a nie dokumenty, można po prostu zdefiniować usługi w ``GenericInitializer``.
Ogólny inicjator oczekuje nazwy do identyfikacji inicjatora, tablicę ścieżek repozytorium,
które będą tworzone, jeśli nie istnieją i opcjonalnie łańcuch definiujący przestrzeń
nazw oraz podstawowe lub wstawione typy węzłów w języku CND, które powinny być
zarejestrowane w repozytorium.

.. versionadded:: 1.1
    Począwszy od wersji 1.1, ``GenericInitializer`` oczekuje parametru nazwy
    jako pierwszego argumentu. W wersji 1.0 nie było sposobu na określenie
    własnej nazwy dla ogólnego inicjatora.

Usługa wykorzystująca ogólny inicjator wygląda nazstępująco:

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/ContentBundle/Resources/config/services.yml
        acme_content.phpcr.initializer:
            class: Doctrine\Bundle\PHPCRBundle\Initializer\GenericInitializer
            arguments:
                - AcmeContentBundle Basepaths
                - [ "/my/content", "/my/menu" ]
                - "%acme.cnd%"
            tags:
                - { name: "doctrine_phpcr.initializer" }

    .. code-block:: xml

        <!-- src/Acme/ContentBundle/Resources/config/services.xml -->
        <service id="acme_content.phpcr.initializer"
                 class="Doctrine\Bundle\PHPCRBundle\Initializer\GenericInitializer">
            <argument>AcmeContentBundle Basepaths</argument>
            <argument type="collection">
                <argument>/my/content</argument>
                <argument>/my/menu</argument>
            </argument>
            <argument>%acme.cnd%</argument>
            <tag name="doctrine_phpcr.initializer"/>
        </service>

    .. code-block:: php

        use Symfony\Component\DependencyInjection\Definition

        // ...

        $definition = new Definition(
            'Doctrine\Bundle\PHPCRBundle\Initializer\GenericInitializer',
            array(
                'AcmeContentBundle Basepaths',
                array('/my/content', '/my/menu'),
                '%acme.cnd%',
            )
        ));
        $definition->addTag('doctrine_phpcr.initializer');
        $container->setDefinition('acme_content.phpcr.initializer', $definition);

Można uruchamiać własne inicjatory stosując następujące polecenie:

.. code-block:: bash

    $ php app/console doctrine:phpcr:repository:init

.. versionadded:: 1.1
    Począwszy od DoctrinePHPCRBundle 1.1 polecenie ładujące dane testowe będzie
    automatycznie wykonywało kod inicjatorów po usunięciu danych z bazy danych,
    przed uruchomieniem konfiguratorów testowania.

Ogólny inicjator tylko tworzy węzły PHPCR. Jeśli chce się utworzyć określone
dokumenty, potrzebny jest własny inicjator. Interesującą metodą nadpisywania
jest metoda ``init``. Przekazuje ona ``ManagerRegistry``, z którego można pobrać
sesję PHPCR, ale również menadżera dokumentów::

    // src/Acme/BasicCmsBundle/Initializer/SiteInitializer.php
    namespace Acme\ContentBundle\Initializer;

    use Doctrine\Bundle\PHPCRBundle\Initializer\InitializerInterface;
    use PHPCR\SessionInterface;
    use PHPCR\Util\NodeHelper;

    class SiteInitializer implements InitializerInterface
    {
        private $basePath;

        public function __construct($basePath = '/cms')
        {
            $this->basePath = $basePath;
        }

        public function init(ManagerRegistry $registry)
        {
            $dm = $registry->getManagerForClass('Acme\BasicCmsBundle\Document\Site');
            if ($dm->find(null, $this->basePath)) {
                return;
            }

            $site = new Acme\BasicCmsBundle\Document\Site();
            $site->setId($this->basePath);
            $dm->persist($site);
            $dm->flush();

            $session = $registry->getConnection();
            // create the 'cms', 'pages', and 'posts' nodes
            NodeHelper::createPath($session, '/cms/pages');
            NodeHelper::createPath($session, '/cms/posts');
            NodeHelper::createPath($session, '/cms/routes');

            $session->save();
        }

        public function getName()
        {
            return 'Site Initializer';
        }
    }

.. versionadded:: 1.1
    Od wersji 1.1, metoda init jest przekazywana do ``ManagerRegistry`` zamiast
    do ``SessionInterface`` PHPCR, aby umożliwić tworzenie dokumentów w inicjatorach.
    W wersji 1.0 trzeba ręcznie ustawić właściwość ``phpcr:class``, aby otrzymać
    prawidłową wartość.

Zdefiniujmy usługę dla inicjatora:

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/BasicCmsBundle/Resources/config/config.yml
        services:
            # ...
            acme_content.phpcr.initializer.site:
                class: Acme\BasicCmsBundle\Initializer\SiteInitializer
                tags:
                    - { name: doctrine_phpcr.initializer }

    .. code-block:: xml

        <!-- src/Acme/BasicCmsBUndle/Resources/config/config.php
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:acme_demo="http://www.example.com/symfony/schema/"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                 http://symfony.com/schema/dic/services/services-1.0.xsd">

            <!-- ... -->
            <services>
                <!-- ... -->
                <service id="acme_content.phpcr.initializer.site"
                    class="Acme\BasicCmsBundle\Initializer\SiteInitializer">
                    <tag name="doctrine_phpcr.initializer"/>
                </service>
            </services>

        </container>

    .. code-block:: php

        // src/Acme/BasicCmsBundle/Resources/config/config.php

        //  ...
        $container
            ->register(
                'acme_content.phpcr.initializer.site',
                'Acme\BasicCmsBundle\Initializer\SiteInitializer'
            )
            ->addTag('doctrine_phpcr.initializer', array('name' => 'doctrine_phpcr.initializer')
        ;

Ładowanie migracji
------------------

DoctrinePHPCRBundle jest również dostarczany z prostym poleceniem uruchamiającym
skrypty migracyjne. Migracje powinny implementować
``Doctrine\Bundle\PHPCRBundle\Migrator\MigratorInterface`` i być rejestrowane jako
usługa ze znacznikiem ``doctrine_phpcr.migrator`` zawierającym atrybut ``alias``,
jednoznacznie identyfikującym migratora. Istnieje opcjonalna klasa
``Doctrine\Bundle\PHPCRBundle\Migrator\AbstractMigrator`` używana jako podstawa.

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/ContentBundle/Resources/config/services.yml
        acme.demo.migration.foo:
            class: Acme\DemoBundle\Migration\Foo
            arguments:
                - { "%acme.content_basepath%", "%acme.menu_basepath%" }
            tags:
                - { name: "doctrine_phpcr.migrator", alias: "acme.demo.migration.foo" }

    .. code-block:: xml

        <!-- src/Acme/ContentBundle/Resources/config/services.xml -->
        <?xml version="1.0" ?>
        <container xmlns="http://symfony.com/schema/dic/services">
            <service id="acme.demo.migration.foo"
                     class="Acme\DemoBundle\Migration\Foo">
                <argument type="collection">
                    <argument>%acme.content_basepath%</argument>
                    <argument>%acme.menu_basepath%</argument>
                </argument>

                <tag name="doctrine_phpcr.migrator" alias="acme.demo.migration.foo"/>
            </service>
        </container>

    .. code-block:: php

        use Symfony\Component\DependencyInjection\Definition

        // ...
        $definition = new Definition('Acme\DemoBundle\Migration\Foo', array(
            array(
                '%acme.content_basepath%',
                '%acme.menu_basepath%',
            ),
        )));
        $definition->addTag('doctrine_phpcr.migrator', array('alias' => 'acme.demo.migration.foo'));

        $container->setDefinition('acme.demo.migration.foo', $definition);

Aby dowiedzieć się, czy są dostępne migracje, uruchommmy:

.. code-block:: bash

    $ php app/console doctrine:phpcr:migrator:migrate

Następnie przekażmy w nazwie uruchamianego migratora, opcjonalne argumenty
``--identifier``, ``--depth`` lub ``--session``. Ostatni argument określa nazwę
sesji do ustawienia w migracji, podczas gdy dwa pierwsze argumenty będą przekazywane
do metody ``migrate()`` migratora.

.. tip::

    Jeśli nie musi się reprodukować wyników, to prostą alternatywą może być
    eksport części repozytorium i ponowny import na docelowym serwerze.
    Jest to opisane w rozdziale :ref:`phpcr-odm-backup-restore`.

.. _phpcr-odm-repository-fixtures:

Ładowanie danych testowych
--------------------------

Do stosowania polecenia ``doctrine:phpcr:fixtures:load``, trzeba dodatkowo
zainstalować `DoctrineFixturesBundle`_, co jest odpowiednikiem
`Doctrine data-fixtures`_ w Symfony2.

Konfiguratory testowania działają w ten sam sposób jak w Doctrine ORM.
Trzeba napisać implementację klasę konfiguratora implementującą interfejs
``Doctrine\Common\DataFixtures\FixtureInterface``. Jeśli umieści się go w
``<Bundle>\DataFixtures\PHPCR``, to będzie on automatycznie wykrywany, jeśli
w poleceniu nie określi się ścieżki.

Prosty przykład klasy konfiguratora wygląda tak::

    // src/Acme/MainBundle/DataFixtures/PHPCR/LoadPageData.php
    namespace Acme\MainBundle\DataFixtures\PHPCR;

    use Doctrine\Common\Persistence\ObjectManager;
    use Doctrine\Common\DataFixtures\FixtureInterface;

    class LoadPageData implements FixtureInterface
    {
        public function load(ObjectManager $manager)
        {
            // ... create and persist your data here
        }
    }

W celu poznania wiecej informacji o konfiguratorach testowych, proszę zapoznać się
z `dokumentacja pakietu DoctrineFixturesBundle <DoctrineFixturesBundle>`_.

.. _`DoctrineFixturesBundle`: http://symfony.com/doc/current/bundles/DoctrineFixturesBundle/index.html
.. _`Doctrine data-fixtures`: https://github.com/doctrine/data-fixtures

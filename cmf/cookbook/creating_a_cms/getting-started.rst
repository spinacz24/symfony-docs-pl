Rozpoczynamy
------------

Rozpoczęcie projektu
~~~~~~~~~~~~~~~~~~~~

Najpierw trzeba wykonać ogólne czynności omówione w :doc:`../../bundles/phpcr_odm/introduction`
w celu utworzenia projektu używającego PHPCR-ODM.

Instalacja dodatkowych pakietów
...............................

Przykład omówiony w tym poradniku wymaga następujących pakietów:

* `symfony-cmf/routing-auto-bundle`_;
* `sonata-project/doctrine-phpcr-admin-bundle`_;
* `doctrine/data-fixtures`_;
* `symfony-cmf/menu-bundle`_.

Każda część tego poradnika będzie poświęcona odrębnemu pakietowi.

Jeśli masz zamiar przerobić cały ten poradnik, to możesz zaoszczędzić trochę 
czasu dodając teraz wszystkie wymagane pakiety.

.. code-block:: javascript

    {
        ...
        require: {
            ...
            "symfony-cmf/routing-auto-bundle": "1.0.*@alpha",
            "symfony-cmf/menu-bundle": "1.1.*",
            "sonata-project/doctrine-phpcr-admin-bundle": "1.1.*",
            "symfony-cmf/tree-browser-bundle": "1.1.*",
            "doctrine/data-fixtures": "1.0.*",

            "doctrine/phpcr-odm": "1.1.*",
            "phpcr/phpcr-utils": "1.1.*",
            "doctrine/phpcr-bundle": "1.1.*",
            "symfony-cmf/routing-bundle": "1.2.*",
            "symfony-cmf/routing": "1.2.*"
        },
        ...
    }

Zmodyfikuj swój plik ``composer.json``, który niezbędny jest do
uruchomienia polecenia ``composer update``.

Zainicjowanie bazy danych
.........................

Postępując zgodnie z główną instrukcją zawartą w :doc:`../../bundles/phpcr_odm/introduction`,
użyj zaplecza PHPCR `Doctrine DBAL Jackalope`_ dla MySQL i utwórz bazę danych:

.. code-block:: bash

    $ php app/console doctrine:database:create

Spowoduje to utworzenie bazy danych, zgodnie z plikiem konfiguracyjnym
``parameters.yml``.

Zaplecze Doctrine DBAL musi zostać zainicjowane. Następujące polecenie utworzy
schemat MySQL do przechowywania hierarchicznej treści węzłów repozytorium treści
PHPCR:

.. code-block:: bash

    $ php app/console doctrine:phpcr:init:dbal

.. note::

    Implementacja `Apache Jackrabbit`_ jest rozwiązaniem opartym na Java i nie
    wymaga takiej inicjalizacji, jednak wymaga użycia Java.

Wygenerowanie pakietu
.....................

Teraz można wygenerować pakiet, w którym będziemy pisać większość kodu:

.. code-block:: bash

    $ php app/console generate:bundle --namespace=Acme/BasicCmsBundle --dir=src --no-interaction

Dokumenty
.........

Utworzy to dwie klasy dokumentów, jedna dla stron a drugą dla wpisów.
Te dwa dokumenty udostępniają w dużym stopniu tą samą logikę, więc utworzymy
cechę (``trait``) aby zmniejszyć powielanie kodu::

    // src/Acme/BasicCmsBundle/Document/ContentTrait.php
    namespace Acme\BasicCmsBundle\Document;

    use Doctrine\ODM\PHPCR\Mapping\Annotations as PHPCR;

    trait ContentTrait
    {
        /**
         * @PHPCR\Id()
         */
        protected $id;

        /**
         * @PHPCR\ParentDocument()
         */
        protected $parent;

        /**
         * @PHPCR\NodeName()
         */
        protected $title;

        /**
         * @PHPCR\String(nullable=true)
         */
        protected $content;

        /**
         * @PHPCR\Referrers(
         *     referringDocument="Symfony\Cmf\Bundle\RoutingBundle\Doctrine\Phpcr\Route",
         *     referencedBy="content"
         * )
         */
        protected $routes;

        public function getId()
        {
            return $this->id;
        }

        public function getParentDocument()
        {
            return $this->parent;
        }

        public function setParentDocument($parent)
        {
            $this->parent = $parent;
        }

        public function getTitle()
        {
            return $this->title;
        }

        public function setTitle($title)
        {
            $this->title = $title;
        }

        public function getContent()
        {
            return $this->content;
        }

        public function setContent($content)
        {
            $this->content = $content;
        }

        public function getRoutes()
        {
            return $this->routes;
        }
    }

.. note::

    Cechy (*ang. traits*) są dostępne dopiero od wersji PHP 5.4. Jeśli używa się
    mniejszą wersję PHP, można skopiować powyższy kod do każdej klasy, aby uzyskać
    ten efekt. Nie można jednak rozszerzyć jednej klasy przez druga, gdyż spowoduje
    to później niezamierzone zachowanie przy integracji z interfejsem administracyjnym.

Klasa ``Page`` jest teraz przejrzysta i prosta::

    // src/Acme/BasicCmsBundle/Document/Page.php
    namespace Acme\BasicCmsBundle\Document;

    use Symfony\Cmf\Component\Routing\RouteReferrersReadInterface;

    use Doctrine\ODM\PHPCR\Mapping\Annotations as PHPCR;

    /**
     * @PHPCR\Document(referenceable=true)
     */
    class Page implements RouteReferrersReadInterface
    {
        use ContentTrait;
    }

Należy mieć na uwadze, że dokument strony powinien być referencyjny (zdolnym do
bycia celem odniesień w innych dokumentach). Umożliwia to innym dokumentom posiadanie
odniesień do tej strony. Klasa ``Post`` będzie również referencyjna i dodatkowo
będzie automatycznie ustawiać datę wykorzystując `zdarzenie cyklu życia przed utrwaleniem`_,
jeśli nie zostało to wcześniej ustawione w sposób jawny::

    // src/Acme/BasicCms/Document/Post.php
    namespace Acme\BasicCmsBundle\Document;

    use Doctrine\ODM\PHPCR\Mapping\Annotations as PHPCR;
    use Symfony\Cmf\Component\Routing\RouteReferrersReadInterface;

    /**
     * @PHPCR\Document(referenceable=true)
     */
    class Post implements RouteReferrersReadInterface
    {
        use ContentTrait;

        /**
         * @PHPCR\Date()
         */
        protected $date;

        /**
         * @PHPCR\PrePersist()
         */
        public function updateDate()
        {
            if (!$this->date) {
                $this->date = new \DateTime();
            }
        }

        public function getDate()
        {
            return $this->date;
        }

        public function setDate(\DateTime $date)
        {
            $this->date = $date;
        }
    }

Zarówno klasa ``Post`` jaki ``Page`` implementują interfejs ``RouteReferrersReadInterface``.
Umożliwia on `generowanie adresów URL przez DynamicRouter`_ w instancji tych klas
(na przykład przy użyciu znacznika ``{{ path(content) }}`` w Twig).

Inicjator repozytorium
~~~~~~~~~~~~~~~~~~~~~~

:ref:`Inicjatory repozytoriów <phpcr-odm-repository-initializers>` umożliwiają
ustanowienie i utrzymanie węzłów PHPCR wymaganych przez aplikację, na przykład
będzie się potrzebowało ścieżki ``/cms/pages``, ``/cms/posts`` i ``/cms/routes``.
Klasa ``GenericInitializer`` może łatwo wykorzystywać inicjowanie listy ścieżek.
Dodajmy konfiguracji kontenera usługi następujący kod:

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/BasicCmsBundle/Resources/config/services.yml
        services:
            acme_basiccms.basic_cms.phpcr.initializer:
                class: Doctrine\Bundle\PHPCRBundle\Initializer\GenericInitializer
                arguments:
                    - My custom initializer
                    - ["/cms/pages", "/cms/posts", "/cms/routes"]
                tags:
                    - { name: doctrine_phpcr.initializer }

    .. code-block:: xml

        <!-- src/Acme\BasicCmsBundle\Resources\services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:acme_demo="http://www.example.com/symfony/schema/"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <!-- ... -->
            <services>
                <!-- ... -->

                <service id="acme_basiccms.basic_cms.phpcr.initializer"
                    class="Doctrine\Bundle\PHPCRBundle\Initializer\GenericInitializer">

                    <argument>My custom initializer</argument>

                    <argument type="collection">
                        <argument>/cms/pages</argument>
                        <argument>/cms/posts</argument>
                        <argument>/cms/routes</argument>
                    </argument>

                    <tag name="doctrine_phpcr.initializer"/>
                </service>
            </services>
        </container>

    .. code-block:: php

        // src/Acme/BasicCmsBundle/Resources/config/services.php
        $container
            ->register(
                'acme_basiccms.basic_cms.phpcr.initializer',
                'Doctrine\Bundle\PHPCRBundle\Initializer\GenericInitializer'
            )
            ->addArgument('My custom initializer')
            ->addArgument(array('/cms/pages', '/cms/posts', '/cms/routes'))
            ->addTag('doctrine_phpcr.initializer')
        ;

.. note::

    Inicjatory działają na poziomie PHPCR, a nie na poziomie PHPCR-ODM – oznacza
    to, że ma się do czynienia z węzłami i dokumentami. Nie musisz teraz rozumieć
    szczegółów tego mechanizmu. Przeczytaj :doc:`../database/choosing_storage_layer`,
    aby dowiedzieć się więcej o PHPCR.

Inicjatory są wykonywane automatycznie po załadowaniu danych testowych (tak jak
podano to w następnym rozdziale) lub alternatywnie można je wykonać ręcznie stosując
następujące polecenie:

.. code-block:: bash

    $ php app/console doctrine:phpcr:repository:init

.. note::

    Polecenie to jest `powtarzalne`_, co oznacza, że jest bezpieczne przy uruchamianiu
    go wiele razy, nawet jeśli ma się już dane w repozytorium. Trzeba jednak pamiętać,
    że realizacja powtarzalności jest obowiązkiem inicjatora!

Można sprawdzić, czy repozytorium zostało zainicjowane przez zrzut repozytorium
treści:

.. code-block:: bash

    $ php app/console doctrine:phpcr:node:dump

Tworzenie danych testowych
~~~~~~~~~~~~~~~~~~~~~~~~~~

Wykorzystamy bibliotekę danych testowych do wygenerowania kilku początkowych
danych dla naszego CMS.

Potrzebna jest instalacja następującego pakietu:

.. code-block:: javascript

    {
        ...
        require: {
            ...
            "doctrine/data-fixtures": "1.0.*"
        },
        ...
    }

Utwórzmy stronę dla CMS::

    // src/Acme/BasicCmsBundle/DataFixtures/PHPCR/LoadPageData.php
    namespace Acme\BasicCmsBundle\DataFixtures\PHPCR;

    use Acme\BasicCmsBundle\Document\Page;
    use Doctrine\Common\DataFixtures\FixtureInterface;
    use Doctrine\Common\Persistence\ObjectManager;

    class LoadPageData implements FixtureInterface
    {
        public function load(ObjectManager $dm)
        {
            $parent = $dm->find(null, '/cms/pages');

            $page = new Page();
            $page->setTitle('Home');
            $page->setParentDocument($parent);
            $page->setContent(<<<HERE
    Welcome to the homepage of this really basic CMS.
    HERE
            );

            $dm->persist($page);
            $dm->flush();
        }
    }

i dodamy trochę wpisów::

    // src/Acme/BasicCmsBundle/DataFixtures/PHPCR/LoadPostData.php
    namespace Acme\BasicCmsBundle\DataFixtures\Phpcr;

    use Doctrine\Common\DataFixtures\FixtureInterface;
    use Doctrine\Common\Persistence\ObjectManager;
    use Acme\BasicCmsBundle\Document\Post;

    class LoadPostData implements FixtureInterface
    {
        public function load(ObjectManager $dm)
        {
            $parent = $dm->find(null, '/cms/posts');

            foreach (array('First', 'Second', 'Third', 'Forth') as $title) {
                $post = new Post();
                $post->setTitle(sprintf('My %s Post', $title));
                $post->setParentDocument($parent);
                $post->setContent(<<<HERE
    This is the content of my post.
    HERE
                );

                $dm->persist($post);
            }

            $dm->flush();
        }
    }

oraz załadujmy dane testowe:

.. code-block:: bash

    $ php app/console doctrine:phpcr:fixtures:load

Teraz w repozytorium treści powinno być kilka danych.

.. _`routingautobundle documentation`: http://symfony.com/doc/current/cmf/bundles/routing_auto.html
.. _`generowanie adresów URL przez DynamicRouter`: http://symfony.com/doc/current/cmf/bundles/routing/dynamic.html#url-generation-with-the-dynamicrouterA
.. _`powtarzalne`: http://en.wiktionary.org/wiki/idempotent
.. _`symfony-cmf/routing-auto-bundle`: https://packagist.org/packages/symfony-cmf/routing-auto-bundle
.. _`symfony-cmf/menu-bundle`: https://packagist.org/packages/symfony-cmf/menu-bundle
.. _`sonata-project/doctrine-phpcr-admin-bundle`: https://packagist.org/packages/sonata-project/doctrine-phpcr-admin-bundle
.. _`doctrine/data-fixtures`: https://packagist.org/packages/doctrine/data-fixtures
.. _`doctrine dbal jackalope`: https://github.com/jackalope/jackalope-doctrine-dbal
.. _`Apache Jackrabbit`: https://jackrabbit.apache.org
.. _`zdarzenie cyklu życia przed utrwaleniem`: http://docs.doctrine-project.org/projects/doctrine-phpcr-odm/en/latest/reference/events.html#lifecycle-callbacks

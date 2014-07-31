Trasowanie i automatyczne trasowanie
------------------------------------

Trasy (ścieżki URL) do treści będą automatycznie  tworzone i aktualizowane przy
użyciu RoutingAutoBundle. W pakiecie tym stosuje się język konfiguracji do
zdefiniowania automatycznego tworzenia tras, co może być nieco trudne przy pierwszym
spojrzeniu.

Proszę zapoznać się z :doc:`../../bundles/routing_auto/index` dla pełnego rozeznania
tego zagadnienia.

Podsumowując, można skonfigurować system automatycznego trasowania w celu automatycznego
tworzenia nowych dokumentów w drzewie trasowania dla każdego tworzonego wpisu lub
strony. Nowa trasa zostanie odniesiona do docelowej treści:

.. image:: ../../_images/cookbook/basic-cms-objects.png

Powyższe ścieżki reprezentują ścieżkę w drzewie dokumentów PHPCR-ODM. W następnym
rozdziale zdefiniujemy ``/cms/routes`` jako ścieżkę bazową dla tras, co umożliwi
dostęp do treści na następujących ścieżkach URL:

* **Home**: ``http://localhost:8000/page/home``
* **About**: ``http://localhost:8000/page/about``
* itd.

Instalacja
~~~~~~~~~~

Upewnij się, że masz zainstalowany następujący pakiet:

.. code-block:: javascript

    {
        ...
        require: {
            ...
            "symfony-cmf/routing-auto-bundle": "1.0.*@alpha"
        },
        ...
    }

.. note::

    Zainstaluj najnowszą wersję pakietu auto trasowania.

Włącz w kernelu pakiet trasowania::

    class AppKernel extends Kernel
    {
        public function registerBundles()
        {
            $bundles = array(
                // ...
                new Symfony\Cmf\Bundle\RoutingBundle\CmfRoutingBundle(),
                new Symfony\Cmf\Bundle\RoutingAutoBundle\CmfRoutingAutoBundle(),
            );

            // ...
        }
    }

.. note:: 

    Pakiet `symfony-cmf/routing-bundle` jest instalowany automatycznie, ponieważ
    jest on zależnością pakietu `symfony-cmf/routing-auto-bundle`.

Włączenie Dynamic Router
~~~~~~~~~~~~~~~~~~~~~~~~

RoutingAutoBundle wykorzystuje `RoutingBundle`_ CMF, który umożliwia aby trasy
były dostarczane z bazy danych (jako uzupełnienie dla zwykłych plików konfiguracyjnych
trasowania w rdzeniu Symfony 2).

Dodajmy go do konfiguracji aplikacji:

.. configuration-block::

    .. code-block:: yaml

        # /app/config/config.yml
        cmf_routing:
            chain:
                routers_by_id:
                    cmf_routing.dynamic_router: 20
                    router.default: 100
            dynamic:
                enabled: true
                persistence:
                    phpcr:
                        route_basepath: /cms/routes

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <container xmlns="http://symfony.com/schema/dic/services">
            <config xmlns="http://cmf.symfony.com/schema/dic/routing">
                <chain>
                    <router-by-id id="cmf_routing.dynamic_router">20</router-by-id>
                    <router-by-id id="router.default">100</router-by-id>
                </chain>
                <dynamic>
                    <persistence>
                        <phpcr route-basepath="/cms/routes" />
                    </persistence>
                </dynamic>
            </config>
       </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('cmf_routing', array(
            'dynamic' => array(
                'persistence' => array(
                    'phpcr' => array(
                        'enabled' => true,
                        'route_basepath' => '/cms/routes',
                    ),
                ),
            ),
        ));

Będzie on:

#. powodował, że domyślny router Symfony zostanie zamieniony przez router łańcuchowy.
   Ruter łańcuchowy umożliwia posiadanie wielu routerów w aplikacji. Dodajmy dynamiczny
   router (który może pobierać trasy z bazy danej) oraz domyślny router Symfony
   (który pobiera trasy z plików konfiguracyjnych). Liczba wskazuje priorytet
   kolejności – trasa z najmniejszym numerem będzie wywoływana pierwsza;
#. konfigurował **dynamiczny** router dodany do routera łańcuchowego.
   Trzeba określić, czy należy używać zaplecze PHPCR i czy trasa *root* będzie
   leżeć na ścieżce ``/cms/routes``.

Auto Routing Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~

Create the following file in your applications configuration directory:

.. code-block:: yaml

    # app/config/routing_auto.yml
    cmf_routing_auto:
        mappings:
            Acme\BasicCmsBundle\Document\Page:
                content_path:
                    pages:
                        provider: [specified, { path: /cms/routes/page }]
                        exists_action: use
                        not_exists_action: create
                content_name:
                    provider: [content_method, { method: getTitle }]
                    exists_action: auto_increment
                    not_exists_action: create

            Acme\BasicCmsBundle\Document\Post:
                content_path:
                    blog_path:
                        provider: [specified, { path: /cms/routes/post }]
                        exists_action: use
                        not_exists_action: create
                    date:
                        provider: [content_datetime, { method: getDate}]
                        exists_action: use
                        not_exists_action: create
                content_name:
                    provider: [content_method, { method: getTitle }]
                    exists_action: auto_increment
                    not_exists_action: create

Skonfiguruje to system automatycznego trasowania do automatycznego tworzenia
i aktualizowania dokumentów tras dla dokumentów ``Page`` i ``Post``.

Podsumowanie:

* Klucz ``content_path`` reprezentuje ścieżkę nadrzędną treści, np.
  ``/jeśli/to/jest/jakaś/ścieżka`` to ``content_path`` reprezentuje
  ``/jeśli/to/jest/jakaś``;
* Każdy element pod ``content_path`` reprezentuje sekcję ścieżki URL;
* Pierwszy element ``blog_path`` używa *dostawcy*, który *określa* ścieżkę.
  Jeśli ścieżka istnieje, to nic się nie dzieje;
* Drugi element używa dostawcy ``content_datetime``, który wykorzystywać będzie
  obiekt ``DateTime``, zwracany z określonej metody obiektu treści (``Post``)
  i będzie tworzyć z niego ścieżkę ścieżkę, np. ``2013/10/13``;
* Klucz ``content_name`` reprezentuje ostatnią część ścieżki, np. ``ścieżka``
  w ``/jeśl/to/jest/jakaś/ścieżka``.

Teraz trzeba będzie dołączyć tą konfigurację:

.. configuration-block::
    
    .. code-block:: yaml

        # app/config/config.yml
        imports:
            - { resource: routing_auto.yml }

    .. code-block:: xml

        <!-- src/Acme/BasicCmsBUndle/Resources/config/config.yml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container 
            xmlns="http://symfony.com/schema/dic/services" 
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
            xsi:schemaLocation="http://symfony.com/schema/dic/services 
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <import resource="routing_auto.yml"/>
        </container>
    
    .. code-block:: php

        // src/Acme/BasicCmsBundle/Resources/config/config.php

        // ...
        $this->import('routing_auto.yml');

i przeładować konfigurator testowania (*ang. fixtures*):

.. code-block:: bash

    $ php app/console doctrine:phpcr:fixtures:load

Popatrz, co utworzyliśmy:

.. code-block:: bash

    $ php app/console doctrine:phpcr:node:dump
    ROOT:
      cms:
        pages:
          Home:
        routes:
          page:
            home:
          post:
            2013:
              10:
                12:
                  my-first-post:
                  my-second-post:
                  my-third-post:
                  my-forth-post:
        posts:
          My First Post:
          My Second Post:
          My Third Post:
          My Forth Post:

Trasy są teraz tworzone automatycznie.

.. _`SonataDoctrinePhpcrAdminBundle`: https://github.com/sonata-project/SonataDoctrinePhpcrAdminBundle
.. _`RoutingBundle`: http://symfony.com/doc/master/cmf/bundles/routing/index.html

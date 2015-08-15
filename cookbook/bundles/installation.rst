.. index::
   single: pakiet; instalacja

Jak zainstalować pakiety zewnetrzne ?
=====================================

Większość pakietów zapewnia swoje instrukcje obsługi. Jednakże, podstawowe
etapy instalacji pakietów są niemalże identyczne:

* `A) Dodanie zależności dla Composer`_
* `B) Aktywowanie pakietu`_
* `C) Skonfigurowanie pakietu`_

A) Dodanie zależności dla Composer
----------------------------------

Zależności są zarządzane przez Composer, tak więc jeśli Composer jest dla Ciebie
nowością zapoznaj sie z `jego dokumentacją`_. Procedura ta obejmuje dwa kroki:

1) Znajdź nazwę pakietu na Packagist
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Plik README pakietu (np. `FOSUserBundle`_) zazwyczaj informuje o nazwie pakietu
(np. ``friendsofsymfony/user-bundle``). Jeśli nie, można odszukać pakiet na stronie
`Packagist.org`_.

.. tip::

    Szukasz pakietów? Spróbuj przeszukać `KnpBundles.com`_: nieoficjalne
    archiwum pakietów Symfony.

2) Zainstaluj pakiet poprzez Composer
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Teraz, gdy już znana jest nazwa pakietu, można zainstalować go stosując Composer:

.. code-block:: bash

    $ composer require friendsofsymfony/user-bundle

Pozwoli to na wybranie najlepszej wersji dla swojego projektu, dodanie jej do
pliku ``composer.json`` i pobranie kodu do katalogu ``vendor/``. Jeśli potrzeba
specyficznej wersji, należy dołączyć ją jako drugi argument polecenia `composer require`_:

.. code-block:: bash

    $ composer require friendsofsymfony/user-bundle "~2.0"

B) Aktywowanie pakietu
----------------------

W tym momencie pakiet jest zainstalowany w projekcie Symfony (w ``vendor/friendsofsymfony/``),
a autoloader rozpoznaje jego klasy. Jedyne co trzeba zrobić, to zarejestrować
pakiet w ``AppKernel``::

    // app/AppKernel.php

    // ...
    class AppKernel extends Kernel
    {
        // ...

        public function registerBundles()
        {
            $bundles = array(
                // ...,
                new FOS\UserBundle\FOSUserBundle(),
            );

            // ...
        }
    }

W nielicznych przypadkach, mozna potrzebować, aby pakiet był tylko włączony w 
:doc:`środowisku </cookbook/configuration/environments>` programistycznym.
Na przykład, DoctrineFixturesBundle pomaga załadować dane testowe - coś, co
przypuszczalnie potrzebuje sie tylko w środowisku programistycznym.
W celu załadowania takiego pakietu tylko w srodowisku ``dev`` i ``test``,
zarejestruj pakiet w ten sposób::

    // app/AppKernel.php

    // ...
    class AppKernel extends Kernel
    {
        // ...

        public function registerBundles()
        {
            $bundles = array(
                // ...
            );

            if (in_array($this->getEnvironment(), array('dev', 'test'))) {
                $bundles[] = new Doctrine\Bundle\FixturesBundle\DoctrineFixturesBundle();
            }

            // ...
        }
    }

C) Skonfigurowanie pakietu
--------------------------

Pakiet zazwyczaj wymaga dodania specjalnej konfiguracji do pliku ``app/config/config.yml``.
Dokumentacja pakietu najprawdopodobniej opisze wszelkie szczegóły, niemniej
można również odwołać się do jego konfiguracji używając polecenia ``config:dump-reference``.

Na przykład, aby zobaczyć odwołania do konfiguracji ``assetic``, można użyć:

.. code-block:: bash
   :linenos:

    $ app/console config:dump-reference AsseticBundle

albo też:

.. code-block:: bash
   :linenos:

    $ app/console config:dump-reference assetic

Na wyjściu powinno się otrzymać coś podobnego do:

.. code-block:: text
   :linenos:

    assetic:
        debug:                %kernel.debug%
        use_controller:
            enabled:              %kernel.debug%
            profiler:             false
        read_from:            %kernel.root_dir%/../web
        write_to:             %assetic.read_from%
        java:                 /usr/bin/java
        node:                 /usr/local/bin/node
        node_paths:           []
        # ...

Inne ustawienia
---------------

W tym momencie powinno się przestudiować plik ``README`` używanego pakietu i
zobaczyć co zrobić dalej.

.. _`jego dokumentacją`: https://getcomposer.org/doc/00-intro.md
.. _`Packagist`:           https://packagist.org
.. _`FOSUserBundle`:       https://github.com/FriendsOfSymfony/FOSUserBundle
.. _`friendsofsymfony/user-bundle`: https://packagist.org/packages/friendsofsymfony/user-bundle
.. _`KnpBundles`:          http://knpbundles.com/

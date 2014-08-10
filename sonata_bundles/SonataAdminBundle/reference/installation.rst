Instalacja
==========

Pakiet SonataAdminBundle można zainstalować na każdym etapie życia projektu, czy
to w czystej instalacji Symfony2 lub w istniejącym projekcie.

Pobranie kodu
-------------

Wykorzystaj program Composer do zarządzania zależnościami i pobierz SonataAdminBundle:

.. code-block:: bash

    php composer.phar require sonata-project/admin-bundle

Użytkownik zostanie poproszony o wpisanie wersji. Wersja 'dev-master'
to najnowsza wersja, kompatybilna z najnowszą wersją Symfony2. Sprawdź
`packagist <https://packagist.org/packages/sonata-project/admin-bundle>`_
dla starszych wersji prosimy podać:

.. code-block:: bash
   
   ... sonata-project/admin-bundle requirement: dev-master

Wybranie i pobranie pakietu magazynowania
-----------------------------------------

SonataAdminBundle jest obojętny pod względem przechowywania danych, co oznacza,
że może pracować z różnymi mechanizmami magazynowania daych. W zależności od tego,
co wykorzystuje się w projekcie, powinno się zainstalować jeden z następujących
pakietów. W odpowiednim odnośniku znajdziesz prostą instrukcję instalacji dla
każdego z tych pakietów:

    - `SonataDoctrineORMAdminBundle <http://sonata-project.org/bundles/doctrine-orm-admin/master/doc/reference/installation.html>`_
    - `SonataDoctrineMongoDBAdminBundle <https://github.com/sonata-project/SonataDoctrineMongoDBAdminBundle/blob/master/Resources/doc/reference/installation.rst>`_
    - `SonataPropelAdminBundle <http://sonata-project.org/bundles/propel-admin/master/doc/reference/installation.html>`_
    - `SonataDoctrinePhpcrAdminBundle <https://github.com/sonata-project/SonataDoctrinePhpcrAdminBundle/blob/master/Resources/doc/reference/installation.rst>`_

.. note::
   
   Nie wiesz co wybrać? Większość nowych użytkowników preferuje SonataDoctrineORMAdmin
   do interakcji z tradycyjnymi relacyjnymi bazami danych (MySQL, PostgreSQL itd.)

Włączenie SonataAdminBundle i jego zależności
---------------------------------------------

SonataAdminBundle zależy od kilku pakietów implementujących kilka funkcjonalności.
Oprócz warstwy magazynowania danych omówionej w kroku 2, są inne pakiety niezbędne
do działania SonataAdminBundle:

    - `SonataBlockBundle <http://sonata-project.org/bundles/block/master/doc/reference/installation.html>`_
    - `KnpMenuBundle <https://github.com/KnpLabs/KnpMenuBundle/blob/master/Resources/doc/index.md#installation>`_ (Version 1.1.*)

Pakiety te są automatycznie pobierane przez Composer jako zależności SonataAdminBundle.
Jednak należy włączyć je w AppKernel.php i ręcznie skonfiguraować. Nie zapomnij
włączyć także SonataAdminBundle:

.. code-block:: php

    <?php
    // app/AppKernel.php
    public function registerBundles()
    {
        return array(
            // ...

            // The admin requires some twig functions defined in the security
            // bundle, like is_granted
            new Symfony\Bundle\SecurityBundle\SecurityBundle(),

            // Add your dependencies
            new Sonata\CoreBundle\SonataCoreBundle(),
            new Sonata\BlockBundle\SonataBlockBundle(),
            new Knp\Bundle\MenuBundle\KnpMenuBundle(),
            //...

            // If you haven't already, add the storage bundle
            // This example uses SonataDoctrineORMAdmin but
            // it works the same with the alternatives
            new Sonata\DoctrineORMAdminBundle\SonataDoctrineORMAdminBundle(),

            // Then add SonataAdminBundle
            new Sonata\AdminBundle\SonataAdminBundle(),
            // ...
        );
    }

.. note::
    Jeśli zależność jest już włączona gdzieś w AppKernel.php, to nie trzeba jej włączać ponownie.

.. note::
    Począwszy od wersji 2.3, nie jest już wymagany pakiet SonatajQueryBundle ponieważ
    aktywa sa dostępne w tym pakiecie. Pakiet ten jest również zarejestrowany w bower.io,
    więc można użyć bower do obsługi aktywów.

Konfigurowanie zależności SonataAdminBundle
-------------------------------------------

Będzie trzeba też skonfigurować zależności SonataAdminBundle. Dla każdego z wyżej
wymienionych pakietów sprawdzić odpowiednie pliki instrukcji instalacji i konfiguracji,
aby zobaczyć co trzeba zmienić w konfiguracji Symfony2.

SonataAdminBundle dostarcza w SonataBlockBundle obsługę bloku wykorzystywanego
w panelach administracyjnych. W celu korzystania z tego, upewnij się, że właczona
jest konfiguracja pakietu SonataBlockBundle:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        sonata_block:
            default_contexts: [cms]
            blocks:
                # Enable the SonataAdminBundle block
                sonata.admin.block.admin_list:
                    contexts:   [admin]
                # Your other blocks

.. note::
   
   Nie przejmuj się zbytnio, jeśli w tym momencie jeszcze nie rozumiesz wszystkiego
   o bloku. SonataBlockBundle jest użytecznym narzędziem, ale na razie nie jest
   istotne, że nie wiesz o nim wszystkiego.

Czyszczenie
-----------

Teraz zainstaluj aktywa z pakietów:

.. code-block:: bash

    php app/console assets:install web

Zazwyczaj, podczas instalowania nowych pakietów dobrą praktyka jest wyczyszczenie
pamięci podręcznej:

.. code-block:: bash

    php app/console cache:clear

W tym momencie instalacja Symfony2 powinna być w pełni funkcjonalna, nie pokazując
błedów z SonataAdminBundle lub jego zależności. SonataAdminBundle jest zainstalowany,
ale jeszcze nie skonfigurowany (więcej na ten temat znajduje się w następnym rozdziale),
więc nie jesteś w stanie go jeszcze używać.

Jeśli w tym momencie lub podczas instalacji pojawiły się jakieś błędy, nie panikuj.

    - Dokładnie przeczytaj komunikat o błędzie. Spróbuj dowiedzieć się, jakie
      dokładnie pakiety są przyczyną błędu. Czy SonataAdminBundle czy też jedna
      z jego zależności?
    - Upewnij się, ze wszystkie instrukcje zostały wykonane poprawnie, zarówno
      dla SonataAdminBundle jak i zależności.
    - Jest szansa, że ktoś miał już podobny problem i że został on gdzieś udokumentowany.
      Sprawdź`Google <http://www.google.com>`_, `Sonata Users Group
      <https://groups.google.com/group/sonata-users>`_, `Symfony2 Users Group
      <https://groups.google.com/group/symfony2>`_ i `Symfony Forum
      <forum.symfony-project.org>`_ aby zobaczyć, czy znaleziono rozwiązanie tego
      problemu.
    - Nadal bez powodzenia? Spróbuj sprawdzić sprawy w otwartym projekcie na GitHub.

Po pomyślnym zainstalowaniu powyższych pakietów należy skonfigurować SonataAdminBundle
do administrowania swoimi modelami. Wszystko co jest potrzebne do szybkiego ustawienia
SonataAdminBundle jest opisane w rozdziale :doc:`getting_started`.

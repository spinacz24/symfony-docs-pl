Rozpoczęcie pracy z SonataAdminBundle
=====================================

Jeśli postępowało się zgodnie z instrukcjami instalacji, to pakiet SonataAdminBundle
powinien być zainstalowany, ale niedostępny. Zanim będzie go można używać, najpierw
trzeba skonfigurować go dla swoich modeli. Oto krótka lista kontrolna tego, co jest
potrzebne do szybkiego skonfigurowania SonataAdminBundle i utworzenia pierwszego
interfejsu administracyjnego dla modeli swojej aplikacji:

* Krok 1: Określenie tras SonataAdminBundle
* Krok 2: Utworzenie klasy Admin
* Krok 3: Utworzenie usługi Admin
* Krok 4: Konfiguracja

Krok 1: Określenie tras SonataAdminBundle
-----------------------------------------

W celu uzyskania dostępu do stron SonataAdminBundle, trzeba dodać jego trasy w pliku
trasowania aplikacji:

.. configuration-block::

    .. code-block:: yaml

        # app/config/routing.yml
        admin:
            resource: '@SonataAdminBundle/Resources/config/routing/sonata_admin.xml'
            prefix: /admin

        _sonata_admin:
            resource: .
            type: sonata_admin
            prefix: /admin

.. note::

    Jeśli używa się XML lub PHP do określenia konfiguracji aplikacji, powyższa
    konfiguracja musi zostać umieszczona w pliku routing.xml lub routing.php,
    odpowiednio do wybranego formatu (czyli XML albo PHP).

.. note::

    Dla osób ciekawych wyjaśnienia ustawienia ``resource: .``: jest to niezwykła
    składnia, ale używana dlatego, że Symfony wymaga, aby zasób był koniecznie
    określony (wskazuje ona na realny plik). Jeśli wyrażenie to przechodzi
    walidację ``AdminPoolLoader`` pakietu Sonata, to jest odpowiedzialne za
    przetworzenie  tej trasy i po prostu powoduje ignorowanie ustawienia zasobu.

W tym momencie można już uzyskać dostęp do pulpitu administracyjnego (pustego)
odwiedzając adres: ``http://yoursite.local/admin/dashboard``.

Krok 2: Utworzenie klasy Admin
------------------------------

SonataAdminBundle pozwala zarządzać danymi przy użyciu graficznego interfejsu,
który umożliwia tworzenie, aktualizowanie i wyszukiwanie instancji modelu. Akcje
takie wymagają skonfigurowania, co odbywa się za pomocą klasy Admin.
Klasa Admin reprezentuje odwzorowanie modelu dla każdej akcji administracyjnej.
W niej, to programista decyduje, które pola mają być wyświetlane w wykazie, które
używane jako filtry lub co pokazać na formularzu tworzenia i edytowania.

Najprostszym sposobem utworzenia klasy Admin dla modelu jest rozszerzenie klasy
``Sonata\AdminBundle\Admin\Admin``.

Załóżmy, że AcmeDemoBundle ma encję Post. Przy takim założeniu bazowa klasa Admin
będzie wyglądać podobnie do tego:

.. code-block:: php

   <?php
   // src/Acme/DemoBundle/Admin/PostAdmin.php

   namespace Acme\DemoBundle\Admin;

   use Sonata\AdminBundle\Admin\Admin;
   use Sonata\AdminBundle\Datagrid\ListMapper;
   use Sonata\AdminBundle\Datagrid\DatagridMapper;
   use Sonata\AdminBundle\Form\FormMapper;

   class PostAdmin extends Admin
   {
       // Fields to be shown on create/edit forms
       protected function configureFormFields(FormMapper $formMapper)
       {
           $formMapper
               ->add('title', 'text', array('label' => 'Post Title'))
               ->add('author', 'entity', array('class' => 'Acme\DemoBundle\Entity\User'))
               ->add('body') //if no type is specified, SonataAdminBundle tries to guess it
           ;
       }

       // Fields to be shown on filter forms
       protected function configureDatagridFilters(DatagridMapper $datagridMapper)
       {
           $datagridMapper
               ->add('title')
               ->add('author')
           ;
       }

       // Fields to be shown on lists
       protected function configureListFields(ListMapper $listMapper)
       {
           $listMapper
               ->addIdentifier('title')
               ->add('slug')
               ->add('author')
           ;
       }
   }

Implementowanie tycz trzech funkcji jest pierwszym krokiem do utworzenia klasy Admin.
Dostępne są inne opcje, pozwalające dostosować sposób, w jaki model jest pokazywany
i obsługiwany. Omówione zostaną one w bardziej zaawansowanych rozdziałach tego podręcznika.

Krok 3: Utworzenie usługi Admin
-------------------------------

Teraz, po utworzeniu klasy Admin, trzeba stworzyć dla niej usługę. Usługa ta musi
mieć znacznik ``sonata.admin``, który jest sposobem pozwalającym, aby SonataAdminBundle
wiedział, że ta konkretna usługa reprezentuje klasę Admin:

Wewnątrz folderu  ``Acme/DemoBundle/Resources/config/`` utwórzmy nowy plik ``admin.xml``
albo ``admin.yml``:

.. configuration-block::

    .. code-block:: xml

       <!-- Acme/DemoBundle/Resources/config/admin.xml -->
       <container xmlns="http://symfony.com/schema/dic/services"
           xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
           xsi:schemaLocation="http://symfony.com/schema/dic/services/services-1.0.xsd">
           <services>
              <service id="sonata.admin.post" class="Acme\DemoBundle\Admin\PostAdmin">
                 <tag name="sonata.admin" manager_type="orm" group="Content" label="Post"/>
                 <argument />
                 <argument>Acme\DemoBundle\Entity\Post</argument>
                 <argument />
                 <call method="setTranslationDomain">
                     <argument>AcmeDemoBundle</argument>
                 </call>
             </service>
          </services>
       </container>


    .. code-block:: yaml

       # Acme/DemoBundle/Resources/config/admin.yml
       services:
           sonata.admin.post:
               class: Acme\DemoBundle\Admin\PostAdmin
               tags:
                   - { name: sonata.admin, manager_type: orm, group: "Content", label: "Post" }
               arguments:
                   - ~
                   - Acme\DemoBundle\Entity\Post
                   - ~
               calls:
                   - [ setTranslationDomain, [AcmeDemoBundle]]

Podstawowa konfiguracja usługi Admin jest dość prosta. Powyższy kod tworzy instancję
usługi opartej na klasie, którą wcześniej się określiło i przyjmuje trzy argumenty:

    1. kod usługi Admin (domyślnie nazwa usługi);
    2. model, który klasa Admin ma odwzorować (obowiązkowy);
    3. kontroler, który ma obsługiwać akcje administracyjne (domyślnie ``SonataAdminBundle:CRUDController``).

Dla większości przypadków zwykle wystarczy podać drugi argument jako pierwszy
i trzecią domyślną wartość.

Wywołanie ``setTranslationDomain`` pozwala wybrać, która domena translacyjna ma
być używana podczas tłumaczenia etykiet na stronie administracyjnej. Więcej informacji
o tym można znaleźć w :ref:`using-message-domains`.

Mamy już teraz plik konfiguracyjny z usługą administracyjną. Wystarczy powiadomić
Symfony2 aby ją załadowało. Są dwa sposoby zrobienia tego:

1 - Zaimpotywanie usługi w głównym pliku config.yml
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Dołączmy nowy plik konfiguracyjny w głównym pliku config.yml (upewnij się, że
stosujesz właściwe rozszerzenie):

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        imports:
            - { resource: @AcmeDemoBundle/Resources/config/admin.xml }

2 - Wykorzystanie pakietu ładującego usługę
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Można również mieć pakiet ładujący plik konfiguracyjny zaplecza administracyjnego.
Wewnątrz pliku rozszerzającego pakiet użyj metodę ``load()`` tak jak opisano to
w `Receptariuszu Symfony`_.

.. configuration-block::

    .. code-block:: xml

        # Acme/DemoBundle/DependencyInjection/AcmeDemoBundleExtension.php for XML configurations
        
        namespace Acme\DemoBundle\DependencyInjection;

        use Symfony\Component\DependencyInjection\Loader;
        use Symfony\Component\Config\FileLocator;
        
        class AcmeDemoBundleExtension extends Extension
        {
            public function load(array $configs, ContainerBuilder $container) {
                // ...
                $loader = new Loader\XmlFileLoader($container, new FileLocator(__DIR__.'/../Resources/config'));
                $loader->load('admin.xml');
            }
        }

    .. code-block:: yaml

        # Acme/DemoBundle/DependencyInjection/AcmeDemoBundleExtension.php for YAML configurations
        
        namespace Acme\DemoBundle\DependencyInjection;

        use Symfony\Component\DependencyInjection\Loader;
        use Symfony\Component\Config\FileLocator;

        class AcmeDemoBundleExtension extends Extension
        {
            public function load(array $configs, ContainerBuilder $container) {
                // ...
                $loader = new Loader\YamlFileLoader($container, new FileLocator(__DIR__.'/../Resources/config'));
                $loader->load('admin.yml');
            }
        }

Krok 4: Konfiguracja
--------------------

W tym momencie mamy podstawowe akcje administracyjne na modelu. Jeśli odwiedzi
się ponownie ``http://yoursite.local/admin/dashboard``, powinno się zobaczyć panel
z odwzorowanym modelem. Można rozpocząć tworzenie, listowanie, edytowanie
i usuwanie instancji.

W celu umieszczenia własnej nazwy projektu i logo na pasku górnym, umieść plik
logo w ``src/Acme/DemoBundle/Resources/public/img/fancy_acme_logo.png``.
    
Zainstaluj swoje aktywa:

.. code-block:: sh

    $ php app/console assets:install

Teraz będzie można zmienić główny plik konfiguracyjny projektu:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        sonata_admin:
            title:      Acme Demo Bundle
            title_logo: bundles/acmedemo/img/fancy_acme_logo.png



Następne kroki - bezpieczeństwo
-------------------------------

Teraz można uzyskać dostęp do pulpitu administracyjnego i danych wpisując tylko
odpowiedni adres URL. Domyślnie SonataAdminBundle nie jest dostarczany z jakimkolwiek
zarządzaniem użytkownikami, w celu zapewnienia elastyczności. Jednak jest niemal
pewne, że Twoja aplikacja wymaga takiej możliwości. Projekt Sonata zawiera pakiet
``SonataUserBundle``, który integruje bardzo popularny pakiet ``FOSUserBundle``.
Prosimy zapoznać się z rozdziałem :doc:`security` tej dokumentacji w celu poznania
więcej szczegółów.

Powinieneś być już teraz gotowy Drogi czytelniku do rozpoczęcia pracy z SonataAdminBundle.
Możesz teraz odwzorowywać dodatkowe modele lub zapoznać się z  zaawansowanymi
możliwościami pakietu. Następne rozdziały są poświęcone określonym funkcjonalnościom
pakietu, wyjaśniając głębiej szczegóły dotyczące konfiguracji i tego co można osiągnąć
z SonataAdminBundle.

.. _`Receptariuszu Symfony`: http://symfony.com/doc/master/cookbook/bundles/extension.html#using-the-load-method


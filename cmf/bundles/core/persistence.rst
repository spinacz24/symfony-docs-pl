.. index::
    single: wielojęzyczność; CoreBundle

.. _bundles-core-persistence:

Utrwalanie
----------

CoreBundle pozwala centralnie konfigurować warstwę utrwalania dla wszystkich pakietów CMF.

Dla uczynienia PHPCR-ODM domyślną warstwą utrwalania wszystkie pakiety CMF muszą
dodać następujący kod do głównego pliku konfiguracyjnego:

.. configuration-block::

    .. code-block:: yaml

        cmf_core:
            persistence:
                phpcr: ~

    .. code-block:: xml

        <?xml version="1.0" charset="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">

            <config xmlns="http://cmf.symfony.com/schema/dic/core">
                <persistence>
                    <phpcr />
                </persistence>
            </config>

        </container>

    .. code-block:: php

        $container->loadFromExtension('cmf_core', array(
            'persistence' => array(
                'phpcr' => array(),
            ),
        ));

.. _bundles-core-multilang-persisting_multilang_documents:

Utrwalanie dokumentów w różnych językach
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Zapoznaj się z `dokumentacją PHPCR-ODM`_ w celu poznania szczegółów o utrwalaniu
dokumentów w różnych językach.

Wybór globalnej strategii tłumaczeń
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

PHPCR-ODM obsługuje wiele różnych strategii utrzymywania tłumaczeń w repozytorium.
Podczas łączenia jego pakietów możliwe jest, że któryś kończy się z mieszanką
rożnych strategii, co może zapewnić bardziej złożone ogólne przeszukiwanie
danych, lecz również może być mniej efektywne, w zależności od liczby języków
używanych w systemie.

W celu rozwiązania tego problemu CoreBundle dostarcza odbiornik (*ang. listner*)
Doctrine, który ewentualnie może wymuszać pojedynczą strategie tłumaczeń dla
wszystkich dokumentów:

.. configuration-block::

    .. code-block:: yaml

        cmf_core:
            persistence:
                phpcr:
                    translation_strategy: attribute

    .. code-block:: xml

        <?xml version="1.0" charset="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">

            <config xmlns="http://cmf.symfony.com/schema/dic/core">
                <persistence>
                    <phpcr
                        translation-strategy="attribute"
                    />
                </persistence>
            </config>

        </container>

    .. code-block:: php

        $container->loadFromExtension('cmf_core', array(
            'persistence' => array(
                'phpcr' => array(
                    'translation_strategy' => 'attribute',
                ),
            ),
        ));

.. caution::

    Zmiana tego ustawienia, gdy dane zostały już utrwalone z inną strategią
    tłumaczeń wymaga ręcznego zaktualizowania bieżących danych.

W celu poznania więcej informacji proszę przeczytać `dokumentację PHPCR-ODM`_.

.. _bundle-core-child-admin-extension:

Używanie modeli podrzędnych: rozszerzenie Child Sonata Admin
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Rozszerzenie to ustawia domyślnego rodzica, dla każdej nowej instancji obiektu,
jeśli parametr ``parent`` znajduje się w ścieżce URL.
Parametr rodzica jest obecny, na przykład, podczas dodawania dokumentów w nakładce
w ``doctrine_phpcr_odm_tree_manager`` lub podczas dodawania dokumentu w drzewie
kokpitu.

.. note::

    Rozszerzeni to jest dostępne tylko, jeśli włączona jest opcja
    ``cmf_core.persistence.phpcr`` i aktywny jest pakiet SonataPHPCRAdminBundle.

W celu włączenia rozszerzenia w klasach administracyjnych, wystarczy zdefiniować
konfigurację rozszerzenia w sekcji ``sonata_admin`` konfiguracji projektu:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        sonata_admin:
            # ...
            extensions:
                cmf_core.admin_extension.child:
                    implements:
                        - Symfony\Cmf\Bundle\CoreBundle\Model\ChildInterface
                        - Doctrine\ODM\PHPCR\HierarchyInterface

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" charset="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">
            <config xmlns="http://sonata-project.org/schema/dic/admin">
                <!-- ... -->
                <extension id="cmf_core.admin_extension.child">
                    <implement>Symfony\Cmf\Bundle\CoreBundle\Model\ChildInterface</implement>
                    <implement>Doctrine\ODM\PHPCR\HierarchyInterface</implement>
                </extension>
            </config>

        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('sonata_admin', array(
            // ...
            'extensions' => array(
                'cmf_core.admin_extension.child' => array(
                    'implements' => array(
                        'Symfony\Cmf\Bundle\CoreBundle\Model\ChildInterface',
                        'Doctrine\ODM\PHPCR\HierarchyInterface',
                    ),
                ),
            ),
        ));

W celu poznania więcej szczegółów proszę przeczytać
`dokumentację rozszerzenia Sonata Admin`_.

.. _bundle-core-translatable-admin-extension:

Edytowanie informacji ustawień regionalnych: rozszerzenie Translatable Sonata Admin
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Kilka pakietów dostarcza klasy modelu tłumaczeń, które implementują
``TranslatableInterface``. Rozszerzenie to dostarcza pole wyboru języka do
określonych formularzy SonataAdminBundle.

W celu włączenia rozszerzeń w klasach administracyjnych, wystarczy zdefiniować
konfigurację rozszerzenia w sekcji ``sonata_admin`` konfiguracji projektu:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        sonata_admin:
            # ...
            extensions:
                cmf_core.admin_extension.translatable:
                    implements:
                        - Symfony\Cmf\Bundle\CoreBundle\Translatable\TranslatableInterface

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" charset="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">
            <config xmlns="http://sonata-project.org/schema/dic/admin">
                <!-- ... -->
                <extension id="cmf_core.admin_extension.translatable">
                    <implement>
                        Symfony\Cmf\Bundle\CoreBundle\Translatable\TranslatableInterface
                    </implement>
                </extension>
            </config>

        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('sonata_admin', array(
            // ...
            'extensions' => array(
                'cmf_core.admin_extension.translatable' => array(
                    'implements' => array(
                        'Symfony\Cmf\Bundle\CoreBundle\Translatable\TranslatableInterface',
                    ),
                ),
            ),
        ));

W celu poznania więcej szczegółów proszę przeczytać
`dokumentację rozszerzenia Sonata Admin`_.

.. _`Sonata Admin extension documentation`: http://sonata-project.org/bundles/admin/master/doc/reference/extensions.html
.. _`dokumentację PHPCR-ODM`: http://docs.doctrine-project.org/projects/doctrine-phpcr-odm/en/latest/reference/multilang.html#full-example

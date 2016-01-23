.. index::
   single: bezpieczeństwo; wymuszanie HTTPS
   single: HTTP; wymuszanie HTTPS

Jak wymuszać HTTPS lub HTTP dla różnych adresów URL
===================================================

W konfiguracji bezpieczeństwa można ustawić wymuszanie, aby niektóre obszary
witryny używały protokołu Robi się to w ustawieniach ról w kluczu
``access_control``. Na przykład, jeśli chce się wymusić protokół HTTPS dla wszystkich
śieżek URL rozpoczynających sie od ``/secure``, można ustawić następujacą konfigurację:

.. configuration-block::

        .. code-block:: yaml
           :linenos:

            # app/config/security.yml
            security:
                # ...

                access_control:
                    - { path: ^/secure, roles: ROLE_ADMIN, requires_channel: https }

        .. code-block:: xml
           :linenos:

            <!-- app/config/security.xml -->
            <?xml version="1.0" encoding="UTF-8"?>
            <srv:container xmlns="http://symfony.com/schema/dic/security"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:srv="http://symfony.com/schema/dic/services"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    http://symfony.com/schema/dic/services/services-1.0.xsd">

                <config>
                    <!-- ... -->

                    <rule path="^/secure" role="ROLE_ADMIN" requires_channel="https" />
                </config>
            </srv:container>

        .. code-block:: php
           :linenos:

            // app/config/security.php
            $container->loadFromExtension('security', array(
                // ...

                'access_control' => array(
                    array(
                        'path'             => '^/secure',
                        'role'             => 'ROLE_ADMIN',
                        'requires_channel' => 'https',
                    ),
                ),
            ));

Sam formularz logowania musi być dostępny anonimowo, inaczej użytkownicy nie będą
w stanie sie uwierzytelnić. W celu wymuszenia protokołu HTTPS w dostępie anonimowym
dla ścieżki ``/login`` można w dalszym ciągu wykorzystać role ``access_control``,
ustawiając tam rolę ``IS_AUTHENTICATED_ANONYMOUSLY``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...

            access_control:
                - { path: ^/login, roles: IS_AUTHENTICATED_ANONYMOUSLY, requires_channel: https }

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->

                <rule path="^/login"
                    role="IS_AUTHENTICATED_ANONYMOUSLY"
                    requires_channel="https"
                />
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'access_control' => array(
                array(
                    'path'             => '^/login',
                    'role'             => 'IS_AUTHENTICATED_ANONYMOUSLY',
                    'requires_channel' => 'https',
                ),
            ),
        ));

Możliwe jest również okreśłanie uzycia protokołu HTTPS w konfiguracji trasowania.
W celu poznania szczegółów proszę przeczytać artykuł :doc:`/cookbook/routing/scheme`.

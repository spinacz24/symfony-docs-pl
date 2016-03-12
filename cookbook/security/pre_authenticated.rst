.. index::
   single: bezpieczeństwo; wczesne uwierzytelnianie
   single: uwierzytelnianie; wczesne
   single: zapora; wczesnego uwierzytelniania

Stosowanie zapór bezpieczeństwa wczesnego uwierzytelniania
==========================================================

Niektóre serwery internetowe, w tym Apache, dostarczają obecnie wiele modułów
uwierzytelniania. Moduły te zwykle ustawiają kilka zmiennych środowiskowych,
które można wykorzystać do ustalenia, czy użytkownik ma dostęp do aplikacji.
Symfony dostarczane jest z obsługą wiekszości mechanizmów uwierzytelniania.
Obsługują one żadania, które nazywamy żądaniami wcześnie uwierzytelnianymi
(*ang. pre authenticated requests*), ponieważ użytkownik zostaje uwierzytelniony
automatycznie w momencie osiągnięcia aplikacji.

.. index::
   single: uwierzytelnianie; certyfikat klienta X.509

Uwierzytelnianie certyfikatem klienta X.509
-------------------------------------------

Podczas używania certyfikatów klienckich, serwer wykonuje sam całość procesu
uwierzytelniania. Na przykład, w Apache można skorzystać z dyrektywy ``SSLVerifyClient Require``.

Włączenie uwierzytelniania dla konkretnej zapory w konfiguracji bezpieczeństwa:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...

            firewalls:
                secured_area:
                    pattern: ^/
                    x509:
                        provider: your_user_provider

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

                <firewall name="secured_area" pattern="^/">
                    <x509 provider="your_user_provider" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'firewalls' => array(
                'secured_area' => array(
                    'pattern' => '^/',
                    'x509'    => array(
                        'provider' => 'your_user_provider',
                    ),
                ),
            ),
        ));

Domyślnie, zapora dostarcza zmienną ``SSL_CLIENT_S_DN_Email`` dla dostawcy
użytkowników i ustawia ``SSL_CLIENT_S_DN`` jako poświadczenie w instancji
:class:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\PreAuthenticatedToken`.
Można to przesłonić odpowiednio przez klucze ``user`` i ``credentials`` w konfiguracji
zapory x509.

.. _cookbook-security-pre-authenticated-user-provider-note:

.. note::

    Wystawca uwierzytelniania będzie informował dostawcę użytkowników tylko o
    nazwie użytkownika wykonującego żądanie. Trzeba będzie stworzyć (lub użyć)
    "dostawcę uzytkowników", który odwołuje się przez parametr konfiguracyjny
    ``provider`` (``your_user_provider`` w przykładzie konfiguracji). Dostawca ten
    będzie włączał nazwę uzytkownika do wybranego obiektu User. Wiecej informacji
    o tworzeniu lub konfigurowaniu dostawcy użytkownika można znaleźć w artykułach:
    
    * :doc:`/cookbook/security/custom_provider`
    * :doc:`/cookbook/security/entity_provider`

.. index::
   single: uwierzytelnianie; REMOTE_USER

Uwierzytelnianie oparte na REMOTE_USER
--------------------------------------

.. versionadded:: 2.6
    Zaporę wczesnego uwierzytelniania REMOTE_USER wprowadzono w Symfony 2.6.

Wiele modułów uwierzytelniania, takich jak ``auth_kerb`` w Apache, dostarcza
nazwę użytkownika przy użyciu zmiennej środowiskowej ``REMOTE_USER``.
Zmienna ta może być zaufanadla aplikacji, ponieważ uwierzytelnianie następuje
przed tym, jak żądanie ją uzyska.

Dla skonfigurowania możliwości używania przez Symfony zmiennej środowiskowej
``REMOTE_USER``, wystarczy włączyć odpowiednią zaporę w konfiguracji bezpieczeństwa:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            firewalls:
                secured_area:
                    pattern: ^/
                    remote_user:
                        provider: your_user_provider

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <?xml version="1.0" ?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:srv="http://symfony.com/schema/dic/services">

            <config>
                <firewall name="secured_area" pattern="^/">
                    <remote-user provider="your_user_provider"/>
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area' => array(
                    'pattern'     => '^/'
                    'remote_user' => array(
                        'provider' => 'your_user_provider',
                    ),
                ),
            ),
        ));

Teraz zapora będzie dostarczać zmienną środowiskową ``REMOTE_USER`` do dostawcy
użytkowników. Można zmienić nazwę zmiennej używanej w ustawieniu klucza ``user``
w konfiguracji zapory ``remote_user``.

.. note::

    Podobnie jak to miało miejsce w tworzeniu uwierzytelniania X509, trzeba
    skonfigurować "dostawcę użytkownika".
    Proszę zapoznać sie z :ref:`poprzednią uwagą <cookbook-security-pre-authenticated-user-provider-note>`
    w celu uzyskania więcej informacji.

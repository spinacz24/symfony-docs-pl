.. highlight:: php
   :linenothreshold: 2

.. index::
    single: bezpieczeństwo; wystawca uwierzytelniania
    single: wystawca uwierzytelniania

Jak utworzyć własnego wystawcę uwierzytelniania w logowaniu formularzowym
=========================================================================

Wyobraźmy sobie, że chcemy umożliwić dostęp do swojej witryny internetowej tylko
między godziną 2pm a 4pm UTC. Przed Symfony 2.4, trzeba było w tym celu utworzyć
własny token, wytwórnię, nasłuch i dostawcę. W tym artykule dowiesz się, jak
można to zrobić dla logowania formularzowego (czyli gdy użytkownik zgłasza poprzez
formularz swój login i hasło).
Przed Symfony 2.6, trzeba było używać kodera do uwierzyteniania hasła użytkownika.

Wystawca uwierzytelniania
-------------------------

.. versionadded:: 2.6
    Interfejs ``UserPasswordEncoderInterface`` został wprowadzony w Symfony 2.6.

Najpierw utworzymy klasę implementującą interfejs
:class:`Symfony\\Component\\Security\\Core\\Authentication\\SimpleFormAuthenticatorInterface`.
Pozwoli ona stworzyć własną logikę dla uwierzytelniania użytkownika::

    // src/Acme/HelloBundle/Security/TimeAuthenticator.php
    namespace Acme\HelloBundle\Security;

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\Security\Core\Authentication\SimpleFormAuthenticatorInterface;
    use Symfony\Component\Security\Core\Authentication\Token\TokenInterface;
    use Symfony\Component\Security\Core\Authentication\Token\UsernamePasswordToken;
    use Symfony\Component\Security\Core\Encoder\UserPasswordEncoderInterface;
    use Symfony\Component\Security\Core\Exception\AuthenticationException;
    use Symfony\Component\Security\Core\Exception\UsernameNotFoundException;
    use Symfony\Component\Security\Core\User\UserProviderInterface;

    class TimeAuthenticator implements SimpleFormAuthenticatorInterface
    {
        private $encoder;

        public function __construct(UserPasswordEncoderInterface $encoder)
        {
            $this->encoder = $encoder;
        }

        public function authenticateToken(TokenInterface $token, UserProviderInterface $userProvider, $providerKey)
        {
            try {
                $user = $userProvider->loadUserByUsername($token->getUsername());
            } catch (UsernameNotFoundException $e) {
                throw new AuthenticationException('Invalid username or password');
            }

            $passwordValid = $this->encoder->isPasswordValid($user, $token->getCredentials());

            if ($passwordValid) {
                $currentHour = date('G');
                if ($currentHour < 14 || $currentHour > 16) {
                    throw new AuthenticationException(
                        'You can only log in between 2 and 4!',
                        100
                    );
                }

                return new UsernamePasswordToken(
                    $user,
                    $user->getPassword(),
                    $providerKey,
                    $user->getRoles()
                );
            }

            throw new AuthenticationException('Invalid username or password');
        }

        public function supportsToken(TokenInterface $token, $providerKey)
        {
            return $token instanceof UsernamePasswordToken
                && $token->getProviderKey() === $providerKey;
        }

        public function createToken(Request $request, $username, $password, $providerKey)
        {
            return new UsernamePasswordToken($username, $password, $providerKey);
        }
    }

Jak to działa
-------------

Świetnie! Teraz musimy tylko dokonać małych zmian w :ref:`konfiguracji <cookbook-security-password-authenticator-config>`,
ale najpierw omówimy  krótko każdą metodę naszej klasy.

1) createToken
~~~~~~~~~~~~~~

Kiedy Symfony rozpoczyna przetwarzanie żądania, wywoływana jest metoda ``createToken()``,
która tworzy obiekt klasy
:class:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\TokenInterface`,
zawierajacy wszystkie informacje potrzebne w ``authenticateToken()`` do uwierzytelnienia
użytkownika (np. nazwę użytkownika i hasło).

Cokolwiek zostanie utworzone w obiekcie tokenu, będzie przekazane później w
``authenticateToken()``.

2) supportsToken
~~~~~~~~~~~~~~~~

.. include:: _supportsToken.rst.inc

3) authenticateToken
~~~~~~~~~~~~~~~~~~~~

Jeśli ``supportsToken`` zwraca ``true``, Symfony wywoła ``authenticateToken()``.
Sprawdzane jest tutaj, czy token jest dozwolony dla logowania, co wykonaliśmy,
pobierając po raz pierwszy obiekt ``User`` za pomocą dostawcy użytkowników
a następnie sprawdzając hasło i aktualny czas.

.. note::

    Sposób pobierania obiektu ``User`` i określanie prowidłowości tokenu 
    (np. sprawdzanie hasła), może bardzo się różnić w zależności od wymagań.

Ostatecznie, naszym zadaniem jest zwrócenie *nowego* obiektu tokenu, który został
"uwierzytelniony" (czyli ma co najmniej 1 rolę) i który wewnątrz ma nasz obiekt
``User``.

W metodzie tej, koder haseł jest potrzebny do sprawdzania ważności hasła::

        $passwordValid = $this->encoder->isPasswordValid($user, $token->getCredentials());

Jest to usługa, która zawsze jest dostępna w Symfony i wykorzystuje algorytm szyfrowania
hasła, jaki został podany w konfiguracji bezpieczeństwa (np. ``security.yml``)
w kluczu ``encoders``. Ponizej zobaczysz, jak wstrzyknąć tą usługę do ``TimeAuthenticator``.

.. _cookbook-security-password-authenticator-config:

Konfiguracja
------------

Teraz skonfigurujmy ``TimeAuthenticator`` jako usługę:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        services:
            # ...

            time_authenticator:
                class:     Acme\HelloBundle\Security\TimeAuthenticator
                arguments: ["@security.password_encoder"]

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">
            <services>
                <!-- ... -->

                <service id="time_authenticator"
                    class="Acme\HelloBundle\Security\TimeAuthenticator"
                >
                    <argument type="service" id="security.password_encoder" />
                </service>
            </services>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;
        
        // ...

        $container->setDefinition('time_authenticator', new Definition(
            'Acme\HelloBundle\Security\TimeAuthenticator',
            array(new Reference('security.password_encoder'))
        ));

Następnie aktywujemy ją w sekcji ``firewalls`` konfiguracji bezpieczeństwa, wykorzystując
klucz ``simple_form``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...

            firewalls:
                secured_area:
                    pattern: ^/admin
                    # ...
                    simple_form:
                        authenticator: time_authenticator
                        check_path:    login_check
                        login_path:    login

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

                <firewall name="secured_area"
                    pattern="^/admin"
                    >
                    <simple-form authenticator="time_authenticator"
                        check-path="login_check"
                        login-path="login"
                    />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php

        // ..

        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area'    => array(
                    'pattern'     => '^/admin',
                    'simple_form' => array(
                        'provider'      => ...,
                        'authenticator' => 'time_authenticator',
                        'check_path'    => 'login_check',
                        'login_path'    => 'login',
                    ),
                ),
            ),
        ));

Klucz ``simple_form`` ma takie same opcje jak normalny klucz ``form_login``,
ale z dodatkowym kluczem ``authenticator``, który wskazuje nową usługę.
Szczegóły są omówione w :ref:`reference-security-firewall-form-login`.

Jeśli w ogóle tworzenie formularza logowania jest dla Ciebie nowym tematem lub
nie rozumiesz działania opcji ``check_path`` lub ``login_path``, przeczytaj
artykuł :doc:`/cookbook/security/form_login`.

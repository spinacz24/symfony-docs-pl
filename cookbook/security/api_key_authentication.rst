.. highlight:: php
   :linenothreshold: 2

.. index::
    single: bezpieczeństwo; wystawca uwierzytelniania
    single: bezpieczeństwo; klucze API
    single: wystawca uwierzytelniania; klucze API

Jak uwierzytelniać użytkowników kluczami API
============================================

Obecnie, jest dość niezwykłe uwierzytelnianie użytkownika poprzez klucz API
(na przykład, podczas tworzenia serwisu internetowego). Klucza API jest dostarczany
przy każdym żądaniu i jest przekazywany jako parametr łańcucha zapytań w adresie
URL lub w nagłówku HTTP.

Wystawca uwierzytelniającego klucza API
---------------------------------------

Uwierzytelnianie użytkownika w oparciu o informacje z żądania powinno byc realizowane
poprzez mechanizm wstępnego uwierzytelniania. Interfejs
:class:`Symfony\\Component\\Security\\Core\\Authentication\\SimplePreAuthenticatorInterface`
pozwala w bardzo łatwy sposób zaimplementować taki schemat.

Twoja sytuacja może odbiegać od tego, co przyjęliśmy tutaj - w niżej prezentowanym
przykładzie token jest odczytywany z parametru zapytania ``apikey``, na podstawie
tej wartości jest ładowana właściwa nazwa użytkownika i następnie tworzony jest
obiekt User::

    // src/AppBundle/Security/ApiKeyAuthenticator.php
    namespace AppBundle\Security;

    use Symfony\Component\Security\Core\Authentication\SimplePreAuthenticatorInterface;
    use Symfony\Component\Security\Core\Authentication\Token\TokenInterface;
    use Symfony\Component\Security\Core\Exception\AuthenticationException;
    use Symfony\Component\Security\Core\Authentication\Token\PreAuthenticatedToken;
    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\Security\Core\User\UserProviderInterface;
    use Symfony\Component\Security\Core\Exception\BadCredentialsException;

    class ApiKeyAuthenticator implements SimplePreAuthenticatorInterface
    {
        public function createToken(Request $request, $providerKey)
        {
            // look for an apikey query parameter
            $apiKey = $request->query->get('apikey');

            // or if you want to use an "apikey" header, then do something like this:
            // $apiKey = $request->headers->get('apikey');

            if (!$apiKey) {
                throw new BadCredentialsException('No API key found');

                // or to just skip api key authentication
                // return null;
            }

            return new PreAuthenticatedToken(
                'anon.',
                $apiKey,
                $providerKey
            );
        }

        public function authenticateToken(TokenInterface $token, UserProviderInterface $userProvider, $providerKey)
        {
            if (!$userProvider instanceof ApiKeyUserProvider) {
                throw new \InvalidArgumentException(
                    sprintf(
                        'The user provider must be an instance of ApiKeyUserProvider (%s was given).',
                        get_class($userProvider)
                    )
                );
            }

            $apiKey = $token->getCredentials();
            $username = $userProvider->getUsernameForApiKey($apiKey);

            if (!$username) {
                throw new AuthenticationException(
                    sprintf('API Key "%s" does not exist.', $apiKey)
                );
            }

            $user = $userProvider->loadUserByUsername($username);

            return new PreAuthenticatedToken(
                $user,
                $apiKey,
                $providerKey,
                $user->getRoles()
            );
        }

        public function supportsToken(TokenInterface $token, $providerKey)
        {
            return $token instanceof PreAuthenticatedToken && $token->getProviderKey() === $providerKey;
        }
    }

Po :ref:`skonfigurowaniu <cookbook-security-api-key-config>` wszystkiego, będzie
są w stanie wykonywać uwierzutelnianie, przez dodawanie parametru ``apikey`` do
łańcucha zapytania, podobnie do tego:
``http://example.com/admin/foo?apikey=37b51d194a7513e45b56f6524f2d51f2``.

Nasz proces uwierzytelniania ma kilka etapów, ale Twoja implementacja będzie
przypuszczalnie sie różnić:

1. createToken
~~~~~~~~~~~~~~

Na początku cyklu przetwarzania żądania, Symfony wywołuje metodę ``createToken()``.
Zadaniem programisty jest tutaj stworzenie obiektu tokenu, który bedzie zawierał
wszystkie informacje z żądania, na podstawie którego dokonywane jest uwierzytelnienie
użytkownika (np. paraetr zapytania ``apikey``). Jeśli tej informacji nie ma,
zrzucany bedzie wyjątek
:class:`Symfony\\Component\\Security\\Core\\Exception\\BadCredentialsException`.
Trzeba sie wiec zabezpieczyć i w takiej sytuacji zwracać ``null``, aby pominąć
uwierzytelnianie i ewentualnie skierować Symfony awaryjnie na inna metodę
uwierzytelnia.

2. supportsToken
~~~~~~~~~~~~~~~~

.. include:: _supportsToken.rst.inc

3. authenticateToken
~~~~~~~~~~~~~~~~~~~~

Jeśli ``supportsToken()`` zwraca ``true``, Symfony bedzie teraz wywoływał ``authenticateToken()``.
Jedną częścią klucza jest parametr ``$userProvider``, wskazujący zewnętrzną klasę
pomagającą załadować informacje o użytkowniku. Więcej na ten temat w dalszej części
artykułu.

W tym konkretnym przykładzie, w ``authenticateToken()`` dzieją się następujace rzeczy:

#. Po pierwsze, wykorzytywany jest ``$userProvider``, aby jakoś zajrzeć
   do ``$username`` korespondujacego z ``$apiKey``;
#. Po drugie, ponownie użyty zostaje ``$userProvider`` do załadowania lub stworzenia
   obiektu ``User`` dla ``$username``;
#. Wreszcie, tworzony jest *token uwierzytelniania* (czyli token z co najmniej
   jedną rolą), który ma odpowiednie role i załączony obiekt User.

Ostatecznym celem jest wykorzystanie ``$apiKey`` do odnalezienia lub utworzenia
obiektu ``User``. Sposób zrobienia tego (np. zapytania do bazy danych) i konkretna
klasa obiektu ``User`` mogą być różne. Różnice te staną się bardziej widoczne przy
omawianiu dostawcy użytkowników.

Dostawca użytkowników
~~~~~~~~~~~~~~~~~~~~~

Wartością ``$userProvider`` może być każdy dostawca użytkowników (patrz :doc:`/cookbook/security/custom_provider`).
W naszym przykładzie, ``$apiKey`` został użyty do odnalezienia nazwy użytkownika
dla konkretnego użytkownika. Wykonywane jest to w metodzie ``getUsernameForApiKey()``
)nie jest to metoda stosowana w rdzennym mechaniźmie dostawcy użytkowników
Symfony).

Kod ``$userProvider`` może mieć taką postać::

    // src/AppBundle/Security/ApiKeyUserProvider.php
    namespace AppBundle\Security;

    use Symfony\Component\Security\Core\User\UserProviderInterface;
    use Symfony\Component\Security\Core\User\User;
    use Symfony\Component\Security\Core\User\UserInterface;
    use Symfony\Component\Security\Core\Exception\UnsupportedUserException;

    class ApiKeyUserProvider implements UserProviderInterface
    {
        public function getUsernameForApiKey($apiKey)
        {
            // Look up the username based on the token in the database, via
            // an API call, or do something entirely different
            $username = ...;

            return $username;
        }

        public function loadUserByUsername($username)
        {
            return new User(
                $username,
                null,
                // the roles for the user - you may choose to determine
                // these dynamically somehow based on the user
                array('ROLE_USER')
            );
        }

        public function refreshUser(UserInterface $user)
        {
            // this is used for storing authentication in the session
            // but in this example, the token is sent in each request,
            // so authentication can be stateless. Throwing this exception
            // is proper to make things stateless
            throw new UnsupportedUserException();
        }

        public function supportsClass($class)
        {
            return 'Symfony\Component\Security\Core\User\User' === $class;
        }
    }

Teraz zarejestrujemy naszego dostawcę użytkowników jako usługę:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/services.yml
        services:
            api_key_user_provider:
                class: AppBundle\Security\ApiKeyUserProvider

    .. code-block:: xml
       :linenos:

        <!-- app/config/services.xml -->
        <?xml version="1.0" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">
            <services>
                <!-- ... -->

                <service id="api_key_user_provider"
                    class="AppBundle\Security\ApiKeyUserProvider" />
            </services>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/services.php

        // ...
        $container
            ->register('api_key_user_provider', 'AppBundle\Security\ApiKeyUserProvider');

.. note::

    Proszę przeczytać dedykowany artykuł, aby dowiedzieć sie
    :doc:`jak utworzyć własnego dostawcę użytkowników </cookbook/security/custom_provider>`.

Logika wewnątrz ``getUsernameForApiKey()`` jest czytelna. Można przekształcić
klucz API (np. ``37b51d``) do nazwy użytkownika (np. ``jondoe``) poprzez spawdzenie
informacji w tabeli bazy danych "token".

To samo odnosi się do ``loadUserByUsername()``. W tym przykładzie, wykorzystaliśmy
rdzenna klasę :class:`Symfony\\Component\\Security\\Core\\User\\User`.
Ma to sens, gdy nie trzeba przechowywać jakichś dodatkowych informacje w obiekcie
User (np. ``firstName``). Jeśli zachodzi taka potrzeba, można stworzyć własną
klasę użytkownika i wypełnić ją tutaj wykorzystując zapytanie do bazy danych.
Umożliwi to posiadanie własnych danych w obiekcie ``User``.

Na koniec, trzeba sie upwenić, że ``supportsClass()`` zwraca ``true`` dla obiektu
User tej samej klasy, co obiekt użytkownika zwracany w ``loadUserByUsername()``.
Jeśli uwierzytelnianie jest bezstanowe, tak jak w tym przykładzie (czyli można
oczekiwać, że użytkownik przesyła klucz API w kazdym żądaniu i dlatego nie zapisywać
loginu w sesji), można po prostu zrzucić wyjątek ``UnsupportedUserException``
w ``refreshUser()``.

.. note::

    Jeśli chcesz przechowywć dane uwierzytelniania w sesji, tak aby nie trzeba
    było przesyłać klucza przy każdym żądaniu, przeczytaj :ref:`cookbook-security-api-key-session`.

Obsługa błędów uwierzytelniania
-------------------------------

In order for your ``ApiKeyAuthenticator`` to correctly display a 403
http status when either bad credentials or authentication fails you will
need to implement the :class:`Symfony\\Component\\Security\\Http\\Authentication\\AuthenticationFailureHandlerInterface` on your
Authenticator. This will provide a method ``onAuthenticationFailure`` which
you can use to create an error ``Response``.

.. code-block:: php

    // src/AppBundle/Security/ApiKeyAuthenticator.php
    namespace AppBundle\Security;

    use Symfony\Component\Security\Core\Authentication\SimplePreAuthenticatorInterface;
    use Symfony\Component\Security\Core\Exception\AuthenticationException;
    use Symfony\Component\Security\Http\Authentication\AuthenticationFailureHandlerInterface;
    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\HttpFoundation\Request;

    class ApiKeyAuthenticator implements SimplePreAuthenticatorInterface, AuthenticationFailureHandlerInterface
    {
        // ...

        public function onAuthenticationFailure(Request $request, AuthenticationException $exception)
        {
            return new Response("Authentication Failed.", 403);
        }
    }

.. _cookbook-security-api-key-config:

Configuration
-------------

Once you have your ``ApiKeyAuthenticator`` all setup, you need to register
it as a service and use it in your security configuration (e.g. ``security.yml``).
First, register it as a service.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        services:
            # ...

            apikey_authenticator:
                class:  AppBundle\Security\ApiKeyAuthenticator
                public: false

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">
            <services>
                <!-- ... -->

                <service id="apikey_authenticator"
                    class="AppBundle\Security\ApiKeyAuthenticator"
                    public="false" />
            </services>
        </container>

    .. code-block:: php

        // app/config/config.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;

        // ...

        $definition = new Definition('AppBundle\Security\ApiKeyAuthenticator');
        $definition->setPublic(false);
        $container->setDefinition('apikey_authenticator', $definition);

Now, activate it and your custom user provider (see :doc:`/cookbook/security/custom_provider`)
in the ``firewalls`` section of your security configuration
using the ``simple_preauth`` and ``provider`` keys respectively:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...

            firewalls:
                secured_area:
                    pattern: ^/admin
                    stateless: true
                    simple_preauth:
                        authenticator: apikey_authenticator
                    provider: api_key_user_provider

            providers:
                api_key_user_provider:
                    id: api_key_user_provider

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

                <firewall name="secured_area"
                    pattern="^/admin"
                    stateless="true"
                    provider="api_key_user_provider"
                >
                    <simple-preauth authenticator="apikey_authenticator" />
                </firewall>

                <provider name="api_key_user_provider" id="api_key_user_provider" />
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php

        // ..

        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area'       => array(
                    'pattern'        => '^/admin',
                    'stateless'      => true,
                    'simple_preauth' => array(
                        'authenticator'  => 'apikey_authenticator',
                    ),
                    'provider' => 'api_key_user_provider',
                ),
            ),
            'providers' => array(
                'api_key_user_provider'  => array(
                    'id' => 'api_key_user_provider',
                ),
            ),
        ));

That's it! Now, your ``ApiKeyAuthenticator`` should be called at the beginning
of each request and your authentication process will take place.

The ``stateless`` configuration parameter prevents Symfony from trying to
store the authentication information in the session, which isn't necessary
since the client will send the ``apikey`` on each request. If you *do* need
to store authentication in the session, keep reading!

.. _cookbook-security-api-key-session:

Storing Authentication in the Session
-------------------------------------

So far, this entry has described a situation where some sort of authentication
token is sent on every request. But in some situations (like an OAuth flow),
the token may be sent on only *one* request. In this case, you will want to
authenticate the user and store that authentication in the session so that
the user is automatically logged in for every subsequent request.

To make this work, first remove the ``stateless`` key from your firewall
configuration or set it to ``false``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...

            firewalls:
                secured_area:
                    pattern: ^/admin
                    stateless: false
                    simple_preauth:
                        authenticator: apikey_authenticator
                    provider: api_key_user_provider

            providers:
                api_key_user_provider:
                    id: api_key_user_provider

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

                <firewall name="secured_area"
                    pattern="^/admin"
                    stateless="false"
                    provider="api_key_user_provider"
                >
                    <simple-preauth authenticator="apikey_authenticator" />
                </firewall>

                <provider name="api_key_user_provider" id="api_key_user_provider" />
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php

        // ..
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area'       => array(
                    'pattern'        => '^/admin',
                    'stateless'      => false,
                    'simple_preauth' => array(
                        'authenticator'  => 'apikey_authenticator',
                    ),
                    'provider' => 'api_key_user_provider',
                ),
            ),
            'providers' => array(
                'api_key_user_provider' => array(
                    'id' => 'api_key_user_provider',
                ),
            ),
        ));

Even though the token is being stored in the session, the credentials - in this
case the API key (i.e. ``$token->getCredentials()``) - are not stored in the session
for security reasons. To take advantage of the session, update ``ApiKeyAuthenticator``
to see if the stored token has a valid User object that can be used::

    // src/AppBundle/Security/ApiKeyAuthenticator.php
    // ...

    class ApiKeyAuthenticator implements SimplePreAuthenticatorInterface
    {
        // ...
        public function authenticateToken(TokenInterface $token, UserProviderInterface $userProvider, $providerKey)
        {
            if (!$userProvider instanceof ApiKeyUserProvider) {
                throw new \InvalidArgumentException(
                    sprintf(
                        'The user provider must be an instance of ApiKeyUserProvider (%s was given).',
                        get_class($userProvider)
                    )
                );
            }

            $apiKey = $token->getCredentials();
            $username = $userProvider->getUsernameForApiKey($apiKey);

            // User is the Entity which represents your user
            $user = $token->getUser();
            if ($user instanceof User) {
                return new PreAuthenticatedToken(
                    $user,
                    $apiKey,
                    $providerKey,
                    $user->getRoles()
                );
            }

            if (!$username) {
                throw new AuthenticationException(
                    sprintf('API Key "%s" does not exist.', $apiKey)
                );
            }

            $user = $userProvider->loadUserByUsername($username);

            return new PreAuthenticatedToken(
                $user,
                $apiKey,
                $providerKey,
                $user->getRoles()
            );
        }
        // ...
    }

Storing authentication information in the session works like this:

#. At the end of each request, Symfony serializes the token object (returned
   from ``authenticateToken()``), which also serializes the ``User`` object
   (since it's set on a property on the token);
#. On the next request the token is deserialized and the deserialized ``User``
   object is passed to the ``refreshUser()`` function of the user provider.

The second step is the important one: Symfony calls ``refreshUser()`` and passes
you the user object that was serialized in the session. If your users are
stored in the database, then you may want to re-query for a fresh version
of the user to make sure it's not out-of-date. But regardless of your requirements,
``refreshUser()`` should now return the User object::

    // src/AppBundle/Security/ApiKeyUserProvider.php

    // ...
    class ApiKeyUserProvider implements UserProviderInterface
    {
        // ...

        public function refreshUser(UserInterface $user)
        {
            // $user is the User that you set in the token inside authenticateToken()
            // after it has been deserialized from the session

            // you might use $user to query the database for a fresh user
            // $id = $user->getId();
            // use $id to make a query

            // if you are *not* reading from a database and are just creating
            // a User object (like in this example), you can just return it
            return $user;
        }
    }

.. note::

    You'll also want to make sure that your ``User`` object is being serialized
    correctly. If your ``User`` object has private properties, PHP can't serialize
    those. In this case, you may get back a User object that has a ``null``
    value for each property. For an example, see :doc:`/cookbook/security/entity_provider`.

Only Authenticating for Certain URLs
------------------------------------

This entry has assumed that you want to look for the ``apikey`` authentication
on *every* request. But in some situations (like an OAuth flow), you only
really need to look for authentication information once the user has reached
a certain URL (e.g. the redirect URL in OAuth).

Fortunately, handling this situation is easy: just check to see what the
current URL is before creating the token in ``createToken()``::

    // src/AppBundle/Security/ApiKeyAuthenticator.php

    // ...
    use Symfony\Component\Security\Http\HttpUtils;
    use Symfony\Component\HttpFoundation\Request;

    class ApiKeyAuthenticator implements SimplePreAuthenticatorInterface
    {
        protected $httpUtils;

        public function __construct(HttpUtils $httpUtils)
        {
            $this->httpUtils = $httpUtils;
        }

        public function createToken(Request $request, $providerKey)
        {
            // set the only URL where we should look for auth information
            // and only return the token if we're at that URL
            $targetUrl = '/login/check';
            if (!$this->httpUtils->checkRequestPath($request, $targetUrl)) {
                return;
            }

            // ...
        }
    }

This uses the handy :class:`Symfony\\Component\\Security\\Http\\HttpUtils`
class to check if the current URL matches the URL you're looking for. In this
case, the URL (``/login/check``) has been hardcoded in the class, but you
could also inject it as the second constructor argument.

Next, just update your service configuration to inject the ``security.http_utils``
service:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        services:
            # ...

            apikey_authenticator:
                class:     AppBundle\Security\ApiKeyAuthenticator
                arguments: ["@security.http_utils"]
                public:    false

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">
            <services>
                <!-- ... -->

                <service id="apikey_authenticator"
                    class="AppBundle\Security\ApiKeyAuthenticator"
                    public="false"
                >
                    <argument type="service" id="security.http_utils" />
                </service>
            </services>
        </container>

    .. code-block:: php

        // app/config/config.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;

        // ...

        $definition = new Definition(
            'AppBundle\Security\ApiKeyAuthenticator',
            array(
                new Reference('security.http_utils')
            )
        );
        $definition->setPublic(false);
        $container->setDefinition('apikey_authenticator', $definition);

That's it! Have fun!

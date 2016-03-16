.. highlight:: php
   :linenothreshold: 2

.. index::
    single: bezpieczeństwo; wystawca uwierzytelniania
    single: bezpieczeństwo; klucze API
    single: wystawca uwierzytelniania; klucze API

Jak uwierzytelniać użytkowników kluczami API
============================================

.. tip::

    Proszę zapoznać sie z artykułem :doc:`/cookbook/security/guard-authentication`
    w celu rozważenia zastosowania prostszego i bardziej elastycznego sposobu
    realizacji własnych rozwiazań uwierzytelniania, niż ten.

Obecnie, jest dość niezwykłe uwierzytelnianie użytkownika poprzez klucz API
(na przykład, podczas tworzenia serwisu internetowego). Klucza API jest dostarczany
przy każdym żądaniu i jest przekazywany jako parametr łańcucha zapytań w adresie
URL lub w nagłówku HTTP.

Wystawca uwierzytelniającego klucza API
---------------------------------------

.. versionadded:: 2.8
   W Symfony 2.8, interfejs ``SimplePreAuthenticatorInterface`` został przeniesiony
   do przestrzeni nazewniczej ``Symfony\Component\Security\Http\Authentication``.
   Wcześniej uzywana była przestrzeń ``Symfony\Component\Security\Core\Authentication``.

Uwierzytelnianie użytkownika w oparciu o informacje z żądania powinno być realizowane
poprzez mechanizm wstępnego uwierzytelniania. Interfejs
:class:`Symfony\\Component\\Security\\Http\\Authentication\\SimplePreAuthenticatorInterface`
pozwala w bardzo łatwy sposób zaimplementować taki schemat.

Twoja sytuacja może odbiegać od tego, co przyjęliśmy tutaj - w niżej prezentowanym
przykładzie token jest odczytywany z parametru zapytania ``apikey``, na podstawie
tej wartości jest ładowana właściwa nazwa użytkownika i następnie tworzony jest
obiekt User::

    // src/AppBundle/Security/ApiKeyAuthenticator.php
    namespace AppBundle\Security;

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\Security\Core\Authentication\Token\PreAuthenticatedToken;
    use Symfony\Component\Security\Core\Authentication\Token\TokenInterface;
    use Symfony\Component\Security\Core\Exception\AuthenticationException;
    use Symfony\Component\Security\Core\Exception\CustomUserMessageAuthenticationException;
    use Symfony\Component\Security\Core\Exception\BadCredentialsException;
    use Symfony\Component\Security\Core\User\UserProviderInterface;
    use Symfony\Component\Security\Http\Authentication\SimplePreAuthenticatorInterface;

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
                // UWAGA: ten komunikat bedzie zwracany do klienta
                // (więc nie umieszczaj tutaj żadnych nie zaufanych komunikatów
                // i łańcuchów tekstowych błędów)
                throw new CustomUserMessageAuthenticationException(
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

.. versionadded:: 2.8
    Klasa ``CustomUserMessageAuthenticationException`` jest nowościa w Symfony 2.8
    i pomaga zwracać własne komunikaty uwierzytelniania. W Symfony 2.7 i wersjach
    wcześniejszych, zrzucany był wyjątek ``AuthenticationException`` lub jakaś
    podklasa (mozna to dalej robić w wersji 2.8).

Po :ref:`skonfigurowaniu <cookbook-security-api-key-config>` wszystkiego, będzie
są w stanie wykonywać uwierzutelnianie, przez dodawanie parametru ``apikey`` do
łańcucha zapytania, podobnie do tego:
``http://example.com/admin/foo?apikey=37b51d194a7513e45b56f6524f2d51f2``.

Nasz proces uwierzytelniania ma kilka etapów, ale Twoja implementacja będzie
przypuszczalnie sie różnić:

1. createToken
~~~~~~~~~~~~~~

Na początku cyklu przetwarzania żądania, Symfony wywołuje metodę ``createToken()``.
Zadaniem programisty jest tutaj stworzenie obiektu tokenu, który będzie zawierał
wszystkie informacje z żądania, na podstawie którego dokonywane jest uwierzytelnienie
użytkownika (np. paraetr zapytania ``apikey``). Jeśli tej informacji nie ma,
zrzucany będzie wyjątek
:class:`Symfony\\Component\\Security\\Core\\Exception\\BadCredentialsException`.
Trzeba sie wiec zabezpieczyć i w takiej sytuacji zwracać ``null``, aby pominąć
uwierzytelnianie i ewentualnie skierować Symfony awaryjnie na inna metodę
uwierzytelnia.

2. supportsToken
~~~~~~~~~~~~~~~~

.. include:: _supportsToken.rst.inc

3. authenticateToken
~~~~~~~~~~~~~~~~~~~~

Jeśli ``supportsToken()`` zwraca ``true``, Symfony będzie teraz wywoływał ``authenticateToken()``.
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

Operator użytkowników
~~~~~~~~~~~~~~~~~~~~~

Wartością ``$userProvider`` może być każdy dostawca użytkowników (patrz :doc:`/cookbook/security/custom_provider`).
W naszym przykładzie, ``$apiKey`` został użyty do odnalezienia nazwy użytkownika
dla konkretnego użytkownika. Wykonywane jest to w metodzie ``getUsernameForApiKey()``
(nie jest to metoda stosowana w rdzennym mechaniźmie dostawcy użytkowników
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

W celu poprawnego wyświetlania stanu 403 HTTP przez ``ApiKeyAuthenticator``, kiedy
nieprawidłowe są poświadczenia albo uwierzylenianie, trzeba zaimplementować klasę
:class:`Symfony\\Component\\Security\\Http\\Authentication\\AuthenticationFailureHandlerInterface`
w swoim mechanizmie uwierzytelniania. Interfejs ten dostarcza metodę
``onAuthenticationFailure``, którą można wykorzystać do utworzenia obiektu
``Response`` dla błędu.

.. code-block:: php
   :linenos:

    // src/AppBundle/Security/ApiKeyAuthenticator.php
    namespace AppBundle\Security;

    use Symfony\Component\Security\Core\Exception\AuthenticationException;
    use Symfony\Component\Security\Http\Authentication\AuthenticationFailureHandlerInterface;
    use Symfony\Component\Security\Http\Authentication\SimplePreAuthenticatorInterface;
    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\HttpFoundation\Request;

    class ApiKeyAuthenticator implements SimplePreAuthenticatorInterface, AuthenticationFailureHandlerInterface
    {
        // ...

        public function onAuthenticationFailure(Request $request, AuthenticationException $exception)
        {
            return new Response(
                // this contains information about *why* authentication failed
                // use it, or return your own message
                strtr($exception->getMessageKey(), $exception->getMessageData())
            , 403)
        }
    }

.. _cookbook-security-api-key-config:

Konfiguracja
------------

Gdy ma się już gotową całą konfigurację ``ApiKeyAuthenticator``, trzeba tą klasę
zarejestrować jako usługę i zastosować ją w konfiguracji bezpieczeństwa
(np. ``security.yml``). 
Najpierw, zarejestrujmy tą klasę jako usługę:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        services:
            # ...

            apikey_authenticator:
                class:  AppBundle\Security\ApiKeyAuthenticator
                public: false

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

                <service id="apikey_authenticator"
                    class="AppBundle\Security\ApiKeyAuthenticator"
                    public="false" />
            </services>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;

        // ...

        $definition = new Definition('AppBundle\Security\ApiKeyAuthenticator');
        $definition->setPublic(false);
        $container->setDefinition('apikey_authenticator', $definition);

Po aktywowaniu usługi, nasz własny dostawca użytkowników (patrz :doc:`/cookbook/security/custom_provider`)
będzie teraz wykorzystywał, w sekcji ``firewalls`` konfiguracji bezpieczeństwa, odpowiednio klucze
``simple_preauth`` i ``provider``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

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
                    stateless="true"
                    provider="api_key_user_provider"
                >
                    <simple-preauth authenticator="apikey_authenticator" />
                </firewall>

                <provider name="api_key_user_provider" id="api_key_user_provider" />
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

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

Od teraz, ``ApiKeyAuthenticator`` powinien być wywoływany na początku
każdego żądania i będzie inicjować proces uwierzytelniania.

Parametr konfiguracyjny ``stateless`` zabezpiecza Symfony przed próbami przechowywania
informacji uwierzytelniania w sesji, co nie jest konieczne, gdyż klient będzie
wysyłał ``apikey`` przy każdym żądaniu. Jeśli zachodzi konieczność przechowywania
danych uwierzytelniania w sesji, to proszę zapoznać sie z następnym rozdziałem.

.. _cookbook-security-api-key-session:

Przechowywanie danych uwierzytelniania w sesji
----------------------------------------------

Dotychczas, w artykule tym opisywano sytuację, w której przy kazdym żądaniu przesyłany
był jakiś rodzaj tokenu uwierzytelniania. Występuja jednak sytuacje(jak w OAuth),
że token może być wysłany tylko przy jednym żądaniu. W takim przypadku, trzeba
uwierzytelnić użytkownika i przechowywać dane uwierzytelniania w sesji, tak aby
użytkownik mógł być automatycznie logowany przy każdym kolejnym żądaniu.

Dla zrobienia tego, trzeba najpierw usunąć klucz ``stateless``z konfiguracji zapory
lub ustawić tą opcję na ``false``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

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
                    stateless="false"
                    provider="api_key_user_provider"
                >
                    <simple-preauth authenticator="apikey_authenticator" />
                </firewall>

                <provider name="api_key_user_provider" id="api_key_user_provider" />
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

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

Nawet jeśli token jest przechowywany w sesji, poświadczenia, w naszym przypadku
klucz API key (czyli ``$token->getCredentials()``), nie są przechowywane w sesji
ze względów bezpieczeństwa. Dla wykorzystania sesji, trzeba zaktualizować ``ApiKeyAuthenticator``,
aby sprawdzić, czy przechowywany token ma prawidłowy obiekt User, który można
użyć::

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
                // this message will be returned to the client
                throw new CustomUserMessageAuthenticationException(
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

Przechowywanie danych uwierzytelniania w sesji działa w poniższy sposób:

#. Na końcu przetwarzania każdego żądania, Symfony serializuje obiekt tokenu (zwracanego przez
   ``authenticateToken()``), który serializuje również obiekt ``User`` (ponieważ
   jest on ustawiany przez właściwość w obiekcie tokenu);
#. Przy następnym żądaniu token jest deserializowany i zdeserializowany obiekt
   ``User`` jest przekazywany do funkcji ``refreshUser()`` dostawcy użytkowników.

Drugi krok jest ważny: Symfony wywołuje ``refreshUser()`` i przekazuje obiekt
użytkownika, który został zserializowany w sesji. Jeśli użytkownicy są przechowywani
w bazie danych, to można wykonać ponownie zapytanie, w celu odświeżenia wersji
użytkownika, aby mieć pewność, że obiekt użytkownika nie zdeaktualizował sie.
Niezależnie od wymagań, ``refreshUser()`` powinien teraz zwracać obiekt użytkownika::

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

    Warto również upewnić się, że obieky ``User`` jest poprawnie serializowany.
    Jeśli obiekt ``User`` ma prywatne własności, PHP nie będzie mógł go zserializować.
    W takim przypadku, można odzyskać obiekt User, którego wszystkie właściwości
    mają wartość ``null``. Dla przykładu proszę przeczytać artykuł :doc:`/cookbook/security/entity_provider`.

Uwierzytelnianie tylko z niektórych adresów URL
-----------------------------------------------

W tym artykule założono również, że chce się sprawdzać uwierzytelnianie ``apikey``
dla każdego żądania. Jednak w niektórych sytuacjach (jak w przypadku OAuth),
trzeba w rzeczywistości sprawdzać dane uwierzytelniania, które użytkownik dostarczył
tylko z określonego adresu URL (np. adres URL przekierowania w OAuth).

Na szczęście, obsługa takiej sytuacji jest łatwa: wystarczy sprawdzić, jaki
adres URL pojawił się tuż przed utworzeniem tokenu w ``createToken()``::

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

W kodzie tym, do sprawdzenia, czy aktualna ścieżka URL zgodny jest z wyszukiwanym
adresem, użyto przydatnej klasy :class:`Symfony\\Component\\Security\\Http\\HttpUtils`.
W naszym przypadku, ścieżka URL (``/login/check``) został sztywno zakodowana w klasie,
ale mozna go również wstrzyknąć jako drugi argument konstruktora.

Następnie, wystarczy skonfigurować swoja usługę, wstrzykując w niej usługę
``security.http_utils``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        services:
            # ...

            apikey_authenticator:
                class:     AppBundle\Security\ApiKeyAuthenticator
                arguments: ["@security.http_utils"]
                public:    false

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

                <service id="apikey_authenticator"
                    class="AppBundle\Security\ApiKeyAuthenticator"
                    public="false"
                >
                    <argument type="service" id="security.http_utils" />
                </service>
            </services>
        </container>

    .. code-block:: php
       :linenos:

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

To wszystko. Przyjemnej zabawy!

.. index::
   single: bezpieczeństwo; wystawca uwierzytelniania

Jak utworzyć własnego wystawcę uwierzytelniania
===============================================

.. tip::

    Tworzenie własnego systemu uwierzytelniania jest trudne. Artykuł niniejszy
    omawia ten proces. Jednak, w zależności od potrzeb, możesz rozwiązać swój
    problem w prostszy sposób lub wykorzystać jakiś pakiet społecznościowy:

    * :doc:`/cookbook/security/guard-authentication`
    * :doc:`/cookbook/security/custom_password_authenticator`
    * :doc:`/cookbook/security/api_key_authentication`
    * W celu uwierzytelniania poprzez OAuth z wykorzystanie usług zewnętrznych,
      takich jak Google, Facebook lub Twitter, spróbuj użyć pakietu społecznościowego
      `HWIOAuthBundle`_.

W rozdziale :doc:`/book/security` podręcznika Symfony, wytłumaczono, jak Symfony
dokonuje uwierzytelniania i autoryzacji w implementacji bezpieczeństwa.
W rozdziale niniejszym omawia się rdzenne klasy zaangażowane w proces uwierzytelniania
oraz to, jak można zaimplementować własnego wystawcę uwierzytelniania.
Ponieważ uwierzytelnianie i autoryzacja są odzielnymi koncepcjami,
to rozszerzenie będzie niezależne od dostawcy użytkowników i będzie funkcjonować
z jakimkolwiek dostawcą zaimplementowanym w aplikacji: dostawcą z pamięci, dostawcą
z encji, czy też innym. 

Zapoznanie się z WSSE
---------------------

W rozdziale tym opisuje się, jak można stworzyć własnego wystawcę uwierzytelniania
dla uwierzytelniania `WS-Security`_ opartego na protokole `WSSE`_.
Protokół bezpieczeństwa WSSE zapewnia kilka korzyści:

#. szyfrowanie nazwy użytkownika i hasła,
#. ochronę przed `atakami replay`_,
#. brak wymagań konfiguracyjnych dla serwera internetowego.

WSSE jest bardzo przydatny dla zabezpieczania usług internetowych, takich jak
SOAP czy REST.

Opublikowana jest obszerna dokumentacja `WSSE`_. Artykuł ten nie skupia się
na samym protokole, ale na sposobie, w jaki zazwyczaj ten protokół może być
zastosowany w indywidulanym kodzie uwierzytelniania w aplikacjach Symfony.
Działanie WSSE polega na tym, że nagłówek żądania jest sprawdzany pod kątem
zaszyfrowanego poświadczenia, weryfikacji wykorzystującej znacznik czasu i token
`nonce`_ oraz uwierzytelnieniu dla zgłaszanego użytkownika przy użyciu
hasła ``Digest``.

.. note::

    WSSE obsługuje również sprawdzanie klucza aplikacji, co jest przydatne dla
    usług internetowych, ale nie jest to tematem tego artykułu.

Token
-----

W kontekście bezpieczeństwa Symfony rola tokenu jest bardzo ważna.
Token reprezentuje w żadaniu bieżące dane uwierzytelniania użytkownika. Po
uwierzytelnieniu żądania, token zachowuje dane użytkownika i dostarcza te
dane w całym kontekście bezpieczeństwa. Po pierwsze, można utworzyć klasę tokenu.
Umożliwia to przekazywanie wszystkich istotnych informacji do wystawcy uwierzytelniania.

.. code-block:: php
   :linenos:

    // src/AppBundle/Security/Authentication/Token/WsseUserToken.php
    namespace AppBundle\Security\Authentication\Token;

    use Symfony\Component\Security\Core\Authentication\Token\AbstractToken;

    class WsseUserToken extends AbstractToken
    {
        public $created;
        public $digest;
        public $nonce;

        public function __construct(array $roles = array())
        {
            parent::__construct($roles);

            // If the user has roles, consider it authenticated
            $this->setAuthenticated(count($roles) > 0);
        }

        public function getCredentials()
        {
            return '';
        }
    }

.. note::

    Klasa ``WsseUserToken`` rozszerza klasę
    :class:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\AbstractToken`
    komponentu Security,
    która dostarcza podstawową funkcjonalność tokenu. W każdej klasie trzeba
    zaimplementować interfejs,
    :class:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\TokenInterface`
    aby umożliwić stosowanie tokenu.

Nasłuch
-------

Następnie musimy stworzyć detektor zdarzeń (*ang. listener*) do nasłuchu zapory.
Detektor zdarzeń jest odpowiedzialny za kierowanie żądań do zapory i wywoływanie
wystawcy uwierzytelniania. Detektor musi być instancją interfejsu
:class:`Symfony\\Component\\Security\\Http\\Firewall\\ListenerInterface`.
Powinien obsługiwać zdarzenie
:class:`Symfony\\Component\\HttpKernel\\Event\\GetResponseEvent` i ustawiać
token uwierzytelniania w razie powodzenia.

.. code-block:: php
   :linenos:

    // src/AppBundle/Security/Firewall/WsseListener.php
    namespace AppBundle\Security\Firewall;

    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\HttpKernel\Event\GetResponseEvent;
    use Symfony\Component\Security\Core\Authentication\AuthenticationManagerInterface;
    use Symfony\Component\Security\Core\Authentication\Token\Storage\TokenStorageInterface;
    use Symfony\Component\Security\Core\Exception\AuthenticationException;
    use Symfony\Component\Security\Http\Firewall\ListenerInterface;
    use AppBundle\Security\Authentication\Token\WsseUserToken;

    class WsseListener implements ListenerInterface
    {
        protected $tokenStorage;
        protected $authenticationManager;

        public function __construct(TokenStorageInterface $tokenStorage, AuthenticationManagerInterface $authenticationManager)
        {
            $this->tokenStorage = $tokenStorage;
            $this->authenticationManager = $authenticationManager;
        }

        public function handle(GetResponseEvent $event)
        {
            $request = $event->getRequest();

            $wsseRegex = '/UsernameToken Username="([^"]+)", PasswordDigest="([^"]+)", Nonce="([^"]+)", Created="([^"]+)"/';
            if (!$request->headers->has('x-wsse') || 1 !== preg_match($wsseRegex, $request->headers->get('x-wsse'), $matches)) {
                return;
            }

            $token = new WsseUserToken();
            $token->setUser($matches[1]);

            $token->digest   = $matches[2];
            $token->nonce    = $matches[3];
            $token->created  = $matches[4];

            try {
                $authToken = $this->authenticationManager->authenticate($token);
                $this->tokenStorage->setToken($authToken);

                return;
            } catch (AuthenticationException $failed) {
                // ... you might log something here

                // To deny the authentication clear the token. This will redirect to the login page.
                // Make sure to only clear your token, not those of other authentication listeners.
                // $token = $this->tokenStorage->getToken();
                // if ($token instanceof WsseUserToken && $this->providerKey === $token->getProviderKey()) {
                //     $this->tokenStorage->setToken(null);
                // }
                // return;
            }

            // By default deny authorization
            $response = new Response();
            $response->setStatusCode(Response::HTTP_FORBIDDEN);
            $event->setResponse($response);
        }
    }

Detektor ten sprawdza żądanie pod kątem oczekiwanego nagłówka ``X-WSSE``, dopasowuje
wartość zwracaną dla oczekiwanych informacji WSSE, tworzy token używając tej informacji
i przekazuje token do menadżera uwierzytelniania. Gdy nie została dostarczona
odpowiednia informacja lub menadżer uwierzytelniania zrzuca wyjątek
:class:`Symfony\\Component\\Security\\Core\\Exception\\AuthenticationException`,
zwracana jest odpowiedź 403.

.. note::

    Nie używana powyżej klasa
    :class:`Symfony\\Component\\Security\\Http\\Firewall\\AbstractAuthenticationListener`,
    jest bardzo przydatną klasą bazową, która dostarcza powszechnie potrzebną
    funkcjonalność dla rozszerzeń bezpieczeństwa. Obejmuje to utrzymanie tokenu
    w sesji, obsługę procedury sukcesu i niepowodzenia, dostarczanie adresów URL
    formularza logowania i wiele więcej. Ponieważ WSSE nie wymaga utrzymywania
    sesji uwierzytelniania lub formularzy logowania, nie będzie tego wykorzystywać
    w tym przykładzie.

.. note::

    Zwracanie przedwczesne przez detektor ma znaczenie tylko, jeśli chce się złączyć
    wystawców uwierzytelniania (na przykład, aby umożliwić dostęp anonimowy).
    Jeśli chce się zabronic anonimowego dostępu i mieć ładny komunikat błędu 403,
    powinno się ustawić kod stanu przed wyrażeniem zwracającym w kodzie odpowiedzi.


.. index::
   wystawca uwierzytelniania 

Wystawca uwierzytelniania
-------------------------

Wystawca uwierzytelniania wykonuje weryfikację ``WsseUserToken``.
Mianowicie, wystawca weryfikuje, czy wartość nagłówka ``Created`` jest ważna w
okresie pięciu minut, czy wartość nagłówka ``Nonce`` jest unikatowa w ciągu pięciu
minut i czy wartość nagłówka ``PasswordDigest`` jest zgodna z hasłem użytkownika.

.. code-block:: php
   :linenos:

    // src/AppBundle/Security/Authentication/Provider/WsseProvider.php
    namespace AppBundle\Security\Authentication\Provider;

    use Symfony\Component\Security\Core\Authentication\Provider\AuthenticationProviderInterface;
    use Symfony\Component\Security\Core\User\UserProviderInterface;
    use Symfony\Component\Security\Core\Exception\AuthenticationException;
    use Symfony\Component\Security\Core\Exception\NonceExpiredException;
    use Symfony\Component\Security\Core\Authentication\Token\TokenInterface;
    use AppBundle\Security\Authentication\Token\WsseUserToken;
    
    class WsseProvider implements AuthenticationProviderInterface
    {
        private $userProvider;
        private $cacheDir;

        public function __construct(UserProviderInterface $userProvider, $cacheDir)
        {
            $this->userProvider = $userProvider;
            $this->cacheDir     = $cacheDir;
        }

        public function authenticate(TokenInterface $token)
        {
            $user = $this->userProvider->loadUserByUsername($token->getUsername());

            if ($user && $this->validateDigest($token->digest, $token->nonce, $token->created, $user->getPassword())) {
                $authenticatedToken = new WsseUserToken($user->getRoles());
                $authenticatedToken->setUser($user);

                return $authenticatedToken;
            }

            throw new AuthenticationException('The WSSE authentication failed.');
        }

        /**
         * Funkcja ta jest specyficzna dla uwierzytelniania WSSE i została jako pomoc
         * w zrozumieniu tego przyklad.
         *
         * Wiecej informacji mozna znaleźć pod adresem
         * https://github.com/symfony/symfony-docs/pull/3134#issuecomment-27699129
         */
        protected function validateDigest($digest, $nonce, $created, $secret)
        {
            // Sprawdzenie, czy utworzony czas nie jest w przyszłości
            if (strtotime($created) > time()) {
                return false;
            }

            // Wygaśniecie znacznika czasowego po 5 minutach
            if (time() - strtotime($created) > 300) {
                return false;
            }

            // Sprawdzenie, czy token noncenie został użyty w ciagu ostatnich 5 minut,
            // jeśli tak, może to być powtórzenie ataku
            if (file_exists($this->cacheDir.'/'.$nonce) && file_get_contents($this->cacheDir.'/'.$nonce) + 300 > time()) {
                throw new NonceExpiredException('Previously used nonce detected');
            }
            // Jeśli nie istnieje katalog pamięci podręcznej, można go utworzyć
            if (!is_dir($this->cacheDir)) {
                mkdir($this->cacheDir, 0777, true);
            }
            file_put_contents($this->cacheDir.'/'.$nonce, time());

            // sprawdzenie sekretu
            $expected = base64_encode(sha1(base64_decode($nonce).$created.$secret, true));

            return hash_equals($expected, $digest);
        }

        public function supports(TokenInterface $token)
        {
            return $token instanceof WsseUserToken;
        }
    }

.. note::

    Interfejs
    :class:`Symfony\\Component\\Security\\Core\\Authentication\\Provider\\AuthenticationProviderInterface`
    wymaga, aby w klasie tokenu użytkownika były określone metody ``authenticate``
    i ``supports``, które informują menadżera uwierzytelniania, czy dla danego
    tokenu trzeba stosować tego wystawcę. W przypadku wielu wystawców, menadżer
    uwierzytelniania będzie się następnie przenosił do kolejnego wystawcy na liście .


Wytwórnia
---------

Utworzyliśmy własny token, własny detektor i własnego wystawcę uwierzytelniania.
Teraz potrzeba powiązać razem te elementy. Jak można wykonać własnego wystawcę
dostępnego dla każdej zapory? Odpowiedzią jest - użycie *wytwórni* (*ang. factory*).
Wytwórnia jest klasą, gdzie można podczepić się do komponentu Security, informując
go o nazwie swojego wystawcy i wszystkich dostępnych opcjach konfiguracyjnych.
Najpierw, musimy utworzyć klasę implementującą
:class:`Symfony\\Bundle\\SecurityBundle\\DependencyInjection\\Security\\Factory\\SecurityFactoryInterface`.

.. code-block:: php
   :linenos:

    // src/AppBundle/DependencyInjection/Security/Factory/WsseFactory.php
    namespace AppBundle\DependencyInjection\Security\Factory;

    use Symfony\Component\DependencyInjection\ContainerBuilder;
    use Symfony\Component\DependencyInjection\Reference;
    use Symfony\Component\DependencyInjection\DefinitionDecorator;
    use Symfony\Component\Config\Definition\Builder\NodeDefinition;
    use Symfony\Bundle\SecurityBundle\DependencyInjection\Security\Factory\SecurityFactoryInterface;

    class WsseFactory implements SecurityFactoryInterface
    {
        public function create(ContainerBuilder $container, $id, $config, $userProvider, $defaultEntryPoint)
        {
            $providerId = 'security.authentication.provider.wsse.'.$id;
            $container
                ->setDefinition($providerId, new DefinitionDecorator('wsse.security.authentication.provider'))
                ->replaceArgument(0, new Reference($userProvider))
            ;

            $listenerId = 'security.authentication.listener.wsse.'.$id;
            $listener = $container->setDefinition($listenerId, new DefinitionDecorator('wsse.security.authentication.listener'));

            return array($providerId, $listenerId, $defaultEntryPoint);
        }

        public function getPosition()
        {
            return 'pre_auth';
        }

        public function getKey()
        {
            return 'wsse';
        }

        public function addConfiguration(NodeDefinition $node)
        {
        }
    }

Interfejs :class:`Symfony\\Bundle\\SecurityBundle\\DependencyInjection\\Security\\Factory\\SecurityFactoryInterface`
wymaga następujących metod:

``create``
    Metoda, która dodaje detektor zdarzeń i wystawcę uwierzytekniania do kontenera
    DI dla odpowiedniego kontekstu bezpieczeństwa.

``getPosition``
    Zwraca wartość wskazującą jak należy wywołać wystawcę uwierzytelniania. Może
    to być jedna z następujących wartości: ``pre_auth``, ``form``, ``http`` lub
    ``remember_me``.

``getKey``
    Metoda definiująca klucz konfiguracyjny stosowany do odwoływania się wystawcy
    uwierzytelniania w konfiguracji zapory.

``addConfiguration``
    Metoda używana do definiowania opcji konfiguracyjnych
    options w kluczu konfiguracyjnym w konfiguracji bezpieczeństwa.
    Ustawienia opcji konfiguracyjnych są wyjaśnione dalej w tym rozdziale.

.. note::

    Nie używana w tym przykładzie klasa
    :class:`Symfony\\Bundle\\SecurityBundle\\DependencyInjection\\Security\\Factory\\AbstractFactory`,
    jest bardzo przydatną klasą bazową, która dostarcza powszechnie potrzebną
    funkcjonalność dla wytwórni bezpieczeństwa. Może być ona użyteczna przy
    definiowaniu wystawcy uwierzytelniania różnego typu.

Teraz, gdy utworzyliśmy klasę wytwórni, klucz ``wsse`` może być wykorzystywany
jako zapora w konfiguracji bezpieczeństwa.

.. note::

    Można się zastanawiać, dlaczego potrzebna jest specjalna klasa wytwórni dla
    dodawania detektorów i wystawców do kontenera wstrzykiwania zależności. Jest
    to bardzo dobre pytanie. Powodem jest to, że można stosować zaporę wiele razy,
    aby zabezpieczyć wiele części swojej aplikacji. Dlatego, przy każdym użyciu
    zapory, tworzona jest nowa usługa w kontenerze DI. Wytwórnia jest mechanizmem
    tworzącym te nowe usługi.

Konfiguracja
------------

Nadszedł czas, aby zobaczyć naszego wystawcę uwierzytelniania w akcji. W tym celu,
będziemy musieli zrobić kilka rzeczy. Pierwszą, jest dodanie powyższej usługi do
kontenera DI. Powyższa klasa wytwórni odwołuje się do identyfikatorów usług, które
jeszcze nie istnieją: ``wsse.security.authentication.provider`` i
``wsse.security.authentication.listener``. Tak wiec musimy je utworzyć.

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/services.yml
        services:
            wsse.security.authentication.provider:
                class: AppBundle\Security\Authentication\Provider\WsseProvider
                arguments:
                    - '' # User Provider
                    - '%kernel.cache_dir%/security/nonces'
                public: false

            wsse.security.authentication.listener:
                class: AppBundle\Security\Firewall\WsseListener
                arguments: ['@security.token_storage', '@security.authentication.manager']
                public: false

    .. code-block:: xml
       :linenos:

        <!-- app/config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="wsse.security.authentication.provider"
                    class="AppBundle\Security\Authentication\Provider\WsseProvider"
                    public="false"
                >
                    <argument /> <!-- User Provider -->
                    <argument>%kernel.cache_dir%/security/nonces</argument>
                </service>

                <service id="wsse.security.authentication.listener"
                    class="AppBundle\Security\Firewall\WsseListener"
                    public="false"
                >
                    <argument type="service" id="security.token_storage"/>
                    <argument type="service" id="security.authentication.manager" />
                </service>
            </services>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/services.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;

        $definition = new Definition(
            'AppBundle\Security\Authentication\Provider\WsseProvider',
            array(
                '', // User Provider
                '%kernel.cache_dir%/security/nonces',
            )
        );
        $definition->setPublic(false);
        $container->setDefinition('wsse.security.authentication.provider', $definition)

        $definition = new Definition(
            'AppBundle\Security\Firewall\WsseListener',
            array(
                new Reference('security.token_storage'),
                new Reference('security.authentication.manager'),
            )
        );
        $definition->setPublic(false);
        $container->setDefinition('wsse.security.authentication.listener', $definition);

Teraz, gdy te usługi zostały zdefiniowane, musimy w klasie pakietu aplikacji
powiadomić kontekst bezpieczeństwa o naszej wytwórni:

.. code-block:: php
   :linenos:

    // src/AppBundle/AppBundle.php
    namespace AppBundle;

    use AppBundle\DependencyInjection\Security\Factory\WsseFactory;
    use Symfony\Component\HttpKernel\Bundle\Bundle;
    use Symfony\Component\DependencyInjection\ContainerBuilder;

    class AppBundle extends Bundle
    {
        public function build(ContainerBuilder $container)
        {
            parent::build($container);

            $extension = $container->getExtension('security');
            $extension->addSecurityListenerFactory(new WsseFactory());
        }
    }

Gotowe! Możemy teraz zdefiniować część aplikacji jako chronionej przez WSSE.

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...

            firewalls:
                wsse_secured:
                    pattern:   ^/api/
                    stateless: true
                    wsse:      true

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

                <firewall
                    name="wsse_secured"
                    pattern="^/api/"
                    stateless="true"
                    wsse="true"
                />
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'firewalls' => array(
                'wsse_secured' => array(
                    'pattern'   => '^/api/',
                    'stateless' => true,
                    'wsse'      => true,
                ),
            ),
        ));

W ten sposób napisaliśmy własny kod niestandardowego wystawcy uwierzytelniania.

Nieco więcej
------------

Czyż tworzenie własnego wystawcy uwierzytelniania WSSE nie jest trochę ekscytujące?
Możliwości są ogromne. Dodajmy trochę blasku do tego cośmy zrobili.

Konfiguracja
~~~~~~~~~~~~

W konfiguracji bezpieczeństwa można dodać własne opcje w kluczu ``wsse``.
Na przykład, termin wygaśnięcia elementu ``Created`` nagłówka domyślnie wynosi
5 minut. Możemy tak ustawić konfigurację, aby różne zapory mogły mieć różny
limit tego czasu.

Najpierw musimy edytować ``WsseFactory`` i zdefiniować nową opcje w metodzie
``addConfiguration``.

.. code-block:: php
   :linenos:

    class WsseFactory implements SecurityFactoryInterface
    {
        // ...

        public function addConfiguration(NodeDefinition $node)
        {
          $node
            ->children()
            ->scalarNode('lifetime')->defaultValue(300)
            ->end();
        }
    }

Teraz, w metodzie ``create`` klasy wytwórni argument ``$config`` będzie zawierał
klucz ``lifetime``, ustawiony na 5 minuts (300 sekund), chyba że ustawi się to
inaczej w konfiguracji. Przekażmy ten argument do naszego wystawcy uwierzytelniaia
w celu jego wykorzystania.

.. code-block:: php
   :linenos:

    class WsseFactory implements SecurityFactoryInterface
    {
        public function create(ContainerBuilder $container, $id, $config, $userProvider, $defaultEntryPoint)
        {
            $providerId = 'security.authentication.provider.wsse.'.$id;
            $container
                ->setDefinition($providerId,
                  new DefinitionDecorator('wsse.security.authentication.provider'))
                ->replaceArgument(0, new Reference($userProvider))
                ->replaceArgument(2, $config['lifetime']);
            // ...
        }

        // ...
    }

.. note::

    Trzeba także dodać trzeci argument do konfiguracji usługi
    ``wsse.security.authentication.provider``, który może być pusty, ale zostanie
    wypełniony w czasie funkcjonowania wytwórni. Klasa ``WsseProvider`` będzie
    teraz przyjmowała trzeci argument konstruktora, ```lifetime``,który powinien
    być używany, zamiast sztywnego kodowania 300 sekund. Te dwa kroki nie zostały
    tutaj pokazane.

Limit czasu każdego żądania WSSE jest teraz możliwy do skonfigurowania i może być
ustawiony na kazdej zaporze na dowolną, pożądaną wartość.

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...

            firewalls:
                wsse_secured:
                    pattern:   ^/api/
                    stateless: true
                    wsse:      { lifetime: 30 }

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

                <firewall name="wsse_secured" pattern="^/api/" stateless="true">
                    <wsse lifetime="30" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'firewalls' => array(
                'wsse_secured' => array(
                    'pattern'   => '^/api/',
                    'stateless' => true,
                    'wsse'      => array(
                        'lifetime' => 30,
                    ),
                ),
            ),
        ));

Reszta zależy od Ciebie! W wytwórni można skonfigurować wszystkie istotne elementy
i je wykorzystać lub przekazać do innych klas w kontenerze.

.. _`HWIOAuthBundle`: https://github.com/hwi/HWIOAuthBundle
.. _`WS-Security`: https://en.wikipedia.org/wiki/WS-Security
.. _`WSSE`: http://www.xml.com/pub/a/2003/12/17/dive.html
.. _`nonce`: https://en.wikipedia.org/wiki/Cryptographic_nonce
.. _`ataków czasowych`: https://en.wikipedia.org/wiki/Timing_attack
.. _`atakami replay`: https://en.wikipedia.org/wiki/Replay_attack
.. highlight:: php
   :linenothreshold: 2

.. index::
    single: bezpieczeństwo; uwierzytelnianie
    single: bezpieczeństwo; Guard 
    single: indywidualny kod autoryzacyjny
    single: komponent Guard 

Jak stworzyć własne uwierzytelnianie z komponentem Guard
========================================================

Jeśli trzeba zbudować tradycyjny formularz logowania, system uwierzytelniania
z API tokenu lub jeśli potrzeba zintegrować jakiś własny system pojedynczego
logowania, może to bardzo ułatwić komponent Guard (uwierzytelnianie strażnicze).

W tym przykładzie. zbudujemy system uwierzytelniania z API tokenu i poznamy,
jak to działa z komponentem Guard.

Utworzenie klasy User i dostawcy użytkowników
---------------------------------------------

Bez względu na sposób uwierzytelniania, trzeba utworzyć klasę implementującą
interfejs ``UserInterface`` oraz skonfigurować
:doc:`dostawcę użytkownikow </cookbook/security/custom_provider>`.
W tym przykładzie, użytkownicy są sa przechowywani w bazie danej poprzez Doctrine
a kazdy użytkownik ma własna właściwość ``apiKey``, wykorzystywaną do uzyskiwania
dostępu do ich kont poprzez API::

    // src/AppBundle/Entity/User.php
    namespace AppBundle\Entity;

    use Symfony\Component\Security\Core\User\UserInterface;
    use Doctrine\ORM\Mapping as ORM;

    /**
     * @ORM\Entity
     * @ORM\Table(name="user")
     */
    class User implements UserInterface
    {
        /**
         * @ORM\Id
         * @ORM\GeneratedValue(strategy="AUTO")
         * @ORM\Column(type="integer")
         */
        private $id;

        /**
         * @ORM\Column(type="string", unique=true)
         */
        private $username;

        /**
         * @ORM\Column(type="string", unique=true)
         */
        private $apiKey;

        public function getUsername()
        {
            return $this->username;
        }

        public function getRoles()
        {
            return ['ROLE_USER'];
        }

        public function getPassword()
        {
        }
        public function getSalt()
        {
        }
        public function eraseCredentials()
        {
        }

        // more getters/setters
    }

.. tip::

    W tej naszej klasie User nie ma hasła, ale można dodać właściwość ``password``,
    jeśli chce się umożliwić użytkownikowi logowanie z hasłem (np. poprzez
    formularz logowania).

Klasa ``User`` nie musi być przechowywana w Doctrine: rób co chcesz.

Następnie, skonfigurujemy "dostawcę uzytkownikow":

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...

            providers:
                your_db_provider:
                    entity:
                        class: AppBundle:User

            # ...

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

                <provider name="your_db_provider">
                    <entity class="AppBundle:User" />
                </provider>

                <!-- ... -->
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'providers' => array(
                'your_db_provider' => array(
                    'entity' => array(
                        'class' => 'AppBundle:User',
                    ),
                ),
            ),

            // ...
        ));

To jest to! Więcej informacji znajduje się w:

* :doc:`/cookbook/security/entity_provider`
* :doc:`/cookbook/security/custom_provider`

Krok 1) Utworzenie klasy wystawcy uwierzytelniania
--------------------------------------------------

Załóżmy, że mamy API, które wysyła klientowi nagłówek ``X-AUTH-TOKEN`` z tokenem
API, przy każdym żądniu. Naszym zadaniem jest odczytanie tego nagłówka i odnalezienie
powiązanego użytkownika, jeśli występuje.

Dla stworzenia własnego systemu uwierzytelniania, wystarczy utworzyć klasę wystawcy
uwierzytelniania i zaimplementować w niej interfejs
:class:`Symfony\\Component\\Security\\Guard\\GuardAuthenticatorInterface`
lub rozszerzzyć prostszą klasę
:class:`Symfony\\Component\\Security\\Guard\\AbstractGuardAuthenticator`.
Wymaga to zaimplementowanie sześciu metod::

    // src/AppBundle/Security/TokenAuthenticator.php
    namespace AppBundle\Security;

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpFoundation\JsonResponse;
    use Symfony\Component\Security\Core\User\UserInterface;
    use Symfony\Component\Security\Guard\AbstractGuardAuthenticator;
    use Symfony\Component\Security\Core\Authentication\Token\TokenInterface;
    use Symfony\Component\Security\Core\Exception\AuthenticationException;
    use Symfony\Component\Security\Core\User\UserProviderInterface;
    use Doctrine\ORM\EntityManager;

    class TokenAuthenticator extends AbstractGuardAuthenticator
    {
        private $em;

        public function __construct(EntityManager $em)
        {
            $this->em = $em;
        }

        /**
         * Called on every request. Return whatever credentials you want,
         * or null to stop authentication.
         */
        public function getCredentials(Request $request)
        {
            if (!$token = $request->headers->get('X-AUTH-TOKEN')) {
                // no token? Return null and no other methods will be called
                return;
            }

            // What you return here will be passed to getUser() as $credentials
            return array(
                'token' => $token,
            );
        }

        public function getUser($credentials, UserProviderInterface $userProvider)
        {
            $apiKey = $credentials['token'];

            // if null, authentication will fail
            // if a User object, checkCredentials() is called
            return $this->em->getRepository('AppBundle:User')
                ->findOneBy(array('apiKey' => $apiKey));
        }

        public function checkCredentials($credentials, UserInterface $user)
        {
            // check credentials - e.g. make sure the password is valid
            // no credential check is needed in this case

            // return true to cause authentication success
            return true;
        }

        public function onAuthenticationSuccess(Request $request, TokenInterface $token, $providerKey)
        {
            // on success, let the request continue
            return null;
        }

        public function onAuthenticationFailure(Request $request, AuthenticationException $exception)
        {
            $data = array(
                'message' => strtr($exception->getMessageKey(), $exception->getMessageData())

                // or to translate this message
                // $this->translator->trans($exception->getMessageKey(), $exception->getMessageData())
            );

            return new JsonResponse($data, 403);
        }

        /**
         * Called when authentication is needed, but it's not sent
         */
        public function start(Request $request, AuthenticationException $authException = null)
        {
            $data = array(
                // you might translate this message
                'message' => 'Authentication Required'
            );

            return new JsonResponse($data, 401);
        }

        public function supportsRememberMe()
        {
            return false;
        }
    }

Pięknie! Każda z tych metod jest wyjaśniona w
:ref:`Metodach Guard Authenticator<guard-auth-methods>`.

Krok 2) Konfigurowanie wystawcy uwierzytelniania
------------------------------------------------

Dla zakończenia, musimy jeszcze zarejestrować klasę wystawcy uwierztelniania
jako usługę:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/services.yml
        services:
            app.token_authenticator:
                class: AppBundle\Security\TokenAuthenticator
                arguments: ['@doctrine.orm.entity_manager']

    .. code-block:: xml
       :linenos:

        <!-- app/config/services.xml -->
        <services>
            <service id="app.token_authenticator" class="AppBundle\Security\TokenAuthenticator">
                <argument type="service" id="doctrine.orm.entity_manager"/>
            </service>
        </services>

    .. code-block:: php
       :linenos:

        // app/config/services.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;

        $container->setDefinition('app.token_authenticator', new Definition(
            'AppBundle\Security\TokenAuthenticator',
            array(new Reference('doctrine.orm.entity_manager'))
        ));

Na koniec, skonfigurujemy klucz ``firewalls`` w pliku ``security.yml``, aby móc
używać wystawcy uwierzytelniania:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...

            firewalls:
                # ...

                main:
                    anonymous: ~
                    logout: ~

                    guard:
                        authenticators:
                            - app.token_authenticator

                    # if you want, disable storing the user in the session
                    # stateless: true

                    # maybe other things, like form_login, remember_me, etc
                    # ...

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

                <firewall name="main"
                    pattern="^/"
                    anonymous="true"
                >
                    <logout />

                    <guard>
                        <authenticator>app.token_authenticator</authenticator>
                    </guard>

                    <!-- ... -->
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php

        // ..

        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'main'       => array(
                    'pattern'        => '^/',
                    'anonymous'      => true,
                    'logout'         => true,
                    'guard'          => array(
                        'authenticators'  => array(
                            'app.token_authenticator'
                        ),
                    ),
                    // ...
                ),
            ),
        ));

Mamy teraz w pełni funkcjonalny system uwierzytelniania z tokenem API. Jeśli
strona poczatkowa wymaga roli ``ROLE_USER``, można przetestować ją dla różnych
warunków:

.. code-block:: bash
   
    # testowanie  bez tokenu
    curl http://localhost:8000/
    # {"message":"Authentication Required"}

    # testowanie ze złym tokenem
    curl -H "X-AUTH-TOKEN: FAKE" http://localhost:8000/
    # {"message":"Username could not be found."}

    # testowanie z działającym tokenem
    curl -H "X-AUTH-TOKEN: REAL" http://localhost:8000/
    # the homepage controller is executed: the page loads normally

Teraz, dowiesz się więcej o tym, jak działa każda z tych metod.

.. _guard-auth-methods:

Metody wystawcy uwierzytelniania Guard
--------------------------------------

Każda klasa wystawcy uwierzytelniania powinna mieć następujące metody:

**getCredentials(Request $request)**
    Jest wywoływana przy każdym żądaniu i zadaniem programisty jest odczytanie
    tokenu (lub jakiejś innej informacji "uwierzytelniania") z żądania i zwrócenie tego.
    Jeśli zwracaną wartością jest ``null``, reszta procesu uwierzytelniania jest
    pomijana. W przeciwnym razie, wywoływana jest metoda ``getUser()`` i zwracana
    wartość zostaje przekazana jako pierwszy argument.

**getUser($credentials, UserProviderInterface $userProvider)**
    Metoda ta jest wywoływana, gdy ``getCredentials()`` zwraca nie pustą wartość,
    która jest przekazywana tutaj jako argument ``$credentials``. Zadaniem programisty
    jest zwrócenie obiektu implementującego ``UserInterface``. Po zrobieniu tego,
    trzeba wywołać metodę ``checkCredentials()``. Jeśli zwróci ona ``null``
    (lub zrzuci wyjątek :ref:`AuthenticationException <guard-customize-error>`)
    uwierzytelniane będzie negatwne.

**checkCredentials($credentials, UserInterface $user)**
    Metoda ta jest wywoływana, gdy ``getUser()`` zwraca obiekt User. Zadaniem
    programisty jest zweryfikowanie, czy poświadczenie jest prawidłowe. Przy
    stosowaniu formularza logowania, trzeba sparwdzić, czy podane hasło jest
    prawidłowe dla danego uzytkownika. Gdy zwrócona zostaje wartość ``true``,
    uwierzytelnianie jest pomyślne. Jeśli zwrócona zostanie każda inna wartość
    (lub zrzucony będzie wyjątek :ref:`AuthenticationException <guard-customize-error>`),
    uwierzytelnienie jest negatywne.

**onAuthenticationSuccess(Request $request, TokenInterface $token, $providerKey)**
    Jest wywoływana po pomyślnym uwierzytelnieniu. Zadaniem programisty jest
    zwrócenie obiektu :class:`Symfony\\Component\\HttpFoundation\\Response`, który
    powinien zostać przesłany klientowi lub ``null`` w celu kontynuowania przetwarzania
    żądania (np. aby umożliwić wywołanie trasy lub kontrolera w sposób normalny).
    Ponieważ jest to API, w którym następuje samo uwierzytelnienie żądania, trzeba
    zwrócić ``null``.

**onAuthenticationFailure(Request $request, AuthenticationException $exception)**
    Jest wywowływana, jeśli uwierzytelnienie bedzie negatywne. Zadaniem programisty
    jest zwrócenie obiektu :class:`Symfony\\Component\\HttpFoundation\\Response`,
    który powinien być przesłany do klienta. Obiekt ``$exception`` powiadamia
    o tym, co poszło nie tak podczas uwierzytelniania.

**start(Request $request, AuthenticationException $authException = null)**
    Zostaje wywołana, gdy klient uzyskuje dostęp do zasobu (URI), wymaga uwierzytelniania,
    ale żadne szczegóły uwierzytelniania nie zostały przesłane (czyli gdy metoda
    ``getCredentials()`` zwraca ``null``). Zadaniem programisty jest zwrócenie
    obiektu :class:`Symfony\\Component\\HttpFoundation\\Response`, który pomaga
    w uwierzytelnianiu użytkowników (np. odpowiedź 401, który mówi "token jest zły!").

**supportsRememberMe**
    Jeśli chce się obsługiwać funkcjonalność "remember me", trzeba w tej metodzie
    zwrócić ``true``.
    W dalszym ciągu trzeba będzie aktywować opcję ``remember_me`` w konfiguracji
    zapory. Ponieważ nasze API jest niezależne, nie będziemy w tym przykładzie
    obsługiwać funkcjonalność "remember me".

.. _guard-customize-error:

Dostosowywanie komunikatów
--------------------------

Gdy wywoływana jest metoda ``onAuthenticationFailure()``, przekazywany jest obiekt
``AuthenticationException``, który opisuje to niepowodzenie uwierzytelniania
poprzez metodę ``$e->getMessageKey()`` (i ``$e->getMessageData()``). Komunikaty
będa się różnić z zależnosci od miejsca, w którym nastąpiło niepowodzenie uwierzytelniania
(np. ``getUser()`` vs ``checkCredentials()``).

Można jednak łatwo zwrócić własny komunikat przez zrzucenie wyjątku
:class:`Symfony\\Component\\Security\\Core\\Exception\\CustomUserMessageAuthenticationException`.
Mozna zrzucić go z metod ``getCredentials()``, ``getUser()`` lub ``checkCredentials()``::

    // src/AppBundle/Security/TokenAuthenticator.php
    // ...

    use Symfony\Component\Security\Core\Exception\CustomUserMessageAuthenticationException;

    class TokenAuthenticator extends AbstractGuardAuthenticator
    {
        // ...

        public function getCredentials(Request $request)
        {
            // ...

            if ($token == 'ILuvAPIs') {
                throw new CustomUserMessageAuthenticationException(
                    'ILuvAPIs is not a real API key: it\'s just a silly phrase'
                );
            }

            // ...
        }

        // ...
    }

W tym przypadku, ponieważ "ILuvAPIs" jest nieprawidłowym (a nawet śmiesznym)
kluczem API, można zawrzeć w zwracanej odpowiedzi coś równie śmiesznego, jeśli
powtórzy się próba uwierzytelnienia z tym kluczem:

.. code-block:: bash

    curl -H "X-AUTH-TOKEN: ILuvAPIs" http://localhost:8000/
    # {"message":"ILuvAPIs is not a real API key: it's just a silly phrase"}

Najczęsciej zadawane pytania
----------------------------

**Czy można mieć wielu wystawców uwierzytelniania?**
    Tak, ale gdy się to zrobi, trzeba będzie wybrać *tylko jednego* wystawcę
    uwierzytelniania w swoim "punkcie wejścia". Oznacza to, musi się wybrać,
    którego wystawcę powinna wywoływać metoda ``start()``, gdy anonimowy użytkownik
    próbuje uzyskać dostęp do chronionego zasobu.
    Na przykład załóżmy, że mamy usługę wystawcy uwierzytelniania
    ``app.form_login_authenticator``, która obsługuje tradycyjne logowanie formularzowe.
    Gdy anonimowy użytkownik chce uzyskać dostęp do chronionej strony, chcemy
    aby użyta została metoda ``start()`` z uwierzytelnianiem formularzowym
    i następowało przekierowanie do strony logowania (zamiast zwracania odpowiedzi
    JSON):

    .. configuration-block::

        .. code-block:: yaml
           :linenos:

            # app/config/security.yml
            security:
                # ...

                firewalls:
                    # ...

                    main:
                        anonymous: ~
                        logout: ~

                        guard:
                            authenticators:
                                - app.token_authenticator

                        # if you want, disable storing the user in the session
                        # stateless: true

                        # maybe other things, like form_login, remember_me, etc
                        # ...

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

                    <firewall name="main"
                        pattern="^/"
                        anonymous="true"
                    >
                        <logout />

                        <guard>
                            <authenticator>app.token_authenticator</authenticator>
                        </guard>

                        <!-- ... -->
                    </firewall>
                </config>
            </srv:container>

        .. code-block:: php
           :linenos:

            // app/config/security.php

            // ..

            $container->loadFromExtension('security', array(
                'firewalls' => array(
                    'main'       => array(
                        'pattern'        => '^/',
                        'anonymous'      => true,
                        'logout'         => true,
                        'guard'          => array(
                            'authenticators'  => array(
                                'app.token_authenticator'
                            ),
                        ),
                        // ...
                    ),
                ),
            ));

**Czy mogę używać to z ``form_login``?**
    Tak. ``form_login`` jest jednym ze sposobów uwierzytelniania użytkowników,
    tak więc można zastosować ten mechanizm i następnie dodać jeden lub więcej
    wystawców uwierzyteniania. Używanie wystawcy uwierzytelniania strażniczego
    nie koliduje z innymi sposobami uwierzytelniania.

**Czy mogą stosować to z FOSUserBundle?**
    Tak. Faktycznie, FOSUserBundle nie obsługuje mechanizmu bezpieczeństwa. Pakiet
    ten po prostu daje obsługę obiektu ``User`` oraz kilku tras i kontrolerów
    pomagajacych w logowaniu, rejestrowaniu, odzyskaniu hasła itd. Gdy używa się
    FOSUserBundle, zwykle stosuje się logowanie formularzowe do rzeczywistego
    uwierzytelniania uzytkowników. Można oprzeć sie na tym rozwiązaniu (patrz
    poprzednie pytania) lub zastosować obiekt ``User`` z pakietu FOSUserBundle
    i utworzyć własnego wystawcę uwierzytelniania, tak jak omówiono to w tym
    artykule.
